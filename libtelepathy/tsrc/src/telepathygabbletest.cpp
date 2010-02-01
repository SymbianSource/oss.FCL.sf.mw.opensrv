/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    For Stiff framework related functions
*
*/





// INCLUDE FILES
#include <StifTestInterface.h>
#include "telepathygabbletest.h"
#include <SettingServerClient.h>




// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::CTelepathyGabbleTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTelepathyGabbleTest::CTelepathyGabbleTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTelepathyGabbleTest::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();
    TFileName logFileName;
        
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KTelepathyGabbleTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KTelepathyGabbleTestLogFile);
        }
    iLog = CStifLogger::NewL( KTelepathyGabbleTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );  
	iLog->Log( _L("CTelepathyGabbleTest::Constructl after CStifLogger::NewL") );   
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksMem );    
    
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests  );
	iLog->Log( _L("CTelepathyGabbleTest::Constructl out") );   
	                               

    }

// -----------------------------------------------------------------------------
// CTelepathyGabbleTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTelepathyGabbleTest* CTelepathyGabbleTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CTelepathyGabbleTest* self = new (ELeave) CTelepathyGabbleTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CTelepathyGabbleTest::~CTelepathyGabbleTest()
    { 
     iLog->Log( _L("CTelepathyGabbleTest::~CTelepathyGabbleTest in") ); 
    // Delete resources allocated from test methods
    Delete();

    iLog->Log( _L("CTelepathyGabbleTest::~CTelepathyGabbleTest out") ); 
    // Delete logger
    delete iLog; 

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CTelepathyGabbleTest::NewL( aTestModuleIf );

    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason code
    {
    return(KErrNone);

    }
#endif // EKA2
//  End of File
