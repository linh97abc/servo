#include "alt_dev_llist.h"
#include <errno.h>

int alt_dev_llist_insert (alt_dev_llist* dev, alt_llist* list)
{
  /*
   * check that the device exists, and that it has a valid name.
   */

  if (!dev || !dev->name)
  {
    return -EINVAL;
  }
  
  /*
   * register the device.
   */
  
  alt_llist_insert(list, &dev->llist);

  return 0;  
}

