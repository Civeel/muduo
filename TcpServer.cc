#include "TcpServer.h"
#include "Logger.h"
#include "TcpConnection.h"

#include <string.h>

static EventLoop *CheckLoopNotNull(EventLoop *loop) {
    if(loop == nullptr) {
        LOG_FATAL("%s: %s: %d mainloop is null \n", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop,
            const InetAddress &listenAddr,
            const std::string &nameArg,
            Option option)
            : loop_(CheckLoopNotNull(loop))
            , ipPort_(listenAddr.toIpPort())
            , name_(nameArg)
            , acceptor_(new Acceptor(loop, listenAddr, option == kReusePort))
            , threadPool_(new EventLoopThreadPool(loop, name_))
            , connectionCallback_()
            , messageCallback_()
            , nextConnId_()
            , started_(0)
{
    //当有新用户连接时，会执行TcpServer::newConnection回调
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, 
        std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer() {
    for(auto &item : connections_) {
        //这个局部的shared_ptr智能指针对象，可以自动释放new出来的TcpConnection对象资源
        TcpConnectionPtr conn(item.second);
        item.second.reset();

        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}


void TcpServer::setThreadNum(int numThreads) {
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {
    if(started_++ == 0) {//防止一个TcpServer对象被start多次
        threadPool_->start(threadInitCallback_); //启动底层loop线程池
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

//有一个新的客户端的连接，会执行这个回调操作
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    //轮询算法选择一个subloop来管理对应的channel
    EventLoop *ioLoop = threadPool_->getNextLoop();
    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    LOG_INFO("TcpServer::newConnection[%s] - new connection [%s] from %s \n", name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());
    
    //通过sockfd获取其绑定的本机的ip地址和端口信息
    sockaddr_in local;
    ::bzero(&local, sizeof local);
    socklen_t addrlen = sizeof local;
    if(::getsockname(sockfd, (sockaddr*)&local, &addrlen) < 0) {
        LOG_ERROR("socket::getLocalAddr");
    }
    InetAddress localAddr(local);

    //根据连接成功的sockfd创建TcpConnection连接对象
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
    connections_[connName] = conn;
    //下面的回调，都是用户设置给TcpServer -> TcpConnection -> Channel -> Poller -> Channel调用回调
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWritetionCompleteCallback(writeCompleteCallback_);

    //设置了如何关闭连接的回调
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    //直接调用TcpConnection::connectEstalished
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstalished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s \n", name_.c_str(), conn->name().c_str());

    connections_.erase(conn->name());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}
