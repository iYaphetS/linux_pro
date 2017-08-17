#include "chatwindow.h"

chatWindow::chatWindow(QWidget *parent) : QWidget(parent)
{
    //整体布局
    QHBoxLayout *m = new QHBoxLayout(this);

    //拉伸的左右布局
    QSplitter *split = new QSplitter(this);
    //左布局
    split->addWidget(userlist = new QListWidget);
    //右布局
    QWidget *rightWiget = new QWidget(this);
    split->addWidget(rightWiget);

    m->addWidget(split);

    //右布局中的布局
    QVBoxLayout *vbox = new QVBoxLayout(rightWiget);
    //QHBoxLayout* hbox;
    vbox->addWidget(output = new QTextBrowser, 3);
    vbox->addWidget(input = new QTextEdit, 1);
    //vbox->addLayout(hbox = new QHBoxLayout);

    //hbox->addStretch(1);
    //hbox->addWidget(set = new QPushButton("设置"));
    //hbox->addWidget(send = new QPushButton("发送"));
#if 1
    QGridLayout *gri = new QGridLayout(input);
    gri->setRowStretch(4, 1);
    //gri->setColumnStretch(4, 1);
    gri->addWidget(sendfile = new QPushButton("发送文件", input), 5, 3);
    gri->addWidget(set = new QPushButton("设置", input), 5, 4);
    gri->addWidget(send = new QPushButton("发送", input), 5, 5);
    vbox->setMargin(0);
    //m->setMargin(0);
    gri->setMargin(0);
#endif
    //vbox->setMargin(0);
    //hbox->setMargin(0);


    //子线程发送新用户登录的信号，主线程调用槽函数更新列表
    connect(&chat_, SIGNAL(sigNewUser(QString, QString)), this, SLOT(onNewUser(QString,QString)));
    //核心类发送用户发送消息的信号，界面类在output显示消息内容
    connect(&chat_, SIGNAL(sigNewContent(QString, QString, bool)), this, SLOT(onNewMsg(QString,QString,bool)));

    connect(&chat_, SIGNAL(sigTransFileRequest(QString,int,int,QString)), this, SLOT(onTransFileRequest(QString,int,int,QString)));
    connect(&chat_, SIGNAL(sigProgress(sendfileInfo*)), this, SLOT(onTransProgress(sendfileInfo*)));
    connect(&chat_, SIGNAL(sigTransFinish(sendfileInfo*)), this, SLOT(onTransFinish(sendfileInfo*)));

    //创建管道，设置广播，将上线信息通知其他用户
    chat_.init();

    //点击发送按钮，调用发送聊天内容信息
    connect(send, SIGNAL(clicked()), this, SLOT(onSend()));
    //设置ip地址
    connect(set, SIGNAL(clicked()), this, SLOT(onSet()));
    connect(sendfile, SIGNAL(clicked()), this, SLOT(onSendfile()));

    initUserlist();
}

void chatWindow::initUserlist()
{
    this->userlist->clear();

    this->userlist->addItem("ALL@" + chat_.broadcast_ip);
}

QString chatWindow::getselectIP()
{
    //得到选中的用户信息  xxxx@192.168.19.30
    QString text = userlist->currentItem()->text();

    QStringList stringlist = text.split("@");

    if(stringlist.length() != 2)
    {
        qDebug() << stringlist;
        return NULL;
    }
    //得到ip
    QString ip = stringlist.at(1);
    return ip;
}

void chatWindow::onNewUser(QString name, QString ip)
{
    int count  = this->userlist->count();
    for(int i = 0; i < count; i++)
    {
        QListWidgetItem *item = userlist->item(i);
        if(item->text().indexOf(ip) != -1)//新上线的用户在用户列表
        {
            delete userlist->takeItem(i);
            break;
        }

    }

    this->userlist->addItem(name + "@" + ip);
}

void chatWindow::onNewMsg(QString name, QString content, bool isbroadcast)
{
    if(isbroadcast)
    {
        this->output->setAlignment(Qt::AlignLeft);//文本靠左
        output->append(name + "对大家说:" + content + "\n");
    }
    else
    {
        this->output->setAlignment(Qt::AlignLeft);//文本靠左
        output->append(name + "说:" + content + "\n");
    }
}

void chatWindow::onTransFileRequest(QString filename, int filesize, int peerid, QString ip)
{
    //弹出对话框，提示是否接受文件
    QString msg = QString("%1 给你发文件(%2),要不要接受?").arg(chat_.others[ip]->name, filename);
    if(QMessageBox::question(this, "有人发文件", msg) == QMessageBox::Yes)
    {
        QString dstfilename = QFileDialog::getSaveFileName(this);
        if(dstfilename.length() == 0)
        {
            chat_.ackFileTransRequest(filename, filesize, peerid, ip, false);
        }
        //创建结构体和线程
        chat_.ackFileTransRequest(filename, filesize, peerid, ip, true, dstfilename);
    }
    else
    {
        chat_.ackFileTransRequest(filename, filesize, peerid, ip, false);
    }
}

void chatWindow::onTransProgress(sendfileInfo *info)
{
    qDebug() << "trans progress:" << info->transSize << info->total;
}

void chatWindow::onTransFinish(sendfileInfo *info)
{

    if(info->server_fd == -1)
        qDebug() << "send trans complete" << info->filepath_src;
    else
        qDebug() << "recv trans complete" << info->filepath_dst;
    chat_.transInfo.remove(info->id);
    //chat_.sends.remove()
    delete info;
}

void chatWindow::onSend()
{
    //取得选择的对方ip
    QString ip = getselectIP();
    //输入的内容
    QString content = input->toPlainText();
    if(content.length() == 0)
    {
        return;
    }
    chat_.sendMsg(content, ip);

    input->clear();
    output->setAlignment(Qt::AlignRight);//文本靠右
    output->append("我说:" + content + "\n");
}

void chatWindow::onSet()
{
    QDialog dlg;
    QComboBox *combo;
    QHBoxLayout *hbox = new QHBoxLayout(&dlg);
    hbox->addWidget(new QLabel("网口"));
    hbox->addWidget(combo = new QComboBox());


    //QStringList ret;
    QList<QNetworkAddressEntry> entrys;
    QList<QNetworkInterface> infs = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface inf, infs)
    {
        entrys.append(inf.addressEntries());
    }

    foreach (QNetworkAddressEntry entry, entrys)
    {
        if(entry.broadcast().toString().isEmpty())
            continue;
        //ret.append(entry.broadcast().toString());
        combo->addItem(entry.ip().toString());
        combo->setItemData(combo->count()-1, entry.broadcast().toString());
    }


    //combo->addItems(ret);
    dlg.exec();

    chat_.broadcast_ip= combo->itemData(combo->currentIndex()).toString();//combo->currentText();

    //chat_.create_socket(combo->currentText());
    chat_.sendOnline();

    initUserlist();
}

void chatWindow::onSendfile()
{
    QString ip = getselectIP();

    QString  filename = QFileDialog::getOpenFileName(NULL, "请选择要发送的文件");
    if(filename.length() == 0)
    {
        return;
    }
    qDebug() << filename;

    chat_.sendfile(filename, ip);
}











