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
* Description:    Utility class for telepathy Test Cases.
*
*/




//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>

#include <glib.h>

#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define DBUS_API_SUBJECT_TO_CHANGE
#endif


//Include files
#include <e32err.h>
#include <e32const.h>
#include <e32base.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <dbus/dbus-glib.h>
#include <glib.h>
	   
#include <unistd.h>

#include "tp-conn.h"
#include "tp-connmgr.h"
#include "tp-chan.h"
#include "tp-chan-gen.h"
#include "tp-ch-gen.h"
#include "tp-chan-iface-group-gen.h"
#include "tp-chan-type-text-gen.h"
#include "tp-chan-type-streamed-media-gen.h"
#include "tp-props-iface.h"
#include "tp-constants.h"
#include "tp-interfaces.h"

#include <pthread.h>

// FORWARD DECLARATION
class CTestData;


// CLASS DECLARATION
NONSHARABLE_CLASS(CTelepathyGabbleUtils) 
    {
    
   
    public: // static Member functions
 	    static guint get_handle_from_contact ( const gchar  *contact_id,
 	    										CTestData* aTestData);
	
 };
