#pragma once
/*
 *  pvr.sql - A PVR client for Kodi using M3U, XMLTV, and FFMPEG
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
#include <kodi/xbmc_pvr_types.h>

#include "../utilities/Utilities.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;

/***********************************************************
 * Define Constants
 ***********************************************************/
#define M3U_START_MARKER           "#EXTM3U"
#define M3U_INFO_MARKER            "#EXTINF"
#define TVG_INFO_ID_MARKER         "tvg-id="
#define TVG_INFO_NAME_MARKER       "tvg-name="
#define TVG_INFO_LOGO_MARKER       "tvg-logo="
#define TVG_INFO_SHIFT_MARKER      "tvg-shift="
#define TVG_INFO_CHNO_MARKER       "tvg-chno="
#define M3U_GROUP_NAME_MARKER      "group-title="
#define M3U_RADIO_MARKER           "radio="
#define M3U_HIDDEN_MARKER          "hidden="

/***********************************************************
 * Parse Function Definitions
 ***********************************************************/
string ReadM3UMarkerValue(const string&, const char*, const char* = "");

/***********************************************************
 * Line Function Definitions
 ***********************************************************/
string GetDisplayName(const string&);