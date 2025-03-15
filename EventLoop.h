#pragma once

/* 
事件循环类，主要包含了Channel Poller（epoll的抽象）
一个线程有一个EventLoop，一个EventLoop有一个Poller，一个Poller监听许多Channel
*/

class EventLoop{

};