#include "InetAddress.h"

#include <string.h>
#include <arpa/inet.h>

InetAddress::InetAddress(uint16_t port, std::string ip) {
    bzero(&addr_, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port); //将主机字节序（通常是小端）的端口号转换为网络字节序（大端）
    addr_.sin_addr.s_addr = inet_addr(ip.c_str()); //将点分十进制 IPv4 地址转换为 32 位网络字节序整数
}

std::string InetAddress::toIp() const {
    //addr_
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf); //将 sockaddr_in 中的 IPv4 地址转换为点分十进制字符串
    return buf;
}

std::string InetAddress::toIpPort() const {
    //ip::port
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
    size_t end = strlen(buf);
    uint16_t port = ntohs(addr_.sin_port);
    sprintf(buf+end, ":%u", port); //从buf+end位置组装
    return buf;
}

uint16_t InetAddress::toPort() const {
    /* 
        ntohs()	网络字节序 → 主机字节序	16 位整数
        ntohl()	网络字节序 → 主机字节序	32 位整数
        htons()	主机字节序 → 网络字节序	16 位整数
        htonl()	主机字节序 → 网络字节序	32 位整数
    */
    return ntohs(addr_.sin_port); //将 16位无符号整数从网络字节序（大端序）转换为主机字节序
}

#include <iostream>
int main() {
    InetAddress addr(8080);
    std::cout << addr.toIpPort() << std::endl;
    return 0;
}