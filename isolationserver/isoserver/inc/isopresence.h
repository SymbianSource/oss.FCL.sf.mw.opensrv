/* ============================================================================
*  Name        : isopresence.h
*  Part of     : isolation server.
*  Version     : %version: 7 %
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

/*!
* /def To avoid multiple inclusion of header
*/

#ifndef __ISOPRESENCE_H__
#define __ISOPRESENCE_H__
#include <glib.h>
#include <gtypes.h>

#include "isoservermain.h"
#include "msg_enums.h"

#include "tp-conn.h"
#include "tp-connmgr.h"
#include "tp-props-iface.h"
#include "tp-interfaces.h"
#include "tp-conn-iface-presence-gen.h"

G_BEGIN_DECLS

/*! /file
*	This file has interfaces for presence implimentation. 
*	client anywhere in the documentation refers to anyone
*	using the isoserver
*/

/*! /typedef struct presence_msg_struct typedefed to presence_msg_struct 
 */
typedef struct presence_msg_struct presence_msg_struct;

/*! /struct presence_msg_struct isopresence
 *  /brief 
 *  
 *  /var hdr_req header request
 *  /var useravailability availability, presence 
 *  /var statustext status text 
 */
struct presence_msg_struct
	{
	message_hdr_req* hdr_req;
	char *useravailability;
	char *statustext; 
	};


#define TELEPATHY_TYPE_PRESENCE         (telepathy_presence_get_type ())
#define TELEPATHY_PRESENCE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TELEPATHY_TYPE_PRESENCE, TelepathyPresence))
#define TELEPATHY_PRESENCE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), TELEPATHY_TYPE_PRESENCE, TelepathyPresenceClass))
#define TELEPATHY_IS_PRESENCE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TELEPATHY_TYPE_PRESENCE))
#define TELEPATHY_IS_PRESENCE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TELEPATHY_TYPE_PRESENCE))
#define TELEPATHY_PRESENCE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TELEPATHY_TYPE_PRESENCE, TelepathyPresenceClass))

/*! /typedef struct _TelepathyPresence typedefed to TelepathyPresence 
 */
typedef struct _TelepathyPresence      TelepathyPresence;

/*! /typedef struct _TelepathyPresenceClass typedefed to TelepathyPresenceClass 
 */
typedef struct _TelepathyPresenceClass TelepathyPresenceClass;

/*! /struct _TelepathyPresence isopresence.h
 *  /var parent 
 */
struct _TelepathyPresence {
	GObject parent;
};

/*! /struct _TelepathyPresenceClass isopresence.h
 *  /var parent_class 
 */
struct _TelepathyPresenceClass {
	GObjectClass parent_class;
};

/*! /typedef struct _TelepathyPresenceClass typedefed to TelepathyPresenceClass 
 */
 
/*! /enum presence states of the user
 */
typedef enum {
	TELEPATHY_PRESENCE_STATE_AVAILABLE,
	TELEPATHY_PRESENCE_STATE_BUSY,
	TELEPATHY_PRESENCE_STATE_AWAY,
	TELEPATHY_PRESENCE_STATE_EXT_AWAY,
	TELEPATHY_PRESENCE_STATE_HIDDEN,      /* When you appear offline to others */
	TELEPATHY_PRESENCE_STATE_UNAVAILABLE,
} TelepathyPresenceState;

GType               telepathy_presence_get_type                 (void) G_GNUC_CONST;

/*!	/brief create a presence object instance
 * 	/return created presence object instance
 */
TelepathyPresence *
telepathy_presence_new (void);

/*!	/brief get the presence state value
 *
 * 	/param presence : presence object
 * 	/return state : state as set in presence object
 */
TelepathyPresenceState 
telepathy_presence_get_state (TelepathyPresence      *presence);


/*!	/brief get the presence status value
 *
 * 	/param presence : presence object
 * 	/return state : status as set in presence object
 */
const gchar *
telepathy_presence_get_status (TelepathyPresence      *presence);

/*!	/brief set the presence state value in presence gobject
 *
 * 	/param presence : presence object
 * 	/param state : state
 * 	/return : void
 */
void
telepathy_presence_set_state (TelepathyPresence      *presence,
				TelepathyPresenceState  state);

/*!	/brief set the presence status text value in presence gobject
 *
 * 	/param presence : presence object
 * 	/param state : status
 * 	/return : void
 */
void
telepathy_presence_set_status (TelepathyPresence      *presence,
				const gchar         *status);

/*!	/brief converts presence state value from enum to string
 *	/param presence_state : presence state enum value
 * 	/return : presence state string value
 */
const gchar *
telepathy_presence_state_to_str (TelepathyPresenceState presence_state);


