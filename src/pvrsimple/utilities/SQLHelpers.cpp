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
#include "SQLHelpers.h"

/***********************************************************
 * Parse Function Definitions
 ***********************************************************/
string ParseSQLValue(const string& strValue, const char* strMarker, const char* strDefault /* = "" */)
{
	// get start of marker in line
	int iMarkerStart = (int)strValue.find(strMarker) + strlen(strMarker);
	int iMarkerEnd                                  ;
	
	// look for close marker
	iMarkerEnd = (int)strValue.find("<", iMarkerStart);
	
	// return value if found otherwise return empty
	return (strValue.find(strMarker) != string::npos) ?  strValue.substr(iMarkerStart, iMarkerEnd - iMarkerStart) : string(strDefault);
}

string ParseSQLValue(const string& strValue, const char* strMarker, const int iDefault /* = 0 */)
{
	// get start of marker in line
	int iMarkerStart = (int)strValue.find(strMarker) + strlen(strMarker);
	int iMarkerEnd                                  ;
	
	// look for close marker
	iMarkerEnd = (int)strValue.find("<", iMarkerStart);
	
	// return value if found otherwise return empty
	return (strValue.find(strMarker) != string::npos) ?  strValue.substr(iMarkerStart, iMarkerEnd - iMarkerStart) : itos(iDefault);
}

string ParseSQLValue(const string& strValue, const char* strMarker, const bool bDefault /* = false */)
{
	// get start of marker in line
	int iMarkerStart = (int)strValue.find(strMarker) + strlen(strMarker);
	int iMarkerEnd                                  ;
	
	// look for close marker
	iMarkerEnd = (int)strValue.find("<", iMarkerStart);
	
	// return value if found otherwise return empty
	return (strValue.find(strMarker) != string::npos) ?  strValue.substr(iMarkerStart, iMarkerEnd - iMarkerStart) : btos(bDefault);
}