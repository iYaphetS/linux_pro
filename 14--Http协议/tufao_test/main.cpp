#include <QCoreApplication>//没有界面的调用这个类
#include "myserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    MyServer myserver;
    
    return app.exec();
    
}
