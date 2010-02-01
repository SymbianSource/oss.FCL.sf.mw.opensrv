/** 
 *  @file MsgQSend.cpp
 *  Description: Source file for MsgQLib's MsgQSend API
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

#include <errno.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <msgqlib.h>

#include <msgqinternal.h>

/*******************************************************************************
* MsgQSend (qName, msg, nBytes, priority, timeout, err)
* Description:  Function for sending a message with internal copy
*********************************************************************************/

EXPORT_C int MsgQSend(ULONG qName, char* msg, ULONG nBytes, ULONG priority, int timeout, int* err)
{
	int         qId;
	struct {
		long mtype;
		char mtext[MAX_MSG_LEN];
	} message;

	/* check parameters */
	if ((priority == MSG_PRI_NORMAL) || (priority == MSG_PRI_URGENT)) {
	
		if((qId = msgget((key_t) qName ,IPC_CREAT | 0666 /*| IPC_EXCL */ )) >=0 ) {
			
				if( 1 ) {
					message.mtype = 1;
					bcopy(msg, message.mtext, nBytes);
					message.mtext[nBytes] = '\0';

					if(msgsnd (qId, &message, (size_t)nBytes+4, timeout) == OK) {
						*err = OK;
						/* After successfull send, unlock the message queue by using post operation on semaphore.*/
						return (OK);
					}
					else {
						*err = errno;
					}
				}
				else
					*err = errno;
		}
		else
			*err = KMsgQLibQIdErr;
	}
	else
		*err = KMsgQLibParamErr;

	return(ERROR);
}

