/** 
 *  @file MsgQCheck.cpp
 *  Description: Source file for MsgQLib's MsgQCheck API
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
#include <msgqinternal.h>
#include <msgliterals.h>



/*****************************************************************************
*  MsgQCheck
*  Function: Function for checking how many messages are in a queue
*******************************************************************************/

EXPORT_C int MsgQCheck(ULONG qName, int* err) {
	
	int nMsgs;
	struct  msqid_ds  qStatus ;
	int         qId;

	
	if((qId = msgget((key_t) qName ,IPC_CREAT | 0666 /*| IPC_EXCL */)) >=0 ) {
		//   get total count of msgs from the msg queue 
		if ((msgctl(qId,IPC_STAT ,&qStatus)) >= 0 ) {
			nMsgs=qStatus.msg_qnum ;
				*err = OK;
				return(nMsgs);
		}
		else
			*err=errno;
	}
	else
		*err = KMsgQLibQIdErr;
	return(ERROR);
}

