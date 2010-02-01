/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  isolationserver.h
*
*/



#ifndef ISOLATIONSERVER_H
#define ISOLATIONSERVER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <glib.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;


// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KisolationserverLogPath, "\\logs\\testframework\\isolationserver\\" ); 
// Log file
_LIT( KisolationserverLogFile, "isolationserver.txt" ); 
_LIT( KisolationserverLogFileWithTitle, "isolationserver_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Cisolationserver;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Cisolationserver test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cisolationserver) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cisolationserver* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cisolationserver();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

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

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        Cisolationserver( CTestModuleIf& aTestModuleIf );

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
        virtual TInt SecondSearch( CStifItemParser& aItem );
        virtual TInt LoginWithCorrectdata( CStifItemParser& aItem );
        virtual TInt LoginWithWrongPassword( CStifItemParser& aItem );
        virtual TInt LoginWithUnexistUserId( CStifItemParser& aItem );
        virtual TInt LoginWithUnexistServer( CStifItemParser& aItem );
        virtual TInt LoginWithMixedletterUserId( CStifItemParser& aItem );
        virtual TInt LoginWithMaximumLengthUserId( CStifItemParser& aItem );
        virtual TInt LoginWithMinimumLengthUserId( CStifItemParser& aItem );
        virtual TInt LogoutFromServer( CStifItemParser& aItem );
        virtual TInt LoginWithBlankUserId(CStifItemParser& aItem);
        virtual TInt LoginLogOutWithReapitiveSession(CStifItemParser& aItem);
        virtual TInt LoginWithCoreectUserIDWithSpace(CStifItemParser& aItem);
        virtual TInt LoginWithInvalidPortAddress(CStifItemParser& aItem);
        virtual TInt LoginWithMissingSettings(CStifItemParser& aItem);
        virtual TInt LoginWhenTGAndDDAreNotRunning(CStifItemParser& aItem);
        virtual TInt SendMessageToCorrectUserID(CStifItemParser& aItem);
        virtual TInt SendMaxLenMessageToCorrectUserID(CStifItemParser& aItem);
        virtual TInt SendTextMessageUnExistUserID(CStifItemParser& aItem);
        virtual TInt SendTextMessageWithSpecialCharacter(CStifItemParser& aItem);
        virtual TInt SendMessageWithBlankUserID(CStifItemParser& aItem);
        virtual TInt SendBlankMessageToUserID(CStifItemParser& aItem);
        virtual TInt SendSmilyToCorrectUserID(CStifItemParser& aItem);
        virtual TInt SendMessageToOfflineContact(CStifItemParser& aItem);
        virtual TInt SendTextMessageToBlockedUser(CStifItemParser& aItem);
        virtual TInt SendContinuousltReapitiveMessage(CStifItemParser& aItem);
        virtual TInt SendTextMessageToUserIdWhoIsBlockedSender(CStifItemParser& aItem);
        virtual TInt SendLongTextMessageToUserId(CStifItemParser& aItem);
        virtual TInt SendTextMessageUserWhoIsNotAddedInContactList(CStifItemParser& aItem);
        //virtual TInt FetchContactList(CStifItemParser& aItem);
        virtual TInt MessageReceiveFromUser(CStifItemParser& aItem);
        virtual TInt AddExistingContact(CStifItemParser& aItem);
        virtual TInt ClientSendBlankUserIdForADD(CStifItemParser& aItem);
        virtual TInt AddUnexistingUserID(CStifItemParser& aItem);
        virtual TInt AddContactWhichIsAlreadyAdded(CStifItemParser& aItem);
        virtual TInt AddExistingContactWIthSpace(CStifItemParser& aItem);
        virtual TInt AddExistingContactWIthMixedLetter(CStifItemParser& aItem);
        virtual TInt AddInvalidContactWIthSpecialLetter(CStifItemParser& aItem);
        virtual TInt DeleteExistingContact(CStifItemParser& aItem);
        virtual TInt ClientSendBlankUserIdForDELETE(CStifItemParser& aItem);
        virtual TInt DeleteUnexistingUserID(CStifItemParser& aItem);
        virtual TInt DeleteContactWhichIsAlreadyDeleted(CStifItemParser& aItem);
        virtual TInt DeleteExistingContactWIthSpace(CStifItemParser& aItem);
        virtual TInt DeleteExistingContactWIthMixedLetter(CStifItemParser& aItem);
        virtual TInt DeleteInvalidContactWIthSpecialLetter(CStifItemParser& aItem);
        virtual TInt Cisolationserver::FetchContatcs( CStifItemParser& aItem );
        virtual TInt Cisolationserver::AddContatcs( CStifItemParser& aItem );
        virtual TInt UpdatePresenceWithAway(CStifItemParser& aItem);
        virtual TInt UpdatePresenceWithDnd(CStifItemParser& aItem);
        virtual TInt UpdatePresenceWithAvailable(CStifItemParser& aItem);
        virtual TInt UpdatePresenceMultipleTimeInSameSession(CStifItemParser& aItem);
        virtual TInt UpdateOnlyStatusText(CStifItemParser& aItem);
        virtual TInt UpdateOnlyUserAvailability(CStifItemParser& aItem);
        virtual TInt UpdateOwnPresenceWithBlankData(CStifItemParser& aItem);
        virtual TInt UpdateStatusTextWithMaxLen(CStifItemParser& aItem);
        virtual TInt UpdateStatusTextWithSpecialCharacter(CStifItemParser& aItem);
        virtual TInt HandelingPresenceNotification(CStifItemParser& aItem);
        virtual TInt Cisolationserver::SearchKeyValue( CStifItemParser& aItem );
        virtual TInt Cisolationserver::SearchKeyValueWithResult( CStifItemParser& aItem );
        virtual TInt Cisolationserver::SearchKeyValueInvalidParam( CStifItemParser& aItem );
        virtual TInt Cisolationserver::SearchForLargeNoOfResult( CStifItemParser& aItem );
        virtual TInt Cisolationserver::SearchKeyValueInvalidParam2( CStifItemParser& aItem );
        virtual TInt Cisolationserver::SearchKeyValueInvalidHashValues( CStifItemParser& aItem );
        virtual TInt Cisolationserver::SearchKeyValueBlank( CStifItemParser& aItem );
        virtual TInt Cisolationserver::SearchKeyValueSpecialChar( CStifItemParser& aItem );
        virtual TInt Cisolationserver::SearchKeyValueLargeString( CStifItemParser& aItem );
        virtual TInt Cisolationserver::SearchAndAddContact( CStifItemParser& aItem );
        virtual TInt AcceptAddRequestFromUser(CStifItemParser& aItem);
        virtual TInt RejectAddRequestFromUser(CStifItemParser& aItem);
        virtual TInt SendAcceptRequestWithoutReceivingAddrequest(CStifItemParser& aItem);
        virtual TInt SendRejectRequestWithoutReceivingAddrequest(CStifItemParser& aItem);
        virtual TInt SendAcceptRequestWithoutAnyUserID(CStifItemParser& aItem);
        virtual TInt SendRejectRequestWithoutAnyUserID(CStifItemParser& aItem);
        virtual TInt LogoutWhileUserIsNotLoggedIn(CStifItemParser& aItem);
        virtual TInt GetKeysAndSearch( CStifItemParser& aItem );
        virtual TInt SetOwnAvatar( CStifItemParser& aItem );
        virtual TInt ClearOwnAvatar( CStifItemParser& aItem );
        virtual TInt FetchCachedContacts( CStifItemParser& aItem );

//ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

	public:
	
		const gchar* GetSearchLabelForGizmoL(gchar* search_key);
        const gchar* GetSearchLabelForYukonL(gchar* search_key);
        const gchar* GetSearchLabelL(gchar* aService,gchar* aSearchKey);        

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // ISOLATIONSERVER_H

// End of File
