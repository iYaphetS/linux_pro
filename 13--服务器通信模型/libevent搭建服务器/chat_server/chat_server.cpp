#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
using namespace std;

class channel
{
public:
    channel()
    {
        readsize = 0;
    }
    bufferevent *bev;
    char buf[4096];
    int readsize;
    int packetsize;
    string ip;
};

map<string, channel*> channels;
/*
typedef void (*bufferevent_data_cb)(struct bufferevent *bev, void *ctx);//readcb
typedef void (*bufferevent_data_cb)(struct bufferevent *bev, void *ctx);//writecb
typedef void (*bufferevent_event_cb)(struct bufferevent *bev, short what, void *ctx);//eventcb
*/

void readHeader(struct bufferevent *bev, channel * chan)
{
    chan->readsize += bufferevent_read(bev,
                      chan->buf + chan->readsize,
                      4 - chan->readsize);

    if(chan->readsize == 4)
    {
        chan->packetsize = ntohl(*(uint32_t *)chan->buf);
    }
}

void readdata(struct bufferevent *bev, channel * chan)
{
    chan->readsize += bufferevent_read(bev,
                         chan->buf + chan->readsize,
                         chan->packetsize + 4 - chan->readsize);

}

void handledata(channel *chan)
{
    chan->buf[chan->readsize] = 0;
    char *buf = chan->buf + 4;

    char *toip = strtok(buf, "|");
    char *content = strtok(NULL, "\0");

    channel *peer = channels[toip];
    if(peer == NULL)
    {
        printf("peer offline");
    }
    else
    {
        char buf[4096];
        sprintf(buf, "%s|%s", chan->ip.c_str(), content);

        uint32_t len = strlen(buf);
        len = htonl(len);

        bufferevent_write(peer->bev, (char *)&len, 4);
        bufferevent_write(peer->bev, buf, strlen(buf));

    }

    chan->readsize = 0;

}

void readcb(struct bufferevent *bev, void *ctx)
{

    channel *chan = (channel *)ctx;
    if(chan->readsize < 4)
    {
        readHeader(bev, chan);
        if(chan->readsize == 4)
        {
            readdata(bev, chan);
        }
    }
    else
    {
        readdata(bev, chan);
    }

    if(chan->readsize == chan->packetsize + 4)
    {
        handledata(chan);
    }
#if 0
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    bufferevent_read(bev, buf, sizeof(buf));

    printf("data = %s\n", buf);
#endif
}

void writecb(struct bufferevent *bev, void *ctx)
{

}

void eventcb(struct bufferevent *bev, short what, void *ctx)
{
    if(what & BEV_EVENT_CONNECTED)
    {
        printf("connected\n");
    }
    else
    {
        if(what & BEV_EVENT_EOF)
        {
            printf("peer close socket\n");
        }

        bufferevent_free(bev);
    }
}


void newUserOnline(channel *chan)
{
    for(auto it = channels.begin(); it != channels.end(); it++)
    {
        channel *user = it->second;

        uint32_t len = strlen(user->ip.c_str());
        len = htonl(len);
        bufferevent_write(chan->bev, (char *)&len, 4);
        bufferevent_write(chan->bev, user->ip.c_str(), strlen(user->ip.c_str()));

        len = strlen(chan->ip.c_str());
        len = htonl(len);
        bufferevent_write(user->bev, (char *)&len, 4);
        bufferevent_write(user->bev, chan->ip.c_str(), strlen(chan->ip.c_str()));

    }
}

/*
typedef void (*evconnlistener_cb)(struct evconnlistener *,
                                 evutil_socket_t,
                                 struct sockaddr *,
                                 int socklen, void *);
*/
void listen_cb(struct evconnlistener *listener, evutil_socket_t newfd, struct sockaddr *addr, int socklen, void *ptr)
{
    struct sockaddr_in *p = (struct sockaddr_in *)addr;
    string ip = inet_ntoa(p->sin_addr);
    printf("connect ip:%s\n", ip.c_str());

    struct event_base *base = (struct event_base*)ptr;

    struct bufferevent *bev = bufferevent_socket_new(base, newfd, BEV_OPT_CLOSE_ON_FREE);

    /*
    void bufferevent_setcb(struct bufferevent *bufev,
                            bufferevent_data_cb readcb,
                            bufferevent_data_cb writecb,
                            bufferevent_event_cb eventcb,
                            void *cbarg);//每个回调函数最后一个参数
    */
    channel *chan = new channel;
    bufferevent_setcb(bev, readcb, NULL, eventcb, chan);
    bufferevent_enable(bev, EV_READ|EV_WRITE);

    chan->ip = ip;
    chan->bev = bev;

    //uint32_t len = 5;
    //len = htonl(len);
    //bufferevent_write(bev, (char *)&len, 4);
    //bufferevent_write(bev, "hello", 5);

    newUserOnline(chan);
    channels[ip] = chan;
}

int main()
{
    //==  epoll_create
    struct event_base *base = event_base_new();

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9988);
    addr.sin_addr.s_addr = INADDR_ANY;

    /*
    struct evconnlistener *evconnlistener_new_bind(
                            struct event_base *base,
                            evconnlistener_cb cb,
                            void *ptr,
                            unsigned flags,
                            int backlog,
                            const struct sockaddr *sa,
                            int socklen);

    */
    //== socket bind listen
    struct evconnlistener *listener=
            evconnlistener_new_bind(base,
                                    listen_cb,
                                    base,
                                    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
                                    250,
                                    (struct sockaddr*)&addr,
                                    sizeof(addr));
    evconnlistener_enable(listener);

    event_base_dispatch(base);

    event_base_free(base);

    return 0;
}
