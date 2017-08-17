#ifndef MYSERVER_H
#define MYSERVER_H

#include <QObject>
#include <QUrl>
#include <tufao-1/Tufao/HttpServer>
#include <tufao-1/Tufao/HttpServerRequest>
#include <tufao-1/Tufao/HttpServerResponse>
using namespace Tufao;

class MyServer : public QObject
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = 0);

    HttpServer server;

    void handlePosData(Tufao::HttpServerRequest&, Tufao::HttpServerResponse&);
signals:

public slots:
    void onRequestReady(Tufao::HttpServerRequest&, Tufao::HttpServerResponse&);
};

#endif // MYSERVER_H
