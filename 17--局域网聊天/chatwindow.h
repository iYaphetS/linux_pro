#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextEdit>
#include <QSplitter>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QString>
#include <QDebug>
#include <QVariant>
#include <QFileDialog>//qfile
#include "chat.h"

class chatWindow : public QWidget
{
    Q_OBJECT
public:
    explicit chatWindow(QWidget *parent = 0);
    //左边的用户列表
    QListWidget *userlist;

    //右边的聊天显示，输入窗口，发送,设置按钮
    QTextBrowser *output;
    QTextEdit *input;
    QPushButton *send;
    QPushButton *set;
    QPushButton *sendfile;

    chat chat_;

    //初始化用户列表
    void initUserlist();
    //得到选择的用户的ip
    QString getselectIP();
signals:

public slots:
    //显示上线用户
    void onNewUser(QString name, QString ip);
    //输出显示聊天消息
    void onNewMsg(QString name, QString content, bool isbroadcast);
    void onTransFileRequest(QString filename, int filesize, int peerid, QString ip);
    //文件传输进度
    void onTransProgress(sendfileInfo *info);
    //文件传输完成
    void onTransFinish(sendfileInfo *info);



    void onSend();
    void onSet();
    void onSendfile();


};

#endif // CHATWINDOW_H
