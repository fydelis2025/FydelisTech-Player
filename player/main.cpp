// ==================================================
// FydelisTech-Player — Ponto de Entrada
// © 2026 FydelisTech. Todos os direitos reservados.
// ==================================================
#include <QApplication>
#include "player.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("FydelisTech-Player");
    a.setOrganizationName("FydelisTech");
    a.setApplicationVersion("1.0.0");

    FydelisTechPlayer w;
    w.show();
    return a.exec();
}