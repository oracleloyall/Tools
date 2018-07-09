#include "common.h"
#include "flog.h"
#include "ngx_event_timer.h"
#include <stdlib.h>
#include<time.h>
#include<unistd.h>
/*
 * 执行命令修改如下2个内核参数
sysctl -w net.ipv4.tcp_timestamps=1  开启对于TCP时间戳的支持,若该项设置为0，则下面一项设置不起作用
sysctl -w net.ipv4.tcp_tw_recycle=1  表示开启TCP连接中TIME-WAIT sockets的快速回收


可能解决方法1--调低time_wait状态端口等待时间：
1. 调低端口释放后的等待时间，默认为60s，修改为15~30s
sysctl -w net.ipv4.tcp_fin_timeout=30
2. 修改tcp/ip协议配置， 通过配置/proc/sys/net/ipv4/tcp_tw_resue, 默认为0，修改为1，释放TIME_WAIT端口给新连接使用
sysctl -w net.ipv4.tcp_timestamps=1
3. 修改tcp/ip协议配置，快速回收socket资源，默认为0，修改为1
sysctl -w net.ipv4.tcp_tw_recycle=1

可能解决办法2--增加可用端口：
CCH:~ # sysctl -a |grep port_range
net.ipv4.ip_local_port_range = 50000    65000      -----意味着50000~65000端口可用

修改参数：
$ vi /etc/sysctl.conf
net.ipv4.ip_local_port_range = 10000     65000      -----意味着10000~65000端口可用

改完后，执行命令“sysctl -p”使参数生效，不需要reboot。
 */
static void signal_handler(int x);

int main(int argc, char* argv[])
{
    //atexit(stop_thread);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    //signal(SIGABRT,signal_handler);
    
    struct epoll_event event_list[EPOLLWAITNUM];
    connection_t* c;
    uint32_t      revents;
    int i;
    ngx_msec_t  wait_time,delta;
    
    //Flogconf logconf = DEFLOGCONF;
    Flogconf logconf = {"/tmp/logtest",LOGFILE_DEFMAXSIZE,L_LEVEL_MAX,0,1};
    if (0 > LOG_INIT(logconf)) {
        printf("LOG_INIT() failed\n");
        return -1;
    }

    srand((unsigned)time(NULL));
    
    //config
    if (0 != make_test_config()) {
        LOG_ERROR("make_test_config() failed");
        goto done;
    }
    
    if (0 != ngx_event_timer_init()) {
        LOG_ERROR("ngx_event_timer_init() failed");
        goto done;
    }
    
    if (0 != create_connection_pool()) {
        LOG_ERROR("create_connection_pool() failed");
        goto done;
    }
    
    epfd = epoll_create(EPOLLMAXEVENTS);    //�������ڴ���accept��epollר�õ��ļ�������, ָ�����������������ΧΪ256 
    if (epfd == -1) {
        LOG_ERROR("epoll_create()");
        goto done;
    }
    one:
    if (0 != start_all_connection()) {
        LOG_ERROR("start_all_connection() failed");
        goto one;
    }
    
    while (1) {
        //if (stop) {printf("stop"); break;}
        
        wait_time = ngx_event_find_timer();
        
        LOG_DEBUG("wait_time %d",wait_time);
        
        delta = ngx_current_msec;
        
        int nfds = epoll_wait(epfd, event_list, EPOLLWAITNUM, wait_time);
        if (-1 == nfds) {
            LOG_ERROR("epoll_wait");
            goto done;
        }
        
        ngx_time_update();
        
        delta = ngx_current_msec - delta;
        LOG_DEBUG("delta %d",delta);
        
        if (nfds == 0) {
            if (wait_time == NGX_TIMER_INFINITE) {
                //return NGX_OK;
                
                LOG_ERROR("epoll_wait() returned no events without timeout");
                goto done;
            }
            
            LOG_DEBUG("nfds 0");
            if (wait_time == 0 || delta) {
                ngx_event_expire_timers();
            }
            continue;
        }
    
        for (i = 0; i < nfds; ++i) {
            
            revents = event_list[i].events;
            c = static_cast<connection_t*>(event_list[i].data.ptr);
            
            if (revents & EPOLLRDHUP) {
                LOG_DEBUG("[%ul][%d]EPOLLRDHUP handle_close_event",c->pos,c->fd);
                handle_close_event(c);
                continue;
            }
            
            if (revents & EPOLLIN) {
                LOG_DEBUG("[%u][%d]EPOLLIN handle_read_event",c->pos,c->fd);
                handle_read_event(c);
                continue;
            }
            
            if (revents & EPOLLOUT) {
                LOG_DEBUG("[%u][%d]EPOLLOUT handle_send_event",c->pos,c->fd);
                handle_send_event(c);
                continue;
            } 
            
            //error
            LOG_WARN("[%u][%d]error handle_close_event",c->pos,c->fd);
            handle_close_event(c);
            //continue;
        }                        //for
        
        if (delta) {
            ngx_event_expire_timers();
        }
    }                            //while

done:
    LOG_DEBUG("program done");
    clean_all_resource();

    LOG_EXIT;

    return 0;
}

static void 
signal_handler(int x)
{
    clean_all_resource();
    LOG_EXIT;
    
    exit(1);
}
