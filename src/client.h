#pragma once
/*
 *  pvr.sql - A PVR client for Kodi using M3U, XMLTV, and FFMPEG2
 *  Copyright © 2018 El_Gonz87 (Gonzalo Vega)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/***********************************************************
 * Headers
 ***********************************************************/
#include <kodi/libXBMC_addon.h>
#include <kodi/libXBMC_pvr.h>
#include <kodi/libKODI_guilib.h>

#include "IPTVClient.h"
#include "DVRClient.h"
#include "pvrsimple/TCPServer.h"
#include "pvrsimple/TCPClient.h"
#include "pvrsimple/SQLConnection.h"
#include "pvrsimple/Settings.h"
#include "pvrsimple/utilities/Callbacks.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;
using namespace ADDON;

/***********************************************************
 * Kodi Objects
 ***********************************************************/
extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr          *PVR;
extern CHelper_libKODI_guilib       *GUI;