/*
 * libgabble_wsd.cpp part of telepathygabble
 * Copyright (C) 2006 Collabora Ltd.
 * 
 *   @author Ole Andre Vadla Ravnaas <ole.andre.ravnaas@collabora.co.uk>
 *   @author Robert McQueen <robert.mcqueen@collabora.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <e32std.h> 
#include "libgabble_wsd_solution.h"
#include "gabble_enums.h"
#include <string.h>
#include "telepathy-interfaces.h"
#include <pls.h> // For emulator WSD API 

const TUid KLibGabbleUid3 = {0x2000F852};     // This is the UID of the library
struct libgabble_global_struct *libgabble_ImpurePtr()
{
	#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct libgabble_global_struct *g = Pls<struct libgabble_global_struct>(KLibGabbleUid3, &libgabble_Init);	
    return g;

	#else

	return NULL;	

	#endif
}

int libgabble_Init(libgabble_global_struct *g)
{


Handler handlers[10] = {
  {
    { "initiate", "session-initiate", NULL },
    JS_STATE_PENDING_CREATED,
    JS_STATE_PENDING_CREATED,
    { NULL },
    JS_STATE_PENDING_INITIATED
  },
  {
    { "accept", "session-accept", NULL },
    JS_STATE_PENDING_INITIATED,
    JS_STATE_PENDING_INITIATED,
    { NULL },
    JS_STATE_ACTIVE
  },
  {
    { "reject", NULL },
    JS_STATE_PENDING_INITIATE_SENT,
    JS_STATE_PENDING_INITIATED,
    { NULL },
    JS_STATE_INVALID
  },
  {
    { "terminate", "session-terminate", NULL },
    JS_STATE_PENDING_INITIATED,
    JS_STATE_ENDED,
    { NULL },
    JS_STATE_INVALID
  },
  {
    { "candidates", "transport-info", NULL },
    JS_STATE_PENDING_INITIATED,
    JS_STATE_ACTIVE,
    { NULL },
    JS_STATE_INVALID
  },
  {
    { "content-add", NULL },
    JS_STATE_ACTIVE,
    JS_STATE_ACTIVE,
    { NULL },
    JS_STATE_INVALID,
  },
  {
    { "content-modify", NULL },
    JS_STATE_PENDING_INITIATED,
    JS_STATE_ACTIVE,
    { NULL },
    JS_STATE_INVALID
  },
  {
    { "content-accept", NULL },
    JS_STATE_PENDING_INITIATED,
    JS_STATE_ACTIVE,
    { NULL },
    JS_STATE_INVALID
  },
  {
    { "content-remove", "content-decline", NULL },
    JS_STATE_PENDING_INITIATED,
    JS_STATE_ACTIVE,
    { NULL },
    JS_STATE_INVALID
  },
  {
    { NULL },
    JS_STATE_INVALID,
    JS_STATE_INVALID,
    { NULL },
    JS_STATE_INVALID
  }
};

GDebugKey keys[] = {
  { "presence",      GABBLE_DEBUG_PRESENCE },
  { "groups",        GABBLE_DEBUG_GROUPS },
  { "roster",        GABBLE_DEBUG_ROSTER },
  { "disco",         GABBLE_DEBUG_DISCO },
  { "properties",    GABBLE_DEBUG_PROPERTIES },
  { "roomlist",      GABBLE_DEBUG_ROOMLIST },
  { "media-channel", GABBLE_DEBUG_MEDIA },
  { "muc",           GABBLE_DEBUG_MUC },
  { "connection",    GABBLE_DEBUG_CONNECTION },
  { "persist",       GABBLE_DEBUG_PERSIST },
  { "vcard",         GABBLE_DEBUG_VCARD },
  { 0, },
};

gchar NO_ALIAS[10] = "none";

gchar assumed_caps[3][150] =
{
  TP_IFACE_CHANNEL_TYPE_TEXT,
  TP_IFACE_CHANNEL_INTERFACE_GROUP,
  NULL
};

	
gchar muc_roles[4][20] =
{
  "none",
  "visitor",
  "participant",
  "moderator",
};

gchar muc_affiliations[4][20] =
{
  "none",
  "member",
  "admin",
  "owner",
};

gchar video_codec_params[6][20] = {
  "x", "y", "width", "height", "layer", "transparent",
};


char list_handle_strings[4][20] =
{
    "publish",      /* GABBLE_LIST_HANDLE_PUBLISH */
    "subscribe",    /* GABBLE_LIST_HANDLE_SUBSCRIBE */
    "known",        /* GABBLE_LIST_HANDLE_KNOWN */
    "deny"          /* GABBLE_LIST_HANDLE_DENY */
};

	  if(g)
	  {	
	    
	      
		//g->GET_WSD_VAR_NAME(flags,gabble_debug,s) = flags;
		g->GET_WSD_VAR_NAME(log_handler, gabble_debug, s)	= 0;
		g->GET_WSD_VAR_NAME(etype,gabble_enum_types,s) = 0;
		
		g->GET_WSD_VAR_NAME(etype,gabble_media_session_enum_types,s) = 0;
		g->GET_WSD_VAR_NAME(etype1,gabble_media_session_enum_types,s) = 0;
		g->GET_WSD_VAR_NAME(etype2,gabble_media_session_enum_types,s) = 0;
		
		
		
		
		g->GET_WSD_VAR_NAME(quark,gabble_disco,s) = 0; 
		g->GET_WSD_VAR_NAME(quark,gabble_error,s) = 0;
		
		
		g->GET_WSD_VAR_NAME(ssl,gabble_conmgr,s) = TRUE; 
		g->GET_WSD_VAR_NAME(httpport,gabble_conmgr,s) = 8080;
		g->GET_WSD_VAR_NAME(httpproxyport,gabble_conmgr,s) = 443;

		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_conmgr,s)),0,LAST_SIGNAL_CON_MGR*sizeof(guint));	  
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_con,s)),0,LAST_SIGNAL_CON*sizeof(guint));	  
		g->GET_WSD_VAR_NAME(arguments,gabble_con, s) = NULL;
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_im,s)),0,LAST_SIGNAL_IM*sizeof(guint));	  
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_search,s)),0,LAST_SIGNAL_SEARCH*sizeof(guint));	   
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_muc,s)),0,LAST_SIGNAL_MUC*sizeof(guint));	  
		
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_med_chnl,s)),0,LAST_SIGNAL_MED_CHANNEL*sizeof(guint));	  
		
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_med_sess,s)),0,LAST_SIGNAL_MED_SESSION*sizeof(guint));	  
		



		g->GET_WSD_VAR_NAME(google_audio_caps,gabble_med_sess,s) = PRESENCE_CAP_GOOGLE_VOICE;
		
		int some = PRESENCE_CAP_JINGLE | PRESENCE_CAP_JINGLE_DESCRIPTION_AUDIO |
		    PRESENCE_CAP_GOOGLE_TRANSPORT_P2P;
		
		g->GET_WSD_VAR_NAME(jingle_audio_caps,gabble_med_sess,s) = (GabblePresenceCapabilities)some;
		
		some = PRESENCE_CAP_JINGLE | PRESENCE_CAP_JINGLE_DESCRIPTION_VIDEO |
		    PRESENCE_CAP_GOOGLE_TRANSPORT_P2P;
		g->GET_WSD_VAR_NAME(jingle_video_caps,gabble_med_sess,s) = 
		(GabblePresenceCapabilities) some;
		
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_med_sess,s)),0,MAX_STREAM_NAME_LEN*sizeof(gchar));	  
		
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_med_stream,s)),0,LAST_SIGNAL_MED_STREAM*sizeof(guint));	  
		
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_pre_cache,s)),0,LAST_SIGNAL_PRE_CACHE*sizeof(guint));	  
		
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_register,s)),0,LAST_SIGNAL_REGISTER*sizeof(guint));	  
		
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_ros_chnl,s)),0,LAST_SIGNAL_ROS_CHNL*sizeof(guint));	  
		
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_room_chnl,s)),0,LAST_SIGNAL_ROOM_CHNL*sizeof(guint));	  
		
		g->GET_WSD_VAR_NAME(offset_quark1,gabble_mixin,s) = 0;
		g->GET_WSD_VAR_NAME(offset_quark,gabble_mixin,s) = 0;
		
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_roster,s)),0,LAST_SIGNAL_ROSTER*sizeof(guint));	  

		g->GET_WSD_VAR_NAME(quark,gabble_errors,s) = 0;
		
		g->GET_WSD_VAR_NAME(busCon,gabble_helpers,s) = NULL;
		g->GET_WSD_VAR_NAME(bus_proxy,gabble_helpers,s) = NULL;

		g->GET_WSD_VAR_NAME(offset_quark1,gabble_txt_mixin,s) = 0;
		g->GET_WSD_VAR_NAME(offset_quark,gabble_txt_mixin,s) = 0;
		
		memset(&(g->GET_WSD_VAR_NAME(alloc1,gabble_txt_mixin,s)),0,sizeof(GabbleAllocator));	  	
		
		g->GET_WSD_VAR_NAME(type,gabble_chnl_iface,s) = 0;
		g->GET_WSD_VAR_NAME(initialized,gabble_chnl_iface,s) = EFalse;    
		
		g->GET_WSD_VAR_NAME(type,gabble_fac_iface,s) = 0;
		g->GET_WSD_VAR_NAME(initialized,gabble_fac_iface,s) = EFalse;	
		
		memset(&(g->GET_WSD_VAR_NAME(signals,gabble_vcard_mgr,s)),0,LAST_SIGNAL_VCARD_MGR*sizeof(guint));	  
		
		g->GET_WSD_VAR_NAME(quark1,gabble_vcard_mgr,s) = 0;
		g->GET_WSD_VAR_NAME(quark2,gabble_vcard_mgr,s) = 0;
		
		g->GET_WSD_VAR_NAME(offset_quark1,gabble_grp_mixin,s) = 0;
		g->GET_WSD_VAR_NAME(offset_quark,gabble_grp_mixin,s) = 0;	    
		
		g->GET_WSD_VAR_NAME(gabble_disco_parent_class,gabble_disco,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_disco,s) = 0;
		
		g->GET_WSD_VAR_NAME(gabble_connection_manager_parent_class,gabble_conmgr,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_conmgr,s) = 0;
		
		g->GET_WSD_VAR_NAME(gabble_connection_parent_class,gabble_con,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_con,s) = 0;
		
		g->GET_WSD_VAR_NAME(gabble_im_channel_parent_class,gabble_im,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_im,s) = 0;
		
		g->GET_WSD_VAR_NAME(gabble_search_channel_parent_class,gabble_search,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_search,s) = 0;


       	g->GET_WSD_VAR_NAME(gabble_muc_channel_parent_class,gabble_muc,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_muc,s) = 0;

        g->GET_WSD_VAR_NAME(gabble_media_channel_parent_class,gabble_med_chnl,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_med_chnl,s) = 0;
		
	    g->GET_WSD_VAR_NAME(gabble_media_stream_parent_class,gabble_med_stream,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_med_stream,s) = 0;

        g->GET_WSD_VAR_NAME(gabble_presence_parent_class,gabble_presence,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_presence,s) = 0;

        g->GET_WSD_VAR_NAME(gabble_presence_cache_parent_class,gabble_pre_cache,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_pre_cache,s) = 0;

        g->GET_WSD_VAR_NAME(gabble_register_parent_class,gabble_register,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_register,s) = 0;
        
        g->GET_WSD_VAR_NAME(gabble_roster_channel_parent_class,gabble_ros_chnl,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_ros_chnl,s) = 0;
        
        
        g->GET_WSD_VAR_NAME(gabble_roomlist_channel_parent_class,gabble_room_chnl,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_room_chnl,s) = 0;

        g->GET_WSD_VAR_NAME(gabble_im_factory_parent_class ,im_factory,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,im_factory,s) = 0;
        
        
        g->GET_WSD_VAR_NAME(gabble_media_factory_parent_class ,media_factory,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,media_factory,s) = 0;

        g->GET_WSD_VAR_NAME(gabble_muc_factory_parent_class ,muc_factory,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,muc_factory,s) = 0;

        g->GET_WSD_VAR_NAME(gabble_vcard_manager_parent_class ,gabble_vcard_mgr,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_vcard_mgr,s) = 0;
		
		g->GET_WSD_VAR_NAME(gabble_media_session_parent_class ,gabble_med_sess,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_med_sess,s) = 0;
		
		g->GET_WSD_VAR_NAME(gabble_roster_parent_class ,gabble_roster,s) = ((void *)0); 
		g->GET_WSD_VAR_NAME(g_define_type_id,gabble_roster,s) = 0;
		
		
	    memcpy (&(g->GET_WSD_VAR_NAME(keys,gabble_debug,s)), keys, 12*sizeof(GDebugKey));
	  	strcpy((g->GET_WSD_VAR_NAME(NO_ALIAS,gabble_vcard_mgr,s)), NO_ALIAS);
	  	
	  	memset(&(g->GET_WSD_VAR_NAME(assumed_caps,gabble_con,s)),0,3*150*sizeof(gchar));
	  	
	  	strcpy((g->GET_WSD_VAR_NAME(assumed_caps,gabble_con,s))[0], assumed_caps[0]);
	  	strcpy((g->GET_WSD_VAR_NAME(assumed_caps,gabble_con,s))[0], assumed_caps[1]);
	 
	  	strcpy((g->GET_WSD_VAR_NAME(muc_roles,gabble_muc,s))[0], muc_roles[0]);
	  	strcpy((g->GET_WSD_VAR_NAME(muc_roles,gabble_muc,s))[1], muc_roles[1]);
	  	strcpy((g->GET_WSD_VAR_NAME(muc_roles,gabble_muc,s))[2], muc_roles[2]);
	  	strcpy((g->GET_WSD_VAR_NAME(muc_roles,gabble_muc,s))[3], muc_roles[3]);

		
		strcpy((g->GET_WSD_VAR_NAME(muc_affiliations,gabble_muc,s))[0], muc_affiliations[0]);
		strcpy((g->GET_WSD_VAR_NAME(muc_affiliations,gabble_muc,s))[1], muc_affiliations[1]);
		strcpy((g->GET_WSD_VAR_NAME(muc_affiliations,gabble_muc,s))[2], muc_affiliations[2]);
		strcpy((g->GET_WSD_VAR_NAME(muc_affiliations,gabble_muc,s))[3], muc_affiliations[3]);
		
		strcpy((g->GET_WSD_VAR_NAME(video_codec_params,gabble_med_stream,s))[0], video_codec_params[0]);
		strcpy((g->GET_WSD_VAR_NAME(video_codec_params,gabble_med_stream,s))[1], video_codec_params[1]);
		strcpy((g->GET_WSD_VAR_NAME(video_codec_params,gabble_med_stream,s))[2], video_codec_params[2]);
		strcpy((g->GET_WSD_VAR_NAME(video_codec_params,gabble_med_stream,s))[3], video_codec_params[3]);
		strcpy((g->GET_WSD_VAR_NAME(video_codec_params,gabble_med_stream,s))[4], video_codec_params[4]);
		strcpy((g->GET_WSD_VAR_NAME(video_codec_params,gabble_med_stream,s))[5], video_codec_params[5]);
		
		strcpy((g->GET_WSD_VAR_NAME(list_handle_strings,handles,s))[0], list_handle_strings[0]);
		strcpy((g->GET_WSD_VAR_NAME(list_handle_strings,handles,s))[1], list_handle_strings[1]);
		strcpy((g->GET_WSD_VAR_NAME(list_handle_strings,handles,s))[2], list_handle_strings[2]);
		strcpy((g->GET_WSD_VAR_NAME(list_handle_strings,handles,s))[3], list_handle_strings[3]);
		
		memcpy (&(g->GET_WSD_VAR_NAME(handlers,gabble_med_sess,s)), handlers, 10*sizeof(Handler));
  
		return 0;
  }
  return 1;
}

