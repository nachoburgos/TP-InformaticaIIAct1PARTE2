#include <QApplication>
#include "ventanaprincipal.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ventanaPrincipal w;
    qDebug() << "TODO OK!!!";
    w.show();

    return a.exec();
}
