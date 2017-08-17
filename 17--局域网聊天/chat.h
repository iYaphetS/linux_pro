#ifndef CHAT_H
#define CHAT_H

#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QProcessEnvironment>

#include <QObject>
#include <QMap>

#ifdef WIN32
#include <winsock2.h>
#define socklen_t int
#else
#include <sys/socket.h>
#include <netinet/in.h>//sockaddr_in
#include <arpa/inet.h>//inet_addr
#include <pthread.h>
#endif

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

#include <QDebug>
#include <QString>
#include <errno.h>
#include <unistd.h>
#include <QFileInfo>
#include "chatdef.h"

class chat;

typedef struct
{
    QString name;
    QString ip;
}user;

static int sendfileInfoID = 0;
//文件信息
typedef struct sendfileInfo
{
    QString filepath_src;
    QString filepath_dst;
    int id;
    int peerid;
    QString ip;
    int total;
    int transSize;
    bool started;

    chat * _chat;
    int server_fd;
    int port;

    sendfileInfo(chat *ch)
    {
        server_fd = -1;
        port = -1;
        _chat = ch;
        peerid = -1;
        started = false;
        transSize = 0;
        //这个变量是全局变量，导致这个构造函数不是线程安全的，所以只能主线程来调用
        id = sendfileInfoID++;

    }
    ~sendfileInfo()
    {
        if(server_fd != -1)
        {
            close(server_fd);
        }
    }

}sendfileInfo;

class chat : public QObject
{
    Q_OBJECT
public:
    explicit chat(QObject *parent = 0);

    //本机名
    QString name;
    //本机的所以ip
    QStringList ips;

    //其他用户信息
    QMap<QString, user*> others;

    //发送的文件信息
    //QMap<int, sendfileInfo*> sends;
    //接受的文件信息
    //QMap<int, sendfileInfo*> recvs;
    QMap<int, sendfileInfo*> transInfo;


    //通信管道
    int udp_fd;
    //线程id
    pthread_t tid;

    //广播的所有地址
    QString broadcast_ip;

    //获得机器所以ip，以及名字
    QStringList getSysIps();
    QString getSysName();

    //用户登录初始化，创建子线程
    void init();
    //创建socket
    void create_socket(QString ip);
    //线程处理函数参数只能是一个，如果是成员函数，会带有this指针，所以要声明static
    static void *thread_recv(void *ptr);


    static void *recv_file_thread(void *ptr);
    void recv_one_file(sendfileInfo *info);
    static void *send_file_thread(void *ptr);
    void send_one_file(sendfileInfo *info);

    //处理发送消息
    void send(const QJsonObject &obj, QString ip);
    void sendMsg(QString content, QString ip);
    void sendOnline();

    //文件信息处理
    void sendfile(QString path, QString ip);
    void recvfile(QString filename, int filesize, int peerid, QString ip);
    void ackFileTransRequest(QString filename, int filesize, int perrid, QString ip, bool isok, QString dstfilename = "");


    //处理接受消息
    void run();
    void handleMsg(const QJsonObject obj, QString ip);
    void addUser(QString name, QString ip);
signals:
    void sigNewUser(QString name, QString ip);
    void sigNewContent(QString name, QString content, bool isbroadcast);
    void sigTransFileRequest(QString filename, int filesize, int peerid, QString ip);
    void sigAckTransFile(int id, int peerid, int port);

    void sigProgress(sendfileInfo *info);
    void sigTransFinish(sendfileInfo *info);
public slots:

    void onAckTransFile(int id, int peerid, int port);
};

#endif // CHAT_H
