/*
 * telepathy-errors-enumtypes.c - Source for Gabble service discovery
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
 *
 * -- LET'S DISCO!!!  \o/ \o_ _o/ /\o/\ _/o/- -\o\_ --
 */
/* Generated data (by glib-mkenums) */
//vinod: changed system path to local
#include "telepathy-errors.h"

#ifdef EMULATOR
#include "libgabble_wsd_solution.h"

	GET_STATIC_VAR_FROM_TLS(etype,gabble_enum_types,GType)
	#define etype (*GET_WSD_VAR_NAME(etype,gabble_enum_types,s)())

#endif

/* enumerations from "telepathy-errors.h" */
#ifdef SYMBIAN
EXPORT_C
#endif
GType
telepathy_errors_get_type (void)
{

#ifndef EMULATOR
  static GType etype = 0;
#endif
  
  if (etype == 0) {
    static const GEnumValue values[] = {
      { ChannelBanned, "ChannelBanned", "ChannelBanned" },
      { ChannelFull, "ChannelFull", "ChannelFull" },
      { ChannelInviteOnly, "ChannelInviteOnly", "ChannelInviteOnly" },
      { Disconnected, "Disconnected", "Disconnected" },
      { InvalidArgument, "InvalidArgument", "InvalidArgument" },
      { InvalidHandle, "InvalidHandle", "InvalidHandle" },
      { NetworkError, "NetworkError", "NetworkError" },
      { NotAvailable, "NotAvailable", "NotAvailable" },
      { NotImplemented, "NotImplemented", "NotImplemented" },
      { PermissionDenied, "PermissionDenied", "PermissionDenied" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("TelepathyErrors", values);
  }
  return etype;
}

/* Generated data ends here */

