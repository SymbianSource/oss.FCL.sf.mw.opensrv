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
* Description:    Used for Presence related Test Cases
*
*/




//#include "config.h"

#include <string.h>
#include "telepathy-presence.h"
//#include "telepathy-time.h"

//#include <glib/gi18n.h> //below code is replacement of this header
//+++++++++++++++++++++++++++++++++++++++++++++++++
#include <glib/gstrfuncs.h>

#define _(String) gettext (String)
#define Q_(String) g_strip_context ((String), gettext (String))
#ifdef gettext_noop
#define N_(String) gettext_noop (String)
#else
#define N_(String) (String)
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++




#define GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TELEPATHY_TYPE_PRESENCE, TelepathyPresencePriv))


typedef long TelepathyTime;         /* Note: Always in UTC. */
typedef struct _TelepathyPresencePriv TelepathyPresencePriv;

struct _TelepathyPresencePriv {
	TelepathyPresenceState  state;

	gchar               *status;
	gchar               *resource;

	gint                 priority;
	TelepathyTime           timestamp;
};

static void         presence_finalize           (GObject             *object);
static void         presence_get_property       (GObject             *object,
						 guint                param_id,
						 GValue              *value,
						 GParamSpec          *pspec);
static void         presence_set_property       (GObject             *object,
						 guint                param_id,
						 const GValue        *value,
						 GParamSpec          *pspec);

enum {
	PROP_0,
	PROP_STATE,
	PROP_STATUS,
	PROP_RESOURCE,
	PROP_PRIORITY
};

G_DEFINE_TYPE (TelepathyPresence, telepathy_presence, G_TYPE_OBJECT);

TelepathyTime
telepathy_time_get_current (void)
{
	return time (NULL);
}

