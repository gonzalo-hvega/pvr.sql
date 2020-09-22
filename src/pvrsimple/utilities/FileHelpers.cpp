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
#include "FileHelpers.h"

/***********************************************************
 * Parse Function Definitions
 ***********************************************************/
string ParseFolderSeparator(string strPath)
{
  // legal chars
  string strLegalCh = "\\/";
  string strSep = "";
  
  // look for instance int path
  for (string::iterator chr = strPath.begin(); chr < strPath.end(); chr++)
  {
    if (strLegalCh.find(*chr) != string::npos)
      strSep = *chr;
  }	
  
  // return separator
  return strSep;
}

/***********************************************************
 * Prep Function Definitions
 ***********************************************************/
string PrepFileName(string strFileName)
{
  // illegal chars
  string strIllegalCh = "\\/:?\"<>|*'";

  // remove illegal chars
  for (string::iterator chr = strFileName.begin(); chr < strFileName.end(); chr++)
  {
    if (strIllegalCh.find(*chr) != string::npos)
      *chr = ' ';
  }	
  
  // return file name
  return strFileName;
}

string GetFileName(string strFilePath)
{
  // get localized string
  size_t iFound       = 0;
  string strFileName  = strFilePath;
  
  // parse through string
  while (iFound != string::npos)
  {
	// subset string
	strFileName = strFileName.substr(iFound+1, strFileName.length());
	
	// look for delimeter
	iFound = strFileName.find(ParseFolderSeparator(strFilePath), (int)iFound);
  }	
  
  // return file name
  return strFileName;
}