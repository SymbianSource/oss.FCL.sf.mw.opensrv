/*
 * libgabble_wsd_solution.h - Header 
 * Copyright (C) 2006 Collabora Ltd.
 *   @author Ole Andre Vadla Ravnaas <ole.andre.ravnaas@collabora.co.uk>
 * 
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

#ifndef _LIBGABBLE_WSD_H
#define _LIBGABBLE_WSD_H
#include "libgabble_wsd_macros.h"
//#include "debug.h"
#include "telepathy-errors.h"
#include "disco.h"
#include "gabble_enums.h"
#include "ghash.h"
#include "gabble-presence.h"
#include "gabble-media-session.h"

#define MAX_GABBLE_UTILS_GCHAR_ARRAY_LEN 5


#if EMULATOR
#ifdef __cplusplus
extern "C" 
{
#endif

typedef gboolean (*StreamHandlerFunc)(GabbleMediaSession *session,
                                      LmMessage *message,
                                      LmMessageNode *content_node,
                                      const gchar *stream_name,
                                      GabbleMediaStream *stream,
                                      LmMessageNode *desc_node,
                                      LmMessageNode *trans_node,
                                      GError **error);
                                      


                                   
typedef struct _Handler Handler;

struct _Handler {
  gchar actions[3][50];
  JingleSessionState min_allowed_state;
  JingleSessionState max_allowed_state;
  StreamHandlerFunc stream_handlers[4];
  JingleSessionState new_state;
};

struct libgabble_global_struct
{
	
	VARIABLE_DECL(flags,s,gabble_debug,GabbleDebugFlags)	
	VARIABLE_DECL(log_handler,s,gabble_debug,guint)	
	VARIABLE_DECL(etype,s,gabble_enum_types,GType)
	
	VARIABLE_DECL(etype,s,gabble_media_session_enum_types,GType)
	VARIABLE_DECL(etype1,s,gabble_media_session_enum_types,GType)
	VARIABLE_DECL(etype2,s,gabble_media_session_enum_types,GType)
	
	VARIABLE_DECL(quark,s,gabble_disco,GQuark)
	VARIABLE_DECL(quark,s,gabble_error,GQuark)
	
	
	VARIABLE_DECL(ssl,s,gabble_conmgr,gboolean)
	VARIABLE_DECL(httpport,s,gabble_conmgr,guint)
	VARIABLE_DECL(httpproxyport,s,gabble_conmgr,guint)
	VARIABLE_DECL_ARRAY(signals,s,gabble_conmgr,guint,LAST_SIGNAL_CON_MGR)
	VARIABLE_DECL_ARRAY(signals,s,gabble_con,guint,LAST_SIGNAL_CON)
	VARIABLE_DECL(arguments,s,gabble_con,GHashTable*)
	VARIABLE_DECL_ARRAY(signals,s,gabble_im,guint,LAST_SIGNAL_IM)
	
	//todo: later ..see what it should be in search
	VARIABLE_DECL_ARRAY(signals,s,gabble_search,guint,LAST_SIGNAL_SEARCH)
	
	VARIABLE_DECL_ARRAY(signals,s,gabble_muc,guint,LAST_SIGNAL_MUC)
	VARIABLE_DECL_ARRAY(signals,s,gabble_med_chnl,guint,LAST_SIGNAL_MED_CHANNEL)
	
	VARIABLE_DECL_ARRAY(signals,s,gabble_med_sess,guint,LAST_SIGNAL_MED_SESSION)
	



	VARIABLE_DECL(google_audio_caps,s,gabble_med_sess,GabblePresenceCapabilities)
	VARIABLE_DECL(jingle_audio_caps,s,gabble_med_sess,GabblePresenceCapabilities)
	VARIABLE_DECL(jingle_video_caps,s,gabble_med_sess,GabblePresenceCapabilities)
	
	VARIABLE_DECL_ARRAY(ret_sess,s,gabble_med_sess,gchar,MAX_STREAM_NAME_LEN)
	
	VARIABLE_DECL_ARRAY(signals,s,gabble_med_stream,guint,LAST_SIGNAL_MED_STREAM)
	
	VARIABLE_DECL_ARRAY(signals,s,gabble_pre_cache,guint,LAST_SIGNAL_PRE_CACHE)
	
	VARIABLE_DECL_ARRAY(signals,s,gabble_register,guint,LAST_SIGNAL_REGISTER)
	
	VARIABLE_DECL_ARRAY(signals,s,gabble_ros_chnl,guint,LAST_SIGNAL_ROS_CHNL)
	
	VARIABLE_DECL_ARRAY(signals,s,gabble_room_chnl,guint,LAST_SIGNAL_ROOM_CHNL)
	
	VARIABLE_DECL(offset_quark1,s,gabble_mixin,GQuark)
	VARIABLE_DECL(offset_quark,s,gabble_mixin,GQuark)
	
	VARIABLE_DECL_ARRAY(signals,s,gabble_roster,guint,LAST_SIGNAL_ROSTER)

	VARIABLE_DECL(quark,s,gabble_errors,GQuark)
	
	VARIABLE_DECL(busCon,s,gabble_helpers,DBusGConnection*)
	VARIABLE_DECL(bus_proxy,s,gabble_helpers,DBusGProxy*)

	VARIABLE_DECL(offset_quark1,s,gabble_txt_mixin,GQuark)
	VARIABLE_DECL(offset_quark,s,gabble_txt_mixin,GQuark)
	VARIABLE_DECL(alloc1,s,gabble_txt_mixin,GabbleAllocator)
	
	VARIABLE_DECL(offset_quark1,s,gabble_search_mixin,GQuark)
	VARIABLE_DECL(offset_quark,s,gabble_search_mixin,GQuark)
	//todo: check if this required for search
	VARIABLE_DECL(alloc1,s,gabble_search_mixin,GabbleAllocator)
	
	VARIABLE_DECL(type,s,gabble_chnl_iface,GType)
	VARIABLE_DECL(initialized,s,gabble_chnl_iface,gboolean)
	
	VARIABLE_DECL(type,s,gabble_fac_iface,GType)
	VARIABLE_DECL(initialized,s,gabble_fac_iface,gboolean)
	
	VARIABLE_DECL_ARRAY(signals,s,gabble_vcard_mgr,guint,LAST_SIGNAL_VCARD_MGR)
	VARIABLE_DECL(quark1,s,gabble_vcard_mgr,GQuark)
	VARIABLE_DECL(quark2,s,gabble_vcard_mgr,GQuark)
	
	
	VARIABLE_DECL(offset_quark1,s,gabble_grp_mixin,GQuark)
	VARIABLE_DECL(offset_quark,s,gabble_grp_mixin,GQuark)
	
	
	VARIABLE_DECL(gabble_disco_parent_class,s,gabble_disco,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_disco,GType)
	
	VARIABLE_DECL(gabble_connection_manager_parent_class,s,gabble_conmgr,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_conmgr,GType)

    VARIABLE_DECL(gabble_connection_parent_class,s,gabble_con,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_con,GType)
	
	VARIABLE_DECL(gabble_im_channel_parent_class,s,gabble_im,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_im,GType)
	
	VARIABLE_DECL(gabble_search_channel_parent_class,s,gabble_search,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_search,GType)

	VARIABLE_DECL(gabble_muc_channel_parent_class,s,gabble_muc,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_muc,GType)

    VARIABLE_DECL(gabble_media_channel_parent_class,s,gabble_med_chnl,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_med_chnl,GType)

    VARIABLE_DECL(gabble_media_stream_parent_class,s,gabble_med_stream,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_med_stream,GType)
    
    VARIABLE_DECL(gabble_presence_parent_class,s,gabble_presence,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_presence,GType)
    
    VARIABLE_DECL(gabble_presence_cache_parent_class,s,gabble_pre_cache,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_pre_cache,GType)

    VARIABLE_DECL(gabble_register_parent_class,s,gabble_register,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_register,GType)

    VARIABLE_DECL(gabble_roster_channel_parent_class,s,gabble_ros_chnl,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_ros_chnl,GType)
    
    VARIABLE_DECL(gabble_roomlist_channel_parent_class,s,gabble_room_chnl,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_room_chnl,GType)

    VARIABLE_DECL(gabble_im_factory_parent_class ,s,im_factory,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,im_factory,GType)
	
	VARIABLE_DECL(gabble_search_factory_parent_class ,s,search_factory,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,search_factory,GType)

    VARIABLE_DECL(gabble_media_factory_parent_class ,s,media_factory,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,media_factory,GType)

	VARIABLE_DECL(gabble_muc_factory_parent_class ,s,muc_factory,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,muc_factory,GType)

	VARIABLE_DECL(gabble_vcard_manager_parent_class ,s,gabble_vcard_mgr,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_vcard_mgr,GType)
	
	VARIABLE_DECL_ARRAY(keys,s,gabble_debug,GDebugKey,12)
	
	VARIABLE_DECL_ARRAY(NO_ALIAS,s,gabble_vcard_mgr,gchar,MAX_GABBLE_UTILS_GCHAR_ARRAY_LEN)


	VARIABLE_DECL_2DARRAY(assumed_caps,s,gabble_con,gchar,3,150)
	
	VARIABLE_DECL_2DARRAY(muc_roles,s,gabble_muc,gchar,4,20)
	VARIABLE_DECL_2DARRAY(muc_affiliations,s,gabble_muc,gchar,4,20)
	
	VARIABLE_DECL_2DARRAY(video_codec_params,s,gabble_med_stream,gchar,6,20)
	
	VARIABLE_DECL_2DARRAY(list_handle_strings,s,handles,char,4,20)
	
	VARIABLE_DECL_ARRAY(handlers,s,gabble_med_sess,Handler,10)
	
	VARIABLE_DECL(gabble_media_session_parent_class ,s,gabble_med_sess,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_med_sess,GType)
	
	VARIABLE_DECL(gabble_roster_parent_class ,s,gabble_roster,gpointer)	
	VARIABLE_DECL(g_define_type_id,s,gabble_roster,GType)
	
	
      /*END-global vars*/
 
    };

struct libgabble_global_struct * libgabble_ImpurePtr();
int libgabble_Init(struct libgabble_global_struct *);
#ifdef __cplusplus
}
#endif
#endif //EMULATOR
#endif //header guard ifdef _LIBGABBLE_WSD_H

