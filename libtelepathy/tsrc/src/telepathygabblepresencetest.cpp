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
* Description:    Used for Presence Test Cases
*
*/



 
// INCLUDE FILES
#include "telepathygabblepresencetest.h"
#include "telepathygabbletestdata.h"

//-----------------------------------------------------------------------------
// function_name	: CTestData
// description     	: Constructor
// Returns          : None
//-----------------------------------------------------------------------------

CTelepathyGabblePresenceTest::CTelepathyGabblePresenceTest()
    {
    }
    
//-----------------------------------------------------------------------------
// function_name	: ConstructL
// description     	: Constructor
// Returns          : None
//-----------------------------------------------------------------------------

void CTelepathyGabblePresenceTest::ConstructL()
	{
 	
	}

//-----------------------------------------------------------------------------
// function_name	: NewL
// description     	: Constructor
// Returns          : None
//-----------------------------------------------------------------------------
CTelepathyGabblePresenceTest* CTelepathyGabblePresenceTest::NewL()
    {
    
	CTelepathyGabblePresenceTest* self = new(ELeave) CTelepathyGabblePresenceTest;
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
// function_name	: ~CTelepathyGabblePresenceTest
// description     	: destructor
// Returns          : None
//-----------------------------------------------------------------------------
CTelepathyGabblePresenceTest::~CTelepathyGabblePresenceTest()
{
	
}


//-----------------------------------------------------------------------------
// function_name	: ~CTestData
// description     	: destructor
// Returns          : None
//-----------------------------------------------------------------------------
TelepathyPresenceState CTelepathyGabblePresenceTest::telepathy_presence_state_from_str (const gchar *str)
{
	char temp[25];
	strcpy(temp,str);
	//roster_log_handler("telepathy_telepathy_presence_state_from_str");
	//roster_log_handler(temp);

	if (strcmp (str, "available") == 0) {
		return TELEPATHY_PRESENCE_STATE_AVAILABLE;
	} else if ((strcmp (str, "dnd") == 0) || (strcmp (str, "busy") == 0)) {
		return TELEPATHY_PRESENCE_STATE_BUSY;
	} else if ((strcmp (str, "away") == 0) || (strcmp (str, "brb") == 0)) {
		return TELEPATHY_PRESENCE_STATE_AWAY;
	} else if (strcmp (str, "xa") == 0) {
		return TELEPATHY_PRESENCE_STATE_EXT_AWAY;
	} else if (strcmp (str, "hidden") == 0) {
		return TELEPATHY_PRESENCE_STATE_HIDDEN;
	} else if (strcmp (str, "offline") == 0) {
		return TELEPATHY_PRESENCE_STATE_UNAVAILABLE;
	} 

	return TELEPATHY_PRESENCE_STATE_AVAILABLE;
}

//-----------------------------------------------------------------------------
// function_name	: ~CTestData
// description     	: destructor
// Returns          : None
//-----------------------------------------------------------------------------
//vinod: gossip_telepathy_contacts_send_presence
void
CTelepathyGabblePresenceTest::telepathy_contacts_send_presence (DBusGProxy *pres_iface,
					 TelepathyPresence *presence/*CTestData *aTestData*/)
{
	GHashTable          *status_ids;
	GHashTable          *status_options;
	const gchar         *status_id;
	const gchar         *message;
	TelepathyPresenceState  presence_state;
	GError              *error = NULL;
	GValue               value_message = {0, };
	
	//iTpConn = aTestData->GetTpConn();

	//g_return_if_fail (contacts != NULL);
	//g_return_if_fail (GOSSIP_IS_PRESENCE (presence));

	if (!pres_iface) {
		return;
	}

	status_ids = g_hash_table_new_full (g_str_hash,
					    g_str_equal,
					    g_free,
					    (GDestroyNotify) g_hash_table_destroy);
	status_options = g_hash_table_new_full (g_str_hash,
						g_str_equal,
						NULL,
						(GDestroyNotify) g_value_unset);

	presence_state = telepathy_presence_get_state (presence);
	status_id = telepathy_presence_state_to_str (presence_state);
	message = telepathy_presence_get_status (presence);

	if (message) {
		g_value_init (&value_message, G_TYPE_STRING);
		g_value_set_string (&value_message, message);
		g_hash_table_insert (status_options, (gpointer)"message", &value_message);
	}

	g_hash_table_insert (status_ids,
			     g_strdup (status_id),
			     status_options);

	//Sync way: gossip_debug (DEBUG_DOMAIN, "sending presence...");
	if (! (DBUS_G_PROXY(pres_iface),
						status_ids,
						&error)) {
		//gossip_debug ("Could not set presence: %s", error->message);
		//roster_log_handler("Could not set presence\n");
		g_clear_error (&error);
	}
	//Async way
/*tp_conn_iface_presence_set_status_async(DBUS_G_PROXY(pres_iface),
						status_ids,
						,
						NULL );
*/
	g_hash_table_destroy (status_ids);
}

//-----------------------------------------------------------------------------
// function_name	: telepathy_presences_foreach
// description     	: destructor
// Returns          : None
//-----------------------------------------------------------------------------
void
CTelepathyGabblePresenceTest::telepathy_presences_foreach (gpointer *key,
		      gpointer        *value,
		      TelepathyPresence **presence)
{
	TelepathyPresenceState state;
	const gchar        *message;
	char temp[100];

	*presence = telepathy_presence_new();

	//roster_log_handler("telepathy_presences_foreach");
//	roster_log_handler(key);

	state = telepathy_presence_state_from_str((const gchar *)key);

	telepathy_presence_set_state(*presence, state);

	message = g_value_get_string((GValue *)
				     g_hash_table_lookup((GHashTable *)
							 value, "message"));
	strcpy(temp,message);
	//roster_log_handler(temp);
		
}

//-----------------------------------------------------------------------------
// function_name	: telepathy_presence_handle_foreach
// description     	: destructor
// Returns          : None
//-----------------------------------------------------------------------------
void
CTelepathyGabblePresenceTest::telepathy_presence_handle_foreach (gpointer *key,
		      gpointer        *value)
{
	gint                 handle_id;
	GHashTable          *presences_hash;
	GValue              *gvalue;
	gboolean             new_item;
	GObject		       *contact;
	GObject      *presence;
	gchar              **handle_name;
	GArray              *handles;

	CTestData* atestData;

	handle_id = GPOINTER_TO_INT(key);
	
	handles = g_array_new (FALSE, FALSE, sizeof (gint));
	g_array_append_val (handles, handle_id);
	tp_conn_inspect_handles(DBUS_G_PROXY(atestData->GetTpConn()),
			       TP_CONN_HANDLE_TYPE_CONTACT, handles,
			       &handle_name, NULL);
	
	gvalue = g_value_array_get_nth((GValueArray *) value, 1);
	
	presences_hash = (GHashTable *) g_value_get_boxed(gvalue);
	
	g_hash_table_foreach(presences_hash,
			     (GHFunc) telepathy_presences_foreach, &presence);
}

//-----------------------------------------------------------------------------
// function_name	: ~CTestData
// description     	: destructor
// Returns          : None
//-----------------------------------------------------------------------------
void CTelepathyGabblePresenceTest::telepathy_presence_handler(DBusGProxy      *proxy,
			   GHashTable      *handle_hash)
{
	//GObject      *presence;
	TelepathyPresence        *presence;
	TelepathyPresenceState state;
	GSList *data = NULL;
	//vinod: setting the new presence status
	presence = telepathy_presence_new();
	//here am hard coding the value, this can be taken from ximpfw
	state = telepathy_presence_state_from_str("dnd");   
	telepathy_presence_set_state(presence, state);
	//vinod
	//roster_log_handler("telepathy_presence_handler callback\n");
	g_hash_table_foreach(handle_hash, (GHFunc) telepathy_presence_handle_foreach, data);


 //vinod: call telepathy_set_presence method here 
 telepathy_contacts_send_presence(proxy, presence);

}

//  End of File
