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
* Description:    For Stif fw related functions.
*
*/





#ifndef TELEPATHYGABBLETEST_H
#define TELEPATHYGABBLETEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <glib.h>



// Logging path
_LIT( KTelepathyGabbleTestLogPath, "\\logs\\testframework\\TelepathyGabbleTest\\" ); 
// Log file
_LIT( KTelepathyGabbleTestLogFile, "TelepathyGabbleTest.txt" ); 
_LIT( KTelepathyGabbleTestLogFileWithTitle, "TelepathyGabbleTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class CTelepathyGabbleTest;

class CTelepathyGabbleLoginTest;
class CTelepathyGabbleSendRecvMessage;
class CTelepathyGabbleAddDeleteContact;
class CTelepathyGabbleFetchContacts;
class CTelepathyGabbleSearch;
class CTestData;


// CLASS DECLARATION

/**
*  CTelepathyGabbleTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CTelepathyGabbleTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTelepathyGabbleTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTelepathyGabbleTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes


    private:

        /**
        * C++ default constructor.
        */
        CTelepathyGabbleTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        //virtual TInt ExampleL( CStifItemParser& aItem );
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
        
        
        //Login
        virtual TInt LoginTestL( CStifItemParser& aItem );
        
        //Logout
        virtual TInt LogoutTestL(CStifItemParser& aItem );   
        
        //Login related other test cases     
        
        virtual TInt LoginInvalidUseridTestL( CStifItemParser& aItem  );
		virtual TInt LoginInvalidPasswdTestL( CStifItemParser& aItem  );
		virtual TInt LoginInvalidUseridPasswdTestL( CStifItemParser& aItem  );
		virtual TInt LoginblankUseridTestL(	CStifItemParser& aItem  ) ;    											
		virtual TInt LoginblankPasswdTestL( CStifItemParser& aItem  );
		virtual TInt LoginblankUseridblankPasswdTestL( CStifItemParser& aItem  );
		virtual TInt LoginCancelTestL( CStifItemParser& aItem  );

   
        
        //Send Message
        virtual TInt SendMessageTestL(CStifItemParser& aItem );
        virtual TInt SendMessageWith400TestL(CStifItemParser& aItem  );
        virtual TInt SendMesasgeBlankTestL( CStifItemParser& aItem  );
        virtual TInt SendMesasgeToBlankUserIdTestL( CStifItemParser& aItem  );        
        virtual TInt SendMessageRepeatedTestL( CStifItemParser& aItem  );
        virtual TInt SendMessagetoinvalidUseridTestL( CStifItemParser& aItem  );
        
        
        //Receive Message
        virtual TInt ReceiveMessageTestL(CStifItemParser& aItem);
        virtual TInt ReceiveMessageMaxLengthTestL(CStifItemParser& aItem);
        virtual TInt ReceiveMessageBlankTestL(CStifItemParser& aItem);
        virtual TInt ReceiveRepeatedMessageTestL(CStifItemParser& aItem);
        
        
        //Add Contact related test cases
        virtual TInt AddContactTestL(CStifItemParser& aItem);
        virtual TInt AddContactWithBlankUserIdTestL(CStifItemParser& aItem);
        virtual TInt AddContactAlreadyInRosterTestL(CStifItemParser& aItem);
        
        //Remove Contact related test cases
        virtual TInt RemoveContactTestL(CStifItemParser& aItem  );
        virtual TInt RemoveInvalidContactTestL( CStifItemParser& aItem  );
        virtual TInt RemoveContactAlreadyRemovedTestL( CStifItemParser& aItem  );
        virtual TInt RemoveContactWithBlankUserIdTestL( CStifItemParser& aItem  );     
    
        //Fetch Contact related test cases
        virtual TInt FetchContactsTestL(CStifItemParser& aItem );
        
        //Search related test cases
        virtual TInt SearchTestL(CStifItemParser& aItem );        
        virtual TInt GetSearchKeysTestL(CStifItemParser& aItem );
        virtual TInt SearchWithAllKeysTestL( CStifItemParser& aItem  );
        
        virtual TInt SearchTwoInvalidKeysTestL( CStifItemParser& aItem  );
        virtual TInt SearchInvalidKeyTestL(CStifItemParser& aItem );
        virtual TInt SearchNoKeyValueTestL(CStifItemParser& aItem );
        virtual TInt SearchBlankStringTestL(CStifItemParser& aItem );
        virtual TInt SearchAfterGetSearchKeysL( CStifItemParser& aItem  );
        virtual TInt SearchTwiceTestL( CStifItemParser& aItem  );
        virtual TInt SearchFiveTimesTestL( CStifItemParser& aItem  );
        virtual TInt SearchTwiceWithDiffStringsTestL( CStifItemParser& aItem  );
        
        
        gchar* GetSearchLabelForGizmoL(gchar* search_key);
        gchar* GetSearchLabelForYukonL(gchar* search_key);
        gchar* GetSearchLabelL(gchar* aService,gchar* aSearchKey);        
        /*static gboolean search_hash_remove( gpointer key, gpointer value,
                      				 gpointer user_data );*/ 
        
        
     private: //Some data members
     
	 	//Login class object
	 	CTelepathyGabbleLoginTest	*iLoginObj;
	 	//Send Message class object
	 	CTelepathyGabbleSendRecvMessage *iSendMsgObj;
	 	
	 	//used for adding/deleting contact
	 	CTelepathyGabbleAddDeleteContact *iAddDelContactObj;
	 	
	 	//used for fetching contacts
	 	CTelepathyGabbleFetchContacts *iFetchContactObj;
	 	
	 	//used for searching contacts
	 	CTelepathyGabbleSearch *iSearchObj;
	 	CTestData* iTestData;

 };

#endif      // TELEPATHYGABBLETEST_H

// End of File

