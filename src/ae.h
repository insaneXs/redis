/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AE_H__
#define __AE_H__

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4

#define AE_NOMORE -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

struct aeEventLoop;

/* Types and data structures */
//四种类型的事件处理器
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

/* File event structure */
//文件事件结构体
typedef struct aeFileEvent {
    //掩码 表示事件类型
    int mask; /* one of AE_(READABLE|WRITABLE) */
    //读事件处理函数
    aeFileProc *rfileProc;
    //写事件处理函数
    aeFileProc *wfileProc;
    //指向相关数据
    void *clientData;
} aeFileEvent;

/* Time event structure */
//时间时间结构体
typedef struct aeTimeEvent {
    //事件ID
    long long id; /* time event identifier. */
    //到达事件
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */
    //时间事件处理函数
    aeTimeProc *timeProc;
    //时间事件处理函数？
    aeEventFinalizerProc *finalizerProc;
    void *clientData;
    //指向下一个时间事件
    struct aeTimeEvent *next;
} aeTimeEvent;

/* A fired event */
//表示事件触发的结构体
typedef struct aeFiredEvent {
    int fd;
    int mask;
} aeFiredEvent;

/* State of an event based program */
//主事件循环结构体
typedef struct aeEventLoop {
    //目前注册在主事件循环中的最大文件描述符
    int maxfd;   /* highest file descriptor currently registered */
    //当前文件描述符的大小
    int setsize; /* max number of file descriptors tracked */
    //记录下一个时间事件的ID
    long long timeEventNextId;
    time_t lastTime;     /* Used to detect system clock skew */
    //
    aeFileEvent *events; /* Registered events */
    aeFiredEvent *fired; /* Fired events */
    //时间事件链表的表头
    aeTimeEvent *timeEventHead;
    int stop;
    void *apidata; /* This is used for polling API specific data */
    aeBeforeSleepProc *beforesleep;
} aeEventLoop;

/* Prototypes */
//创建主事件循环
aeEventLoop *aeCreateEventLoop(int setsize);
//删除主事件循环
void aeDeleteEventLoop(aeEventLoop *eventLoop);
void aeStop(aeEventLoop *eventLoop);

//文件事件相关操作
//创建文件事件
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);
//删除文件事件
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
//获取一个文件事件
int aeGetFileEvents(aeEventLoop *eventLoop, int fd);

//时间事件相关操作
//创建时间事件
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
//删除时间事件
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
//事件处理函数
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
//等待
int aeWait(int fd, int mask, long long milliseconds);

//主事件循环入口
void aeMain(aeEventLoop *eventLoop);

char *aeGetApiName(void);

void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
int aeGetSetSize(aeEventLoop *eventLoop);
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
