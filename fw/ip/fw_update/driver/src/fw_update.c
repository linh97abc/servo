#include "fw_update.h"
#include <io.h>

#include <stdio.h>
#include <string.h>

#include <priv/alt_file.h>
#include <sys/alt_dev.h>
#include <sys/alt_debug.h>

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
