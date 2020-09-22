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
class IPTVChannelGroupMember
{
	/* constructors/destrctors */
	public:
		 IPTVChannelGroupMember(string);
		~IPTVChannelGroupMember(void  );
		
	/* fetch variable api calls */			
	public:
		const char*        GetGroupName       (void) {return strGroupName.c_str();}
		const unsigned int GetChannelUniqueId (void) {return iChannelUniqueId    ;}
		const unsigned int GetChannelNumber   (void) {return iChannelNumber      ;}
		const unsigned int GetSubChannelNumber(void) {return iSubChannelNumber   ;}
		const bool         GetIsRadio         (void) {return bIsRadio            ;}
		
	/* fetch structure api calls */		
	public:
		PVR_CHANNEL_GROUP_MEMBER ChannelGroupMember(void);
						
	/* internal variables */
	private:
		string       strGroupName     ;
		unsigned int iChannelUniqueId ;
		unsigned int iChannelNumber   ;
		unsigned int iSubChannelNumber;
		bool         bIsRadio         ;
};