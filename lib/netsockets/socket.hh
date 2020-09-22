#ifndef LIB_SOCKET_H
#define LIB_SOCKET_H

#if defined (_MSC_VER)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#endif
#include <iostream>
#include <cerrno>
#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <ctime>

//multi platform socket descriptor
#if _WIN32
typedef SOCKET socketfd_t;
#else
typedef int socketfd_t;
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
//utils
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string str_extract(const std::string& str_in);
std::string prt_time();
int set_daemon(const char* str_dir);
void wait(int nbr_secs);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//net_socket_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class net_socket_t
{
public:
  net_socket_t();
  net_socket_t(socketfd_t sockfd, sockaddr_in sock_addr);
  void close();
  int write_all(const void* buf, int size_buf);
  int read_all(void* buf, int size_buf);
  int hostname_to_ip(const char* host_name, char* ip);

public:
  socketfd_t m_sockfd; //socket descriptor 
  sockaddr_in m_sockaddr_in; //client address (used to store return value of server accept())
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class tcp_server_t : public net_socket_t
{
public:
  tcp_server_t(const unsigned short server_port);
  net_socket_t accept();
  ~tcp_server_t();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_client_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class tcp_client_t : public net_socket_t
{
public:
  tcp_client_t();
  ~tcp_client_t();
  int connect();

  tcp_client_t(const char* host_name, const unsigned short server_port);
  int connect(const char* host_name, const unsigned short server_port);

protected:
  std::string m_server_ip;
  unsigned short m_server_port;
};

#endif