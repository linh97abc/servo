/****************************************************************************
 * Copyright © 2006 Altera Corporation, San Jose, California, USA.           *
 * All rights reserved. All use of this software and documentation is        *
 * subject to the License Agreement located at the end of this file below.   *
 ****************************************************************************/
/*****************************************************************************
*  File: custom_boot_copier.c
*
*  This is an example of a custom Nios II boot copier implemented in C.  
*  The boot copier can be built and run from Nios II IDE.  For instructions 
*  on how to build and run this boot copier, refer to the application note
*  that accompianies it.
*
*  Feel free to customize this boot copier, but do so at your own risk.  This 
*  boot copier can only be supported by Altera in its current, unmodified form
*  
*****************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "advanced_boot_copier.h"
#include "system.h"
#include "alt_types.h"
#include "sys/alt_alarm.h"
#include "sys/alt_cache.h"
#include "sys/alt_dev.h"
#include "sys/alt_irq.h"
#include "sys/alt_sys_init.h"
#include "priv/alt_file.h"
#include "altera_avalon_jtag_uart_regs.h"
#include "altera_avalon_jtag_uart.h"
#include "epcs_commands.h"

/*
 *  Edit this define to control the boot method.
 *  Options are:
 *        BOOT_FROM_CFI_FLASH
 *        BOOT_CFI_FROM_ONCHIP_ROM
 *        BOOT_EPCS_FROM_ONCHIP_ROM
 */
#define BOOT_METHOD BOOT_EPCS_FROM_ONCHIP_ROM

/*
 * Edit this define to turn on or turn off the JTAG UART during boot
 */
#define USING_JTAG_UART 1

/*
 * These defines locate our two possible application images at specifically
 * these two offsets in the flash memory.  If you edit these defines, ensure
 * when programming the images to flash that you program them to the new 
 * locations you define here.
 */
#if BOOT_METHOD == BOOT_FROM_CFI_FLASH

#	define BOOT_IMAGE_1_OFFSET  ( 0x00240000 )
#	define BOOT_IMAGE_2_OFFSET  ( 0x00440000 )
#	define BOOT_IMAGE_1_ADDR  ( EXT_FLASH_BASE + BOOT_IMAGE_1_OFFSET )
#	define BOOT_IMAGE_2_ADDR  ( EXT_FLASH_BASE + BOOT_IMAGE_2_OFFSET )

#elif BOOT_METHOD == BOOT_CFI_FROM_ONCHIP_ROM

#	define BOOT_IMAGE_1_OFFSET  ( 0x00240000 )
#	define BOOT_IMAGE_2_OFFSET  ( 0x00440000 )
#	define BOOT_IMAGE_1_ADDR  ( EXT_FLASH_BASE + BOOT_IMAGE_1_OFFSET )
#	define BOOT_IMAGE_2_ADDR  ( EXT_FLASH_BASE + BOOT_IMAGE_2_OFFSET )

#elif BOOT_METHOD == BOOT_EPCS_FROM_ONCHIP_ROM

	// In an EPCS, the data is not present in the CPU's memory map, so we just
	// specify the image addresses as offsets within the EPCS.  The CopyFromFlash
	// routine will work out the details later.
	// It is important to carefully choose the offsets at which to place boot images
	// in EPCS devices, ensuring they do not overlap FPGA configuration data. These
	// offsets should be sufficient for a Cyclone II 2C35 development board.

#   define BOOT_CFG_ADDR ( 0x006F0000 )
#endif // BOOT_METHOD


/*
 * Don't edit these defines
 */
#if (BOOT_METHOD == BOOT_FROM_CFI_FLASH || BOOT_METHOD == BOOT_CFI_FROM_ONCHIP_ROM)
# define FLASH_TYPE CFI
#elif (BOOT_METHOD == BOOT_EPCS_FROM_ONCHIP_ROM)
# define FLASH_TYPE EPCS
#endif // BOOT_METHOD


/*
 * This example prints a lot of information, this macro is intended to make the
 * code a bit easier to read.  If you do not wish to print anything during boot,
 * change the define at the top of this file to "#define USING_JTAG_UART 0"
 */
#if USING_JTAG_UART 

# define JTAGPRINT(name) puts(name)


#endif //USING_JTAG_UART

#define EPCS_CONTROLLER_BASE EPCS_FLASH_CONTROLLER_0_BASE
#define EPCS_CONTROLLER_REGISTER_OFFSET  EPCS_FLASH_CONTROLLER_0_REGISTER_OFFSET


/*
 * The following statement defines "main()" so that when the Nios II SBT4E
 * debugger is set to break at "main()", it will break at the appropriate
 * place in this program, which does not contain a function called "main()".
*/
int main (void) __attribute__ ((weak, alias ("alt_main")));

