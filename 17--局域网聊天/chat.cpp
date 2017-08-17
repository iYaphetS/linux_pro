#include "chat.h"



chat::chat(QObject *parent) : QObject(parent)
{
    this->ips = getSysIps();
    this->name = getSysName();

#ifdef WIN32
    //windows环境下要初始化sockeet运行环境
    WSADATA wsaDATA;
    WSAStartup(MAKEWORD(2, 1), &wsaDATA);
#endif

    this->udp_fd = -1;
    broadcast_ip = "255.255.255.255";

    connect(this, SIGNAL(sigAckTransFile(int,int, int)), this, SLOT(onAckTransFile(int,int, int)));

}

QStringList chat::getSysIps()
{
    QStringList ret;
    QList<QNetworkAddressEntry> entrys;
    QList<QNetworkInterface> infs = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface inf, infs)
    {
        entrys.append(inf.addressEntries());
    }

    foreach (QNetworkAddressEntry entry, entrys)
    {
        if(entry.ip().toString().isEmpty())
            ret.append(entry.ip().toString());
    }

    return ret;

}

QString chat::getSysName()
{
#ifdef WIN32
    return QProcessEnvironment::systemEnvironment().value("USERNAME");
#else
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    FILE *fp = popen("whoami", "r");
    int ret = fread(buf, 1, sizeof(buf), fp);
    buf[ret - 1] = 0;
    fclose(fp);
    return QString(buf);
#endif
}



//子线程处理函数，调用接受信息的处理函数run，让数据的发送和接受分开
void *chat::thread_recv(void *ptr)
{
    chat *This = (chat *)ptr;

    This->run();
    return NULL;
}

//处理接受文件
void *chat::recv_file_thread(void *ptr)
{
    sendfileInfo *info = (sendfileInfo *)ptr;
    info->_chat->recv_one_file(info);
    return NULL;
}

//写文件
void chat::recv_one_file(sendfileInfo *info)
{
    int newfd = accept(info->server_fd, NULL, NULL);
    if(newfd < 0)
    {
        return;
    }

    QFile file(info->filepath_dst);
    file.open(QFile::WriteOnly);

    while(info->transSize < info->total)
    {
        char buf[4096];
        int ret = read(newfd, buf, sizeof(buf));
        if(ret > 0)
        {
            file.write(buf, ret);
            info->transSize += ret;
            emit sigProgress(info);
        }
        else if(ret < 0 && errno == EINTR)
        {
            continue;
        }
        else
            break;
    }

    file.close();
    close(newfd);

    emit sigTransFinish(info);
}

//处理发送文件
void *chat::send_file_thread(void *ptr)
{
    sendfileInfo *info = (sendfileInfo *)ptr;
    info->_chat->send_one_file(info);
    return NULL;
}

//读文件
void chat::send_one_file(sendfileInfo *info)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(info->port);
    addr.sin_addr.s_addr = inet_addr(info->ip.toUtf8().data());

    ::connect(fd, (struct sockaddr *)&addr, sizeof(addr));

    QFile file(info->filepath_src);
    file.open(QFile::ReadOnly);

    while(1)
    {
        QByteArray buf = file.read(4096);
        if(buf.length() == 0)
        {
            break;
        }

        info->transSize += buf.size();
        emit sigProgress(info);

        write(fd, buf.data(), buf.size());

    }
    file.close();
    close(fd);
    emit sigTransFinish(info);

}

//用户登录，send调用sendto()将用户上线信息放在socket缓冲区
void chat::init()
{
    //创建聊天通信管道udp——socket
    create_socket("0.0.0.0");
    //用户上线
    sendOnline();

    //创建一个线程处理接受的数据
    pthread_create(&tid, NULL, thread_recv, this);
}

//创建聊天通信管道udp——socket
void chat::create_socket(QString ip)
{
    if(this->udp_fd != -1)
    {
        close(this->udp_fd);
    }
    //创建socket
    this->udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_fd < 0)
    {
        qDebug() << "create socket fail";
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9988);
    addr.sin_addr.s_addr = inet_addr(ip.toUtf8().data());//

    //绑定端口，udp不用监听
    int ret = bind(udp_fd, (sockaddr *)&addr, sizeof(addr));
    if(ret != 0)
    {
        qDebug() << "bind error";
        exit(1);
    }

    //设置广播功能
    int arg = 1;
#ifdef WIN32
    setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, (char *)&arg, sizeof(arg));
#else
    setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &arg, sizeof(arg));
#endif
}



//界面类点击发送按钮，发送信号调用槽函数，槽函数调用核心类这个发送消息的函数
void chat::sendMsg(QString content, QString ip)
{
    //{
    //    cmd: "chat",
    //    broadcast: true,
    //    content: "msg_body"
    //}

    QJsonObject obj;
    obj.insert(CMD, CHAT);
    //obj.insert(BROADCAST, isbroadcast);
    obj.insert(BROADCAST, ip.indexOf("255")!= -1);//成立就是广播
    obj.insert(CONTENT, content);
    obj.insert(NAME, name);
    send(obj, ip);

}

//上线通知
void chat::sendOnline()
{
    foreach (QString ip, others.keys())
    {
        delete others[ip];
    }

    //上线广播

    //{
    //     cmd: online,
    //     name: acount_name
    //}
    QJsonObject obj;
    obj.insert(CMD, ONLINE);
    obj.insert(NAME, this->name);

    send(obj, broadcast_ip);
}

