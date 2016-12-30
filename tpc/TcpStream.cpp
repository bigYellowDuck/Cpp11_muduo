#include "TcpStream.h"
#include "InetAddress.h"

#include <unistd.h>
#include <signal.h>

namespace
{

class IgnoreSigPipe
{
 public:
    IgnoreSigPipe()
    {
        ::signal(SIGPIPE, SIG_IGN);
    }
} initObj;

bool isSelfConnection(const Socket &sock)
{
    return sock.getLocalAddr() == sock.getPeerAddr();
}

}


TcpStream::TcpStream(Socket &&socket)
  : _sock(std::move(socket))
{
}

int TcpStream::receiveAll(void *buf, int len)
{
    return _sock.read(buf, len);
}

int TcpStream::receiveSome(void *buf, int len)
{
    return ::read(_sock.fd(), buf, len);
}

int TcpStream::sendAll(const void *buf, int len)
{
    return _sock.write(buf, len);
}

int TcpStream::sendSome(const void *buf, int len)
{
    return ::write(_sock.fd(), buf, len);
}

void TcpStream::setTcpNoDelay(bool on)
{
    _sock.setTcpNoDelay(on);
}

void TcpStream::shutdownWrite()
{
    _sock.shutdownWrite();
}


TcpStreamPtr TcpStream::connect(const InetAddress &serverAddr)
{
    return connectInternal(serverAddr, nullptr);
}

TcpStreamPtr TcpStream::connect(const InetAddress &serverAddr, const InetAddress &localAddr)
{
    return connectInternal(serverAddr, &localAddr);
}

TcpStreamPtr TcpStream::connectInternal(const InetAddress &serverAddr, const InetAddress *localAddr)
{
    TcpStreamPtr stream;
    Socket sock(Socket::createTCP());
    if (localAddr)
    {
        sock.bindOrDie(*localAddr);
    }
    if (sock.connect(serverAddr) == 0 && !isSelfConnection(sock))
    {
        stream.reset(new TcpStream(std::move(sock)));
    }

    return stream;
}

