#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    //inițializează aplicația Qt (gestionează resursele grafice)
    QApplication a(argc, argv);

    //creează fereastra principală (care conține Board-ul și Jocul)
    MainWindow w;

    //afișează fereastra
    w.show();

    //pornește bucla infinită de evenimente (așteaptă click-uri)
    return a.exec();
}