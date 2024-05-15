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
#include <unistd.h>
#include "usb_port.hpp"
#include "uhd_thread.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QScreen *screen, QWidget *parent = nullptr);
    ~MainWindow();
    std::vector<char*> get_uhd_input_args();
private:
    QPushButton *connect_button;
    QPushButton *reload_button;
    QComboBox *devices;
    QPushButton *rf1_button;
    QPushButton *rf2_button;
    QPushButton *rf3_button;
    QPushButton *rf4_button;

    USBPort usb_device;
    UHDThread uhd_thread;

    int screen_width;
    int screen_height;
    bool is_usb_connected;
    //bool UHD_feateures;
    std::map<char*, char*> uhd_settings;
    std::vector<char*> uhd_input_args;
    QList<QSerialPortInfo> available_ports;

    void config_widgets();
    void button_clicked();
    void get_ports(QList<QSerialPortInfo> available_ports);
};

#endif