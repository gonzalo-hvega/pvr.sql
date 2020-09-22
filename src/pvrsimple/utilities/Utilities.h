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
#include <sstream>
#include <ctime>
#include <cstdarg>
#include <vector>
#include <mutex>
#include <iomanip>
#include <cmath>
#include <cstring>

#ifdef TARGET_WINDOWS

#include <windows.h>

#endif

#include "zlib.h"
#include "netsockets/socket.hh"
#include "netsockets/http.hh"
#include "rapidxml/rapidxml.hpp"
#include "p8-platform/util/StringUtils.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;
using namespace rapidxml;

/***********************************************************
 * OS Definitions
 ***********************************************************/
#ifdef TARGET_WINDOWS

#define snprintf _snprintf

#endif

/***********************************************************
 * Log Functions
 ***********************************************************/
#define CLog() XBMC->Log(LOG_DEBUG, "C : %s - Called", __FUNCTION__);
#define CLogNYI() XBMC->Log(LOG_ERROR, "C : %s - NYI", __FUNCTION__);
#define CPPLog() XBMC->Log(LOG_DEBUG, "C+: %s - Called", __FUNCTION__);
#define CPPLogNYI() XBMC->Log(LOG_ERROR, "C+: %s - NYI", __FUNCTION__);

/***********************************************************
 * Mutex Functions
 ***********************************************************/
#define SetLock() pMutex.lock();
#define SetUnlock() pMutex.unlock();

/***********************************************************
 * C++ Function Definitions
 ***********************************************************/
int         stoh(const string& str);
bool        stob(const string& str);
const char* stoc(const string& str);

#ifdef __ANDROID__

float       stof(const string& str);
int         stoi(const string& str);

#endif

string      itos(const int&    num);
string      ftos(const float&  num);
string      btos(const bool&   bit);
string      ctos(const char*   chr);
string      dtos(const time_t& dtm);

/***********************************************************
 * String Utils
 ***********************************************************/
#define StringUtils_Join(...) Join   (__VA_ARGS__                 , NULL)
string                        Join   (const string&, const char * , ... );

#define StringUtils_Replace(...) Replace(__VA_ARGS__                                 )
string                           Replace(const string&, const string&, const string &);

#define StringUtils_Trim(...) Trim(__VA_ARGS__  )
string                        Trim(const string&);

/***********************************************************
 * OS Functions
 ***********************************************************/
#ifdef TARGET_WINDOWS
 
void sleep(const int& sec); 

extern "C" char* strptime(const char* buf, const char* format, struct tm* tm);
 
#endif

/***********************************************************
 * Time Functions
 ***********************************************************/
unsigned int ParseWeekDay (const time_t);
string       ParseTime    (const time_t);
 
 /***********************************************************
 * BETA Definitions
 ***********************************************************/
bool isBeta(void);