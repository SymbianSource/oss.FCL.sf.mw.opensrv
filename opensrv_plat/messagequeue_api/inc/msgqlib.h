/** 
 *  @file MsgQLib.h
 *  Description: Header file for MsgQLib
 *  Copyright (c) 2007 Nokia Corporation.
*  All rights reserved.
*  Redistribution and use in source and binary forms, with or without modification, 
*  are permitted provided that the following conditions are met:
*  Redistributions of source code must retain the above copyright notice, this list 
*  of conditions and the following disclaimer.Redistributions in binary form must 
*  reproduce the above copyright notice, this list of conditions and the following 
*  disclaimer in the documentation and/or other materials provided with the distribution.
*  Neither the name of the Nokia Corporation nor the names of its contributors may be used 
*  to endorse or promote products derived from this software without specific prior written 
*  permission.
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
*  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __MSGQLIB_H__
#define __MSGQLIB_H__

#include <_ansi.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MsgQLib sizes */
#define  MAX_MSG_LEN	   2048 

/* hash table parameters - table size must be a prime number  */
#define  MSGQ_TBL_SZ	   101

/* message send states */
#define MSG_Q_READY        0
#define MSG_Q_CLEANING	   1


#define NO_WAIT 			0
#define WAIT_FOREVER 		0xFFFF

#ifndef NULL
#define NULL 				0
#endif /* NULL */

/* errors */
#define KMsgQLibErr			 	0x100
#define KMsgQLibNoMemoryErr 	(KMsgQLibErr | 1) /* out of memory */
#define KMsgQLibQIdErr 			(KMsgQLibErr | 2) /* queue already in use or invalid */
#define KMsgQLibParamErr    	(KMsgQLibErr | 3) /* illegal parm(s)/val(s) */
#define KMsgQLibQFlushErr   	(KMsgQLibErr | 4) /* queue being flushed */
#define KMsgQLibQShortErr   	(KMsgQLibErr | 5) /* internal error:  inconsistency between #of msgs*/

#define  MSG_Q_FIFO       1
#define  MSG_Q_PRIORITY   2
#define  MSG_PRI_NORMAL   3
#define  MSG_PRI_URGENT   4
#define  SEM_Q_FIFO       5
#define  SEM_Q_PRIORITY   6

#define  OK               0
#define  ERROR           -1  

typedef  unsigned long    ULONG ;
typedef  unsigned short   ushort_t;


/**
 *
 * Creates a message queue with the argument passed
 *
 * @param qName   - queue name
 * @param maxMsgs - max messages in the queue
 * @param qOptions - message queue options
 *                        MSG_Q_FIFO
 *                        MSG_Q_PRIORITY
 * @param err - error code to be returned
 *
 * @return Returns status and error code
 */
IMPORT_C int MsgQCreate (ULONG qName, ULONG maxMsgs, ULONG qOptions, int* err);

/**
 *
 * This function deletes a message queue
 *
 * @param qName - queue name
 * @param err - error code to be returned
 *
 * @return Returns status and error code
 **/
IMPORT_C int MsgQDelete (ULONG qName, int* err);

/**
 *
 * This function sends a message with internal copy
 *
 * @param qName - queue name
 * @param msg - message to send
 * @param nBytes - length of message
 * @param priority - message priority
 * @param timeout - milliseconds to wait
 * @param err - error code to be returned
 *
 * @return Returns status and error code
 **/

IMPORT_C int MsgQSend (ULONG qName, char* msg, ULONG nBytes, ULONG priority, int timeout, int* err);

/**
 *
 * This function receives a message with internal copy
 *
 * @param qName - queue name
 * @param msg - buffer for received message
 * @param maxNBytes - length of buffer
 * @param timeout - milliseconds to wait
 * @param err - error code to be returned
 *
 * @return Returns status and error code
 **/

IMPORT_C int MsgQReceive (ULONG qName, char* msg, ULONG maxNBytes, int timeout, int* err);

/**
 *
 * This function checks how many messages are in a queue
 *
 * @param qName - queue name
 * @param err - error code to be returned
 *
 * @return Returns status and error code
 **/
IMPORT_C int MsgQCheck (ULONG qName, int* err);

/**
 * This function checks the maximum number of messages in a queue
 *
 * @param qName - queue name
 * @param err - error code to be returned
 *
 * @return Returns status and error code
 **/
IMPORT_C int MsgQMaxCheck (ULONG qName, int* err);

/**
 *
 * This function empties the specified queue
 *
 * @param qName - queue name
 * @param err - error code to be returned
 *
 * @return Returns status and error code
 **/
IMPORT_C int MsgQClean (ULONG qName, int* err);

#ifdef __cplusplus
}
#endif

#endif // __MSGQLIB_H__ 
