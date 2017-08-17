#include <QApplication>
#include "mywindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    myWindow w;
    w.show();

    return app.exec();
}
