
#include "gui.hpp"

MainWindow::MainWindow(QScreen *screen, QWidget *parent) : QMainWindow(parent){
    screen_width = 640;
    screen_height = 750;
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

    file_arg = new QLabel("file");
    file_name = new QLineEdit;
    file_name->setText("usrp_samples.dat");
    uhd_settings[file_arg] = file_name;
    type_arg = new QLabel("type");
    type_options = new QComboBox;
    type_options->addItem("short");
    type_options->addItem("double");
    type_options->addItem("float");
    type_options->setCurrentIndex(0);
    uhd_settings[type_arg] = type_options;
    nsamps_arg = new QLabel("nsamps");
    nsamps_val = new QLineEdit;
    nsamps_val->setText("0");
    uhd_settings[nsamps_arg] = nsamps_val;
    duration_arg = new QLabel("duration");
    duration_val = new QLineEdit;
    duration_val->setText("0");
    uhd_settings[duration_arg] = duration_val;
    spb_arg = new QLabel("spb");
    spb_val = new QLineEdit;
    spb_val->setText("10000");
    uhd_settings[spb_arg] = spb_val;
    rate_arg = new QLabel("rate");
    rate_val = new QLineEdit;
    rate_val->setText("1e6");
    uhd_settings[rate_arg] = rate_val;
    freq_arg = new QLabel("freq");
    freq_val = new QLineEdit;
    freq_val->setText("0.0");
    uhd_settings[freq_arg] = freq_val;
    lo_offset_arg = new QLabel("lo-offset");
    lo_offset_val = new QLineEdit;
    lo_offset_val->setText("0.0");
    uhd_settings[lo_offset_arg] = lo_offset_val;
    gain_arg = new QLabel("gain");
    gain_val = new QLineEdit;
    uhd_settings[gain_arg] = gain_val;
    channel_arg = new QLabel("channel");
    channel_options = new QComboBox;
    channel_options->addItem("0");
    channel_options->addItem("1");
    channel_options->addItem("0,1");
    channel_options->setCurrentIndex(0);
    uhd_settings[channel_arg] = channel_options;
    bw_arg = new QLabel("bw");
    bw_val = new QLineEdit;
    uhd_settings[bw_arg] = bw_val;
    ref_arg = new QLabel("ref");
    ref_options = new QComboBox;
    ref_options->addItem("");
    ref_options->addItem("internal");
    ref_options->addItem("external");
    ref_options->addItem("mimo");
    ref_options->setCurrentIndex(0);
    uhd_settings[ref_arg] = ref_options;
    wirefmt_arg = new QLabel("wirefmt");
    wirefmt_options = new QComboBox;
    wirefmt_options->addItem("sc8");
    wirefmt_options->addItem("sc16");
    wirefmt_options->addItem("s16");
    wirefmt_options->setCurrentIndex(1);
    uhd_settings[wirefmt_arg] = wirefmt_options;
    setup_arg = new QLabel("setup");
    setup_val = new QLineEdit;
    setup_val->setText("1.0");
    uhd_settings[setup_arg] = setup_val;
    continue_arg = new QLabel("continue");
    continue_state = new QCheckBox;
    uhd_settings[continue_arg] = continue_state;
    skip_lo_arg = new QLabel("skip-lo");
    skip_lo_state = new QCheckBox;
    uhd_settings[skip_lo_arg] = skip_lo_state;
    run_button = new QPushButton("Run");
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

std::string MainWindow::get_settings(){
    std::string converted_arg;
    QString contents;
    converted_arg += "./rx_samples_to_file";
    qDebug()<< gain_val->text().isEmpty();
    for (const auto &pair : uhd_settings){
        converted_arg += " ";
        if(pair.second->inherits("QLineEdit")){
            contents = dynamic_cast<QLineEdit*>(pair.second)->text();
            if(!contents.isEmpty()){
                converted_arg += "--" + pair.first->text().toStdString() + " ";
                converted_arg += contents.toStdString();
            }            
        } else if(pair.second->inherits("QComboBox")){
            contents = dynamic_cast<QComboBox*>(pair.second)->currentText();
            if(!contents.isEmpty()){
                converted_arg += "--" + pair.first->text().toStdString() + " ";
                converted_arg += contents.toStdString();
            }
        } else if (pair.second->inherits("QCheckBox")){
            if(dynamic_cast<QCheckBox*>(pair.second)->isChecked()){
                converted_arg += "--" + pair.first->text().toStdString() + " ";
            }
        }
    }
    return converted_arg;
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

    QHBoxLayout *middle_layout = new QHBoxLayout();

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

    run_button->setObjectName("run");
    connect(run_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    middle_layout->addWidget(rf1_button);
    middle_layout->addWidget(rf2_button);
    middle_layout->addWidget(rf3_button);
    middle_layout->addWidget(rf4_button);
    merged_layout->addLayout(top_layout);
    merged_layout->addLayout(middle_layout);

    QVBoxLayout *settings_layout = new QVBoxLayout();
    settings_layout->setContentsMargins(20,20,20,20);
    QHBoxLayout *setting_row_1 = new QHBoxLayout();
    setting_row_1->addWidget(file_arg,1);
    setting_row_1->addWidget(file_name,1);
    settings_layout->addLayout(setting_row_1);
    QHBoxLayout *setting_row_2 = new QHBoxLayout();
    setting_row_2->addWidget(type_arg,1);
    setting_row_2->addWidget(type_options,1);
    settings_layout->addLayout(setting_row_2);
    QHBoxLayout *setting_row_3 = new QHBoxLayout();
    setting_row_3->addWidget(nsamps_arg,1);
    setting_row_3->addWidget(nsamps_val,1);
    settings_layout->addLayout(setting_row_3);
    QHBoxLayout *setting_row_4 = new QHBoxLayout();
    setting_row_4->addWidget(duration_arg,1);
    setting_row_4->addWidget(duration_val,1);
    settings_layout->addLayout(setting_row_4);
    QHBoxLayout *setting_row_5 = new QHBoxLayout();
    setting_row_5->addWidget(spb_arg,1);
    setting_row_5->addWidget(spb_val,1);
    settings_layout->addLayout(setting_row_5);
    QHBoxLayout *setting_row_6 = new QHBoxLayout();
    setting_row_6->addWidget(rate_arg,1);
    setting_row_6->addWidget(rate_val,1);
    settings_layout->addLayout(setting_row_6);
    QHBoxLayout *setting_row_7 = new QHBoxLayout();
    setting_row_7->addWidget(freq_arg,1);
    setting_row_7->addWidget(freq_val,1);
    settings_layout->addLayout(setting_row_7);
    QHBoxLayout *setting_row_8 = new QHBoxLayout();
    setting_row_8->addWidget(lo_offset_arg,1);
    setting_row_8->addWidget(lo_offset_val,1);
    settings_layout->addLayout(setting_row_8);
    QHBoxLayout *setting_row_9 = new QHBoxLayout();
    setting_row_9->addWidget(gain_arg,1);
    setting_row_9->addWidget(gain_val,1);
    settings_layout->addLayout(setting_row_9);
    QHBoxLayout *setting_row_10 = new QHBoxLayout();
    setting_row_10->addWidget(channel_arg,1);
    setting_row_10->addWidget(channel_options,1);
    settings_layout->addLayout(setting_row_10);
    QHBoxLayout *setting_row_11 = new QHBoxLayout();
    setting_row_11->addWidget(bw_arg,1);
    setting_row_11->addWidget(bw_val,1);
    settings_layout->addLayout(setting_row_11);
    QHBoxLayout *setting_row_12 = new QHBoxLayout();
    setting_row_12->addWidget(ref_arg,1);
    setting_row_12->addWidget(ref_options,1);
    settings_layout->addLayout(setting_row_12);
    QHBoxLayout *setting_row_13 = new QHBoxLayout();
    setting_row_13->addWidget(wirefmt_arg,1);
    setting_row_13->addWidget(wirefmt_options,1);
    settings_layout->addLayout(setting_row_13);
    QHBoxLayout *setting_row_14 = new QHBoxLayout();
    setting_row_14->addWidget(setup_arg,1);
    setting_row_14->addWidget(setup_val,1);
    settings_layout->addLayout(setting_row_14);
    QHBoxLayout *setting_row_15 = new QHBoxLayout();
    setting_row_15->addWidget(continue_arg,1);
    setting_row_15->addWidget(continue_state,1);
    settings_layout->addLayout(setting_row_15);
    QHBoxLayout *setting_row_16 = new QHBoxLayout();
    setting_row_16->addWidget(skip_lo_arg,1);
    setting_row_16->addWidget(skip_lo_state,1);
    settings_layout->addLayout(setting_row_16);

    merged_layout->addLayout(settings_layout);
    merged_layout->addWidget(run_button);

    

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
    } else if (id == "run") {
        get_ports(available_ports);
        if(!uhd_thread.isRunning()){
            uhd_thread.set_input_args(get_settings());
            uhd_thread.start();
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