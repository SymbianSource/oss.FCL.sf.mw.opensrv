/*
* ============================================================================
*  Name        : isoconnectionmanager.c
*  Part of     : isolation server.
*  Version     : %version: 6 %
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


#include "tp-conn.h"
#include "isoavtar.h"
#include "stdio.h"
#include "isoservermain.h"
#include "msgliterals.h"
#include "msgqlib.h"
#include "msg_enums.h"
#include "isoutils.h"


#include <string.h>
#include <stdlib.h>

/*
 * !	\file 
 *	Implements the functions in isoavtar.h
 */


/*! \brief callback for updating own avtar. This function 
 *  sends response to client
 *
 *  \param proxy : unused
 *  \param avatar_sha1: token string 
 *  \param error : error if any
 *  \param message : message header  
 *  \return : void
 */
void update_avtar_cb(DBusGProxy *proxy, char* avatar_sha1, GError* error, gpointer userdata)
    {

    int err = 0;
    //user data is of type message_hdr_req
    message_hdr_req* msg_hdr = ( message_hdr_req* ) userdata;
    // create the msg queue

    iso_logger( "%s", "In - update_avtar_cb\n" );
    UNUSED_FORMAL_PARAM(proxy);

    //msg_hdr should never be NULL

    if ( NULL != error ) 
        {
        //There was some error
        //send the response for the msg_hdr request to client

        iso_logger( "%s", "error in update_avtar_cb" );
        err = send_response_to_client( msg_hdr, error->code, 0 );
        g_error_free(error);
        }
    else 
        {
        //If the set avatar was successful then send the msg_hdr alongwith the avatar sha1 to client
        //<todo> : sending the avatar sha1

        iso_logger( "%s %s", "avatar sha1 is : ", avatar_sha1 );
        err = send_response_to_client( msg_hdr, 0, 1 );
        free ( avatar_sha1 );
        }

    if ( err < 0 )
        {
        // failed to deliver
        iso_logger( "%s", "failed to deliver\n" );
        }
    free( msg_hdr );

    iso_logger( "%s", "Out - update_avtar_cb\n" );

    }

/*! \brief sends avtar of the client to n/w server thru' gabble
 *  calls tp_conn_iface_avatars_set_avatar_async to set avatar
 *  
 *  \param avtarcontent : avatar image content
 *  \param mimetype : mime type
 *  \param msghdr request header that will be passed back to client
 *
 *  \return : error code on failure, 0 on success
 */
gint send_avtar( GArray* avtarcontent , char * mimetype , message_hdr_req *msg_hdr ) 
    {

    gint error = 0;
    DBusGProxy* avatar_face = NULL;
    message_hdr_req *hdr_req = NULL;
    iso_logger( "%s", "In - send_avtar\n" );	

    //Allocate the memory for header req and initialize that to 0 
    hdr_req = ( message_hdr_req* ) calloc ( sizeof( message_hdr_req ), 1 );
    if ( NULL == hdr_req )
        {
        return MEM_ALLOCATION_ERROR;
        }
    //msg_hdr can never be NULL.. handled in the case
    memcpy( hdr_req, msg_hdr,  sizeof( message_hdr_req ) );

    //Registering for signal to be done at different loation
    /*dbus_g_proxy_connect_signal (DBUS_G_PROXY(globalCon.conn), "AvatarUpdated",
                                 G_CALLBACK (avatar_updated_signal),
                                 NULL, NULL);       */
    //get the interface
    avatar_face = tp_conn_get_interface (
            globalCon.conn, TELEPATHY_CONN_IFACE_AVATAR_QUARK);

    if ( avatar_face )
        {
        //make call to the lib telepathy to set the avatar. update_avtar_cb is registered as the callback
        tp_conn_iface_avatars_set_avatar_async (avatar_face, avtarcontent, mimetype, update_avtar_cb, (gpointer)hdr_req);
        }
    else {
    error = TP_AVATAR_IFACE_ERROR; 
    }

    iso_logger( "%s", "Out - send_avtar\n" );
    return error;
    }

/*! \brief callback for clearing own avtar. This function 
 *  sends response to client
 *
 *  \param proxy : unused
 *  \param error : error if any
 *  \param userdata : message header  
 *  \return : void
 */
