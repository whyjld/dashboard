/**
	\brief UDP通讯
	\author whyjld
	\file udp.h
*/
//------------------------------------------------------------------------------
#ifndef _UDP_h_
#define _UDP_h_ "udp.h"
//------------------------------------------------------------------------------
#if defined(_WIN32)
#	include <winsock2.h>
#elif defined(__linux__) || defined(__APPLE__)
#	include <sys/socket.h>
#   include <sys/ioctl.h>
#   include <sys/types.h>
#   include <arpa/inet.h>
#   include <netdb.h>
#   include <string.h>
#   include <unistd.h>
#   define SOCKET int
#   define closesocket close
#   define ioctlsocket ioctl
#   define INVALID_SOCKET (-1)
#   define SOCKET_ERROR (-1)
#else
#	error "Not supported platform"
#endif
#include <cstdint>
#include <functional>
//------------------------------------------------------------------------------
typedef std::function<void(const sockaddr_in& sa, const char* buf, size_t len)> OnReceive_t;
//------------------------------------------------------------------------------
/**
	\brief UDP
*/
class UDP
{
public:
	UDP();
	~UDP() throw();
	void Update();
	bool Bind(int16_t port, OnReceive_t onrece);
	bool SendTo(const char* addr, int16_t port, const char* buf, size_t len);
	bool SendTo(const sockaddr_in& addr, const char* buf, size_t len);
private:
	SOCKET f_Socket;
	OnReceive_t f_OnReceive;
};
//------------------------------------------------------------------------------
#endif//_UDP_h_
