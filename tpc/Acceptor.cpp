#include "Acceptor.h"
#include "InetAddress.h"

Acceptor::Acceptor(const InetAddress &listenaddr)
  : _listenSock(Socket::createTCP())
{
    _listenSock.setReuseAddr(true);
    _listenSock.bindOrDie(listenaddr);
    _listenSock.listenOrDie();
}

TcpStreamPtr Acceptor::accept()
{
    int sockfd = ::accept4(_listenSock.fd(), NULL, NULL, SOCK_CLOEXEC);
    if (sockfd >= 0)
    {
        return TcpStreamPtr(new TcpStream(Socket(sockfd)));
    }
    else
    {
        perror("Acceptor::accept");
        return TcpStreamPtr();
    }
}
