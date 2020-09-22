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
#include "IPTVChannelGroupMember.h"

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
IPTVChannelGroupMember::IPTVChannelGroupMember(string strData)
{
	// assign data to structure	
	strGroupName      =      ParseSQLValue(strData, "<strGroupName>"     ,    "") ;
	iChannelUniqueId  = stoi(ParseSQLValue(strData, "<iChannelUniqueId>" ,     0));
	iChannelNumber    = stoi(ParseSQLValue(strData, "<iChannelNumber>"   ,     0));
	iSubChannelNumber = stoi(ParseSQLValue(strData, "<iSubChannelNumber>",     0));
	bIsRadio          = stob(ParseSQLValue(strData, "<bIsRadio>"         , false));
}

IPTVChannelGroupMember::~IPTVChannelGroupMember(void)
{
	// no need to destruct
}

/***********************************************************
 * Create PVR Object Definitions
 ***********************************************************/
PVR_CHANNEL_GROUP_MEMBER IPTVChannelGroupMember::ChannelGroupMember(void)
{
	// create object
	PVR_CHANNEL_GROUP_MEMBER xbmcChannelGroupMember;
	memset(&xbmcChannelGroupMember, 0, sizeof(PVR_CHANNEL_GROUP_MEMBER));
	
	// assign data to structure
	strcpy(xbmcChannelGroupMember.strGroupName      , this->GetGroupName())      ;
		   xbmcChannelGroupMember.iChannelUniqueId  = this->GetChannelUniqueId() ;
		   xbmcChannelGroupMember.iChannelNumber    = this->GetChannelNumber()   ;
		   xbmcChannelGroupMember.iSubChannelNumber = this->GetSubChannelNumber();
		   
	// return object
	return xbmcChannelGroupMember;
}