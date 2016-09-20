/*
uart.c:

*/

#include "typedef.h"
#include "arch.h"
#include "comm.h"

#define KEY_BACKSPACE 	0x08
#define KEY_ENTER		0x0d
#define KEY_NEWLINE		0x0a
#define KEY_SPACE		0x20


#define UART_RCV_BUF_LEN 1024
typedef struct uart_rcv
{
	char *ptail;
	char *phead;
	char rcv_buf[UART_RCV_BUF_LEN];
}uart_rcv;

void Uart_SendByte(int whichUart, int data);

uart_rcv uart0_rcv;

void init_uart0_rcv(void)
{
	uart0_rcv.phead = uart0_rcv.ptail = uart0_rcv.rcv_buf;
}

void int_uart0_char_handle(char cin)
{
	char cmd[100] = {0};
	char *p_start = uart0_rcv.rcv_buf;
	char *p_end = &uart0_rcv.rcv_buf[UART_RCV_BUF_LEN-1];
	
	if(cin == KEY_BACKSPACE)
	{
		if(uart0_rcv.phead != uart0_rcv.ptail)
		{
			if(uart0_rcv.phead == uart0_rcv.rcv_buf)
			{
				uart0_rcv.phead = p_end;
			}
			else
			{
				uart0_rcv.phead--;
			}

			*(uart0_rcv.phead) = 0;
			
			Uart_SendByte(0, KEY_BACKSPACE);
			Uart_SendByte(0, KEY_SPACE);
			Uart_SendByte(0, KEY_BACKSPACE);
		}
		else
		{
			
		}
	}
	else if((cin == KEY_ENTER)||(cin == KEY_NEWLINE))
	{
		int msg_len = 0;
		
		*(uart0_rcv.phead) = 0;

		memset(cmd, 0, 100);

		if(uart0_rcv.phead < uart0_rcv.ptail)
		{
			int copy_len_first = p_end - uart0_rcv.ptail + 1;
			int copy_len_second = uart0_rcv.phead - p_start;
			
			memcpy(cmd, uart0_rcv.ptail, copy_len_first);
			memcpy(&cmd[copy_len_first], p_start, copy_len_second);
			
			msg_len = copy_len_first + copy_len_second;
		}
		else
		{
			msg_len = uart0_rcv.phead - uart0_rcv.ptail;
			
			memcpy(cmd, uart0_rcv.ptail, msg_len);
		}

		if(0 != msg_len)
		{
			__put_char("\r\n", strlen("\r\n"));
			ltai_get_msg(cmd, msg_len);
		}
		else
		{
			__put_char("\r\n", strlen("\r\n"));
			ltprintf(LTAI_CMD_PROMPT);
		}
		
		uart0_rcv.ptail = uart0_rcv.phead;
	}
	else
	{
		*(uart0_rcv.phead) = cin;
		uart0_rcv.phead++;
		
		Uart_SendByte(0, cin);
	}
}



void Delay(int time)
{
    int i = 0;
	int delayLoopCount = 400;
	
    for(;time>0;time--)
        for(i=0;i<delayLoopCount;i++);
}

#if (CPUARCH == ARM)

void int_uart0(void)
{
	char cin = 0;
	
	if(rUTRSTAT0 & 0x1)
	{
		cin = rURXH0;

		int_uart0_char_handle(cin);
	}

	(*(volatile unsigned int*)SUBSRCPND) = 0x1;
}

void Uart_SendByteInt(int whichUart, int data)
{
    if(whichUart==0)
    {
        while((!(rUTRSTAT0 & 0x2)));   //Wait until THR is empty.
        rUTXH0 = data;
    }
    else if(whichUart==1)
    {
        while((!(rUTRSTAT1 & 0x2)));   //Wait until THR is empty.
        rUTXH1 = data;
    }   
    else if(whichUart==2)
    {
        while((!(rUTRSTAT2 & 0x2)));   //Wait until THR is empty.
        rUTXH2 = data;
    }       
}


void Uart_SendByte(int whichUart, int data)
{
	UINT32 i = 500;
	
    if(whichUart==0)
    {
        while((!(rUTRSTAT0 & 0x2)) && (i-- > 0));   //Wait until THR is empty.
        rUTXH0 = data;
    }
    else if(whichUart==1)
    {
        while((!(rUTRSTAT1 & 0x2)) && (i-- > 0));   //Wait until THR is empty.
        rUTXH1 = data;
    }   
    else if(whichUart==2)
    {
        while((!(rUTRSTAT2 & 0x2)) && (i-- > 0));   //Wait until THR is empty.
        rUTXH2 = data;
    }       
}



