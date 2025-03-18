#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory>

//防止一个线程创建多个EventLoop对象
__thread EventLoop *t_loopInThisThread = nullptr;

//定义默认的poller IO复用接口的超时时间
const int kPollTimeMs = 10000;

//创建wakeupfd，用来唤醒subreactor处理新来的channel
int createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0) {
        LOG_FATAL("eventfd error: %d \n", errno);
    }
    return evtfd;
}

EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , callingPendingFunctors_(false)
    , threadId_(CurrentThread::tid())
    , poller_(Poller::newDefaultPoller(this))
    , wakeupFd_(createEventfd())
    , wakeupChannel_(new Channel(this, wakeupFd_))
{
    LOG_DEBUG("EventLoop created %p in thread %d \n", this, threadId_);
    if(t_loopInThisThread) {
        LOG_FATAL("Another EventLoop %p exists in this thread %d \n", t_loopInThisThread, threadId_);
    } else {
        t_loopInThisThread = this;
    }

    //设置wakeupfd的事件类型以及发生事件后的回调操作
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    //每一个EventLoop都将监听wakeupChannel的EPOLLIN读事件
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping \n", this);

    while(!quit_) {
        activeChannels_.clear();
        //监听两类fd：client的fd wakeup的fd
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for(Channel *channel : activeChannels_) {
            //Poller监听哪些channel发生事件了，然后上报给eventloop，通知channel处理相应事件
            channel->handleEvent(pollReturnTime_);
        }
        //执行当前eventloop事件循环需要处理的回调操作
        /* 
        IO线程（mainloop）主要负责accept新用户的连接，返回一个fd，用Channel打包
        已连接用户的channel分发给subloop（多个）
        mainloop拿到与新用户通信的channel后，唤醒某个subloop
        mainloop事先注册一个回调cb（需要subloop来执行），唤醒subloop后，执行之前mainloop注册的cb操作
        */
        doPendingFunctors();
    }
    LOG_INFO("EventLoop %p stop looping. \n", this);
    looping_ = false;
}

//退出事件循环
/* 
1.loop在自己线程中调用quit()
2.在其他线程中调用quit()

                    mainLoop
                    /      \ 
              通过wakeupFd_唤醒
                  /         \
        subLoop1    subLoop2    subLoop3
若在自己的线程中quit，则直接quit_ = true
若想在subLoop1关闭subLoop2，则需要先wakeup subLoop2，使其looping_ = false;结束循环
*/
void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread()) { //在subloop(worker thread)中调用mainloop(IO thread)的quit
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb) {
    if(isInLoopThread()) { //在当前的loop线程中执行cb
        cb();
    } else { //在非当前的loop线程中执行cb，需要唤醒loop所在线程执行cb
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    //唤醒相应的，需要执行上面回调操作的loop的线程了
    //callingPendingFunctors_ = true说明正在执行回调，但是loop又有了新的回调
    if(!isInLoopThread() || callingPendingFunctors_) {
        wakeup(); //唤醒loop所在线程
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof one);
    if(n != sizeof one) {
        LOG_ERROR("EventLoop::handleRead() reads %ld bytes instead of 8", n);
    }
}

//用来唤醒loop所在的线程，向wakeupfd_写一个数据，wakeupChannel就发生读事件，loop线程从poller_->poll()处被唤醒
void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof one);
    if(n != sizeof one) {
        LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8 \n", n);
    }
}

void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
    return poller_->hasChannel(channel);
}

//执行回调
void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(const Functor &functor : functors) {
        functor(); //执行当前loop需要执行的回调操作
    }
    callingPendingFunctors_ = false;
}
