#include "Buffer.h"

#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

//从fd上读取数据，Poller工作在LT模式
/* 
Buffer缓冲区有大小，但是从fd上读取数据的时候，却不知tcp数据的最终大小
*/
ssize_t Buffer::readFd(int fd, int* saveErrno) {
    char extrabuf[65536] = {0}; //栈上内存空间 64K
    struct iovec vec[2];
    const size_t writable = writableBytes(); //这是buffer底层缓冲区剩余的可写空间大小
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;
    
    //以上空间写完后，剩余数据写入vec[1]
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt); //将fd中的数据读取到iovcnt描述的vec中
    if(n < 0) {
        *saveErrno = errno;
    } else if(n <= writable) {
        writerIndex_ += n;
    } else { //extrabuf中写入了数据
        writerIndex_ = buffer_.size(); //buffer已经写满了
        append(extrabuf, n - writable);
    }

    return n;
}

ssize_t Buffer::writeFd(int fd, int* saveErrno) {
    ssize_t n = ::write(fd, peek(), readableBytes());
    if(n < 0) {
        *saveErrno = errno;
    }
    return n;
}

