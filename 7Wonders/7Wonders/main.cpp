#include "MainWindow.h" // Include your new window header
#include <QApplication> // Include the Qt application header

int main(int argc, char* argv[])
{
    // 1. Initialize the Qt system
    QApplication a(argc, argv);

    // 2. Create your GUI Window
    MainWindow w;

    // 3. Display the window
    w.show();

    // 4. Start the event loop (The app sits here and waits for clicks)
    return a.exec();
}