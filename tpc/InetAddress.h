#ifndef TPC_INETADDRESS_H
#define TPC_INETADDRESS_H

#include <netinet/in.h>
#include <string>

class InetAddress
{
 public:
    explicit InetAddress(const std::string &ipPort);  // "1.2.3.4:5678"
    InetAddress(const std::string &ip, uint16_t port);
    InetAddress(uint16_t port, bool loopbackOnly = false);
    
    explicit InetAddress(const struct sockaddr_in &saddr)
      : _saddr(saddr)
    { }

    // default copy ctor/assignment, move ctor/assignment dtor are Okay
    
    std::string toIp() const noexcept;
    std::string toIpPort() const noexcept;

    const struct sockaddr_in& getSockAddrInet() const noexcept { return _saddr; }
    void setSockAddrInet(const struct sockaddr_in &saddr) { _saddr = saddr; }

    void setPort(uint16_t port) { _saddr.sin_port = ::htons(port); }
    
    uint32_t ipNetEndian() const noexcept { return _saddr.sin_addr.s_addr; }
    uint16_t portNetEndian() const noexcept { return _saddr.sin_port; }

    uint32_t ipHostEndian() const noexcept { return ::ntohl(_saddr.sin_addr.s_addr); }
    uint16_t portHostEndian() const noexcept { return ::ntohs(_saddr.sin_port); }
    
    bool operator==(const InetAddress &rhs) const noexcept;
 private:
    void helpConsturct(const char *ip, uint16_t port);
    struct sockaddr_in _saddr; 
};

#endif  // MUDUO_TPC_INETADDRESS_H

