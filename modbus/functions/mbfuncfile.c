/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006-2018 Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_FILE_FRAME_LEN_OFF  ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_FILE_REF_TYPE_OFF   ( MB_PDU_DATA_OFF + 1 )
#define MB_PDU_FUNC_FILE_NUM_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_FILE_REC_NUM_OFF    ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_FILE_REC_LEN_OFF    ( MB_PDU_DATA_OFF + 6 )
#define MB_PDU_FUNC_FILE_DATA_OFF       ( MB_PDU_DATA_OFF + 8 )

#define MB_PDU_FUNC_FILE_MIN_SIZE       ( 9 )

#define MB_PDU_FUNC_FILE_REF_TYPE       ( 6 )

#define MB_PDU_FUNC_FILE_MAX_LEN        ( MB_PDU_SIZE_MAX - 1 )

/* ----------------------- Static functions ---------------------------------*/
eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );

/* ----------------------- Start implementation -----------------------------*/

#if MB_FUNC_WRITE_FILE_ENABLED > 0

eMBException
eMBFuncWriteFileRecord( UCHAR * pucFrame, USHORT * usLen )
{
    UCHAR           ucFrameLen;
    UCHAR           ucRefType;
    USHORT          usFileNum;
    USHORT          usFileRec;
    USHORT          usFileRecLen;
    eMBException    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    eMBErrorCode    eRegStatus;

    if ( *usLen >= MB_PDU_FUNC_FILE_MIN_SIZE )
    {
        ucFrameLen = pucFrame[MB_PDU_FUNC_FILE_FRAME_LEN_OFF];
        ucRefType = pucFrame[MB_PDU_FUNC_FILE_REF_TYPE_OFF];
        usFileNum = ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_NUM_OFF] << 8 );
        usFileNum |= ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_NUM_OFF + 1] );
        usFileRec = ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_REC_NUM_OFF] << 8 );
        usFileRec |= ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_REC_NUM_OFF + 1] );
        usFileRecLen = ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_REC_LEN_OFF] << 8 );
        usFileRecLen |= ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_REC_LEN_OFF + 1] );

        // TODO ucFrameLen
        if ( (usFileRecLen / 2) <= MB_PDU_FUNC_FILE_MAX_LEN )
        {
            /* Make callback to update the value. */
            eRegStatus = eMBRegFileCB( &pucFrame[MB_PDU_FUNC_FILE_DATA_OFF], usFileNum,
                                       usFileRec, usFileRecLen, MB_REG_WRITE );

            if ( eRegStatus == MB_ENOERR )
            {
                eStatus = MB_EX_NONE;
            }
            else
            {
                /* If an error occured convert it into a Modbus exception. */
                eStatus = prveMBError2Exception( eRegStatus );
            }
        }
        /* else Record length too long - default status illegal data value. */
    }
    /* else Frame length too short - default status illegal data value. */
    
    return eStatus;
}
#endif

#if MB_FUNC_READ_FILE_ENABLED > 0

eMBException
eMBFuncReadFileRecord( UCHAR * pucFrame, USHORT * usLen )
{
    UCHAR           ucFrameLen;
    UCHAR           ucRefType;
    USHORT          usFileNum;
    USHORT          usFileRec;
    USHORT          usFileRecLen;
    eMBException    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    eMBErrorCode    eRegStatus;

    if ( *usLen == MB_PDU_FUNC_FILE_DATA_OFF )
    {
        ucFrameLen = pucFrame[MB_PDU_FUNC_FILE_FRAME_LEN_OFF];
        ucRefType = pucFrame[MB_PDU_FUNC_FILE_REF_TYPE_OFF];
        usFileNum = ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_NUM_OFF] << 8 );
        usFileNum |= ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_NUM_OFF + 1] );
        usFileRec = ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_REC_NUM_OFF] << 8 );
        usFileRec |= ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_REC_NUM_OFF + 1] );
        usFileRecLen = ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_REC_LEN_OFF] << 8 );
        usFileRecLen |= ( USHORT )( pucFrame[MB_PDU_FUNC_FILE_REC_LEN_OFF + 1] );

        // TODO ucFrameLen
        if ( (usFileRecLen / 2) <= MB_PDU_FUNC_FILE_MAX_LEN )
        {
            /* Make callback to update the value. */
            eRegStatus = eMBRegFileCB( &pucFrame[MB_PDU_FUNC_FILE_DATA_OFF], usFileNum,
                                       usFileRec, usFileRecLen, MB_REG_READ );

            if ( eRegStatus == MB_ENOERR )
            {
                *usLen += ( usFileRecLen * 2 );
                pucFrame[MB_PDU_FUNC_FILE_FRAME_LEN_OFF] += ( usFileRecLen * 2 );
                
                eStatus = MB_EX_NONE;
            }
            else
            {
                /* An error occured - convert it into a Modbus exception. */
                eStatus = prveMBError2Exception( eRegStatus );
            }
        }
        /* else Record length too long - default status illegal data value. */
    }
    /* else Frame length too short - default status illegal data value. */

    return eStatus;
}

#endif