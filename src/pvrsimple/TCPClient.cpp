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
#include "TCPClient.h"

/***********************************************************
 * Global Definitions
 ***********************************************************/
extern PVRSettings *settings;
extern IPTVClient  *iptv    ;
extern DVRClient   *dvr     ;

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
TCPClient::TCPClient()
{
	// log attempt to create object
	XBMC->Log(LOG_NOTICE, "C+: %s - Creating TCP client", __FUNCTION__);
	
	// no establish connection at start
	bIsConnected = false;
	bIsWorking   = false;
	bStop        = false;
	
	// log connection attempt
	XBMC->Log(LOG_NOTICE, "C+: %s - Attempting to connect to TCP connection [%s:%i]", __FUNCTION__, settings->GetServerIP().c_str(), settings->GetServerPort());
		
	// call connection to database
	Connect();
	
	// log creation of object
	XBMC->Log(LOG_NOTICE, "C+: %s - Created TCP client", __FUNCTION__);
	
	// create thread
	if (IsConnected()) CreateThread();
}

TCPClient::~TCPClient(void)
{
	// mark as stopped
	bStop = true;
	 
	// attempt to disconnect
	Disconnect();	
}

/***********************************************************
 * Get Staus/Local Variable API Definitions
 ***********************************************************/
bool TCPClient::IsConnected(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return bIsConnected;
}

bool TCPClient::IsWorking(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return bIsWorking;
}

/***********************************************************
 * Records API Definitions
 ***********************************************************/
void TCPClient::AddRecord(const char* strTable, const string strRecord, SQLRecord* sqlRecord /* = NULL */)
{
	// log function call
	CPPLog(); 
	
	// create connection
	tcp_client_t tcpClient(settings->GetServerIP().c_str(), settings->GetServerPort());
	
	// if connected check log
	if (tcpClient.connect() >= 0)
	{
		// create buffer for request
		char strRequest[4096];	

		// create http request
		sprintf(strRequest, "GET /AddRecord(\"%s\", %s) HTTP/1.1\r\n\r\n", strTable, strRecord.c_str());
		
		// send to client and fetch response
		if(tcpClient.write_all(strRequest, strlen(strRequest)) >= 0)
		{
			// only assign return if not null pointer
			if (sqlRecord)
			{
				// prepare for response
				string strResponse;
				
				// get respone
				if (http_get_response(tcpClient, strResponse) > 0)
				{				
					// create records from message string
					SQLRecord sqlReturn(strResponse);
						
					// point pointer to record
					*sqlRecord = sqlReturn;
				}
			}
		}	
	}

	// close connection
	tcpClient.close();	
}

void TCPClient::UpdateRecord(const char* strTable, const string strRecord, SQLRecord* sqlRecord /* = NULL */)
{
	// log function call
	CPPLog(); 
	
	// create connection
	tcp_client_t tcpClient(settings->GetServerIP().c_str(), settings->GetServerPort());
	
	// if connected check log
	if (tcpClient.connect() >= 0)
	{
		// create buffer for request
		char strRequest[4096];	

		// create http request
		sprintf(strRequest, "GET /UpdateRecord(\"%s\", %s) HTTP/1.1\r\n\r\n", strTable, strRecord.c_str());
		
		// send to client and fetch response
		if(tcpClient.write_all(strRequest, strlen(strRequest)) >= 0)
		{
			// only assign return if not null pointer
			if (sqlRecord)
			{
				// prepare for response
				string strResponse;
				
				// get respone
				if (http_get_response(tcpClient, strResponse) > 0)
				{				
					// create records from message string
					SQLRecord sqlReturn(strResponse);
						
					// point pointer to record
					*sqlRecord = sqlReturn;
				}
			}
		}	
	}

	// close connection
	tcpClient.close();	
}

/***********************************************************
 * Tables API Definitions
 ***********************************************************/
vector<SQLRecord> TCPClient::GetRecords(const char* strTable)
{
	// log function call
	CPPLog(); 
	
	// create return vector
	vector<SQLRecord> sqlReturn;
	
	// clear container
	sqlReturn.clear();
	
	// create connection
	tcp_client_t tcpClient(settings->GetServerIP().c_str(), settings->GetServerPort());
	
	// if connected check log
	if (tcpClient.connect() >= 0)
	{
		// create buffer for request
		char strRequest[1024];	

		// create http request
		sprintf(strRequest, "GET /GetRecords(\"%s\") HTTP/1.1\r\n\r\n", strTable);
		
		// send to client and fetch response
		if(tcpClient.write_all(strRequest, strlen(strRequest)) >= 0)
		{
			// prepare for response
			string strResponse;
			
			// get respone
			if (http_get_response(tcpClient, strResponse) > 0)
			{
				// parse response
				vector<string> strLog = StringUtils::Split(strResponse, "\n");
				
				// iterate and trigger update
				for (vector<string>::iterator strMsg = strLog.begin(); strMsg != strLog.end(); strMsg++)
				{
					// create records from message string
					SQLRecord sqlRecord(*strMsg);
					
					// add to return vector
					sqlReturn.push_back(sqlRecord);
				}
			}
		}	
	}

	// close connection
	tcpClient.close();
			
	// return container
	return sqlReturn;
}