static void
telepathy_presence_class_init (TelepathyPresenceClass *class)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (class);

	object_class->finalize     = presence_finalize;
	object_class->get_property = presence_get_property;
	object_class->set_property = presence_set_property;

	g_object_class_install_property (object_class,
					 PROP_STATE,
					 g_param_spec_int ("state",
							   "Presence State",
							   "The current state of the presence",
							   TELEPATHY_PRESENCE_STATE_AVAILABLE,
							   TELEPATHY_PRESENCE_STATE_EXT_AWAY,
							   TELEPATHY_PRESENCE_STATE_AVAILABLE,
							   G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_STATUS,
					 g_param_spec_string ("status",
							      "Presence Status",
							      "Status string set on presence",
							      NULL,
							      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_RESOURCE,
					 g_param_spec_string ("resource",
							      "Presence Resource",
							      "Resource that this presence is for",
							      NULL,
							      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_PRIORITY,
					 g_param_spec_int ("priority",
							   "Presence Priority",
							   "Priority value of presence",
							   G_MININT,
							   G_MAXINT,
							   0,
							   G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (TelepathyPresencePriv));
}

static void
telepathy_presence_init (TelepathyPresence *presence)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (presence);

	priv->state = TELEPATHY_PRESENCE_STATE_AVAILABLE;

	priv->status = NULL;
	priv->resource = NULL;

	priv->priority = 0;

	priv->timestamp = telepathy_time_get_current ();
}

static void
presence_finalize (GObject *object)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (object);

	g_free (priv->status);
	g_free (priv->resource);

	(G_OBJECT_CLASS (telepathy_presence_parent_class)->finalize) (object);
}

static void
presence_get_property (GObject    *object,
		       guint       param_id,
		       GValue     *value,
		       GParamSpec *pspec)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (object);

	switch (param_id) {
	case PROP_STATE:
		g_value_set_int (value, priv->state);
		break;
	case PROP_STATUS:
		g_value_set_string (value,
				    telepathy_presence_get_status (TELEPATHY_PRESENCE (object)));
		break;
	case PROP_RESOURCE:
		g_value_set_string (value,
				    telepathy_presence_get_resource (TELEPATHY_PRESENCE (object)));
		break;
	case PROP_PRIORITY:
		g_value_set_int (value, priv->priority);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}
static void
presence_set_property (GObject      *object,
		       guint         param_id,
		       const GValue *value,
		       GParamSpec   *pspec)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (object);

	switch (param_id) {
	case PROP_STATE:
		priv->state = g_value_get_int (value);
		break;
	case PROP_STATUS:
		telepathy_presence_set_status (TELEPATHY_PRESENCE (object),
					    g_value_get_string (value));
		break;
	case PROP_RESOURCE:
		telepathy_presence_set_resource (TELEPATHY_PRESENCE (object),
					      g_value_get_string (value));
		break;
	case PROP_PRIORITY:
		priv->priority = g_value_get_int (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

TelepathyPresence *
telepathy_presence_new (void)
{
	return g_object_new (TELEPATHY_TYPE_PRESENCE, NULL);
}

TelepathyPresence *
telepathy_presence_new_full (TelepathyPresenceState  state,
			  const gchar         *status)
{
	return g_object_new (TELEPATHY_TYPE_PRESENCE,
			     "state", state,
			     "status", status,
			     NULL);
}

const gchar *
telepathy_presence_get_resource (TelepathyPresence *presence)
{
	TelepathyPresencePriv *priv;

	g_return_val_if_fail (TELEPATHY_IS_PRESENCE (presence), NULL);

	priv = GET_PRIV (presence);

	if (priv->resource) {
		return priv->resource;
	}

	return NULL;
}

const gchar *
telepathy_presence_get_status (TelepathyPresence *presence)
{
	TelepathyPresencePriv *priv;

	/*g_return_val_if_fail (TELEPATHY_IS_PRESENCE (presence),
			      _("Offline"));*/

	priv = GET_PRIV (presence);

	return priv->status;
}

gint
telepathy_presence_get_priority (TelepathyPresence *presence)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (presence);
	g_return_val_if_fail (TELEPATHY_IS_PRESENCE (presence), 0);

	return priv->priority;
}

void
telepathy_presence_set_resource (TelepathyPresence *presence,
			      const gchar    *resource)
{
	TelepathyPresencePriv *priv;

	g_return_if_fail (TELEPATHY_IS_PRESENCE (presence));
	g_return_if_fail (resource != NULL);

	priv = GET_PRIV (presence);

	g_free (priv->resource);
	priv->resource = g_strdup (resource);

	g_object_notify (G_OBJECT (presence), "resource");
}

TelepathyPresenceState
telepathy_presence_get_state (TelepathyPresence *presence)
{
	TelepathyPresencePriv *priv;

	g_return_val_if_fail (TELEPATHY_IS_PRESENCE (presence),
			      TELEPATHY_PRESENCE_STATE_AVAILABLE);

	priv = GET_PRIV (presence);

	return priv->state;
}

void
telepathy_presence_set_state (TelepathyPresence      *presence,
			   TelepathyPresenceState  state)
{
	TelepathyPresencePriv *priv;

	g_return_if_fail (TELEPATHY_IS_PRESENCE (presence));

	priv = GET_PRIV (presence);

	priv->state = state;

	g_object_notify (G_OBJECT (presence), "state");
}

void
telepathy_presence_set_status (TelepathyPresence *presence,
			    const gchar    *status)
{
	TelepathyPresencePriv *priv;

	priv = GET_PRIV (presence);
	g_return_if_fail (TELEPATHY_IS_PRESENCE (presence));

	g_free (priv->status);

	if (status) {
		priv->status = g_strdup (status);
	} else {
		priv->status = NULL;
	}

	g_object_notify (G_OBJECT (presence), "status");
}

void
telepathy_presence_set_priority (TelepathyPresence *presence,
			      gint            priority)
{
	TelepathyPresencePriv *priv;

	g_return_if_fail (TELEPATHY_IS_PRESENCE (presence));

	priv = GET_PRIV (presence);

	priv->priority = priority;

	g_object_notify (G_OBJECT (presence), "priority");
}

gboolean
telepathy_presence_resource_equal (gconstpointer a,
				gconstpointer b)
{
	TelepathyPresencePriv *priv1;
	TelepathyPresencePriv *priv2;

	g_return_val_if_fail (TELEPATHY_IS_PRESENCE (a), FALSE);
	g_return_val_if_fail (TELEPATHY_IS_PRESENCE (b), FALSE);

	priv1 = GET_PRIV (a);
	priv2 = GET_PRIV (b);

	if (!priv1->resource) {
		if (!priv2->resource) {
			return TRUE;
		}

		return FALSE;
	}

	if (!priv2->resource) {
		return FALSE;
	}

	if (strcmp (priv1->resource, priv2->resource) == 0) {
		return TRUE;
	}

	return FALSE;
}

gint
telepathy_presence_sort_func (gconstpointer a,
			   gconstpointer b)
{
	TelepathyPresencePriv *priv_a;
	TelepathyPresencePriv *priv_b;
	gint                diff;

	g_return_val_if_fail (TELEPATHY_IS_PRESENCE (a), 0);
	g_return_val_if_fail (TELEPATHY_IS_PRESENCE (b), 0);

	/* We sort here by priority AND status, in theory, the
	 * priority would be enough for JUST Jabber contacts which
	 * actually abide to the protocol, but for other protocols and
	 * dodgy clients, we will sort by:
	 *   
	 *    1. State
	 *    2. Priority
	 *    3. Time it was set (most recent first).
	 */
	 
	priv_a = GET_PRIV (a);
	priv_b = GET_PRIV (b);

	/* 1. State */
	diff = priv_a->state - priv_b->state;
	if (diff != 0) {
		return diff < 1 ? -1 : +1;
	}

	/* 2. Priority */
	diff = priv_a->priority - priv_b->priority;
	if (diff != 0) {
		return diff < 1 ? -1 : +1;
	}

	/* 3. Time (newest first) */
	diff = priv_b->timestamp - priv_a->timestamp;
	if (diff != 0) {
		return diff < 1 ? -1 : +1;
	}
		
	/* No real difference, except maybe resource */
	return 0;
}


/*static*/ const gchar *
telepathy_presence_state_to_str (TelepathyPresenceState presence_state)
{
	switch (presence_state) {
	case TELEPATHY_PRESENCE_STATE_AVAILABLE:
		return "available";
	case TELEPATHY_PRESENCE_STATE_BUSY:
		return "dnd";
	case TELEPATHY_PRESENCE_STATE_AWAY:
		return "away";
	case TELEPATHY_PRESENCE_STATE_EXT_AWAY:
		return "xa";
	case TELEPATHY_PRESENCE_STATE_HIDDEN:
		return "hidden";
	case TELEPATHY_PRESENCE_STATE_UNAVAILABLE:
		return "offline";
	default:
		return NULL;
	}

	return NULL;
}
