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
#include "TCPServer.h"

/***********************************************************
 * Global Definitions
 ***********************************************************/
extern PVRSettings   *settings;
extern SQLConnection *sqlite  ;

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
TCPServer::TCPServer()
{
	// only server mode
	if (settings->GetDVRMode() == SERVER_MODE)
	{
		// log attempt to create object
		XBMC->Log(LOG_NOTICE, "C+: %s - Creating TCP server", __FUNCTION__);
		
		// no establish connection at start
		bIsConnected = false;
		bIsWorking   = false;
		bStop        = false;
		
		// log connection attempt
		XBMC->Log(LOG_NOTICE, "C+: %s - Attempting to bind to TCP connection [%s:%i]", __FUNCTION__, settings->GetServerIP().c_str(), settings->GetServerPort());
		
		// create tcp server to test
		tcp_server_t tcpServer(settings->GetServerPort());
			
		// call connection to database
		Connect();
		
		// close tcp server test
		tcpServer.close();
		
		// log creation of object
		XBMC->Log(LOG_NOTICE, "C+: %s - Created TCP server", __FUNCTION__);
		
		// create thread
		if (IsConnected()) CreateThread();
	}
}

TCPServer::~TCPServer(void)
{
	// only server mode
	if (settings->GetDVRMode() == SERVER_MODE)
	{
		// mark as stopped
		bStop = true;
		 
		// attempt to disconnect
		Disconnect();	
	}
}

/***********************************************************
 * Get Staus/Local Variable API Definitions
 ***********************************************************/
bool TCPServer::IsConnected(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return bIsConnected;
}

bool TCPServer::IsWorking(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return bIsWorking;
}

/***********************************************************
 * SQLite Logging API Definitions
 ***********************************************************/
string TCPServer::GetDBLog(void)
{
	// log function call
	CPPLog(); 
	
	// call sql equivalent
	string strMsg = sqlite->GetDBLog();
	
	// create response string
	string strResponse("HTTP/1.1 200 OK\r\n");
	
	// construct proper http
	strResponse += "Content-Length: ";
	strResponse += to_string(strMsg.size());
	strResponse += "\r\n";
	strResponse += "\r\n";
	strResponse += strMsg;
	
	// return response for client
	return strResponse;
}

/***********************************************************
 * Records API Definitions
 ***********************************************************/
string TCPServer::AddRecord(const string strAction)
{
	// log function call
	CPPLog(); 
	
	// parse call arguments
	vector<string> vArgs = http_get_argument(strAction);
	
	// create return container
	SQLRecord sqlRecord;

	// send equivalent to sqlite
	sqlite->AddRecord(vArgs[0].c_str(), vArgs[1].c_str(), &sqlRecord);

	// create response
	string strMsg = "";
	
	// iterate through records
	strMsg += sqlRecord.GetRecord();
	
	// create response string
	string strResponse("HTTP/1.1 200 OK\r\n");

	// construct proper http
	strResponse += "Content-Length: ";
	strResponse += to_string(strMsg.size());
	strResponse += "\r\n";
	strResponse += "\r\n";
	strResponse += strMsg;
	
	// return response for client
	return strResponse;
}

string TCPServer::UpdateRecord(const string strAction)
{
	// log function call
	CPPLog(); 
	
	// parse call arguments
	vector<string> vArgs = http_get_argument(strAction);
	
	// create return container
	SQLRecord sqlRecord;

	// send equivalent to sqlite
	sqlite->UpdateRecord(vArgs[0].c_str(), vArgs[1].c_str(), &sqlRecord);

	// create response
	string strMsg = "";
	
	// iterate through records
	strMsg += sqlRecord.GetRecord();
	
	// create response string
	string strResponse("HTTP/1.1 200 OK\r\n");

	// construct proper http
	strResponse += "Content-Length: ";
	strResponse += to_string(strMsg.size());
	strResponse += "\r\n";
	strResponse += "\r\n";
	strResponse += strMsg;
	
	// return response for client
	return strResponse;
}

/***********************************************************
 * Tables API Definitions
 ***********************************************************/
