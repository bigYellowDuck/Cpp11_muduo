#ifndef TPC_SOCKET_H
#define TPC_SOCKET_H

#include "../base/Noncopyable.h"

#include <utility> // swap

class InetAddress;


// RAII handle for socket fd
class Socket : muduo::Noncopyable
{
 public:
    explicit Socket(int sockfd);
    ~Socket(); // close _sockfd;

    Socket(Socket &&rhs)
      : _sockfd(rhs._sockfd)
    {
        rhs._sockfd = -1;
    }

    Socket& operator=(Socket &&rhs)
    {
        swap(rhs);
        return *this;
    }
    
    int fd() const noexcept { return _sockfd; }

    void swap(Socket &rhs)
    {
        std::swap(_sockfd, rhs._sockfd);
    }

    // Sockets API
    void bindOrDie(const InetAddress &addr);
    void listenOrDie();

    // return 0 on success
    int connect(const InetAddress &addr);

    void shutdownWrite();

    void setReuseAddr(bool on);
    void setTcpNoDelay(bool on);

    InetAddress getLocalAddr() const noexcept;
    InetAddress getPeerAddr() const noexcept;

    int read(void *buf, int len);
    int write(const void *buf, int len);

    // factory methods
    static Socket createTCP();
    static Socket createUDP();

 private:
    int _sockfd;
};


#endif  // TCP_SOCKET_H

