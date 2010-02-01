/*
 * search-keys-info.h - Header for Search Keys and reported fields
 * Copyright (C) 2008 Collabora Ltd.
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

#ifndef __SEARCH_KEYS_INFO_H__
#define __SEARCH_KEYS_INFO_H__

#include "gabble-connection.h"

G_BEGIN_DECLS

void search_keys_info_discover (GabbleConnection *conn);
gboolean _gabble_submit_search_form (GabbleConnection *conn);
LmHandlerResult search_keys_iq_cb (LmMessageHandler *handler, LmConnection *lmconn, LmMessage *message, gpointer user_data);
void
get_search_keys_info (GabbleConnection *conn,  const gchar *jid );

G_END_DECLS

#endif /* __SEARCH_KEYS_INFO_H__ */
