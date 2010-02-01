/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2004 Imendio AB
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef __LM_PROXY_H__
#define __LM_PROXY_H__

#if !defined (LM_INSIDE_LOUDMOUTH_H) && !defined (LM_COMPILATION)
#error "Only <loudmouth/loudmouth.h> can be included directly, this file may disappear or change contents."
#endif

G_BEGIN_DECLS

typedef struct _LmProxy LmProxy;

typedef enum {
	LM_PROXY_TYPE_NONE = 0,
	LM_PROXY_TYPE_HTTP
} LmProxyType;

IMPORT_C LmProxy *     lm_proxy_new              (LmProxyType         type);
IMPORT_C LmProxy *     lm_proxy_new_with_server  (LmProxyType         type,
					 const gchar        *server,
					 guint               port);

IMPORT_C LmProxyType   lm_proxy_get_type         (LmProxy            *proxy);
IMPORT_C void          lm_proxy_set_type         (LmProxy            *proxy,
					 LmProxyType         type);

IMPORT_C const gchar * lm_proxy_get_server       (LmProxy            *proxy);
IMPORT_C void          lm_proxy_set_server       (LmProxy            *proxy,
					 const gchar        *server);

IMPORT_C guint         lm_proxy_get_port         (LmProxy            *proxy);
IMPORT_C void          lm_proxy_set_port         (LmProxy            *proxy,
					 guint               port);

IMPORT_C const gchar * lm_proxy_get_username     (LmProxy            *proxy);
IMPORT_C void          lm_proxy_set_username     (LmProxy            *proxy,
					 const gchar        *username);

IMPORT_C const gchar * lm_proxy_get_password     (LmProxy            *proxy);
IMPORT_C void          lm_proxy_set_password     (LmProxy            *proxy,
					 const gchar        *password);

IMPORT_C LmProxy *     lm_proxy_ref              (LmProxy            *proxy);
IMPORT_C void          lm_proxy_unref            (LmProxy            *proxy);

G_END_DECLS
#endif /* __LM_PROXY_H__ */

