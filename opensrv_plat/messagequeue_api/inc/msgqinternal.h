/** 
 *  @file MsgQInternal.h
 *  Description: Internal Header file for MsgQLib
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

#ifndef MSGQINTERNAL_H
#define MSGQINTERNAL_H

#include <msgqlib.h>
#ifdef __cplusplus
extern "C" {
#endif

/* data strucure used for maintaining message queues */

typedef struct MsgQInfo* MSGQ_INFO_PTR;

typedef struct MsgQInfo {
	MSGQ_INFO_PTR	 next;	      /* next entry in list                  */
	ULONG			 qName;       /* queue name                          */
	int				 qId;		  /* queue id                 			 */
	int				 semId ;      /* semaphore id           			 */
	ULONG			 sendState;	  /* message can be sent                 */
	ULONG			 numMsgs;     /* number of messages queued           */
	ULONG			 maxNumMsgs;  /* maximum number of messages queued   */
	struct msqid_ds* slMqInfo_p;  /* msg q info                  		 */
}MSGQ_INFO;


/* Linked List used for maintaining list of all message queues created */

typedef struct MsgQInfoList* MSGQ_INFO_LIST_PTR;

typedef struct MsgQInfoList {
	MSGQ_INFO_LIST_PTR next;     /* next entry in list */
	ULONG               qName;   /* queue name         */
} MSGQ_INFO_LIST;

/* Forward Declaration of Internal Functions */
int   InstallMsqQTable(ULONG qName, int qId, int semId, int* err);
void  AddToMsgQTable(ULONG qName);
int   RemoveFromMsqQTable(ULONG qName, int* err);
void  DeleteFromMsgQTable(ULONG qName);
MSGQ_INFO* MsgQTableLookup(ULONG qName);

#ifdef __cplusplus
}
#endif
#endif /* MSGQINTERNAL_H */