void Uart_Init(int pclk,int baud)
{
    int i;
    if(pclk == 0)
    pclk    = PCLK;
    rUFCON0 = 0x0;   //UART channel 0 FIFO control register, FIFO disable
    rUFCON1 = 0x0;   //UART channel 1 FIFO control register, FIFO disable
    rUFCON2 = 0x0;   //UART channel 2 FIFO control register, FIFO disable
    rUMCON0 = 0x0;   //UART chaneel 0 MODEM control register, AFC disable
    rUMCON1 = 0x0;   //UART chaneel 1 MODEM control register, AFC disable
//UART0
    rULCON0 = 0x3;   //Line control register : Normal,No parity,1 stop,8 bits
     //    [10]       [9]     [8]        [7]        [6]      [5]         [4]           [3:2]        [1:0]
     // Clock Sel,  Tx Int,  Rx Int, Rx Time Out, Rx err, Loop-back, Send break,  Transmit Mode, Receive Mode
     //     0          1       0    ,     0          1        0           0     ,       01          01
     //   PCLK       Level    Pulse    Disable    Generate  Normal      Normal        Interrupt or Polling
    rUCON0  = 0x245;   // Control register
    rUBRDIV0=( (int)(pclk/16./baud+0.5) -1 );   //Baud rate divisior register 0
//UART1
    rULCON1 = 0x3;
    rUCON1  = 0x245;
    rUBRDIV1=( (int)(pclk/16./baud+0.5) -1 );
//UART2
    rULCON2 = 0x3;
    rUCON2  = 0x245;
    rUBRDIV2=( (int)(pclk/16./baud+0.5) -1 );    

    for(i=0;i<100;i++);
}

void uart_init(void)
{
	Uart_Init( 0,115200);

	init_uart0_rcv();
	irq_reg_func(INT_UART0, int_uart0);
	umask_int(INT_UART0);
	usubmask_int(INT_SUB_RXD0);
}

#endif


#if (CPUARCH == MIPS)

volatile long *keyboard_control_reg;
volatile long *display_control_reg;
volatile long *keyboard_data_reg;
volatile long *display_data_reg;
volatile long *control_status_reg;

int keyboard_ready (void)
{
    return spim_console_is_ready (keyboard_control_reg);
}

void int_uart0(void)
{
	int i = 0;
	char cin = 0;

	for(i = 0; i < 5000; i++)
	{
		if(keyboard_ready())
		{
			break;
		}
	}

	if(5000 == i) 
		return;
	
	cin = (char) (*keyboard_data_reg) & 0x0ff;

	//printk("int 0x%x\r\n", cin);
	
	int_uart0_char_handle(cin);
}

int spim_console_is_ready(volatile long *ctrl)
{
  return (((*(ctrl)) & CTL_RDY) != 0);
}

int display_ready (void)
{
    return spim_console_is_ready (display_control_reg);
}

void internal_write_byte (char out)
{
  while (!display_ready ()) { }
  *display_data_reg = (long) out;
}

void Uart_SendByte(int whichUart, int data)
{
	internal_write_byte((char)data);
}

void Uart_SendByteInt(int whichUart, int data)
{
	Uart_SendByte(whichUart, data);
}

void turn_on_keyboard_interrupts (void)
{
	asm volatile
	(
		".set mips1\n\t"
		"mfc0 $8, $12\n\t"
		"li $9, 0x0800\n\t"
		"or $8, $8, $9\n\t"
		"mtc0 $8, $12\n\t"
	);
}

void uart_init(void)
{
	long regvalue = 0;
	
	keyboard_control_reg = (long *)(SPIM_ADDR+KEYBOARD_1_CONTROL);
    display_control_reg = (long *)(SPIM_ADDR+DISPLAY_1_CONTROL);
    keyboard_data_reg = (long *)(SPIM_ADDR+KEYBOARD_1_DATA);
    display_data_reg = (long *)(SPIM_ADDR+DISPLAY_1_DATA);
	control_status_reg = (long *)(SPIM_ADDR+CLOCK_CONTROL);

	init_uart0_rcv();
	irq_reg_func(INT_UART0, int_uart0);
	*keyboard_control_reg = CTL_IE;
	turn_on_keyboard_interrupts();
}

#endif

