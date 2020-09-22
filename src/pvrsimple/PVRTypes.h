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
#include <string>
#include <ctime>

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;

/***********************************************************
 * Server Connect Constants
 ***********************************************************/
 
/***********************************************************
 * Database Constants
 ***********************************************************/
#define DATABASE_FOLDER  "database"
#define DATABASE_FILE    "DVR.db"

/***********************************************************
 * FFMPEG Constants
 ***********************************************************/
#define FFMPEG_LOG_FOLDER "log"
#define FFMPEG_LOG_FILE   "ffmpeg.log"

/***********************************************************
 * Variable Type Lengths
 ***********************************************************/
#define PVR_ADDON_BOOL_STRING_LENGTH              5
#define PVR_ADDON_TIMERTYPE_VALUES_ARRAY_LENGTH 202

/***********************************************************
 * Timer Types Constants
 ***********************************************************/
#define TIMER_ONCE_MANUAL          (PVR_TIMER_TYPE_NONE + 1)
#define TIMER_ONCE_EPG             (PVR_TIMER_TYPE_NONE + 2)
#define TIMER_REPEATING_MANUAL     (PVR_TIMER_TYPE_NONE + 3)
#define TIMER_REPEATING_EPG        (PVR_TIMER_TYPE_NONE + 4)
#define TIMER_REPEATING_SERIESLINK (PVR_TIMER_TYPE_NONE + 5)

/***********************************************************
 * Upcoming EPG Properties
 ***********************************************************/
#define PVR_STREAM_PROPERTY_EPGPLAYBACKASLIVE "epgplaybackaslive"

/***********************************************************
 * Global Constants
 ***********************************************************/
const int CLIENT_MODE = 0;
const int SERVER_MODE = 1;

const string SERVER_IP   = "127.0.0.1";
const int    SERVER_PORT =       3000 ;

const string EPG_CHANNEL_NO_NAME = "SEE TVG-ID";
 
const int EPG_TAG_INVALID_SERIES_EPISODE =    -1;
const int SECONDS_IN_HOUR                =  3600;
const int SECONDS_IN_DAY                 = 86400;

/***********************************************************
 * Refresh Intervals & Logo Preference
 ***********************************************************/
typedef enum
{
  REFRESH_INTERVAL_START  = 0, /*!< @brief startup refresh */
  REFRESH_INTERVAL_DAILY  = 1, /*!< @brief daily refresh */
  REFRESH_INTERVAL_WEEKLY = 2, /*!< @brief weekly refresh */
} REFRESH_INTERVAL;

typedef enum
{
  LOGO_PREFERENCE_IGNORE  = 0, /*!< @brief ignore logo */
  LOGO_PREFERENCE_M3U     = 1, /*!< @brief prefer m3u logo */
  LOGO_PREFERENCE_XMLTV   = 2, /*!< @brief prefer xmltv logo */
} LOGO_PREFERENCE;

/***********************************************************
 * SQL Process
 ***********************************************************/
struct SQLMsg{
		string strTable;
		time_t iModTime;
};

class PVRRecorder;

struct SQLTask{
		unsigned int iClientIndex     ;
		int          iClientChannelUid;
		unsigned int iEpgUid          ;
		PVRRecorder* pProcess         ;
};