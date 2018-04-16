/**
	\brief UDP通讯
	\author whyjld
	\file udp.cpp
*/
//------------------------------------------------------------------------------
#include "udp.h"
#include <exception>
//------------------------------------------------------------------------------
#if defined(_WIN32)
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif
//------------------------------------------------------------------------------
UDP::UDP()
	: f_Socket(INVALID_SOCKET)
{
#if defined(_WIN32)
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		throw std::runtime_error("WSAStartup failed.");
	}
#endif
	if ((f_Socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		throw std::runtime_error("Can't create socket.");
	}
	unsigned long enable = 1;
	if (SOCKET_ERROR == ioctlsocket(f_Socket, FIONBIO, &enable))
	{
		throw std::runtime_error("Can't set socket to non block mode.");
	}
}
//------------------------------------------------------------------------------
UDP::~UDP() throw()
{
	closesocket(f_Socket);
#if defined(_WIN32)
	WSACleanup();
#endif
}
//------------------------------------------------------------------------------
void UDP::Update()
{
	sockaddr_in peer;
	socklen_t plen = sizeof(peer);
	memset(&peer, 0, plen);

	const size_t blen = 1024;
	char buf[blen];
	size_t len;
	for (; (len = recvfrom(f_Socket, buf, blen, 0, (struct sockaddr *) &peer, &plen)) != SOCKET_ERROR;)
	{
		if (len > 0 && !!f_OnReceive)
		{
		    buf[len] = 0;
			f_OnReceive(peer, buf, len);
		}
	}
}
//------------------------------------------------------------------------------
bool UDP::Bind(int16_t port, OnReceive_t onrece)
{
	f_OnReceive = onrece;

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	//Bind
	return (bind(f_Socket, (struct sockaddr *)&server, sizeof(server)) != SOCKET_ERROR);
}
//------------------------------------------------------------------------------
bool UDP::SendTo(const char* addr, int16_t port, const char* buf, size_t len)
{
	sockaddr_in to;

	memset(&to, 0, sizeof(to));
	to.sin_family = AF_INET;
	to.sin_port = htons(port);
	to.sin_addr.s_addr = inet_addr(addr);

	return SendTo(to, buf, len);
}
//------------------------------------------------------------------------------
bool UDP::SendTo(const sockaddr_in& addr, const char* buf, size_t len)
{
	return SOCKET_ERROR != sendto(f_Socket, buf, len, 0, (const sockaddr*)&addr, sizeof(addr));
}
//------------------------------------------------------------------------------
