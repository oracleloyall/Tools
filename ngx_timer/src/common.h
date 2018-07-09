
#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/socket.h>   //connect,send,recv,setsockopt ...
#include <netinet/in.h>     // sockaddr_in, "man 7 ip"  
#include <arpa/inet.h>   //inet_addr,inet_aton    
#include <unistd.h>        //read,write,close
#include <netdb.h>         //gethostbyname
#include <fcntl.h>    //fcntl  

#include <string.h>     //strerror()
#include <stdio.h>    //perror
#include <netinet/tcp.h> //TCP_MAXSEG
//#include <error.h>         //perror
#include <errno.h>         //errno
#include <stdlib.h>     //exit

#include <sys/epoll.h>  //linux epoll

#include <assert.h>
#include <signal.h>

#include "ngx_rbtree.h"
#include "zmalloc.h"
#ifdef __cplusplus
extern "C" {
#endif
#define ngx_nonblocking(s)  fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK)
#define EPOLLWAITNUM     512 
#define EPOLLMAXEVENTS   1
//0240

#define READ_BUF_MAX_LEN 1024

#define ST_CLOSED       0
#define ST_CONNECTING   1
#define ST_IDLE         2
#define ST_SENDING      3
#define ST_SENT         4
#define ST_RECVING      5
#define ST_RECVED       6

#define ngx_abs(value)       (((value) >= 0) ? (value) : - (value))
#define ngx_max(val1, val2)  ((val1 < val2) ? (val2) : (val1))
#define ngx_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))

typedef struct config_s config_t;
typedef struct send_buffer_s send_buffer_t;
typedef struct read_buffer_s read_buffer_t;
typedef struct connection_s connection_t;
typedef struct connection_config_s connection_config_t;
typedef struct test_data_s test_data_t;

struct test_data_s {
    u_char* start;
    u_char* end;
};

struct connection_config_s {
    unsigned int requests;
    unsigned int sleep_min;
    unsigned int sleep_max;
    unsigned retry:1;
    unsigned random:1;
};

struct config_s {
    char server_name[32];
    char ip[16];
    unsigned short port;
    struct sockaddr_in server_addr;
    socklen_t       socklen;
    unsigned int connection_pool_n;
    unsigned int read_max_buf_len; 
    const char* test_data_file;
    test_data_t* test_data;
    unsigned int test_data_n;
    connection_config_t def_con_config;
    connection_config_t *con_config;
};

struct send_buffer_s {
    u_char* start;
    u_char* pos;
    u_char* last;
};

struct read_buffer_s {
    int read_len;
    char head[4];
    u_char* start;
    u_char* end;
};

struct connection_s {
    int        fd;
    
    connection_config_t *conf;

    send_buffer_t   send_buf;
    read_buffer_t   read_buf;
    
    int requests_con;
    unsigned int test_data_index;
    unsigned int pos;
    
    ngx_rbtree_node_t   timer;
    
    //ngx_uint_t sleep;
    
    unsigned active:1;
    unsigned ready_read:1;
    unsigned ready_write:1;
    //unsigned retry:1;
    unsigned timer_set:1;
    unsigned state:3;
};


int create_connection_pool();
int start_all_connection();
void clean_all_resource();

void handle_read_event(connection_t* c);
void handle_send_event(connection_t* c);
void handle_close_event(connection_t* c);
void handle_timer_event(connection_t* c);

int make_test_config();
void clear_test_config();

extern int epfd;
extern connection_t* p_connection_pool;
extern config_t* p_config;
extern int stop;
#ifdef __cplusplus
 }
#endif
#endif  //_COMMON_H_
