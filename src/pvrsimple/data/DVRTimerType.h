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
class DVRTimerType
{
	/* constructors/destrctors */
	public:
		 DVRTimerType(string);
		~DVRTimerType(void  );
		
	/* fetch variable api calls */
	public:
		const unsigned int GetId            (void) {return iId                   ;}
		const unsigned int GetAttributes    (void) {return iAttributes           ;}
		const char*        GetDescription   (void) {return strDescription.c_str();}
		const unsigned int GetPrioritiesSize(void) {return iPrioritiesSize       ;}
		const unsigned int GetLifetimesSize (void) {return iLifetimesSize        ;}
		
	/* fetch structure api calls */
	public:
		PVR_TIMER_TYPE TimerType(void);
		
	/* internal variables */
	private:
		unsigned int iId            ;
		unsigned int iAttributes    ;
		string       strDescription ;
		unsigned int iPrioritiesSize;
		unsigned int iLifetimesSize ;
};