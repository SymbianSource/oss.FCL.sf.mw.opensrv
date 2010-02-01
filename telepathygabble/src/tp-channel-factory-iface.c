/*
 * tp-channel-factory-iface.c - Stubs for Telepathy Channel Factory interface
 *
 * Copyright (C) 2006 Collabora Ltd.
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

#include <glib-object.h>

#include "tp-channel-factory-iface.h"
#include "tp-channel-factory-iface-signals-marshal.h"
#include "tp-channel-iface.h"


#ifdef EMULATOR
#include "libgabble_wsd_solution.h"

	GET_STATIC_VAR_FROM_TLS(initialized,gabble_fac_iface,gboolean)
	#define initialized (*GET_WSD_VAR_NAME(initialized,gabble_fac_iface, s)())	

	GET_STATIC_VAR_FROM_TLS(type,gabble_fac_iface,GType)
	#define type (*GET_WSD_VAR_NAME(type,gabble_fac_iface, s)())	

#endif


static void
tp_channel_factory_iface_base_init (gpointer klass)
{
#ifndef EMULATOR
  static gboolean initialized = FALSE;
#endif

  if (!initialized) {
    initialized = TRUE;

    g_signal_new ("new-channel",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new ("channel-error",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  0,
                  NULL, NULL,
                  tp_channel_factory_iface_marshal_VOID__OBJECT_POINTER,
                  G_TYPE_NONE, 2, G_TYPE_OBJECT, G_TYPE_POINTER);
  }
}

GType
tp_channel_factory_iface_get_type (void)
{

#ifndef EMULATOR
  static GType type = 0;
#endif

  if (type == 0) {
    static const GTypeInfo info = {
      sizeof (TpChannelFactoryIfaceClass),
      tp_channel_factory_iface_base_init,   /* base_init */
      NULL,   /* base_finalize */
      NULL,   /* class_init */
      NULL,   /* class_finalize */
      NULL,   /* class_data */
      0,
      0,      /* n_preallocs */
      NULL    /* instance_init */
    };

    type = g_type_register_static (G_TYPE_INTERFACE, "TpChannelFactoryIface", &info, 0);
  }

  return type;
}

void
tp_channel_factory_iface_close_all (TpChannelFactoryIface *self)
{
  TP_CHANNEL_FACTORY_IFACE_GET_CLASS (self)->close_all (self);
}

void
tp_channel_factory_iface_connecting (TpChannelFactoryIface *self)
{
  TP_CHANNEL_FACTORY_IFACE_GET_CLASS (self)->connecting (self);
}

void
tp_channel_factory_iface_connected (TpChannelFactoryIface *self)
{
  TP_CHANNEL_FACTORY_IFACE_GET_CLASS (self)->connected (self);
}

void
tp_channel_factory_iface_disconnected (TpChannelFactoryIface *self)
{
  TP_CHANNEL_FACTORY_IFACE_GET_CLASS (self)->disconnected (self);
}

void
tp_channel_factory_iface_foreach (TpChannelFactoryIface *self,
                                  TpChannelFunc func,
                                  gpointer data)
{
  TP_CHANNEL_FACTORY_IFACE_GET_CLASS (self)->foreach (self, func, data);
}

TpChannelFactoryRequestStatus
tp_channel_factory_iface_request (TpChannelFactoryIface *self,
                                  const gchar *chan_type,
                                  TpHandleType handle_type,
                                  guint handle,
                                  TpChannelIface **ret,
                                  GError **error)
{
  return (TP_CHANNEL_FACTORY_IFACE_GET_CLASS (self)->request (self, chan_type,
        handle_type, handle, ret, error));
}
