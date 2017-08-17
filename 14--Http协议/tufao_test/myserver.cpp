#include "myserver.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>

MyServer::MyServer(QObject *parent) : QObject(parent)
{
    server.listen(QHostAddress::Any, 9988);
    connect(&server, SIGNAL(requestReady(Tufao::HttpServerRequest&, Tufao::HttpServerResponse&)),
            this, SLOT(onRequestReady(Tufao::HttpServerRequest&,Tufao::HttpServerResponse&)));
}

void MyServer::handlePosData(HttpServerRequest &request, HttpServerResponse &response)
{
    //qDebug() << request.readBody();
    QByteArray body = request.readBody();
    qDebug() << body;

    response.writeHead(HttpResponseStatus::OK);

    //解析报文
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject root = doc.object();
    //root.value("cmd");
    //root.value("username");
    //root.value("passwd")
    QString cmd = root.value("cmd").toString();
    QString username = root.value("username").toString();
    QString passwd = root.value("passwd").toString();

    if(cmd == "register")
    {
        QJsonObject obj;
        obj.insert("result", "ok");
        obj.insert("cmd", "register");
        QJsonDocument doc(obj);
        QByteArray respjson = doc.toJson();


        response.end(respjson);
        return;

    }
    else if(cmd == "login")
    {
        QJsonObject obj;
        obj.insert("result", "ok");
        obj.insert("cmd", "login");
        QJsonDocument doc(obj);
        QByteArray respjson = doc.toJson();


        response.end(respjson);
        return;
    }

}

void MyServer::onRequestReady(Tufao::HttpServerRequest &request, Tufao::HttpServerResponse &response)
{
#if 0
    qDebug() << request.url().path();
    //HttpVersion version = request.httpVersion();
    //qDebug() << request.httpVersion();
    qDebug() << request.method();//POST GET

    response.writeHead(HttpResponseStatus::OK);
    //response.write("hello");
    //response.write("world");
    response.end("hello world");//结束标志字符串
#endif
    if(request.method() == "POST")
    {
        connect(&request, &HttpServerRequest::end, [&](){
            handlePosData(request, response);
        });
    }
}
