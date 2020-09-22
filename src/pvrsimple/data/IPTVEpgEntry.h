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

#include "../utilities/SQLHelpers.h"
#include "../utilities/Utilities.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;

/***********************************************************
 * Class Definitions
 ***********************************************************/
class IPTVEpgEntry
{
	/* constructors/destrctors */
	public:
		 IPTVEpgEntry(string);
		~IPTVEpgEntry(void  );
		
	/* fetch variable api calls */
	public:
		const unsigned int GetUniqueBroadcastId(void) {return iUniqueBroadcastId         ;}
		const unsigned int GetUniqueChannelId  (void) {return iUniqueChannelId           ;}
		const char*        GetTitle            (void) {return strTitle.c_str()           ;}
		const time_t       GetStartTime        (void) {return startTime                  ;}
		const time_t       GetEndTime          (void) {return endTime                    ;}
		const char*        GetPlotOutline      (void) {return strPlotOutline.c_str()     ;}
		const char*        GetPlot             (void) {return strPlot.c_str()            ;}
		const char*        GetOriginalTitle    (void) {return strOriginalTitle.c_str()   ;}
		const char*        GetCast             (void) {return strCast.c_str()            ;}
		const char*        GetDirector         (void) {return strDirector.c_str()        ;}
		const char*        GetWriter           (void) {return strWriter.c_str()          ;}
		const int          GetYear             (void) {return iYear                      ;}
		const char*        GetIMDBNumber       (void) {return strIMDBNumber.c_str()      ;}
		const char*        GetIconPath         (void) {return strIconPath.c_str()        ;}
		const int          GetGenreType        (void) {return iGenreType                 ;}
		const int          GetGenreSubType     (void) {return iGenreSubType              ;}
		const char*        GetGenreDescription (void) {return strGenreDescription.c_str();}
		const time_t       GetFirstAired       (void) {return firstAired                 ;}
		const int          GetParentalRating   (void) {return iParentalRating            ;}
		const int          GetStarRating       (void) {return iStarRating                ;}
		const bool         GetNotify           (void) {return bNotify                    ;}
		const int          GetSeriesNumber     (void) {return iSeriesNumber              ;}
		const int          GetEpisodeNumber    (void) {return iEpisodeNumber             ;}
		const int          GetEpisodePartNumber(void) {return iEpisodePartNumber         ;}
		const char*        GetEpisodeName      (void) {return strEpisodeName.c_str()     ;}
		const unsigned int GetFlags            (void) {return iFlags                     ;}
		const char*        GetSeriesLink       (void) {return strSeriesLink.c_str()      ;}
		const string       GetTvgId            (void) {return strTvgId                   ;}
		
	/* fetch structure api calls */
	public:
		EPG_TAG EpgTag(void);
		
	/* internal variables */
	private:
		unsigned int iUniqueBroadcastId ;
		unsigned int iUniqueChannelId   ;
		string       strTitle           ;
		time_t       startTime          ;
		time_t       endTime            ;
		string       strPlotOutline     ;
		string       strPlot            ;
		string       strOriginalTitle   ;
		string       strCast            ;
		string       strDirector        ;
		string       strWriter          ;
		int          iYear              ;
		string       strIMDBNumber      ;
		string       strIconPath        ;
		int          iGenreType         ;
		int          iGenreSubType      ;
		string       strGenreDescription;
		time_t       firstAired         ;
		int          iParentalRating    ;
		int          iStarRating        ;
		bool         bNotify            ;
		int          iSeriesNumber      ;
		int          iEpisodeNumber     ;
		int          iEpisodePartNumber ;
		string       strEpisodeName     ;
		unsigned int iFlags             ;
		string       strSeriesLink      ;
		string       strTvgId           ;
};