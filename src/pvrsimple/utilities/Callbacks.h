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
#include "../../client.h"

/***********************************************************
 * OS Definitions
 ***********************************************************/
#ifdef TARGET_WINDOWS

#define CreateDirectoryW CreateDirectory
#define RemoveDirectoryW RemoveDirectory
#define DeleteFileW      DeleteFile
  
#endif	

/***********************************************************
 * XBMC Callback Definitions
 ***********************************************************/
int  XBMC_FileSize           (const char*             );
bool XBMC_CopyFile           (const char*, const char*);
bool XBMC_DeleteFileAndFolder(const char*, const char*);

/***********************************************************
 * PVR Callback Definitions
 ***********************************************************/
void PVR_TransferTimerTypeEntry(PVR_TIMER_TYPE [], int *, const PVR_TIMER_TYPE*);