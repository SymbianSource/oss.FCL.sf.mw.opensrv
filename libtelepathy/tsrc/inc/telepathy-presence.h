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
* Description:    Used for Presence related Test Cases.
*
*/



#ifndef __TELEPATHY_PRESENCE_H__
#define __TELEPATHY_PRESENCE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define TELEPATHY_TYPE_PRESENCE         (telepathy_presence_get_type ())
#define TELEPATHY_PRESENCE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TELEPATHY_TYPE_PRESENCE, TelepathyPresence))
#define TELEPATHY_PRESENCE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), TELEPATHY_TYPE_PRESENCE, TelepathyPresenceClass))
#define TELEPATHY_IS_PRESENCE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TELEPATHY_TYPE_PRESENCE))
#define TELEPATHY_IS_PRESENCE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TELEPATHY_TYPE_PRESENCE))
#define TELEPATHY_PRESENCE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TELEPATHY_TYPE_PRESENCE, TelepathyPresenceClass))

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _TelepathyPresence      TelepathyPresence;
typedef struct _TelepathyPresenceClass TelepathyPresenceClass;

#ifdef __cplusplus
}
#endif

struct _TelepathyPresence {
	GObject parent;
};

struct _TelepathyPresenceClass {
	GObjectClass parent_class;
};

#ifdef __cplusplus
extern "C"
{
#endif
typedef enum {
	TELEPATHY_PRESENCE_STATE_AVAILABLE,
	TELEPATHY_PRESENCE_STATE_BUSY,
	TELEPATHY_PRESENCE_STATE_AWAY,
	TELEPATHY_PRESENCE_STATE_EXT_AWAY,
	TELEPATHY_PRESENCE_STATE_HIDDEN,      /* When you appear offline to others */
	TELEPATHY_PRESENCE_STATE_UNAVAILABLE,
} TelepathyPresenceState;

#ifdef __cplusplus
}
#endif
GType               telepathy_presence_get_type                 (void) G_GNUC_CONST;

TelepathyPresence *    telepathy_presence_new                      (void);
TelepathyPresence *    telepathy_presence_new_full                 (TelepathyPresenceState  state,
							      const gchar         *status);

const gchar *       telepathy_presence_get_resource             (TelepathyPresence      *presence);
TelepathyPresenceState telepathy_presence_get_state                (TelepathyPresence      *presence);
const gchar *       telepathy_presence_get_status               (TelepathyPresence      *presence);
gint                telepathy_presence_get_priority             (TelepathyPresence      *presence);

void                telepathy_presence_set_resource             (TelepathyPresence      *presence,
							      const gchar         *resource);
void                telepathy_presence_set_state                (TelepathyPresence      *presence,
							      TelepathyPresenceState  state);
void                telepathy_presence_set_status               (TelepathyPresence      *presence,
							      const gchar         *status);
void                telepathy_presence_set_priority             (TelepathyPresence      *presence,
							      gint                 priority);
gboolean            telepathy_presence_resource_equal           (gconstpointer        a,
							      gconstpointer        b);
gint                telepathy_presence_sort_func                (gconstpointer        a,
							      gconstpointer        b);

/*static*/ const gchar *telepathy_presence_state_to_str        (TelepathyPresenceState                   presence_state);



G_END_DECLS

#endif /* __TELEPATHY_PRESENCE_H__ */

