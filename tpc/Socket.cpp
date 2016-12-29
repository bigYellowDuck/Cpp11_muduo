#include "Socket.h"
#include "InetAddress.h"

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <netinet/tcp.h>

Socket::Socket(int sockfd)
  : _sockfd(sockfd)
{
    assert(_sockfd > 0);
}

Socket::~Socket()
{
    if (_sockfd > 0)
    {
        int ret = ::close(_sockfd);
        assert(ret == 0);
    }
}

void Socket::bindOrDie(const InetAddress &addr)
{
    const struct sockaddr_in &saddr = addr.getSockAddrInet();
    int ret = ::bind(_sockfd, (const struct sockaddr *)&saddr, sizeof(saddr));
    if (ret)
    {
        perror("Socket::bindOrDie");
        abort();
    }
}

void Socket::listenOrDie()
{
    int ret = ::listen(_sockfd, SOMAXCONN);
    if (ret)
    {
        perror("Socket::listenOrDie");
        abort();
    }
}

int Socket::connect(const InetAddress &addr)
{
    const struct sockaddr_in &saddr = addr.getSockAddrInet();
    return ::connect(_sockfd, (const struct sockaddr *)&saddr, sizeof(saddr));
}

void Socket::shutdownWrite()
{
    if (::shutdown(_sockfd, SHUT_WR))
    {
        perror("Socket::shutdownWrite");
    }
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    if (::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        perror("Socket::setReuseAddr");
    }
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    if (::setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) < 0)
    {
        perror("Socket::setTcpNoDelay");
    }
}

InetAddress Socket::getLocalAddr() const noexcept 
{
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof(localaddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(localaddr));
    if (::getsockname(_sockfd, (struct sockaddr *)&localaddr, &addrlen) < 0)
    {
        perror("Socket::getLocalAddr");
    }
    return InetAddress(localaddr); 
}

InetAddress Socket::getPeerAddr() const noexcept 
{
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof(peeraddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
    if (::getpeername(_sockfd, (struct sockaddr *)&peeraddr, &addrlen) < 0)
    {
        perror("Socket::getPeerAddr");
    }
    return InetAddress(peeraddr);
}

int Socket::read(void *buf, int len)
{
    char *ptr = static_cast<char *>(buf);
    int nleft = len;
    int nread;

    while (nleft > 0)
    {
        if ((nread = ::read(_sockfd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;   // call read() again
            else
                return -1;
        } else if (nread == 0)
            break;  // EOF

        nleft -= nread;
        ptr += nread;
    }
    return len-nleft;
}

int Socket::write(void *buf, int len)
{
    char *ptr = static_cast<char *>(buf);
    int nleft = len;
    int nwritten;

    while (nleft > 0)
    {
        if ((nwritten = ::write(_sockfd, ptr, nleft) <= 0))
        {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;  // and call write() again
            else    
                return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return len;
}

Socket Socket::createTCP()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    assert(sockfd >= 0);
    return Socket(sockfd);
}

Socket Socket::createUDP()
{
    int sockfd = ::socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
    assert(sockfd >= 0);
    return Socket(sockfd);
}

