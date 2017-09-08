#include <QCoreApplication>
#include "loopback.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    LoopBack looper;
    return a.exec();
}

