/*
 * FreeModbus Libary: BARE Port
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
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "tim.h"
#include "gpio.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR(void);
static void tim_period_elapsed(TIM_HandleTypeDef *htim);
/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit(USHORT usTim1Timerout50us) {
    MX_TIM1_Init();
    return HAL_OK == HAL_TIM_RegisterCallback(&htim1, HAL_TIM_PERIOD_ELAPSED_CB_ID,
            tim_period_elapsed);
}

inline void vMBPortTimersEnable() {
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
    HAL_TIM_Base_Start_IT(&htim1);
}

inline void vMBPortTimersDisable() {
    /* Disable any pending timers. */
    HAL_TIM_Base_Stop_IT(&htim1);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR(void) {
    (void)pxMBPortCBTimerExpired();
}

static void tim_period_elapsed(TIM_HandleTypeDef *htim) {
    prvvTIMERExpiredISR();
    HAL_GPIO_TogglePin(Led_GPIO_Port, Led_Pin);
}
