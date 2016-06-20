#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    w.setWindowTitle("PAS Annotation Tool");

    w.setWindowIcon(QIcon(":/img/resource/logo.ico"));

    w.show();

    return a.exec();
}