void clear_avtar_cb( DBusGProxy *proxy, GError* error, gpointer userdata )
    {

    int err = 0;
    //user data is of type message_hdr_req
    message_hdr_req* msg_hdr = ( message_hdr_req* ) userdata;

    iso_logger( "%s", "In - clear_avtar_cb\n" );
    UNUSED_FORMAL_PARAM(proxy);

    //msg_hdr should never be NULL

    if ( NULL != error ) 
        {
        //There was some error
        //send the response for the msg_hdr request to client

        iso_logger( "%s", "error in clear_avtar_cb" );
        err = send_response_to_client( msg_hdr, error->code, 0 );
        g_error_free(error);
        }
    else 
        {
        err = send_response_to_client( msg_hdr, 0, 1 );
        }

    if ( err < 0 )
        {
        // failed to deliver
        iso_logger( "%s", "failed to deliver\n" );
        }
    free( msg_hdr );
    iso_logger( "%s", "Out - clear_avtar_cb\n" );

    }
/*! \brief clears the self avtar 
 *
 *  \param msghdr request header that will be passed back to client
 *
 *  \return : error code on failure, 0 on success
 */
gint clear_avatar( message_hdr_req *msg_hdr )
    {
    gint error = 0;
    DBusGProxy* avatar_face = NULL;
    message_hdr_req *hdr_req = NULL;
    iso_logger( "%s", "In - clear_avtar\n" );    

    //Allocate the memory for header req and initialize that to 0 
    hdr_req = ( message_hdr_req* ) calloc ( sizeof( message_hdr_req ), 1 );
    if ( NULL == hdr_req ) 
        {
        return MEM_ALLOCATION_ERROR;    
        }
    //msg_hdr can never be NULL.. handled in the case
    memcpy( hdr_req, msg_hdr,  sizeof( message_hdr_req ) );

    //get the interface
    avatar_face = tp_conn_get_interface (
            globalCon.conn, TELEPATHY_CONN_IFACE_AVATAR_QUARK);

    if ( avatar_face )
        {
        tp_conn_iface_avatars_clear_avatar_async(avatar_face, clear_avtar_cb, (gpointer)hdr_req);
        }
    else {
    error = TP_AVATAR_IFACE_ERROR; 
    }

    iso_logger( "%s", "Out - clear_avtar\n" );

    return error;

    }
/*! \brief clears the self avtar 
 *  \param rmsg : message buffer to be parsed
 *  \param msg_struct : request header 
 *  \param result : message buffer length
 *  \param avtarcontent : pointer to the avatar image data
 * 
 *  \return : error code on failure, 0 on success
 */
gint update_own_avatar(char* rmsg,message_hdr_resp* msg_struct,gint result, GArray** avtarcontent)
    {
    int len = 0;
    int err = 0;
    static gboolean fetch_avatar_data = 0;
    static gchar *mimetype = NULL;


    iso_logger( "%s", "In - EUpdateOwnAvtar\n" );

    if(!fetch_avatar_data)
        {
        //parse mime type
        //calculating len for request header
        //skip the msg_hdr part

        len += sizeof( message_hdr_req );

        *avtarcontent = g_array_new ( FALSE, FALSE, sizeof ( gchar ) );
        if ( NULL == *avtarcontent )
            {
            return MEM_ALLOCATION_ERROR;
            }

        // reading a mimetype from buffer
        err = parse_a_string( rmsg, &mimetype, &len, result );

        if ( err < 0 )
            {
            goto oom;
            }

        fetch_avatar_data = 1;     
        }
    else
        {
        len = sizeof( message_hdr_req );
        }


    g_array_append_vals( *avtarcontent, rmsg + len, result - len );


    if(! msg_struct->hdr_req.continue_flag )
        {
        err = send_avtar( *avtarcontent, mimetype, &(msg_struct->hdr_req) );  

        fetch_avatar_data = 0; //reset the value so that if set avatar requested for
        //falling of to goto oom

        }
    else {
    //Continue flag is set and read the data again return 0
    return 0;
    }

    oom:
    //Free the memory if err or operation complete ( sent to libeteleapthy)
    if ( *avtarcontent )
        {
        g_array_free( *avtarcontent, (*avtarcontent)->len );
        *avtarcontent = NULL;
        }
    if( mimetype )
        {
        free ( mimetype );
        }
    return err;
    }
