/*
 * search-factory.c - Source for GabbleSearchFactory
 * Copyright (C) 2006 Collabora Ltd.
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


#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <glib.h>
#include <glib-object.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "loudmouth/loudmouth.h"

#include "debug.h"
#include "gabble-connection.h"
#include "telepathy-interfaces.h"
#include "tp-channel-factory-iface.h"

#include "search-mixin.h"
#include "gabble-search-channel.h"

#include "search-factory.h"

static void gabble_search_factory_iface_init (gpointer g_iface, gpointer iface_data);

#ifndef EMULATOR
G_DEFINE_TYPE_WITH_CODE (GabbleSearchFactory, gabble_search_factory, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (TP_TYPE_CHANNEL_FACTORY_IFACE, gabble_search_factory_iface_init));
#endif

#ifdef EMULATOR
#include "libgabble_wsd_solution.h"

    GET_STATIC_VAR_FROM_TLS(gabble_search_factory_parent_class,search_factory,gpointer)
	#define gabble_search_factory_parent_class (*GET_WSD_VAR_NAME(gabble_search_factory_parent_class,search_factory,s)())
	
	GET_STATIC_VAR_FROM_TLS(g_define_type_id,search_factory,GType)
	#define g_define_type_id (*GET_WSD_VAR_NAME(g_define_type_id,search_factory,s)())

static void gabble_search_factory_init (GabbleSearchFactory *self); 
static void gabble_search_factory_class_init (GabbleSearchFactoryClass *klass); 
static void gabble_search_factory_class_intern_init (gpointer klass) 
{
 gabble_search_factory_parent_class = g_type_class_peek_parent (klass);
  gabble_search_factory_class_init ((GabbleSearchFactoryClass*) klass);
   }
    EXPORT_C GType gabble_search_factory_get_type (void) 
    {
    if ((g_define_type_id == 0)) { static const GTypeInfo g_define_type_info = { sizeof (GabbleSearchFactoryClass), (GBaseInitFunc) ((void *)0), (GBaseFinalizeFunc) ((void *)0), (GClassInitFunc) gabble_search_factory_class_intern_init, (GClassFinalizeFunc) ((void *)0), ((void *)0), sizeof (GabbleSearchFactory), 0, (GInstanceInitFunc) gabble_search_factory_init, ((void *)0) }; g_define_type_id = g_type_register_static ( ((GType) ((20) << (2))), g_intern_static_string ("GabbleSearchFactory"), &g_define_type_info, (GTypeFlags) 0); { { static const GInterfaceInfo g_implement_interface_info = { (GInterfaceInitFunc) gabble_search_factory_iface_init }; g_type_add_interface_static (g_define_type_id, tp_channel_factory_iface_get_type(), &g_implement_interface_info); } ; } } return g_define_type_id; }    ;

#endif

#define DBUS_API_SUBJECT_TO_CHANGE
#define DEBUG_FLAG GABBLE_DEBUG_IM

#ifdef DEBUG_FLAG
//#define DEBUG(format, ...)
#define DEBUGGING 0
#define NODE_DEBUG(n, s)
#endif /* DEBUG_FLAG */

/* properties */
enum
{
  PROP_CONNECTION = 1,
  LAST_PROPERTY
};

typedef struct _GabbleSearchFactoryPrivate GabbleSearchFactoryPrivate;
struct _GabbleSearchFactoryPrivate
{
  GabbleConnection *conn;
  GArray *channels; 
  gboolean dispose_has_run;
};

#define GABBLE_SEARCH_FACTORY_GET_PRIVATE(o)    (G_TYPE_INSTANCE_GET_PRIVATE ((o), GABBLE_TYPE_SEARCH_FACTORY, GabbleSearchFactoryPrivate))

static GObject *gabble_search_factory_constructor (GType type, guint n_props, GObjectConstructParam *props);

static void
gabble_search_factory_init (GabbleSearchFactory *fac)
{
  GabbleSearchFactoryPrivate *priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);

  priv->channels = g_array_sized_new (FALSE, FALSE, sizeof (guint), 1);
  priv->conn = NULL;
  priv->dispose_has_run = FALSE;
}

static GObject *
gabble_search_factory_constructor (GType type, guint n_props,
                               GObjectConstructParam *props)
{
  GObject *obj;
  /* GabbleSearchFactoryPrivate *priv; */

  obj = G_OBJECT_CLASS (gabble_search_factory_parent_class)->
           constructor (type, n_props, props);
  /* priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (obj); */

  return obj;
}


