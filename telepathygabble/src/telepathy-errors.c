/*
 * telepathy-errors.c - Source for D-Bus error types used in telepathy
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <glib.h>
#include "telepathy-errors.h"

#ifdef EMULATOR
#include "libgabble_wsd_solution.h"

	GET_STATIC_VAR_FROM_TLS(quark,gabble_errors,GQuark)
	#define quark (*GET_WSD_VAR_NAME(quark,gabble_errors, s)())		
	
#endif

#ifdef SYMBIAN
EXPORT_C
#endif
GQuark
telepathy_errors_quark (void)
{
#ifndef EMULATOR
  static GQuark quark = 0;
#endif
  
  if (!quark)
    quark = g_quark_from_static_string ("telepathy_errors");
  return quark;
}