string TCPServer::GetRecords(const string strAction)
{
	// log function call
	CPPLog(); 
	
	// parse call arguments
	vector<string> vArgs = http_get_argument(strAction);
	
	// send equivalent to sqlite
	vector<SQLRecord> sqlRecords = sqlite->GetRecords(vArgs[0].c_str());

	// create response
	string strMsg = "";
	
	// iterate through records
	for (vector<SQLRecord>::iterator sqlRecord = sqlRecords.begin(); sqlRecord != sqlRecords.end(); sqlRecord++)
		strMsg += sqlRecord->GetRecord()+"\n";
	
	// delete last 2 characters (extra line break)
	if(strMsg.size()) strMsg = strMsg.substr(0, strMsg.length()-2);
	
	// create response string
	string strResponse("HTTP/1.1 200 OK\r\n");

	// construct proper http
	strResponse += "Content-Length: ";
	strResponse += to_string(strMsg.size());
	strResponse += "\r\n";
	strResponse += "\r\n";
	strResponse += strMsg;
	
	// return response for client
	return strResponse;
}

/***********************************************************
 * Connect/Disconnect Definitions
 ***********************************************************/
void TCPServer::Connect(void)
{
	// log function call
	CPPLog(); 
		
	// create client to test connection
	tcp_client_t tcpClient(settings->GetServerIP().c_str(), settings->GetServerPort());
	
	// attempt to establish a connection
	bIsConnected = (tcpClient.connect() >= 0);
	
	// close the client connection test
	tcpClient.close();	
	
	// log failure to connect
	if (!bIsConnected)
		XBMC->Log(LOG_ERROR, "C+: %s - Failed to bind to TCP connection [%s:%i]", __FUNCTION__, settings->GetServerIP().c_str(), settings->GetServerPort());
}

void TCPServer::Disconnect(bool bWait /* = true */)
{
	// log function call
	CPPLog(); 
	
	// log waiting for thread to close
	while (bIsWorking)
	{
		// create client to test connection
		tcp_client_t tcpClient(settings->GetServerIP().c_str(), settings->GetServerPort());
		
		// attempt to establish a connection
		bIsConnected = (tcpClient.connect() >= 0);
		
		// close the client connection test
		tcpClient.close();	
	
		// sleep to allow thread time to close
		sleep(1);
	}

	// force close thread
	StopThread(bWait);
}

/***********************************************************
 * SQL Server Process Definitions
 ***********************************************************/
void *TCPServer::Process(void)
{
	// log creation of thread
	XBMC->Log(LOG_NOTICE, "C+: %s - Started TCP server", __FUNCTION__);
	
	// set thread working
	bIsWorking = true;
	
	// create tcp server
	tcp_server_t tcpServer(settings->GetServerPort());
		
	// create last checked interval
	time_t lastCheck = time(NULL) - settings->GetSchedPoll();

	// execute scheduler
	while (!bStop)
	{	
		// start communication with client
		net_socket_t tcpSocket = tcpServer.accept();
			
		// convert client IP addresses from a dots-and-number string to a struct in_addr
		char *str_ip = inet_ntoa(tcpSocket.m_sockaddr_in.sin_addr);

		// parse request
		string strHeader;

		if (parse_http_headers(tcpSocket.m_sockfd, strHeader) < 0)
		{
			XBMC->Log(LOG_ERROR, "C+: %s - Failed to parse HTTP header of client [%s:%i]", __FUNCTION__, str_ip, tcpSocket.m_sockfd);
		}
		else
		{	
			// parse method and action
			string strMethod = http_get_method(strHeader);
			string strAction = http_get_action(strHeader);

			// act on GET method
			if (strMethod.compare("GET") == 0)
			{
				// call api equivalent
				if      (StringUtils::StartsWith(strAction, "GetDBLog"    )){string strResponse = GetDBLog    (         ); tcpSocket.write_all(strResponse.c_str(), strResponse.size());}
				else if (StringUtils::StartsWith(strAction, "AddRecord"   )){string strResponse = AddRecord   (strAction); tcpSocket.write_all(strResponse.c_str(), strResponse.size());}
				else if (StringUtils::StartsWith(strAction, "UpdateRecord")){string strResponse = UpdateRecord(strAction); tcpSocket.write_all(strResponse.c_str(), strResponse.size());}
				else if (StringUtils::StartsWith(strAction, "GetRecords"  )){string strResponse = GetRecords  (strAction); tcpSocket.write_all(strResponse.c_str(), strResponse.size());}
			}
		}

		// end communication with client
		tcpSocket.close();		
	}	

	// close tcp server
	tcpServer.close();

	// end thread work
	bIsWorking = false;

	// log termination of thread
	XBMC->Log(LOG_NOTICE, "C+: %s - Closed TCP server", __FUNCTION__);

	// return completion
	return NULL;
}