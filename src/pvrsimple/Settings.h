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
#include "../client.h"

#include "PVRTypes.h"
#include "utilities/Utilities.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;
using namespace ADDON;

/***********************************************************
 * Class Definitions
 ***********************************************************/
class PVRSettings
{	
	public:
		PVRSettings(void);
		virtual ~PVRSettings(void);

	public:
		string GetM3UPath    (void);
		int    GetM3UStartNum(void);
		int    GetM3URefresh (void);
		bool   GetM3UFilter  (void);
	  
	public:
		string GetEPGPath      (void);
		float  GetEPGTimeShift (void);
		int    GetEPGRefresh   (void);
		bool   GetEPGTSOverride(void);
	  
	public:
		string GetLogoPath(void);
		int    GetLogoEPG (void);
	  
	public:
		string GetDVRPath    (void);
		int    GetSchedPoll  (void);
		int    GetDVRMode    (void);
		string GetServerIP   (void);
		int    GetServerPort (void);
		string GetFFMPEG     (void);
		string GetAVParams   (void);
		string GetFileExt    (void);
		int    GetStrmTimeout(void);
		
	public:
		void   SetClientPath(string);
		string GetClientPath(void  );
		void   SetUserPath  (string);
		string GetUserPath  (void  );

	protected:
		void LoadSettings(void);

	private:  
		int    iM3UPathType  ;
		string strM3UPath    ;
		bool   bM3UCache     ;
		int    iM3UStartNum  ;
		int    iM3URefresh   ;
		bool   bM3UFilter    ;
		int    iEPGPathType  ;
		string strEPGPath    ;
		bool   bEPGCache     ;
		float  iEPGTimeShift ;
		int    iEPGRefresh   ;
		bool   bEPGTSOverride;
		int    iLogoPathType ;
		string strLogoPath   ;
		int    iLogoFromEPG  ;
		string strDVRPath    ;
		int    iDVRPoll      ;
		int    iDVRMode      ;
		string strServerIP   ;
		int    iServerPort   ;
		string strFFMPEGPath ;
		string strAVParams   ;
		string strFileExt    ;
		int    iStrmTimeout  ;
		int    iStrmQuality  ;
		string strUserPath   ;
		string strClientPath ;
};