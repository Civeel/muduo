#pragma once

#include "Timestamp.h"
#include "noncopyable.h"

#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;
//muduo中的多路事件分发器，IO复用模块
//EPollPoller和PollPoller的基类
class Poller : noncopyable {
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    //给所有IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    //判断参数Channel是否在当前Poller中
    bool hasChannel(Channel *channel) const;

    //EventLoop可以通过该接口获取默认的IO复用具体实现
    static Poller* newDefaultPoller(EventLoop *loop);
protected:
    using ChannelMap = std::unordered_map<int, Channel*>;
    //map的key:sockfd value:sockfd所属的Channel通道类型
    ChannelMap channels_;
private:
    EventLoop* ownerLoop_; //定义poller所属的事件循环EventLoop
};