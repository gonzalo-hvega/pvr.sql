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
#include "HTTPHelpers.h"

/***********************************************************
 * Parse Function Definitions
 ***********************************************************/
int http_get_response(net_socket_t &socket, string &response)
{
	// assume fail to return
	int ret = 0;
	
	// create container for header
	string header;
  
	// parse header
	if (parse_http_headers(socket.m_sockfd, header) != 0)
	{
		// get size of header
		unsigned int size_body = (unsigned int) http_get_field("Content-Length: ", header);

		// read from socket with known size
		if (size_body)
		{
			// create buffer to read back
			char* buf = new char[size_body];
			
			// read back the size of the header
			ret = (socket.read_all(buf, size_body) >= 0);

			// convert to string
			string str_json(buf, size_body);

			// assign to response
			response = str_json;

			// delete buffer
			delete[] buf;
		}
	}

  return ret;
}

string http_get_action(const string& header)
{
    // create container for return
    string action = "";
    
	// determine start and end
	size_t start = header.find("/"       );
	size_t end   = header.find("HTTP/1.1");
	
	// substring
	if (start != string::npos && end != string::npos)
	    action = header.substr(start+1, end-start-1);
	    
	// return
	return StringUtils::Trim(action);
}

vector<string> http_get_argument(const string& action)
{
    // create container for arguments
    vector<string> args;
    
    // clear container
    args.clear();
    
    // copy header to substring
    string function = action.substr(action.find("("));
    
	// determine start
	size_t start = 0;
	
	// start to substring
	while (function.find(")", start) != string::npos)
	{
	    // look for tokens
	         if (function.find("\")", start) != string::npos) {string argv = function.substr(start, function.find("\")")); args.push_back(argv.substr(0, argv.length()-0)); start = function.find("\")")+1;}
	    else if (function.find("\",", start) != string::npos) {string argv = function.substr(start, function.find("\",")); args.push_back(argv.substr(0, argv.length()-0)); start = function.find("\",")+1;}
        else if (function.find(";)" , start) != string::npos) {string argv = function.substr(start, function.find(";)" )); args.push_back(argv.substr(0, argv.length()-1)); start = function.find(";)" )+1;}
	    else if (function.find(";," , start) != string::npos) {string argv = function.substr(start, function.find(";," )); args.push_back(argv.substr(0, argv.length()-0)); start = function.find(";," )+1;}
        else if (function.find(">)" , start) != string::npos) {string argv = function.substr(start, function.find(">)" )); args.push_back(argv.substr(0, argv.length()-1)); start = function.find(">)" )+1;}
	    else if (function.find(">," , start) != string::npos) {string argv = function.substr(start, function.find(">," )); args.push_back(argv.substr(0, argv.length()-0)); start = function.find(">," )+1;}
	    else                                                  {                                                                                                             start = function.length()   +1;}
	}

    // clean up arguments
    for (vector<string>::iterator argv = args.begin(); argv != args.end(); argv++)
    {
        if (argv->substr(0, 1) == "(") *argv = argv->substr(2);
        if (argv->substr(0, 1) == ",") *argv = argv->substr(2);
    }
    
    return args;
}