/*!	/brief converts presence state value from string to enum
 *
 * 	/param str : presence state string value
 * 	/return : presence state enum value
 */
TelepathyPresenceState 
telepathy_telepathy_presence_state_from_str (const gchar *str);

/*!	/brief Registered as a signal for presence updates of 
 *	other users. This function will be called whenevr a contact
 *	to whom this user has subscribed for presence changes his 
 *	presence state or status message.
 * 
 * 	/param proxy : unused
 * 	/param handle_hash : hash table containing presence information
 *	as hash value, contact handle as key
 * 	/return : void
 */
void
telepathy_presence_handler(DBusGProxy      *proxy,
			   GHashTable      *handle_hash);

/*!	/brief updates own presence to the n/w server. update_status_cb 
 * 	is registered for the callback
 *
 * 	/param pres_iface : presence interface
 * 	/param presence : presence object 
 * 	/param msg_hdr : presence info list to send presence_msg_struct
 *	has state, status etc., 
 * 	/return : void
 */
void 
telepathy_contacts_send_presence (DBusGProxy *pres_iface, 
				TelepathyPresence *presence, 
				presence_msg_struct* msg_hdr );
					 
/*!	/brief This function will be called for presence of each contact
 *	key has got the state and value status
 *
 *	/param key : hash table key ie presence state
 * 	/param value : hash table value ie presence information
 * 	/param presence : user data
 *	/remark presence object is logically unused ?
 *
 * 	/return : void
 */
static void
telepathy_presences_foreach (gpointer        *key,
		      gpointer        *value,
		      TelepathyPresence **presence);
					 

/*!	/brief This function will be called for each contact. Contact name is 
 *	got from the contcat handle, telepathy_presences_foreach is called 
 *	to get the presence from value of the hash table
 *
 * @param key : hash table key ie contact handle id
 * @param value : hash table value ie presence
 * @return : void
 */
static void
telepathy_presence_handle_foreach (gpointer *key,
		      gpointer *value);


/*!	/brief callback for updating own presence. This function 
 *	sends response to client
 *
 * 	/param proxy : unused
 * 	/param error : error if any
 * 	/param message : message header  
 * 	/return : void
 */
void 
update_status_cb(DBusGProxy *proxy, 
			GError* error, 
			gpointer message);


/*!	/brief Sends presence of conatct to the client. Forms 
 *	the proper presence message format and sends that to client
 *
 * 	/param : availability user availability
 * 	/param : text custom status message set by user
 * 	/return : error code on failure, 0 on success
 */
int 
send_presence_to_client( const gchar *availability, 
			const gchar *text );

/*!	/brief sends presence of the client to n/w server thru' gabble
 * 	calls telepathy_contacts_send_presence to send presence
 *
 * 	/param msghdr request header that will be passed back to client
 * 	/param message_type_err unused
 *	/remark please remove this unused variable(why was this added?)
 *
 * 	/return : error code on failure, 0 on success
 */
void
send_presence( presence_msg_struct* msg_hdr, 
			gint message_type_err ) ;


/*!	/brief parses the message buffer. aPresenceStatus and aStatustext
 *	pointers updated
 *
 * 	/param aMsgBuffer message buffer
 * 	/param aPresenceStatus after call to this function it will
 *	have the presence status to be updated
 * 	/param aStatustext after call to this function it will
 *	have the status text to be updated
 * 
 * 	/param msg_len : msg_len no. of bytes in msg_buf
 * 	/return returns error code on failure, or 0
 */
int 
parse_for_presence( gchar* aMsgBuffer, 
			gchar** aPresenceStatus, 
			gchar** aStatustext, 
			gint msg_len ) ;

/*!	/brief Parses the message from client and sends the presence 
 *	to n/w server. A err is returned if there is any parse error
 *	or out of memory condition
 *
 * 	/param buf : buffer to be parsed
 * 	/param buf_len : buffer length
 * 	/return : error code if any, 0 on success 
 */
int 
action_parse_presence( gchar* buf, 
			gint buf_len ) ;

/*!	/brief GObject finalize funtion
*
*	/param object object to be finalized
*/
static void         presence_finalize           (GObject             *object);

/*!	/brief GObject get property funtion
*
*	/param object Gobject
*	/param param_id
*	/param value
*	/param pspec
*/
static void         presence_get_property       (GObject             *object,
						 guint                param_id,
						 GValue              *value,
						 GParamSpec          *pspec);

/*!	/brief GObject set property funtion
*
*	/param object Gobject
*	/param param_id
*	/param value
*	/param pspec
*/						 
static void         presence_set_property       (GObject             *object,
						 guint                param_id,
						 const GValue        *value,
						 GParamSpec          *pspec);

G_END_DECLS


#endif //__ISOPRESENCE_H__