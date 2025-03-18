#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false){}

Channel::~Channel() {}

//什么时候被调用?一个TcpConnection新连接创建的时候
void Channel::tie(const std::shared_ptr<void> &obj) {
    tie_ = obj;
    tied_ = true;
}

//当改变Channel所表示fd的事件后，负责在Poller中更改fd相应的事件, epoll_ctl
//ChannelLists通过EventLoop更改Poller中的事件
void Channel::update() {
    //通过Channel所属的EventLoop，调用Poller的相应方法，注册fd的events
    loop_->updateChannel(this);
}

//在Channel所属的EventLoop中，删除当前Channel
void Channel::remove() {
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime) {
    std::shared_ptr<void> guard;
    if(tied_) {
        guard = tie_.lock(); //提升为shared_ptr
        //提升成功
        if(guard) {
            handleEventWithGuard(receiveTime);
        }
    } else {
        handleEventWithGuard(receiveTime);
    }
}

//根据Poller通知的Channel发生的具体事件，由Channel执行相应回调操作
void Channel::handleEventWithGuard(Timestamp receiveTime) {
    LOG_INFO("Channel handle revents: %d\n", revents_);

    //revents_为EPOLLHUP或者不为EPOLLIN，则关闭
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if(closeCallback_) {
            closeCallback_();
        }
    }

    //revents_出错
    if(revents_ & EPOLLERR) {
        if(errorCallback_) {
            errorCallback_();
        }
    }

    if(revents_ & (EPOLLIN | EPOLLPRI)) {
        if(readCallback_) {
            readCallback_(receiveTime);
        }
    }

    if(revents_ & EPOLLOUT) {
        if(writeCallback_) {
            writeCallback_();
        }
    }
}

