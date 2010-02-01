/** 
 *  @file MsgQRecv.cpp
 *  Description: Source file for MsgQLib's MsgQReceive API
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
#include <unistd.h>
#include <string.h>
#include <msgqinternal.h>



/*******************************************************************************
* MsgQReceive (qName, msg, maxNBytes, timeout, err)
* Description: Function for receiving a message with internal copy
********************************************************************************/

EXPORT_C int MsgQReceive(ULONG qName, char* msg, ULONG maxNBytes, int timeout, int* err) {
	ssize_t rxBytes;
	int toTicks;

	int         qId;
	struct {
		long  mtype;
		char mtext[MAX_MSG_LEN];
	} message;

	/* check parameters */

	if((qId = msgget((key_t) qName ,IPC_CREAT | 0666 /*| IPC_EXCL */ )) >=0 ) {
		
	        toTicks = timeout;      
		message.mtype = 1;
		/* receive message */
		if((rxBytes = msgrcv(qId, &message, (size_t)maxNBytes, 1, toTicks)) != -1) {
			message.mtext[rxBytes-4] = '\0';
			bcopy(message.mtext, msg, rxBytes);
	
			*err = OK;
			return ((ULONG)rxBytes);
		}
		else {
			*err = errno;
		}
			
	}
	else {
		*err = KMsgQLibQIdErr;
	}
	
	return(ERROR);
}