/***********************************************************
 * Connect/Disconnect Definitions
 ***********************************************************/
void TCPClient::Connect(void)
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
		XBMC->Log(LOG_ERROR, "C+: %s - Failed to connect to TCP connection [%s:%i]", __FUNCTION__, settings->GetServerIP().c_str(), settings->GetServerPort());
}

void TCPClient::Disconnect(bool bWait /* = true */)
{
	// log function call
	CPPLog(); 
	
	// all connections closed in process thread
}

/***********************************************************
 * SQL Server Process Definitions
 ***********************************************************/
void *TCPClient::Process(void)
{
	// wait for pvr objects to initialize
	while (!iptv || !dvr)
	{
		// sleep to allow objects to open
		sleep(1);
	}
	
	// log creation of thread
	XBMC->Log(LOG_NOTICE, "C+: %s - Started TCP client", __FUNCTION__);
	
	// set thread working
	bIsWorking = true;
	
	// create last checked interval
	time_t lastCheck = time(NULL) - settings->GetSchedPoll();
	
	// execute client
	while (!bStop)
	{
		// sleep thread so machine doesn't idle at 100% cpu
		sleep(1);
		
		// if polling interval passed to check database updates
		if (lastCheck + settings->GetSchedPoll() <= time(NULL))
		{	
			// create connection
			tcp_client_t tcpClient(settings->GetServerIP().c_str(), settings->GetServerPort());
			
			// if connected check log
			if (tcpClient.connect() >= 0)
			{
				// log if reconnect
				if (!IsConnected())
				{
					// notify on screen
					PVR->ConnectionStateChange("Connected", PVR_CONNECTION_STATE_CONNECTED, "DVR reconnected");
					
					// send message to log
					XBMC->Log(LOG_NOTICE, "C+: %s - The TCP client reconnected to the TCP server [%s:%i]", __FUNCTION__, settings->GetServerIP().c_str(), settings->GetServerPort());
				}
				
				// set connection
				bIsConnected = true;
			
				// create buffer for request
				char strRequest[1024];	

				// create http request
				sprintf(strRequest, "GET /GetDBLog() HTTP/1.1\r\n\r\n");
				
				// send to client and fetch response
				if(tcpClient.write_all(strRequest, strlen(strRequest)) >= 0)
				{
					// prepare for response
					string strResponse;
					
					// get respone
					if (http_get_response(tcpClient, strResponse) > 0)
					{
						// parse response
						vector<string> strLog = StringUtils::Split(strResponse, "\n");
						
						// iterate and trigger update
						for (vector<string>::iterator strMsg = strLog.begin(); strMsg != strLog.end(); strMsg++)
						{
							// create message from string
							SQLMsg sqlMsg;
							
							sqlMsg.strTable =      ParseSQLValue(*strMsg, "<strTable>", "") ; 
							sqlMsg.iModTime = stoi(ParseSQLValue(*strMsg, "<iModTime>",  0)); 
							
							// check for changes		
							if (sqlMsg.strTable == "Channels"  )
								if (iptv->LastChannelsSync()   < sqlMsg.iModTime) 
									{XBMC->Log(LOG_NOTICE, "C+: %s - The channels were updated on the backend, proceed to sync"   , __FUNCTION__); iptv->TriggerChannelsUpdate (sqlMsg.iModTime);}

							if (sqlMsg.strTable == "EpgEntries")
								if (iptv->LastEpgEntriesSync() < sqlMsg.iModTime) 
									{XBMC->Log(LOG_NOTICE, "C+: %s - The epg entries were updated on the backend, proceed to sync", __FUNCTION__); iptv->TriggerEpgUpdate      (sqlMsg.iModTime);}
								
							if (sqlMsg.strTable == "Timers"    )
								if (dvr->LastTimersSync()      < sqlMsg.iModTime) 
									{XBMC->Log(LOG_NOTICE, "C+: %s - The timers were updated on the backend, proceed to sync"     , __FUNCTION__); dvr->TriggerTimersUpdate    (sqlMsg.iModTime);}
								
							if (sqlMsg.strTable == "Recordings")
								if (dvr->LastRecordingsSync()  < sqlMsg.iModTime) 
									{XBMC->Log(LOG_NOTICE, "C+: %s - The recordings were updated on the backend, proceed to sync" , __FUNCTION__); dvr->TriggerRecordingsUpdate(sqlMsg.iModTime);}
						}
						
						// log last check
						lastCheck = time(NULL);
					}
				}	
			}
			else
			{
				// log if disconnect
				if (IsConnected())
				{
					// notify on screen
					PVR->ConnectionStateChange("Disconnected", PVR_CONNECTION_STATE_DISCONNECTED, "DVR disconnected");
					
					// send message to log
					XBMC->Log(LOG_ERROR, "C+: %s - The TCP client disconnected from the TCP server [%s:%i]", __FUNCTION__, settings->GetServerIP().c_str(), settings->GetServerPort());
				}
				
				// set connection
				bIsConnected = false;				
			}

			// close connection
			tcpClient.close();
		}		
	}

	// end thread work
	bIsWorking = false;

	// log termination of thread
	XBMC->Log(LOG_NOTICE, "C+: %s - Closed TCP client", __FUNCTION__);

	// return completion
	return NULL;
}