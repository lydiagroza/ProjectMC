#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QDebug>
#include <QImageReader>
#include <QDir>
#include <QDirIterator>

int main(int argc, char* argv[])
{
    // 1. Inițializează aplicația Qt (gestionează resursele grafice)
    QApplication a(argc, argv);

    qDebug() << "--- RESOURCE DEBUG ---";
    qDebug() << "Checking :/background.jpg:" << QFile::exists(":/background.jpg");
    qDebug() << "Checking :/resources/UI/quack.mp3:" << QFile::exists(":/resources/UI/quack.mp3");
    
    qDebug() << "Supported Image Formats:";
    for(auto fmt : QImageReader::supportedImageFormats()) {
        qDebug() << fmt;
    }

    qDebug() << "Library Paths:" << QApplication::libraryPaths();
    
    QDir dir(":/");
    qDebug() << "Root Resource Contents:";
    for(auto entry : dir.entryList()) {
        qDebug() << " - " << entry;
    }
    QDir resDir(":/resources/UI");
    qDebug() << "Resources/UI Contents:";
    for(auto entry : resDir.entryList()) {
        qDebug() << " - " << entry;
    }
    qDebug() << "----------------------";

    // 2. Creează fereastra principală (care conține Board-ul și Jocul)
    MainWindow w;

    // 3. Afișează fereastra
    w.show();

    // 4. Pornește bucla infinită de evenimente (așteaptă click-uri)
    return a.exec();
}