/****************************************************************************
 * Copyright © 2006 Altera Corporation, San Jose, California, USA.           *
 * All rights reserved. All use of this software and documentation is        *
 * subject to the License Agreement located at the end of this file below.   *
 ****************************************************************************/

#include "alt_types.h"

/*
 * Define whether we are booting from flash or from on-chip RAM
 */
// Do not edit these defines
#define BOOT_FROM_CFI_FLASH -1
#define BOOT_CFI_FROM_ONCHIP_ROM -2
#define BOOT_EPCS_FROM_ONCHIP_ROM -3
#define CFI -10
#define EPCS -11

/*
 * Some CRC error codes for readability.
 */
#define CRCS_VALID 0 
#define SIGNATURE_INVALID 1
#define HEADER_CRC_INVALID 2 
#define DATA_CRC_INVALID 3

/*
 * Size of buffer for processing flash contents
 */
#define FLASH_BUFFER_LENGTH 256 


/*
 * The boot images stored in flash memory, have a specific header
 * attached to them.  This is the structure of that header.  The 
 * perl script "make_header.pl", included with this example is 
 *  used to add this header to your boot image.
 */
typedef struct {
  alt_u32 signature;
  alt_u32 version;
  alt_u32 timestamp;
  alt_u32 data_length;
  alt_u32 data_crc;
  alt_u32 res1;
  alt_u32 res2;
  alt_u32 header_crc;
} my_flash_header_type;

/*
 * These are the function prototypes for all the function references in this
 * example.
 */

void ReleaseMainCPU(void entry_point(void));
void* CopyFromFlash( void * dest, const void * src, size_t num );
alt_u32 LoadFlashImage ( alt_u32 image );
alt_u32 FlashCalcCRC32(alt_u8 *flash_addr, int bytes);
alt_u32 PickFlashImage(void);
int ValidateFlashImage(void *image_ptr);
void JumpFromBootCopier(void target(void));
int MyJtagWrite(const char *buf, int len);
int MyJtagWrite8(const char *buf, int len);
alt_u32 GenericTimeoutCallback (void* context);


/******************************************************************************
 *                                                                             *
 * License Agreement                                                           *
 *                                                                             *
 * Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
 * All rights reserved.                                                        *
 *                                                                             *
 * Permission is hereby granted, free of charge, to any person obtaining a     *
 * copy of this software and associated documentation files (the "Software"),  *
 * to deal in the Software without restriction, including without limitation   *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
 * and/or sell copies of the Software, and to permit persons to whom the       *
 * Software is furnished to do so, subject to the following conditions:        *
 *                                                                             *
 * The above copyright notice and this permission notice shall be included in  *
 * all copies or substantial portions of the Software.                         *
 *                                                                             *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
 * DEALINGS IN THE SOFTWARE.                                                   *
 *                                                                             *
 * This agreement shall be governed in all respects by the laws of the State   *
 * of California and by the laws of the United States of America.              *
 * Altera does not recommend, suggest or require that this reference design    *
 * file be used in conjunction or combination with any other product.          *
 ******************************************************************************/
