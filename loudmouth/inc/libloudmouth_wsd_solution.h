/*
 * Copyright (C) 2003 Imendio AB
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
 
#ifndef _LIBLOUDMOUTH_WSD_H
#define _LIBLOUDMOUTH_WSD_H
#include "libloudmouth_wsd_macros.h"
#include "lm_enums.h"
#include "ghash.h"
#include <gutils.h>
#include "lm-internals.h"
#include <glib/gtypes.h>
/*#include "lm-debug.h"
#include "lm-message.h"
#include "lm-utils.h"
#include "base64.h"
#include "lm-misc.h"
#include "lm-sha.h"*/

#if EMULATOR
#ifdef __cplusplus
extern "C" 
{
#endif
typedef struct _TypeNames TypeNames;

struct _TypeNames 
	{
    LmMessageType  type;
    const gchar   name[20];

  	};


typedef struct _SubTypeNames SubTypeNames;

struct _SubTypeNames 
	{
    LmMessageSubType  type;
    const gchar   name[20];

  	};


struct libloudmouth_global_struct
	{
	VARIABLE_DECL(debug_flags, g, lm_debug,LmLogLevelFlags)	
	VARIABLE_DECL(initialized, g, lm_debug,gboolean)
	VARIABLE_DECL(log_handler_id, g, lm_debug,guint)
	VARIABLE_DECL_ARRAY(type_names, s, lm_message,TypeNames,13)
	VARIABLE_DECL_ARRAY(sub_type_names, s, lm_message,SubTypeNames,14)
	VARIABLE_DECL(initialized, s, lm_ssl_openssl,gboolean)
	VARIABLE_DECL(last_id, s, lm_utils,guint) 
	VARIABLE_DECL_ARRAY(base64chars, s, lm_utils, gchar, 256) 
	VARIABLE_DECL_ARRAY(ret_val, s, lm_sha, gchar, 256)
	VARIABLE_DECL_ARRAY(encoding, s, lm_base64, gchar, 256) 
	VARIABLE_DECL_ARRAY(buf, s, lm_misc, char, 256)
	VARIABLE_DECL(initialised, g, lm_sock,gboolean)

    
      /*END-global vars*/
          
    };

struct libloudmouth_global_struct * libloudmouth_ImpurePtr();
int libloudmouth_Init(struct libloudmouth_global_struct *);
#ifdef __cplusplus
}
#endif
#endif //EMULATOR
#endif //header guard ifdef _LIBGABBLE_WSD_H

