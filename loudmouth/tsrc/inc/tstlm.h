/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   tstlm.h
*
*/




#ifndef TSTLM_H
#define TSTLM_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KtstlmLogPath, "\\logs\\testframework\\tstlm\\" ); 
// Log file
_LIT( KtstlmLogFile, "tstlm.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctstlm;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctstlm test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctstlm) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctstlm* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctstlm();

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
        Ctstlm( CTestModuleIf& aTestModuleIf );

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
        
        // LmConnection
        virtual TInt lm_connection_newL( CStifItemParser& aItem );
        virtual TInt lm_new_with_contextL( CStifItemParser& aItem );
        virtual TInt lm_connection_openL( CStifItemParser& aItem );
        virtual TInt lm_connection_closeL ( CStifItemParser& aItem );
        virtual TInt lm_authenticateL ( CStifItemParser& aItem );
        virtual TInt lm_connection_is_openL ( CStifItemParser& aItem );
        virtual TInt lm_is_authenticatedL ( CStifItemParser& aItem );
        virtual TInt lm_connection_set_get_serverL ( CStifItemParser& aItem );        
        virtual TInt lm_connection_set_get_jidL ( CStifItemParser& aItem );
        virtual TInt lm_connection_set_get_portL ( CStifItemParser& aItem );
        virtual TInt lm_connection_set_get_sslL ( CStifItemParser& aItem );
        virtual TInt lm_connection_set_get_proxyL ( CStifItemParser& aItem );
        
        virtual TInt lm_connection_sendL(CStifItemParser& aItem );
        virtual TInt lm_connection_send400charL(CStifItemParser& aItem );
        virtual TInt lm_connection_send_repeatedL(CStifItemParser& aItem );
        
        
        virtual TInt lm_send_with_replyL ( CStifItemParser& aItem );
        virtual TInt register_message_handlerL ( CStifItemParser& aItem );
        virtual TInt set_disconnect_functionL ( CStifItemParser& aItem );
        virtual TInt lm_connection_send_rawL ( CStifItemParser& aItem );
        virtual TInt lm_connection_get_stateL ( CStifItemParser& aItem );
        virtual TInt lm_connection_refL ( CStifItemParser& aItem );
        virtual TInt lm_connection_unrefL ( CStifItemParser& aItem );
        
        virtual TInt lm_connection_is_open_WhenConnectionIsClosedL ( CStifItemParser& aItem );
        virtual TInt lm_is_authenticated_WhenNotAuthenticatedL ( CStifItemParser& aItem );
        virtual TInt lm_connection_set_server_WithNULL ( CStifItemParser& aItem );
        virtual TInt lm_connection_set_jid_WithNULL ( CStifItemParser& aItem );
        virtual TInt lm_connection_set_port_WithNULL ( CStifItemParser& aItem );
        virtual TInt lm_connection_set_ssl_WithNULL ( CStifItemParser& aItem );
        virtual TInt lm_connection_closeWhenNotOpenedL ( CStifItemParser& aItem );
        // LmMessage
        virtual TInt lm_message_newL ( CStifItemParser& aItem );
        virtual TInt lm_message_new_with_sub_typeL ( CStifItemParser& aItem );
        virtual TInt lm_message_get_typeL ( CStifItemParser& aItem );
        virtual TInt lm_message_get_sub_typeL ( CStifItemParser& aItem );
        virtual TInt lm_message_get_nodeL ( CStifItemParser& aItem );
        virtual TInt lm_message_refL ( CStifItemParser& aItem );
        virtual TInt lm_message_unrefL ( CStifItemParser& aItem );
        
        // LmMessageHanlder
        virtual TInt lm_message_handler_newL ( CStifItemParser& aItem );
        virtual TInt lm_message_handler_invalidateL ( CStifItemParser& aItem );
        virtual TInt lm_message_handler_is_validL ( CStifItemParser& aItem );
        virtual TInt lm_message_handler_refL ( CStifItemParser& aItem );
        virtual TInt lm_message_handler_unrefL ( CStifItemParser& aItem );
        
        // LmMessageNode
        
        virtual TInt lm_message_node_set_get_valueL ( CStifItemParser& aItem ); 
        virtual TInt lm_message_node_add_childL ( CStifItemParser& aItem ); 
        virtual TInt lm_message_node_get_childL ( CStifItemParser& aItem ); 
        virtual TInt lm_message_node_find_childL ( CStifItemParser& aItem );         
        virtual TInt lm_message_node_set_get_attributesL ( CStifItemParser& aItem );
             
        
        virtual TInt lm_message_node_set_get_raw_modeL ( CStifItemParser& aItem );        
        virtual TInt lm_message_node_refL ( CStifItemParser& aItem );
        virtual TInt lm_message_node_unrefL ( CStifItemParser& aItem );        
        virtual TInt lm_message_node_to_stringL ( CStifItemParser& aItem );
        
        // LmSSL
        virtual TInt lm_ssl_newL( CStifItemParser& aItem );
        virtual TInt lm_ssl_is_supportedL( CStifItemParser& aItem );
        virtual TInt lm_ssl_get_fingerprintL( CStifItemParser& aItem );        
        virtual TInt lm_ssl_refL( CStifItemParser& aItem );
        virtual TInt lm_ssl_unrefL( CStifItemParser& aItem );
        
        // LmProxy
        virtual TInt lm_proxy_newL( CStifItemParser& aItem );
        virtual TInt lm_proxy_new_with_serverL( CStifItemParser& aItem );        
        virtual TInt lm_proxy_set_get_typeL( CStifItemParser& aItem );        
        virtual TInt lm_proxy_set_get_serverL( CStifItemParser& aItem );        
        virtual TInt lm_proxy_set_get_portL( CStifItemParser& aItem );        
        virtual TInt lm_proxy_set_get_usernameL( CStifItemParser& aItem );        
        virtual TInt lm_proxy_set_get_passwordL( CStifItemParser& aItem );        
        virtual TInt lm_proxy_refL( CStifItemParser& aItem );
        virtual TInt lm_proxy_unrefL( CStifItemParser& aItem );
        
        // lm-send-sync example        
        virtual TInt lm_send_syncL ( CStifItemParser& aItem );
        
        //fetching contact list
        
        virtual TInt lm_fetching_contactlist_L( CStifItemParser& aItem );
        
        virtual TInt lm_fetching_presenceL( CStifItemParser& aItem );
        
        // subscribe presence for contact
        
        virtual TInt lm_subscribe_contact_L( CStifItemParser& aItem );
        
        
        virtual TInt lm_authenticate_WithUnexistingJID( CStifItemParser& aItem );
        virtual TInt lm_authenticate_WithBadPasswordL( CStifItemParser& aItem );
        virtual TInt lm_authenticate_WhenConnectionIsNotOpenedL( CStifItemParser& aItem );
        virtual TInt lm_connection_open_with_badserverL(CStifItemParser& aItem); 
        
        virtual TInt lm_connection_send_receiveL(CStifItemParser& aItem);
        virtual TInt lm_login_testL(CStifItemParser& aItem);
        
        virtual TInt lm_add_contactL(CStifItemParser& aItem);
        virtual TInt lm_remove_contactL(CStifItemParser& aItem);
        virtual TInt lm_connection_cancel_openL(CStifItemParser& aItem);
        virtual TInt lm_connection_get_privacy_listsL(CStifItemParser& aItem);
        virtual TInt lm_connection_set_privacy_listsL(CStifItemParser& aItem);
        virtual TInt lm_connection_send_image_bytestream_msgL(CStifItemParser& aItem);
        virtual TInt lm_connection_send_image_bytestream_iqL(CStifItemParser& aItem);
        virtual TInt lm_connection_send_multiple_connL(CStifItemParser& aItem);
        virtual TInt lm_connection_block_contactL(CStifItemParser& aItem);
        virtual TInt lm_connection_unblock_contactL(CStifItemParser& aItem);
        virtual TInt lm_connection_set_active_listL(CStifItemParser& aItem);
        virtual TInt lm_connection_set_unexist_active_listL(CStifItemParser& aItem);
        virtual TInt lm_connection_set_decline_active_listL(CStifItemParser& aItem);
        virtual TInt lm_connection_get_one_privacy_listL(CStifItemParser& aItem);
        virtual TInt lm_connection_set_one_privacy_listL(CStifItemParser& aItem);
        virtual TInt lm_connection_get_many_privacy_list_from_listnameL(CStifItemParser& aItem);
        virtual TInt lm_connection_set_one_unexist_privacy_listL(CStifItemParser& aItem);
        virtual TInt lm_connection_get_one_unexist_privacy_listL(CStifItemParser& aItem);
        virtual TInt lm_connection_create_one_privacy_listL(CStifItemParser& aItem);
        virtual TInt lm_service_discoveryL(CStifItemParser& aItem);
        virtual TInt lm_service_discovery_connected_resourceL(CStifItemParser& aItem);
        virtual TInt lm_simple_communications_blockingL(CStifItemParser& aItem);
        virtual TInt lm_send_im_with_replyL(CStifItemParser& aItem);
        virtual TInt lm_receive_any_messageL(CStifItemParser& aItem);
        virtual TInt lm_login_multiple_sessionsL(CStifItemParser& aItem);
        virtual TInt lm_change_own_presenceL(CStifItemParser& aItem);
        virtual TInt  lm_login_test_invalid_serverL(CStifItemParser& aItem);
        virtual TInt  lm_login_test_invalid_portL(CStifItemParser& aItem);
        
        virtual TInt lm_memory_leak_testL(CStifItemParser& aItem);
        virtual TInt lm_nft_updating_presenceL(CStifItemParser& aItem);
        virtual TInt lm_nft_presence_notificationL(CStifItemParser& aItem);
        virtual TInt lm_nft_send_text_messageL(CStifItemParser& aItem);
        virtual TInt lm_nft_open_conversation_with_multipleL(CStifItemParser& aItem);
        virtual TInt lm_nft_fetch300_contactL(CStifItemParser& aItem);
        virtual TInt lm_nft_loginlogoutmultipletime(CStifItemParser& aItem);
        virtual TInt lm_nft_receiving100_message_L(CStifItemParser& aItem);
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

#endif      // TSTLM_H

// End of File
