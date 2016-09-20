
#ifndef ARCH_H
#define ARCH_H


#if (CPUARCH == ARM)

#define INT_MAX_NUM 32

#define INT_UART0 28

#define INT_SUB_RXD0 0

#define INT_BASE	(0xca000000)
#define INTMSK		(INT_BASE+0x8)
#define INTOFFSET	(INT_BASE+0x14)
#define INTPND		(INT_BASE+0x10)
#define SRCPND		(INT_BASE+0x0)

#define SUBSRCPND	(INT_BASE+0x18)
#define SUBINTMASK     (INT_BASE+0x1C)

#define FCLK	296352000
#define HCLK (FCLK/3)
#define PCLK (HCLK/2)
#define UCLK 48000000

// CLOCK & POWER MANAGEMENT
#define rLOCKTIME   (*(volatile unsigned *)0xcc000000)	//PLL lock time counter
#define rMPLLCON    (*(volatile unsigned *)0xcc000004)	//MPLL Control
#define rUPLLCON    (*(volatile unsigned *)0xcc000008)	//UPLL Control
#define rCLKCON     (*(volatile unsigned *)0xcc00000c)	//Clock generator control
#define rCLKSLOW    (*(volatile unsigned *)0xcc000010)	//Slow clock control
#define rCLKDIVN    (*(volatile unsigned *)0xcc000014)	//Clock divider control
#define rCAMDIVN    (*(volatile unsigned *)0xcc000018)	//USB, CAM Clock divider control


#define rUTXH0 (*(volatile unsigned char *)0xd0000020)	//UART 0 Transmission Hold
#define rURXH0 (*(volatile unsigned char *)0xd0000024)	//UART 0 Receive buffer
#define rUTXH1 (*(volatile unsigned char *)0xd0004020)	//UART 1 Transmission Hold
#define rURXH1 (*(volatile unsigned char *)0xd0004024)	//UART 1 Receive buffer
#define rUTXH2 (*(volatile unsigned char *)0xd0008020)	//UART 2 Transmission Hold
#define rURXH2 (*(volatile unsigned char *)0xd0008024)	//UART 2 Receive buffer

// UART
#define rULCON0     (*(volatile unsigned *)0xd0000000)	//UART 0 Line control
#define rUCON0      (*(volatile unsigned *)0xd0000004)	//UART 0 Control
#define rUFCON0     (*(volatile unsigned *)0xd0000008)	//UART 0 FIFO control
#define rUMCON0     (*(volatile unsigned *)0xd000000c)	//UART 0 Modem control
#define rUTRSTAT0   (*(volatile unsigned *)0xd0000010)	//UART 0 Tx/Rx status
#define rUERSTAT0   (*(volatile unsigned *)0xd0000014)	//UART 0 Rx error status
#define rUFSTAT0    (*(volatile unsigned *)0xd0000018)	//UART 0 FIFO status
#define rUMSTAT0    (*(volatile unsigned *)0xd000001c)	//UART 0 Modem status
#define rUBRDIV0    (*(volatile unsigned *)0xd0000028)	//UART 0 Baud rate divisor

#define rULCON1     (*(volatile unsigned *)0xd0004000)	//UART 1 Line control
#define rUCON1      (*(volatile unsigned *)0xd0004004)	//UART 1 Control
#define rUFCON1     (*(volatile unsigned *)0xd0004008)	//UART 1 FIFO control
#define rUMCON1     (*(volatile unsigned *)0xd000400c)	//UART 1 Modem control
#define rUTRSTAT1   (*(volatile unsigned *)0xd0004010)	//UART 1 Tx/Rx status
#define rUERSTAT1   (*(volatile unsigned *)0xd0004014)	//UART 1 Rx error status
#define rUFSTAT1    (*(volatile unsigned *)0xd0004018)	//UART 1 FIFO status
#define rUMSTAT1    (*(volatile unsigned *)0xd000401c)	//UART 1 Modem status
#define rUBRDIV1    (*(volatile unsigned *)0xd0004028)	//UART 1 Baud rate divisor
                                             
#define rULCON2     (*(volatile unsigned *)0xd0008000)	//UART 2 Line control
#define rUCON2      (*(volatile unsigned *)0xd0008004)	//UART 2 Control
#define rUFCON2     (*(volatile unsigned *)0xd0008008)	//UART 2 FIFO control
#define rUMCON2     (*(volatile unsigned *)0xd000800c)	//UART 2 Modem control
#define rUTRSTAT2   (*(volatile unsigned *)0xd0008010)	//UART 2 Tx/Rx status
#define rUERSTAT2   (*(volatile unsigned *)0xd0008014)	//UART 2 Rx error status
#define rUFSTAT2    (*(volatile unsigned *)0xd0008018)	//UART 2 FIFO status
#define rUMSTAT2    (*(volatile unsigned *)0xd000801c)	//UART 2 Modem status
#define rUBRDIV2    (*(volatile unsigned *)0xd0008028)	//UART 2 Baud rate divisor

#endif

#if (CPUARCH == MIPS)

#define SPIM_ADDR           0xa2000000

#define KEYBOARD_1_CONTROL  0x00
#define KEYBOARD_1_DATA     0x04
#define DISPLAY_1_CONTROL   0x08
#define DISPLAY_1_DATA      0x0C
#define CLOCK_CONTROL       0x20

#define INT_MAX_NUM 8

/* Interrupt line 3 (Cause bit 0x0800) is wired to the #1 Keyboard */
#define INT_UART0 3 

#define CTL_IE    0x00000002
#define CTL_RDY   0x00000001

#define INTERRUPT_MAGIC_WORD 0x5a5aa5a5

#define Status_IEc_MASK 0x00000001   /* Current Interrupt Enable status (0) */

#define CLOCK_ADDR 0xa1010000

/* Register offsets */
#define REAL_SECONDS     0x00
#define REAL_MICRO       0x04
#define SIM_SECONDS      0x08
#define SIM_MICRO        0x0C
#define CLOCKDEV_CONTROL 0x10

#endif


#endif

