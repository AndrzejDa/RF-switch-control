
#include "gui.hpp"

MainWindow::MainWindow(QScreen *screen, QWidget *parent) : QMainWindow(parent){
    screen_width = 640;
    screen_height = 320;
    is_usb_connected = false;
    setGeometry(int((screen->size().width()-screen_width)/2), int((screen->size().height()-screen_height)/2), screen_width, screen_height);
    setWindowTitle("RF Switch Controller :>");
    setFixedSize(size());
    
    connect_button = new QPushButton("Connect", this);
    reload_button = new QPushButton("reload", this);
    devices = new QComboBox(this);
    rf1_button = new QPushButton("RF_1", this);
    rf2_button = new QPushButton("RF_2", this);
    rf3_button = new QPushButton("RF_3", this);
    rf4_button = new QPushButton("RF_4", this);
    config_widgets();
}

MainWindow::~MainWindow() {
    delete connect_button;
    delete reload_button;
    delete devices;
    delete rf1_button;
    delete rf2_button;
    delete rf3_button;
    delete rf4_button;
    if(uhd_thread.isRunning()){
        uhd_thread.wait();
    }
}

std::vector<char*> MainWindow::get_uhd_input_args(){
    return uhd_input_args;
}

void MainWindow::config_widgets(){
    QVBoxLayout *merged_layout = new QVBoxLayout();
    QHBoxLayout *top_layout = new QHBoxLayout();

    connect_button->setFixedSize(90, 40);
    connect_button->setObjectName("connect");
    connect(connect_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    reload_button->setFixedSize(70, 40);
    reload_button->setObjectName("reload");
    connect(reload_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    available_ports = usb_device.list_devices();
    get_ports(available_ports);

    top_layout->addWidget(connect_button);
    top_layout->addWidget(devices);
    top_layout->addWidget(reload_button);

    QHBoxLayout *bottom_layout = new QHBoxLayout();

    rf1_button->setFixedHeight(60);
    rf1_button->setObjectName("rf1");
    rf1_button->setEnabled(false);
    connect(rf1_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    rf2_button->setFixedHeight(60);
    rf2_button->setObjectName("rf2");
    rf2_button->setEnabled(false);
    connect(rf2_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    rf3_button->setFixedHeight(60);
    rf3_button->setObjectName("rf3");
    rf3_button->setEnabled(false);
    connect(rf3_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    rf4_button->setFixedHeight(60);
    rf4_button->setObjectName("rf4");
    rf4_button->setEnabled(false);
    connect(rf4_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    bottom_layout->addWidget(rf1_button);
    bottom_layout->addWidget(rf2_button);
    bottom_layout->addWidget(rf3_button);
    bottom_layout->addWidget(rf4_button);
    merged_layout->addLayout(top_layout);
    merged_layout->addLayout(bottom_layout);

    QWidget *widget = new QWidget();
    widget->setLayout(merged_layout);
    setCentralWidget(widget);
}

void MainWindow::button_clicked() {
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());
    QString id = senderButton->objectName();
    if (id == "rf1") {
        qDebug() << "rf1";
        usb_device.write_data("1");
        if(!uhd_thread.isRunning()){
            uhd_thread.start();
        }
    } else if (id == "rf2") {
        qDebug() << "rf2";
        usb_device.write_data("2");
    } else if (id == "rf3") {
        qDebug() << "rf3";
        usb_device.write_data("3");
    } else if (id == "rf4") {
        qDebug() << "rf4";
        usb_device.write_data("4");
    } else if (id == "reload") {
        qDebug() << "reload";
        available_ports = usb_device.list_devices();
        get_ports(available_ports);
    } else if (id == "connect") {
        if(!is_usb_connected){            
            for(const auto &port : available_ports){
                if(port.description() == devices->currentText()){
                    if(port.vendorIdentifier() == VENDOR_ID_PASS){
                        usb_device.open_port(port.portName());
                        is_usb_connected = true;
                        reload_button->setEnabled(false);
                        rf1_button->setEnabled(true);
                        rf2_button->setEnabled(true);
                        rf3_button->setEnabled(true);
                        rf4_button->setEnabled(true);
                        devices->setDisabled(true);
                        connect_button->setText("Disconnect");
                    }else{
                        qDebug() << "Wrong Vendor ID";
                    }                    
                }
            }
        }else{
            is_usb_connected = false;
            usb_device.close_port();
            connect_button->setText("Connect");
            reload_button->setEnabled(true);
            rf1_button->setEnabled(false);
            rf2_button->setEnabled(false);
            rf3_button->setEnabled(false);
            rf4_button->setEnabled(false);
            devices->setDisabled(false);
        }
    }
}

void MainWindow::get_ports(QList<QSerialPortInfo> available_ports){
    devices->clear();
    for(const auto &port : available_ports){
        devices->addItem(port.description());
    }
    devices->setCurrentIndex(0);
}