#include "mywindow.h"
#include <QSplitter>
#include <QHBoxLayout>
#include <QtEndian>
#include <QDebug>

myWindow::myWindow(QWidget *parent) : QWidget(parent)
{
    left = new QListWidget;
    right = new QWidget;

    QHBoxLayout *lay = new QHBoxLayout(this);

    QSplitter *splitter = new QSplitter;
    splitter->addWidget(left);
    splitter->addWidget(right);

    lay->addWidget(splitter);

    QVBoxLayout *vbox = new QVBoxLayout(right);
    vbox->setMargin(0);
    vbox->addWidget(output = new QTextBrowser, 2);
    vbox->addWidget(input = new QTextEdit, 1);
    vbox->addWidget(send = new QPushButton("send"));
    //send = new QPushButton("send", input);

    socket = new QTcpSocket;
    socket->bind(QHostAddress("192.168.19.28"));//19.28
    socket->connectToHost(QHostAddress("192.168.19.28"), 9988);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onreadyRead()));
    connect(send, SIGNAL(clicked()), this, SLOT(onsend()));


}


void myWindow::readdata()
{
    buf += socket->read(packetsize + 4 - buf.size());
}

void myWindow::onreadyRead()
{
    while(socket->bytesAvailable() > 0)
    {
        if(buf.size() < 4)
        {
            buf += socket->read(4 - buf.size());
            if(buf.size() == 4)
            {
                uint32_t p = *(uint32_t *)buf.data();

                packetsize = qFromBigEndian(p);
                qDebug() << "packetsize:" << packetsize;

                readdata();
            }
        }
        else
        {
            readdata();
        }

        if(packetsize + 4 == buf.size())
        {
            //得到数据包的字符串
            buf = buf.mid(4);//从第四个字节开始的字符串
            qDebug() << buf;

            //192.168.175.139----上线
            //192.168.175.139|content----聊天
            int index = buf.indexOf("|");
            QString ip;

            //buf === ip-----上线消息
            if(index == -1)
            {
                ip = buf;
                this->left->addItem(ip);
            }
            //buf ==== ip|content-----对方发送的消息
            else
            {
                QString msg;
                ip = buf.left(index);//得到第index左边的字符串
                msg = buf.mid(index + 1);
                //ip = buf.right(index);
                this->output->append("[" + ip + "]" + "~RECV:" + msg);
            }
        }

        buf.clear();
    }
}

void myWindow::onsend()
{
    if(input->toPlainText().size() == 0)
        return;
    if(left->currentItem() == NULL)
        return;

    QListWidgetItem *item = left->currentItem();
    QString ip = item->text();

    QByteArray buf =ip.toUtf8() + "|" + input->toPlainText().toUtf8();
    uint32_t size = buf.size();
    size = qToBigEndian(size);

    socket->write((const char*)&size, 4);
    socket->write(buf);

    output->append("[" + ip + "]" + "~SEND:" + input->toPlainText());
    input->clear();
}
