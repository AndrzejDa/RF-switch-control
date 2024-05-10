#ifndef GUI_HPP
#define GUI_HPP

#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QScreen *screen, QWidget *parent = nullptr);
    ~MainWindow();

private:
    QPushButton *connect_button;
    QPushButton *reload_button;
    QComboBox *devices;
    QPushButton *rf1_button;
    QPushButton *rf2_button;
    QPushButton *rf3_button;
    QPushButton *rf4_button;
    int screen_width;
    int screen_height;

    void config_widgets();
    void button_clicked();
};

#endif