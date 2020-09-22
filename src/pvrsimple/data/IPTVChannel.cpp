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
#include "IPTVChannel.h"

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
IPTVChannel::IPTVChannel(string strData)
{
	// assign data to structure
	iUniqueId         = stoi(ParseSQLValue(strData, "<iUniqueId>"        ,     0));
	bIsRadio          = stob(ParseSQLValue(strData, "<bIsRadio>"         , false));
	iChannelNumber    = stoi(ParseSQLValue(strData, "<iChannelNumber>"   ,     0));
	iSubChannelNumber = stoi(ParseSQLValue(strData, "<iSubChannelNumber>",     0));
	strChannelName    =      ParseSQLValue(strData, "<strChannelName>"   ,    "") ;
	strInputFormat    =      ParseSQLValue(strData, "<strInputFormat>"   ,    "") ;
	iEncryptionSystem = stoi(ParseSQLValue(strData, "<iEncryptionSystem>",     0));
	strIconPath       =      ParseSQLValue(strData, "<strIconPath>"      ,    "") ;
	bIsHidden         = stob(ParseSQLValue(strData, "<bIsHidden>"        , false));
	strGroupName      =      ParseSQLValue(strData, "<strGroupName>"     ,    "") ;
	strStreamURL      =      ParseSQLValue(strData, "<strStreamURL>"     ,    "") ;
	strTvgId          =      ParseSQLValue(strData, "<strTvgId>"         ,    "") ;
	strTvgName        =      ParseSQLValue(strData, "<strTvgName>"       ,    "") ;
	strTvgLogo        =      ParseSQLValue(strData, "<strTvgLogo>"       ,    "") ;
	iTvgShift         = stof(ParseSQLValue(strData, "<iTvgShift>"        ,     0));
}

IPTVChannel::~IPTVChannel(void)
{
	// no need to destruct
}

/***********************************************************
 * Create PVR Object Definitions
 ***********************************************************/
PVR_CHANNEL IPTVChannel::Channel(void)
{
	// create object
	PVR_CHANNEL xbmcChannel;
	memset(&xbmcChannel, 0, sizeof(PVR_CHANNEL));
			
	// assign data to structure
		   xbmcChannel.iUniqueId         = this->GetUniqueId()        ;
		   xbmcChannel.bIsRadio          = this->GetIsRadio()         ;
		   xbmcChannel.iChannelNumber    = this->GetChannelNumber()   ;
		   xbmcChannel.iSubChannelNumber = this->GetSubChannelNumber();
	strcpy(xbmcChannel.strChannelName    , this->GetChannelName())    ;
	strcpy(xbmcChannel.strInputFormat    , this->GetInputFormat())    ;
		   xbmcChannel.iEncryptionSystem = this->GetEncryptionSystem();
	strcpy(xbmcChannel.strIconPath       , this->GetIconPath())       ;
		   xbmcChannel.bIsHidden         = this->GetIsHidden()        ;
		   
	// return object
	return xbmcChannel;
}