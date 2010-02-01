/*
 * Copyright (C) 2003-2007 Imendio AB
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary/subsidiaries.
 * Copyright (C) 2007 Collabora Ltd.
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

#include <e32std.h> 
#include "libloudmouth_wsd_solution.h"
#include <string.h>


#include <pls.h> // For emulator WSD API 

const TUid KLibLoudmouthUid3 = {0x10281F99};     // This is the UID of the library
struct libloudmouth_global_struct *libloudmouth_ImpurePtr()
{
	#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct libloudmouth_global_struct *g = Pls<struct libloudmouth_global_struct>(KLibLoudmouthUid3, &libloudmouth_Init);	
    return g;

	#else

	return NULL;	

	#endif
};

int libloudmouth_Init(libloudmouth_global_struct *g)
{
 
TypeNames type_names[14] = {
	{ LM_MESSAGE_TYPE_MESSAGE,         "message"         },
	{ LM_MESSAGE_TYPE_PRESENCE,        "presence"        },
	{ LM_MESSAGE_TYPE_IQ,              "iq"              },
	{ LM_MESSAGE_TYPE_STREAM,          "stream:stream"   },
	{ LM_MESSAGE_TYPE_STREAM_FEATURES, "stream:features" },
	{ LM_MESSAGE_TYPE_STREAM_ERROR,    "stream:error"    },
	{ LM_MESSAGE_TYPE_AUTH,            "auth"            },
	{ LM_MESSAGE_TYPE_CHALLENGE,       "challenge"       },
	{ LM_MESSAGE_TYPE_RESPONSE,        "response"        },
	{ LM_MESSAGE_TYPE_SUCCESS,         "success"         },
	{ LM_MESSAGE_TYPE_FAILURE,         "failure"         },
	{ LM_MESSAGE_TYPE_PROCEED,         "proceed"         },
	{ LM_MESSAGE_TYPE_STARTTLS,        "starttls"        },
	{ LM_MESSAGE_TYPE_UNKNOWN,         NULL              }

};

SubTypeNames sub_type_names[14] = {
	{ LM_MESSAGE_SUB_TYPE_NORMAL,          "normal"        },
        { LM_MESSAGE_SUB_TYPE_CHAT,            "chat"          },
	{ LM_MESSAGE_SUB_TYPE_GROUPCHAT,       "groupchat"     },
	{ LM_MESSAGE_SUB_TYPE_HEADLINE,        "headline"      },
	{ LM_MESSAGE_SUB_TYPE_UNAVAILABLE,     "unavailable"   },
        { LM_MESSAGE_SUB_TYPE_PROBE,           "probe"         },
	{ LM_MESSAGE_SUB_TYPE_SUBSCRIBE,       "subscribe"     },
	{ LM_MESSAGE_SUB_TYPE_UNSUBSCRIBE,     "unsubscribe"   },
	{ LM_MESSAGE_SUB_TYPE_SUBSCRIBED,      "subscribed"    },
	{ LM_MESSAGE_SUB_TYPE_UNSUBSCRIBED,    "unsubscribed"  },
	{ LM_MESSAGE_SUB_TYPE_GET,             "get"           },
	{ LM_MESSAGE_SUB_TYPE_SET,             "set"           },
	{ LM_MESSAGE_SUB_TYPE_RESULT,          "result"        }, 
	{ LM_MESSAGE_SUB_TYPE_ERROR,           "error"         }
};
gchar base_64_chars[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

gchar encoding1[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

gchar retval[41] = "";

char buffer[256] = "";

	  if(g)
	  {	
       //g->GET_WSD_VAR_NAME(debug_flags,lm_debug,g) = Flags;

	   g->GET_WSD_VAR_NAME(initialized,lm_debug,g) = EFalse;
	   g->GET_WSD_VAR_NAME(log_handler_id,lm_debug,g) = 0;
	   memcpy (&(g->GET_WSD_VAR_NAME(type_names,lm_message,s)), type_names, 13*sizeof(TypeNames));
	   memcpy (&(g->GET_WSD_VAR_NAME(sub_type_names,lm_message,s)), sub_type_names, 14*sizeof(SubTypeNames));
       g->GET_WSD_VAR_NAME(initialized,lm_ssl_openssl,s) = EFalse;
       g->GET_WSD_VAR_NAME(last_id,lm_utils, s) = 0;
       
       strcpy((g->GET_WSD_VAR_NAME(base64chars,lm_utils,s)), base_64_chars);
       
       strcpy((g->GET_WSD_VAR_NAME(ret_val,lm_sha,s)), retval);
       
       strcpy((g->GET_WSD_VAR_NAME(encoding,lm_base64,s)), encoding1);
       
       strcpy((g->GET_WSD_VAR_NAME(buf,lm_misc,s)), buffer);
       
       g->GET_WSD_VAR_NAME(initialised,lm_sock,g) = EFalse;
		return 0;
       }
  return 1;
}

