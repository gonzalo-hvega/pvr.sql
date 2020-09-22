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
#include "LOGHelpers.h"

/***********************************************************
 * Parse Function Definitions
 ***********************************************************/
string ReadLOGLineValue(const string& strLine, const char* strMarker, const char* strDefault /*= ""*/)
{
	// get start of marker in line
	int iMarkerStart = (int)strLine.find(strMarker);
	int iMarkerEnd                                 ;

	// parse through line for value
	if (iMarkerStart >= 0)
	{
		// start substring at end of marker
		iMarkerStart += strlen(strMarker);

		// find the end of the content
		if (iMarkerStart < (int)strLine.length())
		{
			// assume the character to look for is a space
			char cFind = ' ';

			// if marker has quote then look for next quote
			if (strLine[iMarkerStart] == '"')
			{
				cFind = '"';
				iMarkerStart++;
			}

			// look for end marker
			iMarkerEnd = (int)strLine.find(cFind, iMarkerStart);

			// if could not find marker then set to end
			if (iMarkerEnd < 0)
			{
				iMarkerEnd = strLine.length();
			}
		}
	}

	// return value if found otherwise return default
	return (iMarkerStart >= 0 && iMarkerStart < (int)strLine.length()) ?  strLine.substr(iMarkerStart, iMarkerEnd - iMarkerStart) : string(strDefault);
}