/*
 * FreeModbus Libary: RTX5 (CMSIS_OS2) Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portevent.c 5756 2022-05-10 08:07:38Z akaushal $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "cmsis_os2.h"
#include "rtx_os.h"

/* ----------------------- Variables ----------------------------------------*/
static eMBEventType		eQueuedEvent;
static bool				xEventInQueue;

static osRtxThread_t	mbThreadCtrlBlock __attribute__((section(".bss.os.thread.cb")));
static uint64_t			mbThreadStack[(512+512) / 8] __attribute__((section(".bss.os.thread.stack")));
static uint32_t			mbSemaphoreCB[osRtxSemaphoreCbSize / 4] __attribute__((section(".bss.os.semaphore.cb")));
static osSemaphoreId_t	mbSemaphore;

/* ----------------------- Start implementation -----------------------------*/
bool xMBPortEventInit( void )
{
	xEventInQueue = false;
	return true;
}

bool xMBPortEventPost( eMBEventType eEvent )
{
	xEventInQueue = true;
	eQueuedEvent = eEvent;
	if (mbSemaphore)
	{
		return (osOK == osSemaphoreRelease(mbSemaphore));
	}
	return true;
}

bool xMBPortEventGet( eMBEventType* eEvent )
{
	bool xEventHappened = false;

	if ( xEventInQueue )
	{
		*eEvent = eQueuedEvent;
		xEventInQueue = false;
		xEventHappened = true;
	}
	
	return xEventHappened;
}

void	xMBPortThread(void* args)
{
	for (;;)
	{
		if (osOK == osSemaphoreAcquire(mbSemaphore, osWaitForever))
		{
			eMBPoll();
		}
	}
	
	osThreadExit();
}

bool xMBPortStartThread()
{
	osSemaphoreAttr_t attr = {0};
	attr.name = "mb";
	attr.cb_mem = mbSemaphoreCB;
	attr.cb_size = sizeof(mbSemaphoreCB);
	mbSemaphore = osSemaphoreNew(1, 0, &attr);
	if (NULL == mbSemaphore)
	{
		return false;
	}
	
	osThreadAttr_t threadAttr = {0};
	threadAttr.name = "mb";
	threadAttr.priority = osPriorityAboveNormal;
	threadAttr.cb_mem = &mbThreadCtrlBlock;
	threadAttr.cb_size = sizeof(mbThreadCtrlBlock);
	threadAttr.stack_mem = mbThreadStack;
	threadAttr.stack_size = sizeof(mbThreadStack);
	return (NULL != osThreadNew(xMBPortThread, NULL, &threadAttr));	// Create modbus thread
}
