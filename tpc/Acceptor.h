#ifndef TPC_ACCEPTOR_H
#define TPC_ACCEPTOR_H

#include "../base/Noncopyable.h"
#include "Socket.h"

#include <memory>

class InetAddress;

class TcpStream;
using TcpStreamPtr = std::unique_ptr<TcpStream>;

class Acceptor : muduo::Noncopyable
{
 public:
    explicit Acceptor(const InetAddress &listenaddr);

    ~Acceptor() = default;
    Acceptor(Acceptor &&) = default;
    Acceptor& operator=(Acceptor &&) = default;

    TcpStreamPtr accept();

 private:
    Socket _listenSock;
};


#endif  // TPC_ACCEPTOR_H

