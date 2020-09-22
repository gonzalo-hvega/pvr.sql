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
#include "../client.h"
#include "sqlite/sqlite3.h"
#include "p8-platform/threads/threads.h"

#include "PVRTypes.h"
#include "data/SQLRecord.h"
#include "data/IPTVChannel.h"
#include "data/IPTVEpgEntry.h"
#include "data/DVRTimer.h"
#include "data/DVRRecording.h"
#include "utilities/LOGHelpers.h"
#include "utilities/Subprocess.h"
#include "utilities/Utilities.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;
using namespace ADDON;

/***********************************************************
 * Class Definitions
 ***********************************************************/
class PVRRecorder : P8PLATFORM::CThread
{		
	/* constructors/destrctors */
	public:
		         PVRRecorder(const int, const int, const int);
		virtual ~PVRRecorder(void                           );
		
	/* status and variable api calls */
	public:
		bool IsOpen   (void);
		bool IsWorking(void);

	/* ffmpeg controls  */
	private:
		void Open (void             );
		void Close(bool bWait = true);
	  
	/* dvr recorder */
	private:
		void *Process(void);

	/* special file operators */
	protected:
		void CorrectDurationFLV(const string&, const int);

	/* recorder variables */
	private:
		bool       bIsOpen    ;
		bool       bIsWorking ;	
		bool       bStop      ;
		int        iChannelId ;
		int        iTimerId   ;
		int        iEpgId     ;
		string     strTvgId   ;
		string     strTvgName ;
		string     strLogPath ;
		subprocess libFFMPEG  ;
};