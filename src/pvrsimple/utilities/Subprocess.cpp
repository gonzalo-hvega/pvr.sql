/*
 *  subprocess - A simple subprocess caller for POSIX and Window
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
#include "Subprocess.h"

/***********************************************************
 * Class Definitions
 ***********************************************************/
subprocess::subprocess(void)
{
  // stub, we use popen/pclose
  process = NULL;
  bytread =    0;
}

subprocess::~subprocess(void)
{
  // close file and remove pointer
  pterm();
}

void subprocess::pstart(const char* command, const char* mode)
{
  // create copy of mode
  string pipe(mode);

  // OS specific
#ifdef TARGET_WINDOWS

  // hide windows
  AllocConsole();
  ShowWindow(GetConsoleWindow(), SW_HIDE);

#else

  // substring for unix (does not accept rb)
  pipe = pipe.substr(0, 1);
  
#endif

  // call system specific open
  process = popen(command, pipe.c_str());
}

void subprocess::pread(void* buffer, unsigned int size)
{
  // call system specific read
  if (process)
    bytread = read(fileno(process), buffer, size);
}

int subprocess::gcount(void)
{
  // call system bytes read
  return bytread;
}

int subprocess::pterm(void)
{
  // set up return
  int ret = 0;

  // call system specific close
  if (process)
    ret = pclose(process);

  // remove pointer
  process = NULL;

  // return value
  return ret;
}