/*
 * libgabble_wsd_macros.h - Header 
 * Copyright (C) 2006 Collabora Ltd.
 *   @author Ole Andre Vadla Ravnaas <ole.andre.ravnaas@collabora.co.uk>
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

#ifndef _LIBGABBLEMACROS_WSD_DEFS_H_
#define _LIBGABBLEMACROS_WSD_DEFS_H_

#if (defined(SYMBIAN) && (defined(__WINSCW__) || defined(__WINS__)))
#if EMULATOR
#undef EMULATOR
#endif
#ifdef GET_WSD_VAR_NAME
#undef GET_WSD_VAR_NAME
#endif

#define EMULATOR (defined(SYMBIAN) && (defined(__WINSCW__) || defined(__WINS__)))
#define GET_WSD_VAR_NAME(var,filename,prefix) 	_##prefix##_##filename##_##var

#define RETURN_WSD_VAR(var,filename,prefix) (libgabble_ImpurePtr()->GET_WSD_VAR_NAME(var,filename,prefix))

#define GET_WSD_VAR_FROM_TLS(var,filename,type,prefix)\
type *_##prefix##_##filename##_##var()\
{\
	return (&RETURN_WSD_VAR(var,filename,prefix));\
}

#define GET_WSD_ARRAY_FROM_TLS(var,filename,type,prefix)\
type *_##prefix##_##filename##_##var()\
{\
	return (RETURN_WSD_VAR(var,filename,prefix));\
}

#define VARIABLE_DECL(var,varprefix,filename,datatype)\
datatype GET_WSD_VAR_NAME(var,filename,varprefix);

#define VARIABLE_DECL_ARRAY(var,prefix,filename,datatype,size) \
datatype GET_WSD_VAR_NAME(var,filename,prefix)[size];

#define VARIABLE_DECL_2DARRAY(var,prefix,filename,datatype,size,size1) \
datatype GET_WSD_VAR_NAME(var,filename,prefix)[size][size1];


#define GET_STATIC_VAR_FROM_TLS(var,filename,type) 		GET_WSD_VAR_FROM_TLS(var,filename,type,s)

#define GET_GLOBAL_VAR_FROM_TLS(var,filename,type) 	GET_WSD_VAR_FROM_TLS(var,filename,type,g)

#define GET_STATIC_ARRAY_FROM_TLS(var,filename,type) 	GET_WSD_ARRAY_FROM_TLS(var,filename,type,s)

#define GET_GLOBAL_ARRAY_FROM_TLS(var,filename,type) 	GET_WSD_ARRAY_FROM_TLS(var,filename,type,g)

#endif
#endif		//_LIBGABBLEMACROS_WSD_DEFS_H_