#ifndef GUI_HPP
#define GUI_HPP

#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <unistd.h>
#include "usb_port.hpp"
#include "uhd_thread.hpp"
#include <QString>
#include <QTimer>

#define RX_SAMPLES_TO_FILE    //switch GUI mode
#define MIN_WINDOW_HEIGHT 350
#define MIN_WINDOW_WIDTH 640

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QScreen *screen, QWidget *parent = nullptr);
    ~MainWindow();
    std::string get_settings();
private:
    QScreen *screen_ptr;

    QPushButton *connect_button;
    QPushButton *refresh_button;
    QComboBox *devices;

    QLabel *mode_choice;
    QCheckBox *auto_switch_mode_state;
    QLineEdit *switching_time;
    QLabel *unit;
    QPushButton *auto_switch_start_button;
    QPushButton *auto_switch_stop_button;

    QPushButton *rf1_button;
    QPushButton *rf2_button;
    QPushButton *rf3_button;
    QPushButton *rf4_button;

    QLabel *file_arg;
    QLineEdit *file_name;
    QLabel *type_arg;
    QComboBox *type_options;
    QLabel *nsamps_arg;
    QLineEdit *nsamps_val;
    QLabel *duration_arg;
    QLineEdit *duration_val;
    QLabel *spb_arg;
    QLineEdit *spb_val;
    QLabel *rate_arg;
    QLineEdit *rate_val;
    QLabel *freq_arg;
    QLineEdit *freq_val;
    QLabel *lo_offset_arg;
    QLineEdit *lo_offset_val;
    QLabel *gain_arg;
    QLineEdit *gain_val;
    QLabel *ant_arg;
    QComboBox *ant_options;
    QLabel *channel_arg;
    QComboBox *channel_options;
    QLabel *bw_arg;
    QLineEdit *bw_val;
    QLabel *ref_arg;
    QComboBox *ref_options;
    QLabel *wirefmt_arg;
    QComboBox *wirefmt_options;
    QLabel *setup_arg;
    QLineEdit *setup_val;
    QLabel *continue_arg;
    QCheckBox *continue_state;
    QLabel *skip_lo_arg;
    QCheckBox *skip_lo_state;
    QLabel *additional_args;
    QLineEdit *args_val;

    QPushButton *run_button;

    QTimer *timer;
    USBPort usb_device;
    UHDThread uhd_thread;

    int window_width;
    int window_height;
    bool is_usb_connected;
    bool is_auto_switch_mode_selected;
    bool is_sending_data;
    //bool UHD_feateures;
    std::map<QLabel*, QObject*> uhd_settings;
    std::vector<char*> uhd_input_args;
    QList<QSerialPortInfo> available_ports;
    std::vector<QByteArray> tmp_auto_switch_ports_data;
    std::vector<QByteArray> final_auto_switch_ports_data;
    QString usb_port_name;
    int auto_switch_interval;

    void config_widgets();
    void button_clicked();
    void checkbox_marked(int);
    void button_toggled(bool);
    void uncheck_rf_buttons();
    void set_enabled_rf_buttons(bool);
    void get_ports(QList<QSerialPortInfo> available_ports);
    void write_multi_data();
};

#endif