/*****************************************************************************
*  Function: alt_main
*
*  Purpose: This is our boot copier's entry point. We are implementing 
*  this as an alt_main() instead of a main(), so that we can better control 
*  the drivers that load and the system resources that are enabled.  Since
*  code size may be a consideration, this method allows us to keep the 
*  memory requirements small.
*
*****************************************************************************/
int alt_main(void)
{
  /*
   * Define the local variables used by this function.
   */
  alt_u32 boot_image;
  alt_u32 entry_point;
  /*
   * Define the strings used in this function for printing out STDOUT.  The 
   * purpose of this is to avoid using the potentially large printf C library.
   */
# if USING_JTAG_UART

    char stars_str[] = "****************************************\n";

#   if BOOT_METHOD == BOOT_FROM_CFI_FLASH

      char hello_str0[] = "  Example Custom Boot Copier Starting\n        Booting from Flash\n\n";
      char hello_str1[] = "\nThis copier expects application images\nto be located at offset 0x00240000 or \n0x00440000 in flash memory.\n";

#   elif BOOT_METHOD == BOOT_CFI_FROM_ONCHIP_ROM

      char hello_str0[] = "  Example Custom Boot Copier Starting\n     Booting CFI from On-Chip RAM\n\n";
      char hello_str1[] = "\nThis copier expects application images\nto be located at offset 0x00240000 or \n0x00440000 in flash memory.\n";

#   elif BOOT_METHOD == BOOT_EPCS_FROM_ONCHIP_ROM

      char hello_str0[] = "  Custom Boot Copier Starting\n     Booting EPCS from On-Chip RAM\n\n";


#   endif //BOOT_FROM_FLASH

    char boot_proceed_str[] = "\nNow attempting to boot.\n";
    char reset_str[] = "\nNow jumping back to reset vector.\n";
    char no_image_str[] = "\nNo valid application image found in\nflash memory.\n";

    char jumping_str[] = "\nNow attempting to load and jump to the\napplication.\n\n";

    // Initialize the STDIO defaults as selected in the Nios II IDE.  
    alt_io_redirect (ALT_STDOUT, ALT_STDIN, ALT_STDERR);

# endif // USING_JTAG_UART

  /* 
   * Perform system initialization.  Since our entry point is alt_main instead 
   * of simply main, it's our responsibility to initialize any drivers we 
   * wish to use.  An example of how to do this can be found in the 
   * hello_world freestanding demo.
   */

  /*
   * In order to allow interrupts to occur while the boot copier executes we 
   * initialize the main irq handler.
   */
  alt_irq_init (ALT_IRQ_BASE);
  
  /*
   * Now we initialize the drivers that we require.
   */
  alt_sys_init();
  alt_irq_disable_all ();

  /*
   * If you intend to print anything during boot, you'll need to initialize
   * the STDIO defaults as selected in the Nios II IDE.  If you dont need
   * to print anything during boot, you can comment out this line.
   */
# if USING_JTAG_UART
    alt_io_redirect (ALT_STDOUT, ALT_STDIN, ALT_STDERR);
# endif  // USING_JTAG_UART

  
  
  /*
   * Print a startup message to let the user see we're alive.
   */
# if USING_JTAG_UART
    JTAGPRINT(stars_str);
    JTAGPRINT(hello_str0);
    JTAGPRINT(stars_str);

# endif  //USING_JTAG_UART

  /*
   * Now we begin the boot process.
   */
 
  // Print a message to the JTAG UART that we're beginning to boot 
#if USING_JTAG_UART

  JTAGPRINT(boot_proceed_str);  

#endif  //USING_JTAG_UART

  /*
   * Pick a flash image to load.  The criteria for picking an image are 
   * discussed the text of the application note, and also in the code comments 
   * preceeding the function "PickFlashImage()" found in this file.
   */

  boot_image = PickFlashImage();
  if( boot_image != 0 )
  {
    /*
     * Now we're going to try to load the application into memory, this will
     * likely overwrite our current exception handler, so before we do that 
     * we'll disable interrupts and not turn them back on again. 
     * 
     * It's also important minimize your reliance on the ".rwdata", ".bss" and
     * stack sections.  Since all of these sections can exist in the exception 
     * memory they are all subject to being overwritten.  You can inspect how 
     * much of the ".rwdata" and ".bss" sections the bootcopier uses by 
     * looking at the disassembly for the bootcopier.  The disassembly can be 
     * generated by running "nios2-elf-objdump.exe" with the -d option from a 
     * Nios II Command Shell.
     */
//    alt_irq_disable_all ();
   
    entry_point = LoadFlashImage( boot_image );
    if( entry_point >= 0 )  // load the image
    {

      JTAGPRINT(jumping_str);
      // Jump to the entry point of the application
      JumpFromBootCopier((void(*)(void))(entry_point));
    }
    else
    {


      usleep(5000000);

      // If the entry point is not found, then we should jump back to the
      // reset vector.
      JumpFromBootCopier((void(*)(void))(NIOS2_RESET_ADDR));
    }
  }
  else
  {
    // If we didn't find an applicatoin to boot, jump back to the reset vector
#   if USING_JTAG_UART
      JTAGPRINT(no_image_str);
      JTAGPRINT(reset_str);
#   endif  //USING_JTAG_UART


    
    // Wait 5 seconds
    usleep(5000000);
      
    // Jump back to the reset address
    JumpFromBootCopier((void(*)(void))(NIOS2_RESET_ADDR));
  }
  
  // We should never get here
  exit(0);
}


