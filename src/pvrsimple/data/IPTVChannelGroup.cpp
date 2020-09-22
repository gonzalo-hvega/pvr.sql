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
#include "IPTVChannelGroup.h"

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
IPTVChannelGroup::IPTVChannelGroup(string strData)
{
	// assign data to structure
	strGroupName =      ParseSQLValue(strData, "<strGroupName>",    "") ;
	bIsRadio     = stob(ParseSQLValue(strData, "<bIsRadio>"    , false));
	iPosition    = stoi(ParseSQLValue(strData, "<iPosition>"   ,     0));
}

IPTVChannelGroup::~IPTVChannelGroup(void)
{
	// no need to destruct
}

/***********************************************************
 * Create PVR Object Definitions
 ***********************************************************/
PVR_CHANNEL_GROUP IPTVChannelGroup::ChannelGroup(void)
{
	// create object
	PVR_CHANNEL_GROUP xbmcChannelGroup;
	memset(&xbmcChannelGroup, 0, sizeof(PVR_CHANNEL_GROUP));
	
	// assign data to structure
	strcpy(xbmcChannelGroup.strGroupName , this->GetGroupName());
		   xbmcChannelGroup.bIsRadio     = this->GetIsRadio()   ;
		   xbmcChannelGroup.iPosition    = this->GetPosition()  ;
		   
	// return object
	return xbmcChannelGroup;
}