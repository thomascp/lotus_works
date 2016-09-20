
#include "arch.h"
#include "typedef.h"

#if (CPUARCH == ARM)
void ChangeMPllValue(int mdiv,int pdiv,int sdiv)
{
    rMPLLCON = (mdiv<<12) | (pdiv<<4) | sdiv;
}


void ChangeClockDivider(int hdivn_val,int pdivn_val)
{
	int hdivn=2, pdivn=0;
	
     // hdivn_val (FCLK:HCLK)ratio hdivn
     // 11           1:1       (0)
     // 12           1:2       (1)
     // 13           1:3       (3) 
     // 14           1:4       (2)
     // pdivn_val (HCLK:PCLK)ratio pdivn
     // 11           1:1       (0)
     // 12           1:2       (1)	 FCLK:HCLK:PCLK=1:4:8
	switch(hdivn_val) {
		case 11: hdivn=0; break;
		case 12: hdivn=1; break;
		case 13:
		case 16: hdivn=3; break;
		case 14: 
		case 18: hdivn=2; break;
	}
	
	switch(pdivn_val) {
		case 11: pdivn=0; break;
		case 12: pdivn=1; break;
	}
	
	//Uart_Printf("Clock division change [hdiv:%x, pdiv:%x]\n", hdivn, pdivn);
	rCLKDIVN = (hdivn<<1) | pdivn;

	switch(hdivn_val) {
		case 16:		// when 1, HCLK=FCLK/8.
			rCAMDIVN = (rCAMDIVN & ~(3<<8)) | (1<<8); 
		break; 
		case 18: 	// when 1, HCLK=FCLK/6.
			rCAMDIVN = (rCAMDIVN & ~(3<<8)) | (1<<9); 
		break;
	}
}

void clk_init(void)
{
	UINT32 key = 14;
	UINT32 mpll_val = (92<<12)|(1<<4)|(1);
		
	//init FCLK=400M, so change MPLL first   FCLK:HCLK:PCLK=1:4:8
	ChangeMPllValue((mpll_val>>12)&0xff, (mpll_val>>4)&0x3f, mpll_val&3);
	ChangeClockDivider(key, 12);
}

#elif (CPUARCH == MIPS)

void clk_init(void)
{
	/* external clk device, configured by hardware ini file */
}

#endif

