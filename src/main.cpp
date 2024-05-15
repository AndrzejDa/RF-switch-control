#include <QApplication>
#include <QScreen>
#include "gui.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QScreen *screen = QGuiApplication::primaryScreen();
    MainWindow window(screen);
    window.show();
    return app.exec();    
}
