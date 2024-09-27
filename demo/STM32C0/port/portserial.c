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

#include "port.h"
#include "usart.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);
static void uart_tx_ready(UART_HandleTypeDef *huart);
static void uart_rx_complete(UART_HandleTypeDef *huart);
/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable) {
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    
}

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
                       eMBParity eParity) {
    MX_USART2_UART_Init();
    
    BOOL tx_cb_ok = (HAL_OK == HAL_UART_RegisterCallback(&huart2,
                     HAL_UART_TX_COMPLETE_CB_ID, uart_tx_ready));
    BOOL rx_cb_ok = (HAL_OK == HAL_UART_RegisterCallback(&huart2,
                     HAL_UART_RX_COMPLETE_CB_ID, uart_rx_complete));
                     
    return tx_cb_ok && rx_cb_ok;
}

BOOL xMBPortSerialPutByte(CHAR ucByte) {
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    const uint8_t data = ucByte;
    const uint16_t bytes = 1;
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, &data, bytes, 100);
    return HAL_OK == status;
}

BOOL xMBPortSerialGetByte(CHAR *pucByte) {
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR(void) {
    static uint8_t count = 0;
    
    if (count < 10) {
        (void)xMBPortSerialPutByte('a');
    } else {
        vMBPortSerialEnable(FALSE, FALSE);
    }
    
    pxMBFrameCBTransmitterEmpty();
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR(void) {
    pxMBFrameCBByteReceived();
}

static void uart_tx_ready(UART_HandleTypeDef *huart) {
    prvvUARTTxReadyISR();
}

static void uart_rx_complete(UART_HandleTypeDef *huart) {
    prvvUARTRxISR();
}
