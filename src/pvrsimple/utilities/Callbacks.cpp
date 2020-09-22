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
#include "Callbacks.h"

/***********************************************************
 * XBMC Callback Definitions
 ***********************************************************/
int XBMC_FileSize(const char* strFile)
{
	// assume empty
	int size = 0;
	
	// open file
	void *readHandle = XBMC->OpenFile (strFile , 0);
	
	// check handle
	if (readHandle)
	{
		// get file length
		size = XBMC->GetFileLength(readHandle);
	}
	
	// close files
	XBMC->CloseFile(readHandle);

	// return length
	return size;	
}

bool XBMC_CopyFile(const char* strCopy, const char* strPaste)
{
	// assume fail
	bool ret = false;
	
	// init return buffer
	char buffer[1024];

	// open files
	void *readHandle = XBMC->OpenFile (strCopy , 0);
	void *writeHandle = XBMC->OpenFileForWrite(strPaste, true);

	// check handles
	if (readHandle && writeHandle)
	{	
		// copy data over
		while (int bytes = XBMC->ReadFile(readHandle, buffer, 1024))
		{
			XBMC->WriteFile(writeHandle, buffer, bytes);
		}
		
		// success
		ret = true;
	}

	// close files
	XBMC->CloseFile(readHandle );
	XBMC->CloseFile(writeHandle);

	// return success
	return ret;
}

bool XBMC_DeleteFileAndFolder(const char* strFile, const char* strFolder)
{
	// assume will fail deletion
	bool ret = false;
	
	// delete file
	if (XBMC->DeleteFile(strFile))
	{
		// mark success deletion
		ret = true;

		// remove directory if empty
		unsigned int iNumFiles;
		VFSDirEntry* pRecFolder;
		
		if (XBMC->GetDirectory(strFolder, "", &pRecFolder, &iNumFiles))
		{
			// if empty remove
			if (iNumFiles == 0)
				if (!XBMC->RemoveDirectory(strFolder))
					ret = false;
		}
		
		XBMC->FreeDirectory(pRecFolder, iNumFiles);
	}
	
	// return success
	return ret;
}

/***********************************************************
 * PVR Callback Definitions
 ***********************************************************/
void PVR_TransferTimerTypeEntry(PVR_TIMER_TYPE types[], int* size, const PVR_TIMER_TYPE* entry)
{
	// pass data to kodi
	types[(*size)] = *entry;

	// increment counter
	(*size)++;
}
