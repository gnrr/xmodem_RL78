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
* File Name    : r_cg_userdefine.h
* Version      : CodeGenerator for RL78/G13 V2.03.02.01 [15 May 2015]
* Device(s)    : R5F100LE
* Tool-Chain   : CCRL
* Description  : This file includes user definition.
* Creation Date: 2016/10/13
***********************************************************************************************************************/

#ifndef _USER_DEF_H
#define _USER_DEF_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/

/* Start user code for function. Do not edit comment generated here */
typedef unsigned char         bool;
typedef unsigned char         u1;
typedef   signed char         s1;
typedef unsigned short        u2;
typedef   signed short        s2;
typedef unsigned long         u4;
typedef   signed long         s4;

#define FALSE	0
#define TRUE	1

#define SW_ON	0
#define SW_OFF	1

#define LED_ON	0
#define LED_OFF	1

// pin definition
#define	P30		(P3_bit.no0)
#define	P62		(P6_bit.no2)
#define	P63		(P6_bit.no3)

// extern references
extern u1 sw;
extern u1 sw_old;

#define WK_DELAY (10000/100)		// delay timer (UNIT=ms, 1LSB=100ms)
extern u2 wk_cnt;							// wakeup delay timer

extern const u4 content_sz;
extern const u1 content[]; // 34,654 byte max
extern u1 rcv_buf;
extern bool rcved;
/* End user code. Do not edit comment generated here */
#endif
