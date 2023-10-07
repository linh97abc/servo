#ifndef __FW_UPDATE_H__
#define __FW_UPDATE_H__

#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>

#define FW_UPDATE_VER_REG 0
#define FW_UPDATE_LOCK_REG 1
#define FW_UPDATE_DATA_REG 2

#define FW_UPDATE_DATA_REG_NUM 6
#define FW_UPDATE_VERSION_STR_LEN 10

#define FW_UPDATE_CONFIG_BANK_SIZE 256
#define FW_UPDATE_CONFIG_BANK_NUM 256

#ifdef __cplusplus
extern "C"
{
#endif

    typedef union
    {
        struct
        {
            uint32_t minor : 8;
            uint32_t major : 8;
            uint32_t product_id : 8;
            /* data */
        } field;
        uint32_t data;
    } fw_update_VER_REG;

    typedef struct tag_fw_update_Reg
    {
        volatile fw_update_VER_REG ver;
        volatile uint32_t lock_password;
        volatile uint32_t user_data[FW_UPDATE_DATA_REG_NUM];
    } fw_update_Reg;

    /// @brief FW Update Device Type
    typedef struct tag_fw_update_Dev
    {
        struct alt_dev_s dev;

        /// @brief FW_Update Base Address
        struct tag_fw_update_Reg *const BASE;

        const unsigned PRODUCT_ID;
        const unsigned VERSION_MAJOR;
        const unsigned VERSION_MINOR;

        // vxxx.xxx
        char version[FW_UPDATE_VERSION_STR_LEN];

    } fw_update_dev;

    /// @brief Find FW_Update Device by device name
    /// @param name Device name
    /// @return Pointer to FW_Update Device, NULL if device not found
    fw_update_dev *fw_update_open_dev(const char *name);

    static inline const char *fw_update_get_version_str(fw_update_dev *dev)
    {
        ALT_DEBUG_ASSERT((dev));
        return dev->version;
    }

    static inline uint8_t fw_update_get_product_id(fw_update_dev *dev)
    {
        ALT_DEBUG_ASSERT((dev));
        return dev->BASE->ver.field.product_id;
    }

    /// @brief Check if software compatible with this firmware
    /// @param dev Pointer to FW_Update Device
    /// @return 0 if OK, -ENOTSUP if not support
    int fw_update_check_current_fw(fw_update_dev *dev);

    uint32_t fw_update_get_fw_size(fw_update_dev *dev);
    uint32_t fw_update_get_sw_size(fw_update_dev *dev);

    int fw_update_save_firmware(
        fw_update_dev *dev,
        const void *data,
        uint32_t len);

    int fw_update_save_software(
        fw_update_dev *dev,
        const void *data,
        uint32_t len);

    int fw_update_save_config(
        fw_update_dev *dev,
        unsigned bank_id,
        const void *data);

    int fw_update_read_firmware(
        fw_update_dev *dev,
        void *data);

    int fw_update_read_software(
        fw_update_dev *dev,
        void *data);

    int fw_update_read_config(
        fw_update_dev *dev,
        unsigned bank_id,
        void *data);

#ifdef __cplusplus
}
#endif

#include "LL/fw_update_fd_ll.h"
#endif
