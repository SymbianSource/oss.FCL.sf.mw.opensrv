/* tp-props-iface.c
 *
 * Copyright (C) 2005 Collabora Ltd.
 * 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <dbus/dbus-glib.h>
#include <string.h>
#include "tp-interfaces.h"
#include "tp-ifaces-signals-marshal.h"
#include "tp-props-iface-gen.h"
#include "tp-props-iface.h"

#ifdef EMULATOR
#include "libtelepathy_wsd_solution.h"
#endif


#define TP_TYPE_PROPERTY_DESCRIPTION (dbus_g_type_get_struct ("GValueArray", \
      G_TYPE_UINT, \
      G_TYPE_STRING, \
      G_TYPE_STRING, \
      G_TYPE_UINT, \
      G_TYPE_INVALID))

#define TP_TYPE_PROPERTY_CHANGE (dbus_g_type_get_struct ("GValueArray", \
      G_TYPE_UINT, \
      G_TYPE_VALUE, \
      G_TYPE_INVALID))

#define TP_TYPE_PROPERTY_FLAGS_CHANGE (dbus_g_type_get_struct ("GValueArray", \
      G_TYPE_UINT, \
      G_TYPE_UINT, \
      G_TYPE_INVALID))




/*signal enum*/
enum
{
  PROPERTIES_READY,
  PROPERTY_CHANGED,
  LAST_SIGNAL 
#ifdef EMULATOR  
  = LAST_SIGNAL_TP_PROPS_IFACE
#endif
  
};

#ifndef EMULATOR
	static guint signals[LAST_SIGNAL] = {0};
#endif

/* looking up properties is linear time on the grounds that number of properties
 * will always be small, so this will be more cache-friendly
 */
typedef struct _PropertyMapping PropertyMapping;
struct _PropertyMapping
{
  guint user_id;
  guint32 server_id;
  gchar *name;
  GValue *value;
  guint32 flags;
};

typedef struct _TpPropsPrivate TpPropsPrivate;

struct _TpPropsPrivate
{
  gboolean properties_ready;

  int mappings_len;
  PropertyMapping *mappings;
};

#ifndef EMULATOR
	static GObjectClass *parent_class = NULL;
#endif


#ifdef EMULATOR
	
	GET_STATIC_ARRAY_FROM_TLS(signals,tp_props_iface,guint)
	#define signals (GET_WSD_VAR_NAME(signals,tp_props_iface, s)())	
	
	GET_STATIC_VAR_FROM_TLS(parent_class,tp_props_iface,GObjectClass *)
	#define parent_class (*GET_WSD_VAR_NAME(parent_class,tp_props_iface,s)())
	
	GET_STATIC_VAR_FROM_TLS(type1,tp_props_iface,GType)
	#define type1 (*GET_WSD_VAR_NAME(type1,tp_props_iface,s)())
	
	GET_STATIC_VAR_FROM_TLS(ret,tp_props_iface,GQuark)
	#define ret (*GET_WSD_VAR_NAME(ret,tp_props_iface,s)())
	
#endif
	

#define PRIV(o) ((TpPropsPrivate*)(o->priv))

static void properties_listed_cb (DBusGProxy *proxy, GPtrArray *properties, GError *error, gpointer user_data);

static void tp_props_iface_init(GTypeInstance *instance, gpointer g_class)
{
  TpPropsIface *self = TELEPATHY_PROPS_IFACE(instance);

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE ((self),
                 TELEPATHY_PROPS_IFACE_TYPE, TpPropsPrivate);

}

static GObject *
tp_props_iface_constructor (GType type, guint n_props,
                             GObjectConstructParam *props)
{
  GObject *obj;

  obj = G_OBJECT_CLASS (parent_class)->
           constructor (type, n_props, props);

  dbus_g_proxy_add_signal(DBUS_G_PROXY(obj), "PropertiesChanged",
      dbus_g_type_get_collection ("GPtrArray", TP_TYPE_PROPERTY_CHANGE),
                                  G_TYPE_INVALID);
  dbus_g_proxy_add_signal(DBUS_G_PROXY(obj), "PropertyFlagsChanged",
      dbus_g_type_get_collection ("GPtrArray", TP_TYPE_PROPERTY_FLAGS_CHANGE),
                                  G_TYPE_INVALID);

  return obj;
}

