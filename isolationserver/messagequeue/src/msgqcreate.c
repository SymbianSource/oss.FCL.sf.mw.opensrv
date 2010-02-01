/** 
 *  @file MsgQCreate.cpp
 *  Description: Source file for MsgQLib's MsgQCreate API
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
#include <msgqinternal.h>
#include <msgliterals.h>


/********************************************************************************
* MsgQCreate
* Description: Creates a message queue
* Inputs:
*  qName   : ULONG that represents the queue name
*  maxMsgs : ULONG that represents maximum size of messages 
*********************************************************************************/
EXPORT_C int MsgQCreate(ULONG qName, ULONG maxMsgs, ULONG qOptions, int* err) {
	

	    UNUSED_FORMAL_PARAM(qName);
	    UNUSED_FORMAL_PARAM(maxMsgs);
	    UNUSED_FORMAL_PARAM(err);
		if((qOptions == MSG_Q_FIFO) || (qOptions == MSG_Q_PRIORITY)) {
			// Set msg queue options to  FIFO and create the message queue
			qOptions= MSG_Q_FIFO ;
			
		if((msgget((key_t) qName ,IPC_CREAT | 0666 | IPC_EXCL )) == -1 ) {
			if ( errno == 17 ){
			// message queue already exists
			// now just get the hanlde to it
			// no need to leave over here;
			}
		}
			
			if((msgget((key_t) qName ,IPC_CREAT | 0666 )) >=0 ) {
				
				    //  set msg queue parameter max # bytes in queue
				
					/* if( msgctl(qId,IPC_STAT,&qStatus) == 0  ) 
					
					if( qStatus.msg_qbytes > (maxMsgs * MAX_MSG_LEN) ) {
						qStatus.msg_qbytes = maxMsgs * MAX_MSG_LEN ;              
						if( msgctl(qId,IPC_SET,&qStatus) < 0) {
							// delete message queue on error 
							msgctl(qId,IPC_RMID,0);
							*err = errno;
							return(ERROR);
						}
					} 
					
					*/
				
				*err = OK;                
				return (OK);
					
				
			}
			else {
				*err = errno;
			}       
		}
		else {
			*err = KMsgQLibParamErr;
		}
		
		return(ERROR);
}

