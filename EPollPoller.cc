#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>

const int kNew = -1; //未添加至Poller  index_ = -1
const int kAdded = 1; //已添加至Poller  index_ = 1
const int kDeleted = 2; //已从Poller删除  index_ = 2

EPollPoller::EPollPoller(EventLoop *loop) 
    : Poller(loop)
    , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize) //初始化events_的大小
{
    if(epollfd_ < 0) {
        LOG_FATAL("epollfd_create error: %d\n", errno);
    }
}

EPollPoller::~EPollPoller() {
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels) {
    //实际应该用LOG_DEBUG更合理
    LOG_INFO("func = %s => fd total count: %lu\n", __FUNCTION__, channels_.size());

    int numEvents = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());
    if(numEvents > 0) {
        LOG_INFO("%d events happened \n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if(numEvents == 0) {
        LOG_DEBUG("%s timeout! \n", __FUNCTION__);
    } else {
        if(saveErrno != EINTR) {
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll() err!");
        }
    }
    return now;
}

/* 
Channel的update remove 调用 EventLoop的updateChannel removeChannel 调用 Poller的updateChannel removeChannel

                                EventLoop
                                /       \
                               /         \
                         ChannelList    Poller
                                          |
                                          | 
                                      ChannelMap : <fd, channel*>
ChannelList(事件列表) >= ChannelMap(注册在Poller上的事件)
*/
//epoll_ctl  add mod 的实现
void EPollPoller::updateChannel(Channel *channel) {
    const int index = channel->index();
    LOG_INFO("func = %s => fd = %d events = %d index = %d \n", __FUNCTION__, channel->fd(), channel->events(), index);
    if(index == kNew || index == kDeleted) {
        if(index == kNew) { //从未添加过
            int fd = channel->fd();
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel); //注册进epoll
    } else { //channel已经在poller上注册过
        int fd = channel->fd();
        if(channel->isNoneEvent()) { //对任何事件都不感兴趣
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

//epoll_ctl  del 的实现
//从poller中删除channel
void EPollPoller::removeChannel(Channel *channel) {
    int fd = channel->fd();
    channels_.erase(fd);
    LOG_INFO("func = %s => fd = %d \n", __FUNCTION__, fd);
    int index = channel->index();
    if(index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
    for(int i = 0; i < numEvents; i++) {
        Channel *channel = static_cast<Channel*> (events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel); //EventLoop就拿到了它的poller给它返回的所有发生事件的channel列表
    }
}

//epoll_ctl add/mod/del的具体操作
void EPollPoller::update(int operation, Channel *channel) {
    epoll_event event;
    int fd = channel->fd();
    memset(&event, 0, sizeof event);
    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    if(::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        if(operation == EPOLL_CTL_DEL) {
            LOG_ERROR("epoll_ctl del error: %d\n", errno);
        } else {
            LOG_FATAL("epoll_ctl mod/add error: %d\n", errno);
        }
    }
}