#include "Poller.h"
#include "EPollPoller.h"

#include <stdlib.h>

Poller* Poller::newDefaultPoller(EventLoop *loop) {
    //查看环境变量是否要求使用poll
    if(::getenv("MUDUO_USE_POLL")) {
        return nullptr; //生成poll的实例
    } else {
        return new EPollPoller(loop); //生成epoll的实例
    }
}
