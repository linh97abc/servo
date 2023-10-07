#include "fw_update.h"
#include <io.h>

#include <stdio.h>
#include <string.h>
#include <system.h>

#include <priv/alt_file.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>
#include <epcs_commands.h>

#define FW_UPDATE_CONFIG_OFFSET 0x700000
#define BOOT_CFG_ADDR (0x006F0000)
#define BOOT_FW_CFG_ADDR (0x006F0100)

#define SIGNATURE 0xa5a5a5a5

#define EPCS_CONTROLLER_BASE EPCS_FLASH_CONTROLLER_0_BASE
#define EPCS_CONTROLLER_REGISTER_OFFSET EPCS_FLASH_CONTROLLER_0_REGISTER_OFFSET

struct fw_update_boot_config_info
{
    alt_u32 sw_offset;
    alt_u32 fw_offset;
};

typedef struct
{
    alt_u32 signature;
    alt_u32 data_length;
    alt_u32 data_crc;
} fw_update_flash_fw_header_type;

typedef struct
{
    alt_u32 signature;
    alt_u32 version;
    alt_u32 timestamp;
    alt_u32 data_length;
    alt_u32 data_crc;
    alt_u32 res1;
    alt_u32 res2;
    alt_u32 header_crc;
} fw_update_flash_sw_header_type;

static void *CopyFromFlash(void *dest, const void *src, size_t num)
{

    // If we're dealing with EPCS, "src" has already been defined for us as
    // an offset into the EPCS, not an absolute address.
    epcs_read_buffer(EPCS_CONTROLLER_BASE + EPCS_CONTROLLER_REGISTER_OFFSET,
                     (int)src,
                     (alt_u8 *)dest,
                     (int)num,
                     0);

    return (dest);
}

static void *WriteToFlash(void *dest, const void *src, size_t num)
{

    // If we're dealing with EPCS, "src" has already been defined for us as
    // an offset into the EPCS, not an absolute address.
    epcs_write_buffer(EPCS_CONTROLLER_BASE + EPCS_CONTROLLER_REGISTER_OFFSET,
                      (int)dest,
                      (alt_u8 *)src,
                      (int)num,
                      0);

    return (dest);
}

static alt_u32 FlashCalcCRC32(const void *data, int bytes)
{
    alt_u32 crcval = 0xffffffff;
    int i;
    alt_u8 *cval = (alt_u8 *)data;

    while (bytes != 0)
    {
        crcval ^= *cval;
        for (i = 8; i > 0; i--)
        {
            crcval = (crcval & 0x00000001) ? ((crcval >> 1) ^ 0xEDB88320) : (crcval >> 1);
        }
        bytes--;
        cval++;
    }

    return crcval;
}

fw_update_dev *fw_update_open_dev(const char *name)
{
    extern alt_llist alt_dev_list;
    fw_update_dev *dev = (fw_update_dev *)alt_find_dev(name, &alt_dev_list);

    return dev;
}

int fw_update_init(fw_update_dev *dev)
{
    ALT_DEBUG_ASSERT((dev));
    ALT_DEBUG_ASSERT((dev->BASE));

    uint8_t minor = dev->BASE->ver.field.minor;
    uint8_t major = dev->BASE->ver.field.major;

    sprintf(dev->version, "v%d.%d", major, minor);

    alt_dev_reg(&dev->dev);

    return 0;
}

