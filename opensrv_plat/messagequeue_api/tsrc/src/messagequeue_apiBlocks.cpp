/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description:  This file contains testclass implementation.
*
*/


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include <msgqlib.h>
#include <msg_enums.h>
#include <msgliterals.h>
#include <stdlib.h>
#include <msgqinternal.h>
#include "messagequeue_api.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/
  //-----------------------------------------------------------------------------
// function_name    : running isolation server exe
// description      : Deallocates all the data
// Returns          : None
//-----------------------------------------------------------------------------
  int  run_isoserver()
  {
    // running isoserver 
    RProcess isosrv_launcher;
    TInt lunch_err = 0;
    _LIT(KProcess, "isoserver.exe");
    lunch_err = isosrv_launcher.Create( KProcess, KNullDesC );
     if ( KErrNone != lunch_err ) 
        {
        return KErrGeneral;
            
        }
    isosrv_launcher.Resume();
    return KErrNone;
  }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cmessagequeue_api::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cmessagequeue_api::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cmessagequeue_api::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cmessagequeue_api::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TestMessageQCreate", Cmessagequeue_api::TestMessageQCreate ),
        ENTRY( "TestMessageQSend", Cmessagequeue_api::TestMessageQSend ),
        ENTRY( "TestMessageQDelete", Cmessagequeue_api::TestMessageQDelete ),
        ENTRY( "TestMessageQReceive", Cmessagequeue_api::TestMessageQReceive ),
        ENTRY( "TestMessageQClean", Cmessagequeue_api::TestMessageQClean ),
        ENTRY( "TestMessageQCheck", Cmessagequeue_api::TestMessageQCheck ),
        ENTRY( "TestMsgQMaxCheck", Cmessagequeue_api::TestMsgQMaxCheck ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cmessagequeue_api::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmessagequeue_api::TestMessageQCreate( CStifItemParser& aItem )
    {
        char smsg[MAX_MSG_SIZE];
        char rmsg[MAX_MSG_SIZE];
        int index=0;
        int timeout = 100;
        int nBytes;
        int result = 0;
        int err = 0;
        int status = 0;
        int run_iso;
        FILE *fp;
        message_hdr_req *msgHdr = NULL;  
        message_hdr_resp* msg_struct = NULL;
        TInt return_value = 0;
        memset( smsg, '\0', MAX_MSG_SIZE );
        memset( rmsg, '\0', MAX_MSG_SIZE );
        //running isoserver   
        run_iso = run_isoserver(); 
        if(run_iso)
           {
           iLog->Log ( _L ( "running isolation server is failed" ) );
           return KErrGeneral;
            }             
         //create message queue for send request
        result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
        if(err)
           {
           iLog->Log ( _L ( "creating requet queue is failed" ) );
           return KErrGeneral;
            }
       //create message queue for response request
        result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
        if(err)
           {
            iLog->Log ( _L ( "creating requet queue is failed" ) );
            return KErrGeneral;
            }
            index=0;
            memset( smsg, '\0', MAX_MSG_SIZE );
            memset( rmsg, '\0', MAX_MSG_SIZE );
            msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
            memset( msgHdr, '\0', sizeof( message_hdr_req ) );
            msgHdr->message_type = EKill_Process;
            memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
            index += sizeof( message_hdr_req );
            result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
            free(msgHdr);           
           return KErrNone;
            
    }


TInt Cmessagequeue_api::TestMessageQSend( CStifItemParser& aItem )
    {
        char smsg[MAX_MSG_SIZE];
        char rmsg[MAX_MSG_SIZE];
        int index=0;
        int timeout = 100;
        int nBytes;
        int result = 0;
        int err = 0;
        int status = 0;
        int run_iso;
        FILE *fp;
        message_hdr_req *msgHdr = NULL;  
        message_hdr_resp* msg_struct = NULL;
        TInt return_value = 0;
        memset( smsg, '\0', MAX_MSG_SIZE );
        memset( rmsg, '\0', MAX_MSG_SIZE );
        //running isoserver   
        run_iso = run_isoserver(); 
        if(run_iso)
           {
           iLog->Log ( _L ( "running isolation server is failed" ) );
           return KErrGeneral;
            }             
         //create message queue for send request
        result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
        if(err)
           {
           iLog->Log ( _L ( "creating requet queue is failed" ) );
           return KErrGeneral;
            }
       //create message queue for response request
        result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
        if(err)
           {
            iLog->Log ( _L ( "creating requet queue is failed" ) );
            return KErrGeneral;
            }
            index=0;
            memset( smsg, '\0', MAX_MSG_SIZE );
            memset( rmsg, '\0', MAX_MSG_SIZE );
            msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
            memset( msgHdr, '\0', sizeof( message_hdr_req ) );
            msgHdr->message_type = EKill_Process;
            memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
            index += sizeof( message_hdr_req );
            result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
            free(msgHdr);           
           return KErrNone;
            
    }


TInt Cmessagequeue_api::TestMessageQDelete( CStifItemParser& aItem )
    {
        char smsg[MAX_MSG_SIZE];
        char rmsg[MAX_MSG_SIZE];
        int index=0;
        int timeout = 100;
        int nBytes;
        int result = 0;
        int err = 0;
        int status = 0;
        int run_iso;
        FILE *fp;
        message_hdr_req *msgHdr = NULL;  
        message_hdr_resp* msg_struct = NULL;
        TInt return_value = 0;
        memset( smsg, '\0', MAX_MSG_SIZE );
        memset( rmsg, '\0', MAX_MSG_SIZE );
        //running isoserver   
        run_iso = run_isoserver(); 
        if(run_iso)
           {
           iLog->Log ( _L ( "running isolation server is failed" ) );
           return KErrGeneral;
            }             
         //create message queue for send request
        result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
        if(err)
           {
           iLog->Log ( _L ( "creating requet queue is failed" ) );
           return KErrGeneral;
            }
       //create message queue for response request
        result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
        if(err)
           {
            iLog->Log ( _L ( "creating requet queue is failed" ) );
            return KErrGeneral;
            }
            index=0;
            memset( smsg, '\0', MAX_MSG_SIZE );
            memset( rmsg, '\0', MAX_MSG_SIZE );
            msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
            memset( msgHdr, '\0', sizeof( message_hdr_req ) );
            msgHdr->message_type = EKill_Process;
            memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
            index += sizeof( message_hdr_req );
            result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
            free(msgHdr); 
            result=MsgQDelete( REQUEST_QUEUE, &err );
            if(result)
                {
            iLog->Log ( _L ( "failing at message queue" ) );  
            return KErrGeneral;
                }
           return KErrNone;
            
    }

TInt Cmessagequeue_api::TestMessageQReceive( CStifItemParser& aItem )
    {
        char smsg[MAX_MSG_SIZE];
        char rmsg[MAX_MSG_SIZE];
        int index=0;
        int timeout = 100;
        int nBytes;
        int result = 0;
        int err = 0;
        int status = 0;
        int run_iso;
        FILE *fp;
        message_hdr_req *msgHdr = NULL;  
        message_hdr_resp* msg_struct = NULL;
        TInt return_value = 0;
        memset( smsg, '\0', MAX_MSG_SIZE );
        memset( rmsg, '\0', MAX_MSG_SIZE );
        
        //running isoserver   
        run_iso = run_isoserver(); 
        if(run_iso)
           {
           iLog->Log ( _L ( "running isolation server is failed" ) );
           return KErrGeneral;
            }             
         //create message queue for send request
        result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
        if(err)
           {
           iLog->Log ( _L ( "creating requet queue is failed" ) );
           return KErrGeneral;
            }
       //create message queue for response request
        result = MsgQCreate(RESPONSE_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
        if(err)
           {
            iLog->Log ( _L ( "creating requet queue is failed" ) );
            return KErrGeneral;
            }
        //request formation 
            memset( smsg, '\0', MAX_MSG_SIZE );
            memset( rmsg, '\0', MAX_MSG_SIZE );
            
            //message header use
            msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
            memset( msgHdr, '\0', sizeof( message_hdr_req ) );
            
            msgHdr->message_type = ELogin_Request;
            
            memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
            index += sizeof( message_hdr_req );
            /* Send Message to queueOne */
                
            result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
                
            result = MsgQReceive(RESPONSE_QUEUE, rmsg, MAX_MSG_SIZE, timeout, &err); 
                
                if (status < 0)
                {
                return  KErrGeneral;
                  
                }
            
            index=0;
            memset( smsg, '\0', MAX_MSG_SIZE );
            memset( rmsg, '\0', MAX_MSG_SIZE );
            memset( msgHdr, '\0', sizeof( message_hdr_req ) );
            msgHdr->message_type = EKill_Process;
            memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
            index += sizeof( message_hdr_req );
            result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
            free(msgHdr); 
            result=MsgQDelete( REQUEST_QUEUE, &err );
            if(result)
                {
            iLog->Log ( _L ( "failing at message queue" ) );  
            return KErrGeneral;
                }
           return KErrNone;
            
    }


TInt Cmessagequeue_api::TestMessageQCheck( CStifItemParser& aItem )
    {
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    int timeout = 100;
    int nBytes;
    int result = 0,r1=0 ,r2=0;
    int err = 0;
    int status = 0;
    int run_iso;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
    memset( smsg, '\0', MAX_MSG_SIZE );
    memset( rmsg, '\0', MAX_MSG_SIZE );
    //running isoserver   
    run_iso = run_isoserver(); 
    if(run_iso)
       {
       iLog->Log ( _L ( "running isolation server is failed" ) );
       return KErrGeneral;
        }             
     //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    if(err)
       {
       iLog->Log ( _L ( "creating requet queue is failed" ) );
       return KErrGeneral;
        }
   
    //request formation 
        memset( smsg, '\0', MAX_MSG_SIZE );
               
        //message header use
        msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
        memset( msgHdr, '\0', sizeof( message_hdr_req ) );
        
        msgHdr->message_type = ELogin_Request;
        
        memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
        index += sizeof( message_hdr_req );
        /* Send Message to queueOne */
            
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        
        r1 = MsgQCheck(REQUEST_QUEUE,&err);
        
        result = MsgQClean(REQUEST_QUEUE,&err);
        
        r2 = MsgQCheck(REQUEST_QUEUE,&err);
        
        if(r1==r2)
            {
            iLog->Log ( _L ( "message q clean  is failed" ) );
            return KErrGeneral;
            }
        index=0;
        memset( smsg, '\0', MAX_MSG_SIZE );
        memset( rmsg, '\0', MAX_MSG_SIZE );
        memset( msgHdr, '\0', sizeof( message_hdr_req ) );
        msgHdr->message_type = EKill_Process;
        memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
        index += sizeof( message_hdr_req );
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
        free(msgHdr); 
        result=MsgQDelete( REQUEST_QUEUE, &err );
        if(result)
            {
        iLog->Log ( _L ( "failing at message queue" ) );  
        return KErrGeneral;
            }
       return KErrNone;
        
}

TInt Cmessagequeue_api::TestMessageQClean( CStifItemParser& aItem )
    {
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    int timeout = 100;
    int nBytes;
    int result = 0,r1=0 ,r2=0;
    int err = 0;
    int status = 0;
    int run_iso;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
    memset( smsg, '\0', MAX_MSG_SIZE );
    memset( rmsg, '\0', MAX_MSG_SIZE );
    //running isoserver   
    run_iso = run_isoserver(); 
    if(run_iso)
       {
       iLog->Log ( _L ( "running isolation server is failed" ) );
       return KErrGeneral;
        }             
     //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    if(err)
       {
       iLog->Log ( _L ( "creating requet queue is failed" ) );
       return KErrGeneral;
        }
   
    //request formation 
        memset( smsg, '\0', MAX_MSG_SIZE );
               
        //message header use
        msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
        memset( msgHdr, '\0', sizeof( message_hdr_req ) );
        
        msgHdr->message_type = ELogin_Request;
        
        memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
        index += sizeof( message_hdr_req );
        /* Send Message to queueOne */
            
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        
        r1 = MsgQCheck(REQUEST_QUEUE,&err);
        
        r1 = MsgQMaxCheck(REQUEST_QUEUE,&err);
        
        result = MsgQClean(REQUEST_QUEUE,&err);
        
        r2 = MsgQCheck(REQUEST_QUEUE,&err);
        
        if(r1==r2)
            {
            iLog->Log ( _L ( "message q clean  is failed" ) );
            return KErrGeneral;
            }
        index=0;
        memset( smsg, '\0', MAX_MSG_SIZE );
        memset( rmsg, '\0', MAX_MSG_SIZE );
        memset( msgHdr, '\0', sizeof( message_hdr_req ) );
        msgHdr->message_type = EKill_Process;
        memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
        index += sizeof( message_hdr_req );
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
        free(msgHdr); 
        result=MsgQDelete( REQUEST_QUEUE, &err );
        if(result)
            {
        iLog->Log ( _L ( "failing at message queue" ) );  
        return KErrGeneral;
            }
       return KErrNone;
        
}

TInt Cmessagequeue_api::TestMsgQMaxCheck( CStifItemParser& aItem )
    {
    char smsg[MAX_MSG_SIZE];
    char rmsg[MAX_MSG_SIZE];
    int index=0;
    int timeout = 100;
    int nBytes;
    int result = 0,r1=0 ,r2=0;
    int err = 0;
    int status = 0;
    int run_iso;
    FILE *fp;
    message_hdr_req *msgHdr = NULL;  
    message_hdr_resp* msg_struct = NULL;
    TInt return_value = 0;
    memset( smsg, '\0', MAX_MSG_SIZE );
    memset( rmsg, '\0', MAX_MSG_SIZE );
    //running isoserver   
    run_iso = run_isoserver(); 
    if(run_iso)
       {
       iLog->Log ( _L ( "running isolation server is failed" ) );
       return KErrGeneral;
        }             
     //create message queue for send request
    result = MsgQCreate(REQUEST_QUEUE, MAX_MSG_Q_SIZE , MSG_Q_FIFO, &err);
    if(err)
       {
       iLog->Log ( _L ( "creating requet queue is failed" ) );
       return KErrGeneral;
        }
   
    //request formation 
        memset( smsg, '\0', MAX_MSG_SIZE );
               
        //message header use
        msgHdr = ( message_hdr_req* ) malloc( sizeof( message_hdr_req ) );
        memset( msgHdr, '\0', sizeof( message_hdr_req ) );
        
        msgHdr->message_type = ELogin_Request;
        
        memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
        index += sizeof( message_hdr_req );
        /* Send Message to queueOne */
            
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err);
        
        r1 = MsgQCheck(REQUEST_QUEUE,&err);
        
        r1 = MsgQMaxCheck(REQUEST_QUEUE,&err);
        
        result = MsgQClean(REQUEST_QUEUE,&err);
        
        r2 = MsgQCheck(REQUEST_QUEUE,&err);
        
        if(r1==r2)
            {
            iLog->Log ( _L ( "message q clean  is failed" ) );
            return KErrGeneral;
            }
        index=0;
        memset( smsg, '\0', MAX_MSG_SIZE );
        memset( rmsg, '\0', MAX_MSG_SIZE );
        memset( msgHdr, '\0', sizeof( message_hdr_req ) );
        msgHdr->message_type = EKill_Process;
        memcpy( smsg, msgHdr, sizeof( message_hdr_req ) );
        index += sizeof( message_hdr_req );
        result = MsgQSend(REQUEST_QUEUE, smsg, index, MSG_PRI_NORMAL, timeout, &err); 
        free(msgHdr); 
        result=MsgQDelete( REQUEST_QUEUE, &err );
        if(result)
            {
        iLog->Log ( _L ( "failing at message queue" ) );  
        return KErrGeneral;
            }
       return KErrNone;
        
}

// -----------------------------------------------------------------------------
// Cmessagequeue_api::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cmessagequeue_api::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
