
#include "UDPClient.h"

// Function to fill in address structure given an address and port
static void fillAddr(const string &address, unsigned short port,
                     sockaddr_in &addr) {
  memset(&addr, 0, sizeof(addr));  // Zero out address structure
  addr.sin_family = AF_INET;       // Internet address

  hostent *host;  // Resolve name
  host = gethostbyname(address.c_str());
  addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);

  addr.sin_port = htons(port);     // Assign port in network byte order
}


Socket::Socket(int type, int protocol) {
  // Make a new socket
  sockDesc = socket(PF_INET, type, protocol);
}

Socket::Socket(int sockDesc) {
  this->sockDesc = sockDesc;
}

Socket::~Socket() {
	close(sockDesc);
  sockDesc = -1;
}

string Socket::getLocalAddress() {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len);
  return inet_ntoa(addr.sin_addr);
}

unsigned short Socket::getLocalPort() {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len);
  return ntohs(addr.sin_port);
}

void Socket::setLocalPort(unsigned short localPort)
{
	int errno;
	// Bind the socket to its port
	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
  	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(localPort);

	errno = 0;
	if(bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0)
	{
		//cout << "localPort = " << localPort << " errno = " << errno << "\n";
		throw "bind";
	}
}

void Socket::setLocalAddressAndPort(const string &localAddress,
    unsigned short localPort) {
  // Get the address of the requested host
  sockaddr_in localAddr;
  fillAddr(localAddress, localPort, localAddr);

  if(bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in))<1)
  {
	  throw "bind";
  }
}

void Socket::cleanUp() {
}

unsigned short Socket::resolveService(const string &service,
                                      const string &protocol) {
  struct servent *serv;        /* Structure containing service information */

  if ((serv = getservbyname(service.c_str(), protocol.c_str())) == NULL)
    return atoi(service.c_str());  /* Service is port number */
  else
    return ntohs(serv->s_port);    /* Found port (network byte order) by name */
}

// CommunicatingSocket Code

CommunicatingSocket::CommunicatingSocket(int type, int protocol)
    : Socket(type, protocol) {
}

CommunicatingSocket::CommunicatingSocket(int newConnSD) : Socket(newConnSD) {
}

void CommunicatingSocket::connect(const string &foreignAddress,
    unsigned short foreignPort) {
  // Get the address of the requested host
  sockaddr_in destAddr;
  fillAddr(foreignAddress, foreignPort, destAddr);

  // Try to connect to the given port
  ::connect(sockDesc, (sockaddr *) &destAddr, sizeof(destAddr));
}

void CommunicatingSocket::send(const void *buffer, int bufferLen) {
  ::send(sockDesc, (raw_type *) buffer, bufferLen, 0);
}

int CommunicatingSocket::recv(void *buffer, int bufferLen) {
  int rtn;
  rtn = ::recv(sockDesc, (raw_type *) buffer, bufferLen, 0);

  return rtn;
}

string CommunicatingSocket::getForeignAddress() {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  getpeername(sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len);
  return inet_ntoa(addr.sin_addr);
}

unsigned short CommunicatingSocket::getForeignPort()  {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  getpeername(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len);
  return ntohs(addr.sin_port);
}

// UDPSocket Code

UDPSocket::UDPSocket()  : CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP) {
  setBroadcast();
}

UDPSocket::UDPSocket(unsigned short localPort) : CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP)
{
	setLocalPort(localPort);
	setBroadcast();
}

UDPSocket::UDPSocket(const string &localAddress, unsigned short localPort)
      : CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP) {
  setLocalAddressAndPort(localAddress, localPort);
  setBroadcast();
}

void UDPSocket::setBroadcast() {
  // If this fails, we'll hear about it when we try to send.  This will allow
  // system that cannot broadcast to continue if they don't plan to broadcast
  int broadcastPermission = 1;
  struct timeval tvRcv;
  struct timeval tvSnd;
  tvRcv.tv_usec = 100 * 1000;
  tvSnd.tv_usec = 100 * 1000;
  setsockopt(sockDesc, SOL_SOCKET, SO_BROADCAST,
             (raw_type *) &broadcastPermission, sizeof(broadcastPermission));
  setsockopt(sockDesc, SOL_SOCKET, SO_RCVTIMEO,
             (struct timeval*) &tvRcv, sizeof(struct timeval));
  setsockopt(sockDesc, SOL_SOCKET, SO_SNDTIMEO,
               (struct timeval*) &tvSnd, sizeof(struct timeval));
}

void UDPSocket::disconnect()  {
  sockaddr_in nullAddr;
  memset(&nullAddr, 0, sizeof(nullAddr));
  nullAddr.sin_family = AF_UNSPEC;

  // Try to disconnect
  ::connect(sockDesc, (sockaddr *) &nullAddr, sizeof(nullAddr));
}

void UDPSocket::sendTo(const void *buffer, int bufferLen,
    const string &foreignAddress, unsigned short foreignPort, bool NeedCloseSocket)
     {
  sockaddr_in destAddr;
  fillAddr(foreignAddress, foreignPort, destAddr);

  // Write out the whole buffer as a single message.
  int send = sendto(sockDesc, (raw_type *) buffer, bufferLen, 0,
             (sockaddr *) &destAddr, sizeof(destAddr));

  if (send < 0)
  {
	  //cout<< "errno: " << errno <<" TimeOut\\n";
	  if(NeedCloseSocket) close(sockDesc);
	  throw "SendTo";
  }
}

int UDPSocket::recvFrom(void *buffer, int bufferLen, string &sourceAddress, unsigned short &sourcePort, bool NeedCloseSocket)
{
  sockaddr_in clntAddr;
  socklen_t addrLen = sizeof(clntAddr);
  int rtn;
  int errno;
  //cout << "UDPSocket::recvFrom. buffer = " << buffer << " bufferLen = " << bufferLen << " sockDesc = " << sockDesc << "\n";
  rtn = recvfrom(sockDesc, (raw_type *) buffer, bufferLen, 0,(sockaddr *) &clntAddr, (socklen_t *) &addrLen);
  //cout << "After recvfrom(sockDesc...\n";

  errno = 0;
  if (rtn == -1)
  {
	 // cout << "UDPSocket::recvFrom. rtn = -1. errno = " << errno << "\n";
	  //cout<< "errno: " << errno <<" TimeOut\\n";
	  if(NeedCloseSocket) close(sockDesc);
	  throw "recvfrom";
  }

  sourceAddress = inet_ntoa(clntAddr.sin_addr);
  sourcePort = ntohs(clntAddr.sin_port);
  return rtn;
}

void UDPSocket::setMulticastTTL(unsigned char multicastTTL)  {
  setsockopt(sockDesc, IPPROTO_IP, IP_MULTICAST_TTL,
                 (raw_type *) &multicastTTL, sizeof(multicastTTL));
}

void UDPSocket::joinGroup(const string &multicastGroup)  {
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
  setsockopt(sockDesc, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                 (raw_type *) &multicastRequest,
                 sizeof(multicastRequest));
}

void UDPSocket::leaveGroup(const string &multicastGroup)  {
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
  setsockopt(sockDesc, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                 (raw_type *) &multicastRequest,
                 sizeof(multicastRequest));
}
