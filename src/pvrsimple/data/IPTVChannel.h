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
class IPTVChannel
{
	/* constructors/destrctors */
	public:
		 IPTVChannel(string);
		~IPTVChannel(void  );
			
	/* fetch variable api calls */
	public:
		const unsigned int GetUniqueId        (void) {return iUniqueId             ;}
		const bool         GetIsRadio         (void) {return bIsRadio              ;}
		const unsigned int GetChannelNumber   (void) {return iChannelNumber        ;}
		const unsigned int GetSubChannelNumber(void) {return iSubChannelNumber     ;}
		const char*        GetChannelName     (void) {return strChannelName.c_str();}
		const char*        GetInputFormat     (void) {return strInputFormat.c_str();}
		const unsigned int GetEncryptionSystem(void) {return iEncryptionSystem     ;}
		const char*        GetIconPath        (void) {return strIconPath.c_str()   ;}
		const bool         GetIsHidden        (void) {return bIsHidden             ;}
		const string       GetGroupName       (void) {return strGroupName          ;}
		const string       GetStreamURL       (void) {return strStreamURL          ;}
		const string       GetTvgId           (void) {return strTvgId              ;}
		const string       GetTvgName         (void) {return strTvgName            ;}
		const string       GetTvgLogo         (void) {return strTvgLogo            ;}
		const float        GetTvgShift        (void) {return iTvgShift             ;}
		
	/* fetch structure api calls */
	public:
		PVR_CHANNEL Channel(void);
			
	/* internal variables */
	private:
		unsigned int iUniqueId        ;
		bool         bIsRadio         ;
		unsigned int iChannelNumber   ;
		unsigned int iSubChannelNumber;
		string       strChannelName   ;
		string       strInputFormat   ;
		unsigned int iEncryptionSystem;
		string       strIconPath      ;
		bool         bIsHidden        ;
		string       strGroupName     ;
		string       strStreamURL     ;
		string       strTvgId         ;
		string       strTvgName       ;
		string       strTvgLogo       ;
		float        iTvgShift        ;
};