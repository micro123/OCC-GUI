#include "occmainwindow.h"
#include "utils.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OccMainWindow w;
    w.show();

    return a.exec();
}
