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
#include "usb_port.hpp"

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

    USBPort usb_device;

    int screen_width;
    int screen_height;
    bool is_connected;
    QList<QSerialPortInfo> available_ports;

    void config_widgets();
    void button_clicked();
    void get_ports(QList<QSerialPortInfo> available_ports);
};

#endif