//发送文件的名字，大小，编号，并在结构体中保存文件信息
void chat::sendfile(QString path, QString ip)
{

    QFile file(path);
    file.open(QFile::ReadOnly);
    int filesize = file.size();
    file.close();

    QFileInfo info(path);


    sendfileInfo *sfinfo = new sendfileInfo(this);

    QJsonObject obj;
    obj.insert(CMD, SENDFILE);
    obj.insert(FILENAME, info.fileName());
    obj.insert(FILESIZE, filesize);
    obj.insert(ID, sfinfo->id);
    send(obj, ip);

    //保存文件信息
    sfinfo->filepath_src = path;
    sfinfo->total = filesize;
    sfinfo->ip = ip;

    transInfo[sfinfo->id] = sfinfo;

}

//主线程中，回应文件传输请求，如果对话框选择yes，SENDFILEACK回应ok，否则回应cancel
void chat::ackFileTransRequest(QString filename, int filesize, int peerid, QString ip, bool isok, QString dstfilename)
{
    if(isok)
    {
        //创建结构体
        sendfileInfo *info = new sendfileInfo(this);
        info->filepath_src= filename;
        info->filepath_dst = dstfilename;
        info->peerid = peerid;
        info->total = filesize;
        info->ip = ip;
        transInfo[info->id] = info;

        //创建socket
        info->server_fd = socket(AF_INET, SOCK_STREAM, 0);
        listen(info->server_fd, 10);
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        getsockname(info->server_fd, (struct sockaddr *)&addr, &len);
        qDebug() << ntohs(addr.sin_port);



        //创建接受文件的线程
        pthread_t t;
        pthread_create(&t, NULL, recv_file_thread, info);
        pthread_detach(t);

        //发送报文
        QJsonObject obj;
        obj.insert(CMD, SENDFILEACK);
        obj.insert(RESULT, OK);
        obj.insert(PEERID, info->id);
        obj.insert(PORT, (int)ntohs(addr.sin_port));

        send(obj, ip);
    }
    else
    {
        QJsonObject obj;
        obj.insert(CMD, SENDFILEACK);
        obj.insert(RESULT, CANCEL);
        send(obj, ip);
    }

}

//接受消息
void chat::run()
{
    while(1)
    {
        //做输出参数
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        char buf[4096];

        int ret = recvfrom(this->udp_fd, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &len);
        if(ret < 0 && errno == EINTR)
        {
            qDebug() << "recv error";
            //exit(2);
            break;
        }
        else
        {
            buf[ret] = 0;
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray(buf));
            handleMsg(doc.object(), inet_ntoa(addr.sin_addr));
        }
    }
}


void chat::send(const QJsonObject &obj, QString ip)
{
    QByteArray buf = QJsonDocument(obj).toJson();

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9988);
    addr.sin_addr.s_addr = inet_addr(ip.toUtf8().data());
    //...
    sendto(this->udp_fd, buf.data(), buf.size(), 0, (struct sockaddr *)&addr, sizeof(addr));
}




//处理接受的数据
void chat::handleMsg(const QJsonObject obj, QString ip)
{
#if 1
    //如果发送给自己
    if(ips.indexOf(ip) != -1)
    {
        qDebug() << "屏蔽发送消息给自己";
        return;
    }
#endif

    QString cmd = obj.value(CMD).toString();

    //如果接受过来的是上线消息
    if(cmd == ONLINE)
    {
        QString name = obj.value(NAME).toString();
        addUser(name, ip);

        QJsonObject resp;
        resp.insert(CMD, ONLINEACK);
        resp.insert(NAME, this->name);
        send(resp, ip);
    }

    //如果接受过来的是对方上线的回应消息
    if(cmd == ONLINEACK)
    {
        QString name = obj.value(NAME).toString();
        addUser(name, ip);
    }
    //如果接受的是聊天消息
    if(cmd == CHAT)
    {
        //判断下，发送过来的ip是否为广播地址
        bool isbroadcast = obj.value(BROADCAST).toBool();
        QString name = obj.value(NAME).toString();
        QString content = obj.value(CONTENT).toString();

        emit sigNewContent(name, content, isbroadcast);
    }
    //如果接受的是对方的发送文件请求--------接受端
    if(CMD == SENDFILE)
    {
        QString filename = obj.value(FILENAME).toString();
        int filesize = obj.value(FILESIZE).toInt();
        int peerid = obj.value(ID).toInt();

        //子线程中不能创建sendfileInfo对象，因为它的构造函数不安全
        emit this->sigTransFileRequest(filename, filesize, peerid, ip);
    }
    //如果接受的是对方的文件请求回应--------发送端
    if(cmd == SENDFILEACK)
    {
        QString isok = obj.value(RESULT).toString();
        if(isok == OK)
        {
            int id = obj.value(ID).toInt();
            int peerid = obj.value(PEERID).toInt();
            int port = obj.value(PORT).toInt();

            //子线程中不能创建sendfileInfo对象，因为它的构造函数不安全
            emit sigAckTransFile(id, peerid, port);
        }
        else
        {

        }
    }
}

//发送信号，通知主界面更新用户列表
void chat::addUser(QString name, QString ip)
{
    user * usr = new user;

    usr->name = name;
    usr->ip = ip;

    if(others[ip])
        delete others[ip];

    others[ip] = usr;
    //在子线程发送信号，槽函数在主线程处理界面更新
    emit sigNewUser(name, ip);

}

//创建sendfileinfo对象，创建发送文件的线程
void chat::onAckTransFile(int id, int peerid, int port)
{
     sendfileInfo *info = transInfo[id];
     info->peerid = peerid;
     info->port = port;

     pthread_t t;
     pthread_create(&t, NULL, send_file_thread, info);
     pthread_detach(t);
}


