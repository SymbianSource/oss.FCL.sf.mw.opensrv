/* 
 *
 * Copyright (C) 2008 Nokia Corporation.
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
#include <e32std.h> 
#include "libtelepathy_wsd_solution.h"
#include <pls.h> // For emulator WSD API 

const TUid KLibTelUid3 = {0x2000F850};     // This is the UID of the library

struct libtelepathy_global_struct* libtelepathy_ImpurePtr()
{
	  #if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct libtelepathy_global_struct *g = Pls<struct libtelepathy_global_struct>(KLibTelUid3, &libtelepathy_Init);	
    return g;

	#else

	return NULL;	

	#endif

}

int libtelepathy_Init(libtelepathy_global_struct *g)
{
	  if(g)
	  {  	
	  		
	  	  g->GET_WSD_VAR_NAME(parent_class,tp_chan,s) = NULL;
	  	  g->GET_WSD_VAR_NAME(type1,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret1,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret2,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret3,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret4,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret5,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret6,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret7,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret8,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret9,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret10,tp_chan,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret11,tp_chan,s) = 0;
	  	  
	  	  g->GET_WSD_VAR_NAME(parent_class,tp_conn,s) = NULL;
	  	  g->GET_WSD_VAR_NAME(type1,tp_conn,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret1,tp_conn,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret2,tp_conn,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret3,tp_conn,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret4,tp_conn,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret5,tp_conn,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret6,tp_conn,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret7,tp_conn,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret8,tp_conn,s) = 0;	  	  
	  	  
	  	  g->GET_WSD_VAR_NAME(parent_class,tp_connmgr,s) = NULL;
	  	  g->GET_WSD_VAR_NAME(type1,tp_connmgr,s) = 0;
	  	  
	  	  g->GET_WSD_VAR_NAME(bus_proxy,tp_helpers,s) = NULL;
	  	  g->GET_WSD_VAR_NAME(bus1,tp_helpers,s) = NULL;
	  	  
	  	  
	  	  g->GET_WSD_VAR_NAME(parent_class,tp_props_iface,s) = NULL;
	  	  g->GET_WSD_VAR_NAME(type1,tp_props_iface,s) = 0;
	  	  g->GET_WSD_VAR_NAME(ret,tp_props_iface,s) = 0;
	  	  
	  	  memset(&(g->GET_WSD_VAR_NAME(signals,tp_props_iface,s)),0,LAST_SIGNAL_TP_PROPS_IFACE*sizeof(guint));	  
	  	  
			return 0;	
	  }
	  return 1;
}

