#pragma once

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

class Channel;
class Poller;
/* 
事件循环类，主要包含了Channel Poller（epoll的抽象）
一个线程有一个EventLoop，一个EventLoop有一个Poller，一个Poller监听许多Channel
*/

class EventLoop : noncopyable {
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    //开启事件循环
    void loop();
    //退出事件循环
    void quit();

    Timestamp pollReturnTime() const {return pollReturnTime_;}

    //在当前loop中执行
    void runInLoop(Functor cb);
    //把cb放入队列中，唤醒loop所在的线程执行cb
    void queueInLoop(Functor cb);

    //唤醒loop所在线程
    void wakeup();

    //EventLoop方法，调用Poller方法
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    //判断EventLoop对象是否在自己的线程里面
    bool isInLoopThread() const {return threadId_ == CurrentThread::tid();}
private:
    void handleRead();
    //执行回调
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    //原子操作的布尔值，通过CAS实现
    std::atomic_bool looping_;
    //标志退出loop循环
    std::atomic_bool quit_;
    //记录当前loop所在线程的id
    const pid_t threadId_;
    //poller返回发生事件的channels的时间点
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    //主要作用：当mainloop(mainReactor)获取一个新用户的channel，通过轮询算法选择一个subloop(subReactor)
    //通过该成员唤醒subloop处理channel
    //通常是一个 eventfd 或管道（pipe）的文件描述符，用于线程间通知。
    //当其他线程需要唤醒事件循环处理任务时，会向 wakeupFd_ 写入数据，触发可读事件
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;

    //标识当前loop是否有需要执行的回调操作
    std::atomic_bool callingPendingFunctors_;
    //存储loop需要执行的所有回调操作
    std::vector<Functor> pendingFunctors_;
    //互斥锁，用于保护上面vector容器的线程安全操作
    std::mutex mutex_;
};