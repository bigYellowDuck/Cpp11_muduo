#ifndef TPC_TCPSTREAM_H
#define TPC_TCPSTREAM_H

#include "../base/Noncopyable.h"
#include "Socket.h"

#include <memory>

class InetAddress;


class TcpStream;
using TcpStreamPtr = std::unique_ptr<TcpStream>; 

// A blocking TCP stream
class TcpStream : muduo::Noncopyable
{
 public:
    explicit TcpStream(Socket &&socket);  // sock must be connect
    ~TcpStream() = default;
    TcpStream(TcpStream &&) = default;
    TcpStream& operator=(TcpStream &&) = default;

    static TcpStreamPtr connect(const InetAddress &serverAddr);
    static TcpStreamPtr connect(const InetAddress &serverAddr, const InetAddress &localAddr);

    // NOT thread safe
    int receiveAll(void *buf, int len);  // read len bytes, unless error happens;
    int receiveSome(void *buf, int len); // read len or less bytes;

    int sendAll(const void *buf, int len);   // send len bytes, unless error happens;
    int sendSome(const void *buf, int len);  // send len or less bytes

    // thread safe
    void setTcpNoDelay(bool on);
    void shutdownWrite();

 private:
    static TcpStreamPtr connectInternal(const InetAddress &serverAddr, const InetAddress *localAddr);
    Socket _sock;
};


#endif  // TPC_TCPSTREAM_H

