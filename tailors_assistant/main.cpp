#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/resources/tailors-assistant.svg"));
    a.setApplicationName("Tailor's Assistant");
    MainWindow w;
    w.show();

    return a.exec();
}
