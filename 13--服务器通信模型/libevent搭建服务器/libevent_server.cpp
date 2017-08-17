#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <cstring>
/*
typedef void (*bufferevent_data_cb)(struct bufferevent *bev, void *ctx);//readcb
typedef void (*bufferevent_data_cb)(struct bufferevent *bev, void *ctx);//writecb
typedef void (*bufferevent_event_cb)(struct bufferevent *bev, short what, void *ctx);//eventcb
*/

void readcb(struct bufferevent *bev, void *ctx)
{
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    bufferevent_read(bev, buf, sizeof(buf));

    printf("data = %s\n", buf);
}

void write(struct bufferevent *bev, void *ctx)
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

/*
typedef void (*evconnlistener_cb)(struct evconnlistener *,
                                 evutil_socket_t,
                                 struct sockaddr *,
                                 int socklen, void *);
*/
void listen_cb(struct evconnlistener *listener, evutil_socket_t newfd, struct sockaddr *addr, int socklen, void *ptr)
{
    struct event_base *base = (struct event_base*)ptr;

    struct bufferevent *bev = bufferevent_socket_new(base, newfd, BEV_OPT_CLOSE_ON_FREE);

    /*
    void bufferevent_setcb(struct bufferevent *bufev,
                            bufferevent_data_cb readcb,
                            bufferevent_data_cb writecb,
                            bufferevent_event_cb eventcb,
                            void *cbarg);
    */
    bufferevent_setcb(bev, readcb, NULL, eventcb, base);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
}

int main()
{
    //==  epoll_create
    struct event_base *base = event_base_new();

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9989);
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