static void
gabble_search_factory_dispose (GObject *object)
{
  GabbleSearchFactory *fac = GABBLE_SEARCH_FACTORY (object);
  GabbleSearchFactoryPrivate *priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);

  if (priv->dispose_has_run)
    return;

  gabble_debug (DEBUG_FLAG, "dispose called");
  priv->dispose_has_run = TRUE;

  tp_channel_factory_iface_close_all (TP_CHANNEL_FACTORY_IFACE (object));
  g_assert (priv->channels == NULL);

  if (G_OBJECT_CLASS (gabble_search_factory_parent_class)->dispose)
    G_OBJECT_CLASS (gabble_search_factory_parent_class)->dispose (object);
}

static void
gabble_search_factory_get_property (GObject    *object,
                                 guint       property_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  GabbleSearchFactory *fac = GABBLE_SEARCH_FACTORY (object);
  GabbleSearchFactoryPrivate *priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);
	
  switch (property_id) {
    case PROP_CONNECTION:
      g_value_set_object (value, priv->conn);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gabble_search_factory_set_property (GObject      *object,
                                 guint         property_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GabbleSearchFactory *fac = GABBLE_SEARCH_FACTORY (object);
  GabbleSearchFactoryPrivate *priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);
  
  switch (property_id) {
    case PROP_CONNECTION:
      priv->conn = g_value_get_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gabble_search_factory_class_init (GabbleSearchFactoryClass *gabble_search_factory_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (gabble_search_factory_class);
  GParamSpec *param_spec;

  g_type_class_add_private (gabble_search_factory_class, sizeof (GabbleSearchFactoryPrivate));

  object_class->constructor = gabble_search_factory_constructor;
  object_class->dispose = gabble_search_factory_dispose;

  object_class->get_property = gabble_search_factory_get_property;
  object_class->set_property = gabble_search_factory_set_property;

  
  param_spec = g_param_spec_object ("connection", "GabbleConnection object",
                                    "Gabble connection object that owns this "
                                    "Search channel factory object.",
                                    GABBLE_TYPE_CONNECTION,
                                    G_PARAM_CONSTRUCT_ONLY |
                                    G_PARAM_READWRITE |
                                    G_PARAM_STATIC_NICK |
                                    G_PARAM_STATIC_BLURB);
  g_object_class_install_property (object_class, PROP_CONNECTION, param_spec);

}

static GabbleSearchChannel *new_search_channel (GabbleSearchFactory *fac, GabbleHandle handle);

static void search_channel_closed_cb (GabbleSearchChannel *chan, gpointer user_data);



/**
 * search_channel_closed_cb:
 *
 * Signal callback for when search channel is closed. Removes the references
 * that #GabbleConnection holds to them.
 */
static void
search_channel_closed_cb (GabbleSearchChannel *chan, gpointer user_data)
{
  GabbleSearchFactory *fac = GABBLE_SEARCH_FACTORY (user_data);
  GabbleSearchFactoryPrivate *priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);
  g_message("in search_channel_closed_cb");
  if (priv->channels)
    {
    g_array_remove_index( priv->channels, chan->channel_index );
    }
    
  lm_connection_unregister_message_handler (priv->conn->lmconn, chan->iq_cb,
                                            LM_MESSAGE_TYPE_IQ);
  lm_message_handler_unref (chan->iq_cb);
  chan->iq_cb = NULL;
  g_object_unref (chan);
  chan = NULL;
}

/**
 * new_search_channel
 */
static GabbleSearchChannel *
new_search_channel (GabbleSearchFactory *fac, GabbleHandle handle)
{
  GabbleSearchFactoryPrivate *priv;
  GabbleSearchChannel *chan;
  char *object_path;
 
  g_assert (GABBLE_IS_SEARCH_FACTORY (fac));

  priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);
  
  g_message("in new_search_channel:handle %u\n",handle);
  
  object_path = g_strdup_printf ("%s/SearchChannel%u", priv->conn->object_path, handle);
  
  gabble_debug (DEBUG_FLAG, "object path %s", object_path);
  
  
 
 // handle type should be none and handle value should be zero
 // so handle passed in g_object_new
  chan = g_object_new (GABBLE_TYPE_SEARCH_CHANNEL,
                       "connection", priv->conn,
                       "object-path", object_path,
                        "handle", handle,
                       NULL);
  
  g_message("in new_search_channel: chan is %u\n", chan);
  
  g_array_append_val(priv->channels, chan);
  
  g_signal_connect (chan, "closed", (GCallback) search_channel_closed_cb,fac);
  

  g_signal_emit_by_name (fac, "new-channel", chan);

  g_free (object_path);
  chan->channel_index = priv->channels->len - 1;
  
  //added for search as chan need to be passed as userdata in search_channel_iq_cb
  chan->iq_cb = lm_message_handler_new (search_channel_iq_cb, chan , NULL);
  lm_connection_register_message_handler (priv->conn->lmconn, chan->iq_cb,
                                          LM_MESSAGE_TYPE_IQ,
                                          LM_HANDLER_PRIORITY_NORMAL);

  return chan;
}

