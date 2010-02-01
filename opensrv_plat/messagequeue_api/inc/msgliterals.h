/*
* ============================================================================
*  Name        : isoliterals.h
*  Part of     : isolation server.
*  Version     : %version: 1 %
*
*  Copyright © 2007-2008 Nokia.  All rights reserved.
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
* ============================================================================
* Template version: 1.0
*/


#ifndef __MSGLITERALS_H__
#define __MSGLITERALS_H__

#include <msgqlib.h>

#define REQUEST_QUEUE 1

#define RESPONSE_QUEUE 2

#define RESPONSE_DATA_QUEUE 3

#define MAX_SIZE_DATA 1024

#define MAX_MSG_Q_SIZE 10

#define MAX_MSG_RECEIPIENTS ( 10 + 1 )
#define MAX_MSG_SIZE 1024

#define PORTSRV 443


#define TIME_OUT 500

#define ServerAddr "talk.google.com"
#define TestId "mytest" 
#define ServerStr "server"
#define ResourceStr "resource"
#define AccountStr "account"
#define PasswdStr "password"
#define PortStr "port"
#define OldsslStr "old-ssl"

#define NewChannelStr "NewChannel"
#define StatusChangedStr "StatusChanged"

/**
 * Macro for isoserver implementation to suppress warnings
 * from unused formal parameters.
 */
#define UNUSED_FORMAL_PARAM(p) (void) p

#endif //__MSGLITERALS_H__