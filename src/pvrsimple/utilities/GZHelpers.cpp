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
#include "GZHelpers.h"

/***********************************************************
 * Decompress Function Definitions
 ***********************************************************/
bool GzipInflate(const string& compressedBytes, string& uncompressedBytes) 
{
	/*
	 * This method uses zlib to decompress a gzipped file in memory.
	 * Author: Andrew Lim Chong Liang
	 * http://windrealm.org
	 */
	#define HANDLE_CALL_ZLIB(status) {if(status != Z_OK) {free(uncomp); return false;}}

	if (compressedBytes.size() == 0)
	{
		uncompressedBytes = compressedBytes;
		return true;
	}

	uncompressedBytes.clear() ;

	unsigned full_length = compressedBytes.size() ;
	unsigned half_length = compressedBytes.size() / 2;

	unsigned uncompLength = full_length ;
	char* uncomp = (char*) calloc(sizeof(char), uncompLength);

	z_stream strm;
	strm.next_in = (Bytef *) compressedBytes.c_str();
	strm.avail_in = compressedBytes.size() ;
	strm.total_out = 0;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;

	bool done = false ;

	HANDLE_CALL_ZLIB(inflateInit2(&strm, (16+MAX_WBITS)));

	while (!done)
	{
		// If our output buffer is too small
		if (strm.total_out >= uncompLength)
		{
			// Increase size of output buffer
			uncomp = (char *) realloc(uncomp, uncompLength + half_length);
			if (uncomp == NULL)
				return false;
			uncompLength += half_length;
		}

		strm.next_out = (Bytef *) (uncomp + strm.total_out);
		strm.avail_out = uncompLength - strm.total_out;

		// Inflate another chunk.
		int err = inflate (&strm, Z_SYNC_FLUSH);
		
		if (err == Z_STREAM_END)
			done = true;
		else if (err != Z_OK)
			break;
	}

	HANDLE_CALL_ZLIB(inflateEnd(&strm));

	for ( size_t i=0; i<strm.total_out; ++i)
	{
		uncompressedBytes += uncomp[i];
	}

	free(uncomp);
	return true;
}