/*****************************************************************************
*  Function: JumpFromBootCopier
*
*  Purpose: This routine shuts down the boot copier and jumps somewhere else. 
*  The place to jump is passed in as a function pointer named "target".
*
*****************************************************************************/
void JumpFromBootCopier(void target(void))
{
  /*
   * If you have any outstanding I/O or system resources that needed to be 
   * cleanly disabled before leaving the boot copier program, then this is 
   * the place to do that.
   * 
   * In this example we only need to ensure the state of the Nios II cpu is 
   * equivalent to reset.  If we disable interrupts, and flush the caches, 
   * then the program we jump to should receive the cpu just as it would 
   * coming out of a hardware reset.
   */
  alt_irq_disable_all ();
  alt_dcache_flush_all ();
  alt_icache_flush_all ();
  
  /*
   * The cpu state is as close to reset as we can get it, so we jump to the new
   * application.
   */
  target();
  
  /*
   * In the odd event that the program we jump to decides to return, we should 
   * probably just jump back to the reset vector. We pass in the reset address
   * as a function pointer.
   */
   
  // Wait 5 seconds
  usleep(5000000);

  // Jump back to the reset address
  JumpFromBootCopier((void(*)(void))(NIOS2_RESET_ADDR));
}

/*****************************************************************************
*  Function: CopyFromFlash
*
*  Purpose:  This subroutine copies data from a flash memory to a buffer
*  The function uses the appropriate copy routine for the flash that is
*  defined by FLASH_TYPE.  EPCS devices cant simply be read from using
*  memcpy().
*
*****************************************************************************/
void* CopyFromFlash( void * dest, const void * src, size_t num )
{
# if( FLASH_TYPE == CFI )

    memcpy( dest, src, num );


# elif( FLASH_TYPE == EPCS )

    // If we're dealing with EPCS, "src" has already been defined for us as 
    // an offset into the EPCS, not an absolute address.
    epcs_read_buffer( EPCS_CONTROLLER_BASE + EPCS_CONTROLLER_REGISTER_OFFSET,
                      (int)src,
                      (alt_u8*)dest,
                      (int)num,
					  0);

# endif //FLASH_TYPE

  return (dest);
}

/*****************************************************************************
*  Function: LoadFlashImage
*
*  Purpose:  This subroutine loads an image from flash into the Nios II 
*  memory map.  It decodes boot records in the format produced from the 
*  elf2flash utility, and loads the image as directed by those records.  
*  The format of the boot record is described in the text of the application 
*  note.
* 
*  The input operand, "image" is expected to be the image selector indicating
*  which flash image, 1 or 2, should be loaded.
*
*****************************************************************************/
alt_u32 LoadFlashImage ( alt_u32 image )
{
  alt_u8 *next_flash_byte;
  alt_u32 length;
  alt_u32 address;
  
  /*
   * Load the image pointer based on the value of "image"
   * The boot image header is 32 bytes long, so we add an offset of 32.
   */
  next_flash_byte = (alt_u8 *)image + 32;

  /*
   * Flash images are not guaranteed to be word-aligned within the flash 
   * memory, so a word-by-word copy loop should not be used.
   * 
   * The "memcpy()" function works well to copy non-word-aligned data, and 
   * it is relativly small, so that's what we'll use.
   */
   
  // Get the first 4 bytes of the boot record, which should be a length record
  CopyFromFlash( (void*)(&length), (void*)(next_flash_byte), (size_t)(4) );
  next_flash_byte += 4;
  
  // Now loop until we get jump record, or a halt recotd
  while( (length != 0) && (length != 0xffffffff) )
  {
    // Get the next 4 bytes of the boot record, which should be an address 
    // record
    CopyFromFlash( (void*)(&address), (void*)(next_flash_byte), (size_t)(4) );
    next_flash_byte += 4;
    
    // Copy the next "length" bytes to "address"
    CopyFromFlash( (void*)(address), (void*)(next_flash_byte), (size_t)(length) );
    next_flash_byte += length;
    
    // Get the next 4 bytes of the boot record, which now should be another 
    // length record
    CopyFromFlash( (void*)(&length), (void*)(next_flash_byte), (size_t)(4) );
    next_flash_byte += 4;
  }
  
  // "length" was read as either 0x0 or 0xffffffff, which means we are done 
  // copying.
  if( length == 0xffffffff )
  {
    // We read a HALT record, so return a -1
    return -1;
  }
  else // length == 0x0
  {
    // We got a jump record, so read the next 4 bytes for the entry address
    CopyFromFlash( (void*)(&address), (void*)(next_flash_byte), (size_t)(4) );
    next_flash_byte += 4;
    
    // Return the entry point address
    return address;
  }
}


