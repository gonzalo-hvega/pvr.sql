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
class DVRRecording
{
	/* constructors/destrctors */
	public:
		 DVRRecording(string);
		 DVRRecording(void  );
		~DVRRecording(void  );
		
	/* operator overloads */
	public:
		DVRRecording& operator= (const DVRRecording &rhs);
			
	/* fetch variable api calls */
	public:
		const char*        GetRecordingId       (void) {return strRecordingId.c_str()     ;}
		const char*        GetTitle             (void) {return strTitle.c_str()           ;}
		const char*        GetEpisodeName       (void) {return strEpisodeName.c_str()     ;}
		const int          GetSeriesNumber      (void) {return iSeriesNumber              ;}
		const int          GetEpisodeNumber     (void) {return iEpisodeNumber             ;}
		const int          GetYear              (void) {return iYear                      ;}
		const char*        GetDirectory         (void) {return strDirectory.c_str()       ;}
		const char*        GetPlotOutline       (void) {return strPlotOutline.c_str()     ;}
		const char*        GetPlot              (void) {return strPlot.c_str()            ;}
		const char*        GetGenreDescription  (void) {return strGenreDescription.c_str();}
		const char*        GetChannelName       (void) {return strChannelName.c_str()     ;}
		const char*        GetIconPath          (void) {return strIconPath.c_str()        ;}
		const char*        GetThumbnailPath     (void) {return strThumbnailPath.c_str()   ;}
		const char*        GetFanartPath        (void) {return strFanartPath.c_str()      ;}
		const time_t       GetRecordingTime     (void) {return recordingTime              ;}
		const int          GetDuration          (void) {return iDuration                  ;}
		const int          GetPriority          (void) {return iPriority                  ;}
		const int          GetLifetime          (void) {return iLifetime                  ;}
		const int          GetGenreType         (void) {return iGenreType                 ;}
		const int          GetGenreSubType      (void) {return iGenreSubType              ;}
		const int          GetPlayCount         (void) {return iPlayCount                 ;}
		const int          GetLastPlayedPosition(void) {return iLastPlayedPosition        ;}
		const bool         GetIsDeleted         (void) {return bIsDeleted                 ;}
		const unsigned int GetEpgEventId        (void) {return iEpgEventId                ;}
		const int          GetChannelUid        (void) {return iChannelUid                ;}
		const int          GetChannelType       (void) {return channelType                ;}
		const string       GetFilePath          (void) {return strFilePath                ;}
		const string       GetFileName          (void) {return strFileName                ;}
		
	/* fetch structure api calls */
	public:
		PVR_RECORDING Recording(void);
	
	/* internal variables */	
	private:
		string       strRecordingId     ;
		string       strTitle           ;
		string       strEpisodeName     ;
		int          iSeriesNumber      ;
		int          iEpisodeNumber     ;
		int          iYear              ;
		string       strDirectory       ;
		string       strPlotOutline     ;
		string       strPlot            ;
		string       strGenreDescription;
		string       strChannelName     ;
		string       strIconPath        ;
		string       strThumbnailPath   ;
		string       strFanartPath      ;
		time_t       recordingTime      ;
		int          iDuration          ;
		int          iPriority          ;
		int          iLifetime          ;
		int          iGenreType         ;
		int          iGenreSubType      ;
		int          iPlayCount         ;
		int          iLastPlayedPosition;
		bool         bIsDeleted         ;
		unsigned int iEpgEventId        ;
		int          iChannelUid        ;
		int          channelType        ;
		string       strFilePath        ;
		string       strFileName        ;
};