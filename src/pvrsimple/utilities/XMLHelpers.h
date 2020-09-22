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
using namespace rapidxml;

/***********************************************************
 * Define Constants
 ***********************************************************/
#define XMLTV_START_MARKER         "tv"
#define XMLTV_CHANNEL_MARKER       "channel"
#define XMLTV_CHANNEL_ID_MARKER    "id"
#define XMLTV_CHANNEL_NAME_MARKER  "display-name"
#define XMLTV_PROGRAM_MARKER       "programme"
#define XMLTV_TITLE_MARKER         "title"
#define XMLTV_EPISODE_MARKER       "sub-title"
#define XMLTV_PLOT_MARKER          "desc"
#define XMLTV_STARTTM_MARKER       "start"
#define XMLTV_STOPTM_MARKER        "stop"
#define XMLTV_ICON_MARKER          "icon"
#define XMLTV_CREDITS_MARKER       "credits"
#define XMLTV_ACTOR_MARKER         "actor"
#define XMLTV_DIRECTOR_MARKER      "director"
#define XMLTV_WRITER_MARKER        "writer"
#define XMLTV_DATE_MARKER          "date"
#define XMLTV_SOURCE_MARKER        "src"
#define XMLTV_GENRE_MARKER         "category"
#define XMLTV_SEASON_MARKER        "episode-num"
#define XMLTV_SEASON_ATTRIB_MARKER "system"
#define XMLTV_SEASON_ATTRIB_VALUE  "xmltv_ns"
#define XMLTV_SERIES_ATTRIB_VALUE  "thetvdb.com"
#define XMLTV_STAR_RATING_MARKER   "star-rating"
#define XMLTV_STAR_RATING_VALUE    "value"

static const float STAR_RATING_SCALE = 10.0f;

/***********************************************************
 * Node Function Definitions
 ***********************************************************/
string GetXMLAttributeValue     (const xml_node<>*, const char*, const char*                                       );
string GetXMLNodeValue          (const xml_node<>*, const char*, const char*, const char*                          );
string GetXMLChildAttributeValue(const xml_node<>*, const char*, const char*, const char*                          );
string GetXMLChildNodeValue     (const xml_node<>*, const char*, const char*, const char*, const char*             );
string GetXMLNodeValueAttr      (const xml_node<>*, const char*, const char*, const char*, const char*, const char*);

/***********************************************************
 * Parse Function Definitions
 ***********************************************************/
int          ParseXMLYear      (const string&);
time_t       ParseXMLDate      (const string&);
unsigned int ParseXMLWeekDay   (const time_t );
string       ParseXMLTime      (const time_t );
time_t       ParseXMLDateTime  (const string&);
int          ParseXMLStarRating(const string&);
int          ParseXMLSeason    (const string&);
int          ParseXMLEpisode   (const string&);
int          ParseXMLEpPart    (const string&);
string       ParseXMLSeries    (const string&);

/***********************************************************
 * Date Function Definitions
 ***********************************************************/
 string GetDate(const time_t);