static void tp_props_iface_dispose(GObject *obj)
{

  /* Call parent class dispose method */
  if (G_OBJECT_CLASS(parent_class)->dispose)
  {
    G_OBJECT_CLASS(parent_class)->dispose(obj);
  }

}


static void tp_props_iface_finalize(GObject *obj)
{
  TpPropsIface *self = TELEPATHY_PROPS_IFACE(obj);
  int i;
  for (i=0; i < PRIV(self)->mappings_len; i++)
    {
      if (PRIV(self)->mappings[i].value)
        {
          g_value_unset (PRIV(self)->mappings[i].value);
          g_free (PRIV(self)->mappings[i].value);
        }
      if (PRIV(self)->mappings[i].name)
        g_free (PRIV(self)->mappings[i].name);
    }
  
  g_free (PRIV(self)->mappings);

  if (G_OBJECT_CLASS(parent_class)->finalize)
    {
      G_OBJECT_CLASS(parent_class)->finalize(obj);
    }
}


static void tp_props_iface_class_init(TpPropsIfaceClass *klass)
{
  GObjectClass *obj = G_OBJECT_CLASS(klass);
  parent_class = g_type_class_peek_parent(klass);

  obj->set_property = parent_class->set_property;
  obj->get_property = parent_class->get_property;

  obj->constructor = tp_props_iface_constructor;
  obj->dispose = tp_props_iface_dispose;
  obj->finalize = tp_props_iface_finalize;

  g_type_class_add_private (klass, sizeof (TpPropsPrivate));
  /**
   * TpPropsIface::properties-ready:
   * @self: #TpPropsIface that emmitted the signal
   * @property_id: property that changed
   * @change_flags: #TpPropsChanged for what changed on the property
   *
   * This signal is emitted when the properties 1st become avaible for
   * reading or writing.
   */

  signals[PROPERTIES_READY] =
  g_signal_new ("properties-ready",
                G_OBJECT_CLASS_TYPE (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                0,
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);



  /**
   * TpPropsIface::properties-changed:
   * @self: #TpPropsIface that emmitted the signal
   * @property_id: property that changed
   * @change_flags: #TpPropsChanged for what changed on the property
   *
   * This signal is emitted when a property changes.
   */

  signals[PROPERTY_CHANGED] =
  g_signal_new ("properties-changed",
                G_OBJECT_CLASS_TYPE (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                0,
                NULL, NULL,
                tp_ifaces_signals_marshal_VOID__UINT_UINT,
                G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);


  /* register marshaller for  PropertiesChanged and PropertyFlagsChanged*/
  dbus_g_object_register_marshaller(tp_ifaces_signals_marshal_VOID__UINT_UINT_UINT, G_TYPE_NONE, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID);

}

#ifdef SYMBIAN
EXPORT_C
#endif
GType tp_props_iface_get_type(void)
{
#ifndef EMULATOR
  static GType type1 = 0;
#endif
  
  if (type1 == 0)
    {
      static const GTypeInfo info =
        {
          sizeof(TpPropsIfaceClass),
          NULL,
          NULL,
          (GClassInitFunc)tp_props_iface_class_init,
          NULL,
          NULL,
          sizeof(TpPropsIface),
          0,
          (GInstanceInitFunc)tp_props_iface_init
        };
      type1 = g_type_register_static(DBUS_TYPE_G_PROXY,
                                    "TpPropsIface", &info, 0);
    }
  return type1;
}

/* The interface name getters */
#ifdef SYMBIAN
EXPORT_C
#endif
GQuark
tp_get_props_interface (void)
{
#ifndef EMULATOR
  static GQuark ret = 0;
#endif

  if (ret == 0)
  {
    ret = g_quark_from_static_string(TP_IFACE_PROPERTIES);
  }

  return ret;
}

TpPropsIface *
tp_props_iface_new (DBusGConnection *connection,
                   const char      *name,
                   const char      *path_name)
{
  /* The properties are order dependant in dbus <= 0.61. Thanks dbus*/
  return g_object_new (TELEPATHY_PROPS_IFACE_TYPE,
                       "name", name,
                       "path", path_name,
                       "interface",TP_IFACE_PROPERTIES,
                       "connection", connection,
                       NULL);
}

static void properties_changed_cb (DBusGProxy *proxy, GPtrArray *properties, gpointer user_data);
static void property_flags_changed_cb (DBusGProxy *proxy, GPtrArray *properties, gpointer user_data);

/**
 * tp_props_iface_set_mapping:
 * @iface: #TpPropsIface on which to set mapping
 * @first_name: First name in list to set a mapping for
 *
 * Set a mapping between propery names and your chosen ID's for these
 * names. Takes a list of property name, id pairs, terminated by NULL.
 *
 * Typically the user will define an enum of properties that they're
 * interested in, and set a mapping like:
 *   tp_props_iface_set_mapping (props, "foo", FOO,
 *                                     "bar", BAR,
 *                                     "baz", BAZ,
 *                                     NULL);
 * the user should bind to the
 * <link linkend="TpPropsIface-properties-ready">properties-ready signal</link>
 * before calling this. Property queries will only be possible *after* this
 * signal has been emitted.
 */
void tp_props_iface_set_mapping (TpPropsIface *self,
                                const gchar *first_property_name,
                                ...)
{
  va_list var_args;
  const gchar *name = first_property_name;
  guint id;
  GArray *array;
  PropertyMapping map = {0,0,NULL,NULL,0};

  g_return_if_fail (TELEPATHY_IS_PROPS_IFACE (self));
  g_return_if_fail (PRIV(self)->mappings == NULL);

  va_start (var_args, first_property_name);

  array = g_array_new (FALSE, FALSE, sizeof (PropertyMapping));

  while (name)
    {
      id = va_arg (var_args, guint);
      map.user_id = id;
      map.name = g_strdup (name);
      g_array_append_val (array, map);
      name = va_arg (var_args, gchar *);
    }

  va_end (var_args);

  PRIV (self)->mappings_len = array->len;
  PRIV (self)->mappings = (PropertyMapping*) g_array_free (array, FALSE);

  dbus_g_proxy_connect_signal (DBUS_G_PROXY (self), "PropertiesChanged",
                               G_CALLBACK(properties_changed_cb), self, NULL);
  dbus_g_proxy_connect_signal (DBUS_G_PROXY (self), "PropertyFlagsChanged",
                               G_CALLBACK(property_flags_changed_cb),
                               self, NULL);

  tp_props_iface_list_properties_async (DBUS_G_PROXY (self),
                                        properties_listed_cb, self);
}

/**
 * tp_props_iface_get_value:
 * @self: #TpPropsIface on which to get a property value
 * @prop_id: Identifier for property as set in #tp_props_iface_set_mapping
 * @value: GValue to return the property's value in.
 *
 * Get the value of a property on this interface
 */
gboolean tp_props_iface_get_value (TpPropsIface* self, guint prop_id,
                                  GValue *return_value)
{
  int i;

  if (!PRIV (self)->properties_ready)
    return FALSE;

  for (i = 0; i < PRIV (self)->mappings_len; i++)
    {
      if (PRIV (self)->mappings[i].user_id == prop_id)
        {
          if (PRIV (self)->mappings[i].value)
            {
              g_value_copy (PRIV (self)->mappings[i].value, return_value);
              return TRUE;
            }
          else
            {
              return FALSE;
            }
        }
    }

  return FALSE;
}

/* dummy callback handler for async calling calls with no return values */
static void
dummy_callback (DBusGProxy *proxy, GError *error, gpointer user_data)
{
  if (error)
    {
      g_warning ("%s calling %s", error->message, (char*)user_data);
      g_error_free (error);
    }
}

/**
 * tp_props_iface_set_value:
 * @self: #TpPropsIface on which to set a property value
 * @prop_id: Identifier for property as set in #tp_props_iface_set_mapping
 * @value: GValue to use to set the property's value
 *
 * Set the value of a property on this interface
 */
gboolean tp_props_iface_set_value (TpPropsIface* self, guint prop_id,
                                  const GValue *value)
{
  /*TODO add option for an error callback*/
  int i;
  GPtrArray *props;

  if (!PRIV(self)->properties_ready)
    return FALSE;

  for (i=0; i < PRIV(self)->mappings_len; i++)
    {
      if (PRIV(self)->mappings[i].user_id == prop_id)
        {
          GValue prop = {0,};
          g_value_init (&prop, TP_TYPE_PROPERTY_CHANGE);
          g_value_take_boxed (&prop,
              dbus_g_type_specialized_construct (TP_TYPE_PROPERTY_CHANGE));

          dbus_g_type_struct_set (&prop,
              0, PRIV(self)->mappings[i].server_id,
              1, value,
              G_MAXUINT);

          props = g_ptr_array_sized_new (1);
          g_ptr_array_add (props, g_value_get_boxed (&prop));
          tp_props_iface_set_properties_async (DBUS_G_PROXY(self), props,
                                               dummy_callback, "SetProperties");
          g_value_unset (&prop);
          g_ptr_array_free (props, TRUE);
          return TRUE;
        }
    }
  return FALSE;
}


static void
set_properties_values (TpPropsIface *self, GPtrArray *properties)
{
  int i,j;

  for (i = 0; i < properties->len; i++)
    {
      GValue property = {0};
      guint32 id;
      GValue *value;

      g_value_init (&property, TP_TYPE_PROPERTY_CHANGE);
      g_value_set_static_boxed (&property, g_ptr_array_index (properties, i));
      dbus_g_type_struct_get (&property, 0, &id, G_MAXUINT);

      for (j = 0; j < PRIV(self)->mappings_len; j++)
        {
          PropertyMapping *mapping = &(PRIV (self)->mappings[j]);

          if (mapping->server_id == id)
            {
              dbus_g_type_struct_get (&property, 1, &value, G_MAXUINT);
              g_assert (value);

              if (mapping->value)
                {
                  g_value_unset (mapping->value);
                  g_free (mapping->value);
                }

              mapping->value = value;
              value = NULL; /* just to be on the safe side... */

              if (PRIV (self)->properties_ready)
                g_signal_emit (self, signals[PROPERTY_CHANGED], 0,
                               mapping->user_id, TP_PROPS_CHANGED_VALUE);

              break;
            }
        }
    }

}

static void
properties_changed_cb (DBusGProxy *proxy, GPtrArray *properties,
                       gpointer user_data)
{
  TpPropsIface *self = TELEPATHY_PROPS_IFACE (user_data);
  if (!PRIV(self)->properties_ready)
    return;
  set_properties_values (self, properties);
}

static void
properties_got_cb (DBusGProxy *proxy, GPtrArray *properties, GError *error, gpointer user_data)
{
  TpPropsIface *self = TELEPATHY_PROPS_IFACE (user_data);

  if (error)
    {
      g_debug ("getting properties failed: %s (%s)", error->message,
        dbus_g_error_get_name (error));
      g_error_free (error);
      return;
    }

  set_properties_values (self, properties);

  if (!PRIV (self)->properties_ready)
    {
      PRIV (self)->properties_ready = TRUE;
      g_signal_emit (self, signals[PROPERTIES_READY], 0);
    }
}

static void
property_flags_changed_cb (DBusGProxy *proxy, GPtrArray *properties,
                       gpointer user_data)
{
  TpPropsIface *self = TELEPATHY_PROPS_IFACE (user_data);
  GArray *get_props;
  int i, j;

  if (!PRIV(self)->properties_ready)
    return;

  get_props = g_array_sized_new (FALSE, FALSE, sizeof (guint32),
                                 properties->len);

  for (i = 0; i < properties->len; i++)
    {
      GValue property = {0};
      guint32 id, flags;

      g_value_init (&property, TP_TYPE_PROPERTY_CHANGE);
      g_value_set_static_boxed (&property, g_ptr_array_index (properties, i));
      dbus_g_type_struct_get (&property, 0, &id, G_MAXUINT);

      for (j = 0; j < PRIV (self)->mappings_len; j++)
        {
          PropertyMapping *mapping = &(PRIV (self)->mappings[j]);

          if (mapping->server_id == id)
            {
              dbus_g_type_struct_get (&property, 1, &flags, G_MAXUINT);

              if (!(mapping->flags & TP_PROPERTY_FLAG_READ) &&
                  flags & TP_PROPERTY_FLAG_READ)
                /* property has become readable; fetch it */
                g_array_append_val (get_props, mapping->server_id);

              mapping->flags = flags;
              g_signal_emit (self, signals[PROPERTY_CHANGED], 0,
                mapping->user_id, TP_PROPS_CHANGED_FLAGS);
              break;
            }
        }
    }

  tp_props_iface_get_properties_async (DBUS_G_PROXY (self), get_props,
                                       properties_got_cb, self);
  g_array_free (get_props, TRUE);
}

static void
properties_listed_cb (DBusGProxy *proxy, GPtrArray *properties, GError *error, gpointer user_data)
{
  TpPropsIface *self = TELEPATHY_PROPS_IFACE (user_data);
  int i,j;
  guint32 id, flags;
  gchar *name;
  GArray *get_props;

  if (error)
    {
      g_debug ("listing properties failed: %s (%s)", error->message,
        dbus_g_error_get_name (error));
      g_error_free (error);
      return;
    }

  for (i = 0; i < properties->len; i++)
    {
      GValue property = {0};
      g_value_init (&property, TP_TYPE_PROPERTY_DESCRIPTION);
      g_value_set_static_boxed (&property, g_ptr_array_index (properties, i));

      dbus_g_type_struct_get (&property,
          0, &id,
          1, &name,
          3, &flags,
          G_MAXUINT);

      for (j = 0; j < PRIV (self)->mappings_len; j++)
        {
          if (0 == strcmp (PRIV (self)->mappings[j].name, name))
            {
              PRIV (self)->mappings[j].server_id = id;
              PRIV (self)->mappings[j].flags = flags;
            }
        }

      g_free (name);
    }

  get_props = g_array_sized_new (FALSE, FALSE, sizeof (guint32),
                                 properties->len);

  for (i = 0; i < PRIV(self)->mappings_len; i++)
    {
      PropertyMapping *mapping = &(PRIV(self)->mappings[i]);

      if (mapping->flags & TP_PROPERTY_FLAG_READ)
        g_array_append_val (get_props, mapping->server_id);
    }

  tp_props_iface_get_properties_async (DBUS_G_PROXY (self), get_props,
                                       properties_got_cb, self);
  g_array_free (get_props, TRUE);
}

void
tp_props_interface_set_signatures (DBusGProxy *proxy)
{
  dbus_g_proxy_add_signal(proxy, "PropertiesChanged",
      dbus_g_type_get_collection ("GPtrArray",
          dbus_g_type_get_struct ("GValueArray", G_TYPE_UINT,
                                   G_TYPE_VALUE, G_TYPE_INVALID)),
      G_TYPE_INVALID);
  dbus_g_proxy_add_signal(proxy, "PropertyFlagsChanged",
      dbus_g_type_get_collection ("GPtrArray",
          dbus_g_type_get_struct ("GValueArray", G_TYPE_UINT,
                                   G_TYPE_UINT, G_TYPE_INVALID)),
      G_TYPE_INVALID);
}

