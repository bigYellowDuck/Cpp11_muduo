#include "InetAddress.h"

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

InetAddress::InetAddress(const std::string &ipPort)
{
    auto colon = ipPort.find(':');
    if (colon == std::string::npos)
    {
        fprintf(stderr, "string ipPort error");
        exit(1);
    }

    uint16_t port = static_cast<uint16_t>(::atoi(std::string(ipPort.begin()+colon+1, ipPort.end()).data()));
    helpConsturct(std::string(ipPort.begin(), ipPort.begin()+colon).data(), port);
    
}

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    helpConsturct(ip.data(), port);
}

InetAddress::InetAddress(uint16_t port, bool loopbackOnly)
{
    ::bzero(&_saddr, sizeof(_saddr));
    _saddr.sin_family = AF_INET;
    _saddr.sin_addr.s_addr = ::htonl(loopbackOnly ? INADDR_LOOPBACK :INADDR_ANY);
    _saddr.sin_port = ::htons(port);
}

std::string InetAddress::toIp() const noexcept
{
    char buf[32] = "";
    ::inet_ntop(AF_INET, &_saddr.sin_addr, buf, sizeof(buf));
    return buf;
}

std::string InetAddress::toIpPort() const noexcept 
{
    char buf[32] = ""; // "255.255.255.255:65536" = 4*4+5+1 = 22
    ::inet_ntop(AF_INET, &_saddr.sin_addr, buf, sizeof(buf));
    int end = ::strlen(buf);
    uint16_t port = portHostEndian();
    snprintf(buf+end, sizeof(buf) - end, ":%u", port);
    return buf;
}

bool InetAddress::operator==(const InetAddress &rhs) const noexcept
{
    return _saddr.sin_family == rhs._saddr.sin_family
        && ipNetEndian() == rhs.ipNetEndian()
        && portNetEndian() == rhs.portNetEndian();
}

void InetAddress::helpConsturct(const char *ip, uint16_t port)
{
    ::bzero(&_saddr, sizeof(_saddr));
    _saddr.sin_family = AF_INET;
    ::inet_pton(AF_INET, ip, &_saddr.sin_addr);
    _saddr.sin_port = ::htons(port);
}

