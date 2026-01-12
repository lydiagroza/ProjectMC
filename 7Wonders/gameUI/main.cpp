#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    // 1. Inițializează aplicația Qt (gestionează resursele grafice)
    QApplication a(argc, argv);

    // 2. Creează fereastra principală (care conține Board-ul și Jocul)
    MainWindow w;

    // 3. Afișează fereastra
    w.show();

    // 4. Pornește bucla infinită de evenimente (așteaptă click-uri)
    return a.exec();
}