int fw_update_check_current_fw(fw_update_dev *dev)
{
    uint8_t product_id = dev->BASE->ver.field.product_id;
    uint8_t minor = dev->BASE->ver.field.minor;
    uint8_t major = dev->BASE->ver.field.major;

    if (minor != dev->VERSION_MINOR)
    {
        return -ENOTSUP;
    }

    if (major != dev->VERSION_MAJOR)
    {
        return -ENOTSUP;
    }

    if (product_id != dev->PRODUCT_ID)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int fw_update_read_partition(
    fw_update_dev *dev,
    void *data,
    int src,
    int length)
{
    int read_len;

    alt_u8 *dest = (alt_u8 *)data;
    int remain = length;

    int old_remain = remain >> 16;

    while (remain > 0)
    {
        read_len = (remain > FW_UPDATE_CONFIG_BANK_SIZE) ? FW_UPDATE_CONFIG_BANK_SIZE : remain;

        epcs_read_buffer(EPCS_CONTROLLER_BASE + EPCS_CONTROLLER_REGISTER_OFFSET,
                         src,
                         dest,
                         read_len,
                         0);

        src += FW_UPDATE_CONFIG_BANK_SIZE;
        dest += FW_UPDATE_CONFIG_BANK_SIZE;
        remain -= read_len;

        int new_remain = remain >> 16;

        if (old_remain != new_remain)
        {
            int percent = 100 - 100 * remain / length;
            if (dev->On_progress)
            {
                dev->On_progress(dev, percent);
            }

            old_remain = new_remain;
        }
    }

    return length;
}

static int fw_update_write_partition(
    fw_update_dev *dev,
    const void *data,
    int offset,
    int length)
{
    int write_len;

    alt_u8 *src = (alt_u8 *)data;
    int remain = length;

    int old_remain = remain >> 16;

    while (remain > 0)
    {
        write_len = (remain > FW_UPDATE_CONFIG_BANK_SIZE) ? FW_UPDATE_CONFIG_BANK_SIZE : remain;

        epcs_write_buffer(EPCS_CONTROLLER_BASE + EPCS_CONTROLLER_REGISTER_OFFSET,
                          offset,
                          src,
                          write_len,
                          0);

        src += FW_UPDATE_CONFIG_BANK_SIZE;
        offset += FW_UPDATE_CONFIG_BANK_SIZE;
        remain -= write_len;

        int new_remain = remain >> 16;

        if (old_remain != new_remain)
        {
            int percent = 100 - 100 * remain / length;
            if (dev->On_progress)
            {
                dev->On_progress(dev, percent);
            }

            old_remain = new_remain;
        }
    }

    return length;
}

int fw_update_read_firmware(
    fw_update_dev *dev,
    void *data)
{
    ALT_DEBUG_ASSERT((dev));
    ALT_DEBUG_ASSERT((data));

    struct fw_update_boot_config_info boot_info;
    fw_update_flash_fw_header_type header;

    CopyFromFlash(&boot_info, (void *)BOOT_CFG_ADDR, sizeof(boot_info));
    CopyFromFlash(&header, (void *)BOOT_FW_CFG_ADDR, sizeof(header));

    if (header.signature != SIGNATURE)
    {
        return 0;
    }
    

    if (header.data_length >= FW_UPDATE_FW_MAX_SIZE)
    {
        return 0;
    }

    fw_update_read_partition(dev, data, boot_info.fw_offset, header.data_length);

    alt_u32 fw_crc = FlashCalcCRC32(data, header.data_length);

    if (fw_crc != header.data_crc)
    {
        return 0;
    }

    return header.data_length;
}

int fw_update_read_software(
    fw_update_dev *dev,
    void *data)
{
    ALT_DEBUG_ASSERT((dev));
    ALT_DEBUG_ASSERT((data));

    struct fw_update_boot_config_info boot_info;
    CopyFromFlash(&boot_info, (void *)BOOT_CFG_ADDR, sizeof(boot_info));

    int sw_len = fw_update_get_sw_size(dev);

    if (sw_len == 0)
    {
        return 0;
    }

    fw_update_read_partition(dev, data, boot_info.sw_offset, sw_len);

    return sw_len;
}

int fw_update_read_config(
    fw_update_dev *dev,
    unsigned bank_id,
    void *data)
{
    ALT_DEBUG_ASSERT((dev));
    ALT_DEBUG_ASSERT((data));

    if (bank_id >= FW_UPDATE_CONFIG_BANK_NUM)
    {
        return -EINVAL;
    }

    fw_update_read_partition(
        dev,
        data,
        FW_UPDATE_CONFIG_OFFSET + bank_id * FW_UPDATE_CONFIG_BANK_SIZE,
        FW_UPDATE_CONFIG_BANK_SIZE);

    return 0;
}

int fw_update_get_fw_size(fw_update_dev *dev)
{
    struct fw_update_boot_config_info boot_info;
    fw_update_flash_fw_header_type header;

    CopyFromFlash(&boot_info, (void *)BOOT_CFG_ADDR, sizeof(boot_info));
    CopyFromFlash(&header, (void *)BOOT_FW_CFG_ADDR, sizeof(header));

    if (header.signature != SIGNATURE)
    {
        return 0;
    }
    

    return header.data_length;
}

int fw_update_get_sw_size(fw_update_dev *dev)
{
    struct fw_update_boot_config_info boot_info;
    fw_update_flash_sw_header_type header;

    CopyFromFlash(&boot_info, (void *)BOOT_CFG_ADDR, sizeof(boot_info));
    CopyFromFlash(&header, (void *)boot_info.sw_offset, sizeof(fw_update_flash_sw_header_type));
    alt_u32 crc = FlashCalcCRC32(&header, sizeof(fw_update_flash_sw_header_type) - sizeof(alt_u32));

    if (crc != header.header_crc)
    {
        return 0;
    }

    int sw_len = header.data_length + sizeof(fw_update_flash_sw_header_type);

    if (sw_len > FW_UPDATE_SW_MAX_SIZE)
    {
        return 0;
    }

    return sw_len;
}

int fw_update_save_firmware(
    fw_update_dev *dev,
    const void *data,
    uint32_t len)
{
    ALT_DEBUG_ASSERT((dev));
    ALT_DEBUG_ASSERT((data));

    if (len >= FW_UPDATE_FW_MAX_SIZE)
    {
        return -EINVAL;
    }

    alt_u32 fw_crc = FlashCalcCRC32(data, len);

    struct fw_update_boot_config_info boot_info;
    fw_update_flash_fw_header_type header;

    CopyFromFlash(&boot_info, (void *)BOOT_CFG_ADDR, sizeof(boot_info));
    CopyFromFlash(&header, (void *)BOOT_FW_CFG_ADDR, sizeof(header));

    header.signature = SIGNATURE;
    header.data_length = 0;
    header.data_crc = fw_crc;
    WriteToFlash((void *)BOOT_FW_CFG_ADDR, &header, sizeof(header));

    int write_len = fw_update_write_partition(dev, data, boot_info.fw_offset, len);

    header.data_length = write_len;
    WriteToFlash((void *)BOOT_FW_CFG_ADDR, &header, sizeof(header));

    return write_len;
}

int fw_update_save_software(
    fw_update_dev *dev,
    const void *data,
    uint32_t len)
{
    ALT_DEBUG_ASSERT((dev));
    ALT_DEBUG_ASSERT((data));

    if (len >= FW_UPDATE_SW_MAX_SIZE)
    {
        return -EINVAL;
    }

    struct fw_update_boot_config_info boot_info;
    CopyFromFlash(&boot_info, (void *)BOOT_CFG_ADDR, sizeof(boot_info));

    return fw_update_write_partition(dev, data, boot_info.sw_offset, len);
}

int fw_update_save_config(
    fw_update_dev *dev,
    unsigned bank_id,
    const void *data)
{
    ALT_DEBUG_ASSERT((dev));
    ALT_DEBUG_ASSERT((data));

    if (bank_id >= FW_UPDATE_CONFIG_BANK_NUM)
    {
        return -EINVAL;
    }

    return fw_update_write_partition(
        dev,
        data,
        FW_UPDATE_CONFIG_OFFSET + bank_id * FW_UPDATE_CONFIG_BANK_SIZE,
        FW_UPDATE_CONFIG_BANK_SIZE);
}
