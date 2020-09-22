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
class DVRTimer
{
	/* constructors/destrctors */
	public:
		 DVRTimer(string);
		 DVRTimer(void  );
		~DVRTimer(void  );
		
	/* operator overloads */
	public:
		DVRTimer& operator= (const DVRTimer &rhs);
			
	/* fetch variable api calls */
	public:
		const unsigned int GetClientIndex             (void) {return iClientIndex              ;}
		const unsigned int GetParentClientIndex       (void) {return iParentClientIndex        ;}
		const int          GetClientChannelUid        (void) {return iClientChannelUid         ;}
		const time_t       GetStartTime               (void) {return startTime                 ;}
		const time_t       GetEndTime                 (void) {return endTime                   ;}
		const bool         GetStartAnyTime            (void) {return bStartAnyTime             ;}
		const bool         GetEndAnyTime              (void) {return bEndAnyTime               ;}
		const int          GetState                   (void) {return state                     ;}
		const unsigned int GetTimerType               (void) {return iTimerType                ;}
		const char*        GetTitle                   (void) {return strTitle.c_str()          ;}
		const char*        GetEpgSearchString         (void) {return strEpgSearchString.c_str();}
		const bool         GetFullTextEpgSearch       (void) {return bFullTextEpgSearch        ;}
		const char*        GetDirectory               (void) {return strDirectory.c_str()      ;}
		const char*        GetSummary                 (void) {return strSummary.c_str()        ;}
		const int          GetPriority                (void) {return iPriority                 ;}
		const int          GetLifetime                (void) {return iLifetime                 ;}
		const int          GetMaxRecordings           (void) {return iMaxRecordings            ;}
		const unsigned int GetRecordingGroup          (void) {return iRecordingGroup           ;}
		const time_t       GetFirstDay                (void) {return firstDay                  ;}
		const unsigned int GetWeekdays                (void) {return iWeekdays                 ;}
		const unsigned int GetPreventDuplicateEpisodes(void) {return iPreventDuplicateEpisodes ;}
		const unsigned int GetEpgUid                  (void) {return iEpgUid                   ;}
		const unsigned int GetMarginStart             (void) {return iMarginStart              ;}
		const unsigned int GetMarginEnd               (void) {return iMarginEnd                ;}
		const int          GetGenreType               (void) {return iGenreType                ;}
		const int          GetGenreSubType            (void) {return iGenreSubType             ;}
		const char*        GetSeriesLink              (void) {return strSeriesLink.c_str()     ;}
		
	/* fetch structure api calls */
	public:
		PVR_TIMER Timer(void);
	
	/* internal variables */	
	private:
		unsigned int iClientIndex             ;
		unsigned int iParentClientIndex       ;
		int          iClientChannelUid        ;
		time_t       startTime                ;
		time_t       endTime                  ;
		bool         bStartAnyTime            ;
		bool         bEndAnyTime              ;
		int          state                    ;
		unsigned int iTimerType               ;
		string       strTitle                 ;
		string       strEpgSearchString       ;
		bool         bFullTextEpgSearch       ;
		string       strDirectory             ;
		string       strSummary               ;
		int          iPriority                ;
		int          iLifetime                ;
		int          iMaxRecordings           ;
		unsigned int iRecordingGroup          ;
		time_t       firstDay                 ;
		unsigned int iWeekdays                ;
		unsigned int iPreventDuplicateEpisodes;
		unsigned int iEpgUid                  ;
		unsigned int iMarginStart             ;
		unsigned int iMarginEnd               ;
		int          iGenreType               ;
		int          iGenreSubType            ;
		string       strSeriesLink            ;
};