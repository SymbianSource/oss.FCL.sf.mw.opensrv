/** 
 *  @file MsgQInternals.cpp
 *  Description: Source file for MsgQLib
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
#include <stdlib.h>
#include <msgqinternal.h>
#include <e32property.h>

/* Declaration and definition of Internal Functions and data structures */
#ifdef __WINSCW__

#include <e32std.h>
typedef void TAny;
#include <pls.h> // For emulator WSD API

const TUid KMsgQLibUid3 = {0xA0001319};  


typedef struct {
	MSGQ_INFO_PTR        _MsgQInfo[MSGQ_TBL_SZ];
	MSGQ_INFO_LIST_PTR   _MsgQListHead;
}MsgQGlobalData;

int InitWSDVar(MsgQGlobalData* p) {
	p->_MsgQListHead = NULL;
	return 0;
}
MsgQGlobalData* GetGlobal() {
	 // Access the PLS of this process 
	MsgQGlobalData* p = Pls<MsgQGlobalData>(KMsgQLibUid3, &InitWSDVar);
	return p;
}


MSGQ_INFO_PTR* _MsgQInfo() {
	//return &(GetGlobal()->_MsgQInfo[0]); // orig code	
	MSGQ_INFO_PTR* p = (GetGlobal()->_MsgQInfo);
	return p;
	//return (GetGlobal()->_MsgQInfo);     // modified line
};

MSGQ_INFO_LIST_PTR* _MsgQListHead() {
	return &(GetGlobal()->_MsgQListHead);	
};

#define MsgQInfo (_MsgQInfo())
#define MsgQListHead (*_MsgQListHead())

#else
MSGQ_INFO_PTR        MsgQInfo[MSGQ_TBL_SZ];
MSGQ_INFO_LIST_PTR   MsgQListHead = NULL;
#endif

inline unsigned int HashIndex(ULONG qName);

/*************** INTERNAL FUNCTIONS ******************************/

/***************************************************************************
*  InstallMsqQTable (qName, qid, semId, sendState, numMsgs, err)
*  Function: This function installs a queue into the hash table
****************************************************************************/

int InstallMsqQTable(ULONG qName, int qId, int semId, int* err) {

	MSGQ_INFO_PTR pMsgQInfo = NULL;
	unsigned int  index;

	if ((pMsgQInfo = MsgQTableLookup(qName)) == NULL) {
		pMsgQInfo = (MSGQ_INFO_PTR)malloc(sizeof(*pMsgQInfo));

		if(pMsgQInfo != NULL) {
			index = HashIndex(qName);

			pMsgQInfo->next  = MsgQInfo[index];
			MsgQInfo[index]   = pMsgQInfo;
			pMsgQInfo->qName = qName;
			pMsgQInfo->qId   = qId;
			pMsgQInfo->semId = semId;
			pMsgQInfo->sendState = MSG_Q_READY;
			pMsgQInfo->numMsgs   = 0;

			*err = OK;
			return (OK);
		}
		else
			*err = KMsgQLibNoMemoryErr;
	}
	else
		*err = KMsgQLibQIdErr;

	return(ERROR);

}


/******************************************************************************
*  HashIndex
*  Function: This function returns the hash index
*******************************************************************************/

inline unsigned int HashIndex(ULONG qName) {
    unsigned int hash_index = (qName % MSGQ_TBL_SZ);
	//return(qName % MSGQ_TBL_SZ);
	return hash_index;
}


/************************************************************************
*  MsgQTableLookup (qName)
*  Function: This function finds the block pointer for each queue
*************************************************************************/

MSGQ_INFO* MsgQTableLookup(ULONG qName) {
	MSGQ_INFO_PTR pMsgQInfo = NULL;

	for (pMsgQInfo = MsgQInfo[HashIndex(qName)]; pMsgQInfo != NULL; pMsgQInfo = pMsgQInfo->next) {
		if (qName == pMsgQInfo->qName)
			return(pMsgQInfo);
	}

	return(NULL);
}


/*************************************************************************
*  RemoveFromMsqQTable (qName, err)
*  Function: This function removes a queue from the hash table
**************************************************************************/


int RemoveFromMsqQTable(ULONG qName, int* err) {
	unsigned int  index = 0;
	MSGQ_INFO_PTR prev = NULL;
	MSGQ_INFO_PTR pMsgQInfo = NULL;

	index = HashIndex(qName);
	for (pMsgQInfo = MsgQInfo[index]; pMsgQInfo != NULL; pMsgQInfo = pMsgQInfo->next) {
		if (qName == pMsgQInfo->qName)
			break;
		prev = pMsgQInfo;
	}

	if (pMsgQInfo != NULL) {
		if (prev == NULL)
			MsgQInfo[index] = pMsgQInfo->next;
		else
			prev->next = pMsgQInfo->next;

		free((void*)pMsgQInfo);
		*err = OK;
		return (OK);
	}
	else
		*err = KMsgQLibQIdErr;

	return(ERROR);
}


/************************************************************************
*  AddToMsgQTable (qName)
*  Function: Adding a queue to list
*************************************************************************/

void AddToMsgQTable(ULONG qName) {
	MSGQ_INFO_LIST_PTR tempNext;

	if (MsgQListHead != NULL) {
		/* subsequent entries */
		tempNext = MsgQListHead->next;

		if ((MsgQListHead->next = (MSGQ_INFO_LIST*)malloc(sizeof(MSGQ_INFO_LIST))) != NULL) 		{
			MsgQListHead->next->next = tempNext;
			MsgQListHead->next->qName = qName;
		}
		else 
			MsgQListHead->next = tempNext;
	}
	else {
		if ((MsgQListHead = (MSGQ_INFO_LIST*)malloc(sizeof(MSGQ_INFO_LIST))) != NULL) {
			MsgQListHead->next = NULL;
			MsgQListHead->qName = qName;
		}
	}
}

/************************************************************************
*  DeleteFromMsgQTable (qName)
*  Function:  removing a queu entry from  list
*************************************************************************/

void DeleteFromMsgQTable(ULONG qName) {
	MSGQ_INFO_LIST_PTR prev = NULL;
	MSGQ_INFO_LIST_PTR pMsgQInfo = NULL;

	for (pMsgQInfo = MsgQListHead; pMsgQInfo != NULL; pMsgQInfo = pMsgQInfo->next) {
		if (qName == pMsgQInfo->qName)
			break;
		prev = pMsgQInfo;
	}

	if (pMsgQInfo != NULL) {
		/* Check whether prev pointer is null or not. If it is Null, update Head pointer */
		if( prev == NULL )
			MsgQListHead = MsgQListHead->next ;

		/* Else update the linked list by removing present node and updating prev next pointer */
		else
			prev->next = pMsgQInfo->next;

		/* Now free up the memory used by the present node */
		free((void*) pMsgQInfo);
	}
}

// end of fil
