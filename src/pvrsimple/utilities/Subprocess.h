#pragma once
/*
 *  subprocess - A simple subprocess caller for POSIX and Windows
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
#include <stdio.h>

#ifdef TARGET_WINDOWS

#include <io.h>

#else

#include <unistd.h>

#endif

#include "../utilities/Utilities.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;

/***********************************************************
 * OS Definitions
 ***********************************************************/
#ifdef TARGET_WINDOWS

#define popen    _popen
#define fileno   _fileno
#define read     _read
#define pclose   _pclose

#endif

/***********************************************************
 * Class Definitions
 ***********************************************************/
class subprocess
{
	public:
	           subprocess(void);
	  virtual ~subprocess(void);

	  void pstart(const char* command, const char* mode );
	  void pread (void*       buffer , unsigned int size);
	  int  gcount(void                                  );
	  int  pterm (void                                  );

	private:
	  FILE* process;
	  int   bytread;
};