/*****************************************************************************
*  Function: PickFlashImage
*
*  Purpose:  This subroutine scans the two possible flash images and picks 
*  one to be loaded by the boot copier.  The selection criteria are pretty 
*  simple:
*    1 - if there's only one valid image, then we choose that one.
*    2 - if there are two valid images then
*          A - We choose the one with the latest revision
*          B - If both images have the same revision, the we choose the one
*              with the latest time stamp.
*          C - If both time stamp values are equal, the we choose image #2
*    3 - If neither image is valid, we return 0
*
*****************************************************************************/
alt_u32 PickFlashImage(void)
{
  alt_u32 img_offset;
  CopyFromFlash(&img_offset, (void*) BOOT_CFG_ADDR, 4);

  // Start by validating image 1
  if( ValidateFlashImage((alt_u32 *)(img_offset)) == CRCS_VALID )
  {
	  return img_offset;
  }

  return 0;
}


/*****************************************************************************
*  Function: ValidateFlashImage
*
*  Purpose:  This routine validates a flash image based upon three critera:
*            1.) It contains the correct flash image signature
*            2.) A CRC check of the image header 
*            3.) A CRC check of the image data (payload)
*  
*  Since it's inefficient to read individual bytes from EPCS, and since
*  we dont really want to expend RAM to buffer the entire image, we comprimise
*  in the case of EPCS, and create a medium-size buffer, who's size is 
*  adjustable by the user.
*
*****************************************************************************/
int ValidateFlashImage(void *image_ptr)
{
  my_flash_header_type temp_header  __attribute__((aligned(4)));
  
  /*
   * Again, we don't assume the image is word aligned, so we copy the header
   * from flash to a word aligned buffer.
   */
    CopyFromFlash(&temp_header, image_ptr, 32);
  
  // Check the signature first
  if( temp_header.signature == 0xa5a5a5a5 )
  {
    // Signature is good, validate the header crc
    if( temp_header.header_crc != FlashCalcCRC32( (alt_u8*)image_ptr, 28) )
    {
      // Header crc is not valid
      return HEADER_CRC_INVALID; 
     }
    else
    {
    	return CRCS_VALID;

//      // header crc is valid, now validate the data crc
//      if ( temp_header.data_crc == FlashCalcCRC32( image_ptr + 32, temp_header.data_length) )
//      {
//        // data crc validates, the image is good
//        return CRCS_VALID;
//      }
//      else
//      {
//        // data crc is not valid
//        return DATA_CRC_INVALID;
//      }
    }
  }
  else
  {
    // bad signature, return 1
    return SIGNATURE_INVALID;
  }
} 
  
/*****************************************************************************
*  Function: FlashCalcCRC32
*
*  Purpose:  This subroutine calcuates a reflected CRC32 on data located
*  flash.  The routine buffers flash contents locally in order
*  to support EPCS flash as well as CFI
*
*****************************************************************************/
alt_u32 FlashCalcCRC32(alt_u8 *flash_addr, int bytes)
{
  alt_u32 crcval = 0xffffffff;
  int i, buf_index, copy_length;
  alt_u8 cval;
  char flash_buffer[FLASH_BUFFER_LENGTH];
  
  while(bytes != 0)
  {
    copy_length = (FLASH_BUFFER_LENGTH < bytes) ? FLASH_BUFFER_LENGTH : bytes;
    CopyFromFlash(flash_buffer, flash_addr, copy_length);
    for(buf_index = 0; buf_index < copy_length; buf_index++ )
    {
      cval = flash_buffer[buf_index];
      crcval ^= cval;
      for (i = 8; i > 0; i-- )
      {
        crcval = (crcval & 0x00000001) ? ((crcval >> 1) ^ 0xEDB88320) : (crcval >> 1);
      }
      bytes--;
    }
    flash_addr += FLASH_BUFFER_LENGTH;
  }
  return crcval;
}

