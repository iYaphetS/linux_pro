#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QTextBrowser>
#include <QPushButton>
#include <QTcpSocket>
#include <QHostAddress>

class myWindow : public QWidget
{
    Q_OBJECT
public:
    explicit myWindow(QWidget *parent = 0);
    QListWidget *left;
    QWidget *right;

    QTextEdit *input;
    QTextBrowser *output;
    QPushButton *send;

    QTcpSocket *socket;

    int packetsize;
    QByteArray buf;

    void readdata();

signals:

public slots:
    void onreadyRead();
    void onsend();
};

#endif // MYWINDOW_H