static void
gabble_search_factory_iface_close_all (TpChannelFactoryIface *iface)
{
  GabbleSearchFactory *fac = GABBLE_SEARCH_FACTORY (iface);
  GabbleSearchFactoryPrivate *priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);

  gabble_debug (DEBUG_FLAG, "closing channels");

  if (priv->channels)
    {
      g_array_free ( priv->channels, TRUE);
      priv->channels = NULL;
    }
}

static void
gabble_search_factory_iface_connecting (TpChannelFactoryIface *iface)
{
  /* nothing to do */
}



static void
gabble_search_factory_iface_connected (TpChannelFactoryIface *iface)
{
  /* nothing to do */
}

static void
gabble_search_factory_iface_disconnected (TpChannelFactoryIface *iface)
{
  // free any object here
  GabbleSearchFactory *fac = GABBLE_SEARCH_FACTORY (iface);
  GabbleSearchFactoryPrivate *priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);

  if (priv->channels)
    {
      g_array_free ( priv->channels, TRUE);
      priv->channels = NULL;
    }
}

struct _ForeachData
{
  TpChannelFunc foreach;
  gpointer user_data;
};

static void
_foreach_slave ( gpointer value, gpointer user_data )
{
  struct _ForeachData *data = (struct _ForeachData *) user_data;
  TpChannelIface *chan = TP_CHANNEL_IFACE (value);

  data->foreach (chan, data->user_data);
}


static void
gabble_search_factory_iface_foreach (TpChannelFactoryIface *iface, TpChannelFunc foreach, gpointer user_data)
{
  GabbleSearchFactory *fac = GABBLE_SEARCH_FACTORY (iface);
  GabbleSearchFactoryPrivate *priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);
  struct _ForeachData data;
  guint i = 0;
  GabbleSearchChannel *chan = NULL;

  data.user_data = user_data;
  data.foreach = foreach;

  for( i = 0; i< priv->channels->len ; i++ )
  {
  chan = GABBLE_SEARCH_CHANNEL ( g_array_index(priv->channels,guint,i) );
  _foreach_slave(chan, &data);	
  }
  
}

static TpChannelFactoryRequestStatus
gabble_search_factory_iface_request (TpChannelFactoryIface *iface,
                                 const gchar *chan_type,
                                 TpHandleType handle_type,
                                 guint handle,
                                 TpChannelIface **ret,
                                 GError **error)
{
  GabbleSearchFactory *fac = GABBLE_SEARCH_FACTORY (iface);
  GabbleSearchFactoryPrivate *priv = GABBLE_SEARCH_FACTORY_GET_PRIVATE (fac);
  GabbleSearchChannel *chan;

  if (strcmp (chan_type, TP_IFACE_CHANNEL_TYPE_CONTACT_SEARCH))
    return TP_CHANNEL_FACTORY_REQUEST_STATUS_NOT_IMPLEMENTED;
  
  // should always pass handle type as none and handle as zero

  if ((handle_type != TP_HANDLE_TYPE_NONE) || handle )
    return TP_CHANNEL_FACTORY_REQUEST_STATUS_NOT_AVAILABLE;

  
  g_message("in gabble_search_factory_iface_request:handle %u\n",handle);
  
  chan = new_search_channel (fac, handle);
  g_assert (chan);
  *ret = TP_CHANNEL_IFACE (chan);
  return TP_CHANNEL_FACTORY_REQUEST_STATUS_DONE;
}

static void
gabble_search_factory_iface_init (gpointer g_iface,
                              gpointer iface_data)
{
  TpChannelFactoryIfaceClass *klass = (TpChannelFactoryIfaceClass *) g_iface;

  klass->close_all = gabble_search_factory_iface_close_all;
  klass->connecting = gabble_search_factory_iface_connecting;
  klass->connected = gabble_search_factory_iface_connected;
  klass->disconnected = gabble_search_factory_iface_disconnected;
  klass->foreach = gabble_search_factory_iface_foreach;
  klass->request = gabble_search_factory_iface_request;
}

