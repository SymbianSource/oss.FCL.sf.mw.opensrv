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

#ifndef enum_lm
#define enum_lm
#include <glib/gmessages.h>
typedef enum 
{
	LM_LOG_LEVEL_VERBOSE = 1 << (G_LOG_LEVEL_USER_SHIFT),
	LM_LOG_LEVEL_NET     = 1 << (G_LOG_LEVEL_USER_SHIFT+1),
	LM_LOG_LEVEL_PARSER  = 1 << (G_LOG_LEVEL_USER_SHIFT+2),
	LM_LOG_LEVEL_SSL     = 1 << (G_LOG_LEVEL_USER_SHIFT+3),
	LM_LOG_LEVEL_SASL    = 1 << (G_LOG_LEVEL_USER_SHIFT+4),
	LM_LOG_LEVEL_ALL     = ((1 << ((8) +1)) |
				(1 << ((8))) |
				(1 << ((8) +2)) |
				(1 << ((8) +3)) |
				(1 << ((8) +4)))
} LmLogLevelFlags;

#endif
