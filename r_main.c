/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V2.03.02.01 [15 May 2015]
* Device(s)    : R5F100LE
* Tool-Chain   : CCRL
* Description  : This file implements main function.
* Creation Date: 2016/10/13
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_wdt.h"
#include "r_cg_it.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
#define SEND(buf)     R_UART1_Send(buf, sizeof(buf));


/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
u1 hello_buf[] = "Ready.";
u1 rcv_buf;


void xm_init(void);
void xm_send(u1 rcv);
/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */

	R_IT_Start();
	
	R_UART1_Receive(&rcv_buf, 1);
	R_UART1_Start();

	while(wk_cnt < WK_DELAY) { R_WDT_Restart(); }			// wait

	R_UART1_Send(hello_buf, sizeof(hello_buf)-1);

	xm_init();
	
	while (1U)
    {
		R_WDT_Restart();

		if(sw_old == SW_ON)
		{
			P63 = LED_OFF;							// indicate 'now resetting'

			// for soft reset
			IAWCTL=0x80;							// enable invalid memory access detecting
			*(__far volatile char *)0x00 = 0x00;	// write to 0x00 (invalid address)
			NOP();
		}
		else
		{
			P63 = LED_ON;
		}

		xm_send(rcv_buf);

    }
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
	P62 = LED_OFF;
	P63 = LED_OFF;
	
    EI();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
#define SOH    0x01
#define STX    0x02
#define EOT    0x04
#define ACK    0x06
#define NAK    0x15
#define CAN    0x18
#define CTRL_Z 0x1A

#define XMS_ST_INIT					0
#define XMS_ST_SEND_BLK				1
#define XMS_ST_BLK_WAIT_RESPONSE	2
#define XMS_ST_SEND_EOT				3
#define XMS_ST_EOT_WAIT_RESPONSE	4

#define XMS_BLK_SZ 1+2+128+1
bool rcved = FALSE;
u1 send_buf[XMS_BLK_SZ]; // SOH + SEQP + SEQN + DATA[128] + SUM

const u1 *cp;    // &content[0];
const u1 *cp_old;

s4 n_blk;  // blocks
u1 n_last; // bytes

void xm_init(void)
{
	n_blk  = (u4)(content_sz / 128);
	n_last = (u1)(content_sz % 128);

	rcved = FALSE;
}

void xm_send(u1 rcv)
{
	static u1 st = XMS_ST_INIT;
	static u1 seq = 1;

	switch(st)
	{
	case XMS_ST_INIT:
		cp = cp_old = &content[0];

		if(rcved == FALSE) return;
		rcved = FALSE;

		if(rcv == NAK)
		{
			P62 = LED_ON;            // indicate 'now sending'
			st = XMS_ST_SEND_BLK;
		}
		break;

	case XMS_ST_SEND_BLK:
		{ // buf all clear, header set
			u1 i;
			for(i=0; i<XMS_BLK_SZ; i++)
				send_buf[i] = CTRL_Z;
		}

		{ // content[] --> send_buf[3..130]
			u1 i, sz;

			sz = (n_blk > 0)? 128 : n_last;
			{
				for(i=0; i<sz; i++)
					send_buf[i+3] = *cp++;		// send_buf[3..130]
			}
		}

		{ // sum
			u1 i, sum = 0;
			for(i=0; i<128; i++)
				sum = (u1)(sum + send_buf[i+3]);
			
			send_buf[131] = sum;				// (last byte)
		}
		
		send_buf[0] = SOH;
		send_buf[1] = seq;
		send_buf[2] = 0xFF - seq;
		
		R_UART1_Send(send_buf, XMS_BLK_SZ);
		st = XMS_ST_BLK_WAIT_RESPONSE;
		break;

	case XMS_ST_BLK_WAIT_RESPONSE:
		if(rcved == FALSE) return;
		rcved = FALSE;

		if(rcv == ACK)
		{
			if(n_blk > 0)
			{
				seq++;
				n_blk--;
				cp_old = cp; // next block
				st = XMS_ST_SEND_BLK;
			}
			else
			{
				st = XMS_ST_SEND_EOT;
			}
		}
		else if(rcv == NAK)
		{
			cp = cp_old; // re-sending again
			st = XMS_ST_SEND_BLK;
		}
		else
		{
			// nop
		}
		break;

	case XMS_ST_SEND_EOT:
		{
			u1 v = EOT;
			R_UART1_Send(&v, 1);
		}
		
		st = XMS_ST_EOT_WAIT_RESPONSE;
		break;

	case XMS_ST_EOT_WAIT_RESPONSE:
		if(rcved == FALSE) return;
		rcved = FALSE;

		if(rcv == ACK)
		{
			P62 = LED_OFF;            // indicate 'idle'
			seq = 1;
			st = XMS_ST_INIT;  // done
		}
		else if(rcv == NAK)
		{
			st = XMS_ST_SEND_EOT;  // se-send
		}
		else
		{
			// nop
		}
		break;

	default:
		rcved = FALSE;
		st = XMS_ST_INIT;
		break;
	}
}
/* End user code. Do not edit comment generated here */
