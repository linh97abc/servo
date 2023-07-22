/*
*********************************************************************************************************
*                                                uC/OS-II
*                                The Real-Time Kernel (by Jean J. Labrosse)
*
*                                             WIN32 PORT
*
*                          (c) Copyright 2004-... Werner.Zimmermann@hs-esslingen.de
*                                           All Rights Reserved
*
* File : OS_CPU.H
* By   : linh97abc
*********************************************************************************************************
*/

#ifndef OS_CPU_H
#define OS_CPU_H

/*
*********************************************************************************************************
   uCOS-II standard definitions and declarations.
*********************************************************************************************************
*/

/* This is the definition for Nios32.  */
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned long  INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   long  INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */
typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bits                        */


/****************************************************************************
*                           Nios2 Miscellaneous defines
****************************************************************************/

#define  OS_STK_GROWTH        1        /* Stack grows from HIGH to LOW memory */
#define  OS_TASK_SW           OSCtxSw  

/******************************************************************************************
 *                Disable and Enable Interrupts - 2 methods
 *
 * Method #1:  Disable/Enable interrupts using simple instructions.  After critical 
 *             section, interrupts will be enabled even if they were disabled before 
 *             entering the critical section.
 *
 * Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In 
 *             other words, if interrupts were disabled before entering the critical 
 *             section, they will be disabled when leaving the critical section.  
 *
 * Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
 *             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
 *             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to 
 *             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
 *             into the CPU's status register.
 *
 *****************************************************************************************/

#define  OS_CRITICAL_METHOD     3


typedef unsigned int   		OS_CPU_SR;
#define  OS_ENTER_CRITICAL() OSDisableInterruptFlag(&cpu_sr)
#define  OS_EXIT_CRITICAL() OSEnableInterruptFlag(&cpu_sr)

/* Prototypes */
void OSCtxSw(void);
void OSIntCtxSw (void);
void OSStartHighRdy(void);

/* Port */
void OSEnableInterruptFlag(OS_CPU_SR* pCpu_Sr);
void OSDisableInterruptFlag(OS_CPU_SR* pCpu_Sr);
#endif

