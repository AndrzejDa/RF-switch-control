
#include "gui.hpp"

MainWindow::MainWindow(QScreen *screen, QWidget *parent) : QMainWindow(parent), screen_ptr(screen){
    is_usb_connected = false;
    is_auto_switch_mode_selected = false;
    is_sending_data = false;
    window_height = MIN_WINDOW_HEIGHT;
    window_width = MIN_WINDOW_WIDTH;
    //setMinimumHeight(MIN_WINDOW_HEIGHT);
    //setMinimumWidth(MIN_WINDOW_WIDTH);
    setWindowTitle("RF Switch Controller");
    setGeometry(int((screen_ptr->size().width()-window_width)/2), int((screen_ptr->size().height()-window_height)/2), window_width, window_height);
    //setFixedSize(size());
    
    timer = new QTimer(this);
    connect_button = new QPushButton("Connect", this);
    refresh_button = new QPushButton("refresh", this);
    devices = new QComboBox(this);
    mode_choice = new QLabel("Auto switch ports mode");
    auto_switch_mode_state = new QCheckBox;
    switching_time = new QLineEdit;
    unit = new QLabel("[ms]");
    auto_switch_start_button = new QPushButton("Start", this);
    auto_switch_stop_button = new QPushButton("Stop", this);
    rf1_button = new QPushButton("RF_1", this);
    rf2_button = new QPushButton("RF_2", this);
    rf3_button = new QPushButton("RF_3", this);
    rf4_button = new QPushButton("RF_4", this);

    #ifdef RX_SAMPLES_TO_FILE
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
        ant_arg = new QLabel("ant");
        ant_options = new QComboBox;
        ant_options->addItem("");
        ant_options->addItem("TX/RX");
        ant_options->addItem("RX2");
        ant_options->setCurrentIndex(0);
        uhd_settings[ant_arg] = ant_options;
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
        additional_args = new QLabel("args");
        args_val = new QLineEdit;
        //args_val->setText("xd");
        uhd_settings[additional_args] = args_val;
        run_button = new QPushButton("Run");
        
    #endif
    config_widgets();
}

MainWindow::~MainWindow() {
    delete connect_button;
    delete refresh_button;
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
    qDebug() << converted_arg.c_str();
    return converted_arg;
}

void MainWindow::config_widgets(){
    QVBoxLayout *merged_layout = new QVBoxLayout();
    QHBoxLayout *top_layout = new QHBoxLayout();

    connect_button->setFixedSize(90, 40);
    connect_button->setObjectName("connect");
    connect(connect_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    refresh_button->setFixedSize(70, 40);
    refresh_button->setObjectName("refresh");
    connect(refresh_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    available_ports = usb_device.list_devices();
    get_ports(available_ports);

    top_layout->addWidget(connect_button);
    top_layout->addWidget(devices);
    top_layout->addWidget(refresh_button);

    QHBoxLayout *mode_layout = new QHBoxLayout();
    QHBoxLayout *mode_row = new QHBoxLayout();
    
    mode_row->addWidget(mode_choice,1);

    mode_row->addWidget(auto_switch_mode_state,1);
    auto_switch_mode_state->setObjectName("auto_switch_mode");
    auto_switch_mode_state->setEnabled(true);
    connect(auto_switch_mode_state, &QCheckBox::stateChanged, this, &MainWindow::checkbox_marked);
    
    mode_row->addWidget(switching_time,1);
    switching_time->setEnabled(false);

    mode_row->addWidget(unit,1);

    mode_row->addWidget(auto_switch_start_button,1);
    auto_switch_start_button->setEnabled(false);
    auto_switch_start_button->setObjectName("auto_switch_start");
    connect(auto_switch_start_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    mode_row->addWidget(auto_switch_stop_button,1);
    auto_switch_stop_button->setEnabled(false);
    auto_switch_stop_button->setObjectName("auto_switch_stop");
    connect(auto_switch_stop_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

    mode_layout->addLayout(mode_row);

    QHBoxLayout *middle_layout = new QHBoxLayout();

    rf1_button->setFixedHeight(60);
    rf1_button->setObjectName("rf1");
    rf1_button->setCheckable(true);
    connect(rf1_button, &QPushButton::clicked, this, &MainWindow::button_toggled);

    rf2_button->setFixedHeight(60);
    rf2_button->setObjectName("rf2");
    rf2_button->setCheckable(true);
    connect(rf2_button, &QPushButton::clicked, this, &MainWindow::button_toggled);

    rf3_button->setFixedHeight(60);
    rf3_button->setObjectName("rf3");
    rf3_button->setCheckable(true);
    connect(rf3_button, &QPushButton::clicked, this, &MainWindow::button_toggled);

    rf4_button->setFixedHeight(60);
    rf4_button->setObjectName("rf4");
    rf4_button->setCheckable(true);
    connect(rf4_button, &QPushButton::clicked, this, &MainWindow::button_toggled);

    set_enabled_rf_buttons(false);

    connect(timer, &QTimer::timeout, this, &MainWindow::write_multi_data);
    

    middle_layout->addWidget(rf1_button);
    middle_layout->addWidget(rf2_button);
    middle_layout->addWidget(rf3_button);
    middle_layout->addWidget(rf4_button);
    merged_layout->addLayout(top_layout);
    merged_layout->addLayout(mode_layout);
    merged_layout->addLayout(middle_layout);

    #ifdef RX_SAMPLES_TO_FILE
        run_button->setObjectName("run");
        connect(run_button, &QPushButton::clicked, this, &MainWindow::button_clicked);

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
        setting_row_10->addWidget(ant_arg,1);
        setting_row_10->addWidget(ant_options,1);
        settings_layout->addLayout(setting_row_10);
        QHBoxLayout *setting_row_11 = new QHBoxLayout();
        setting_row_11->addWidget(channel_arg,1);
        setting_row_11->addWidget(channel_options,1);
        settings_layout->addLayout(setting_row_11);
        QHBoxLayout *setting_row_12 = new QHBoxLayout();
        setting_row_12->addWidget(bw_arg,1);
        setting_row_12->addWidget(bw_val,1);
        settings_layout->addLayout(setting_row_12);
        QHBoxLayout *setting_row_13 = new QHBoxLayout();
        setting_row_13->addWidget(ref_arg,1);
        setting_row_13->addWidget(ref_options,1);
        settings_layout->addLayout(setting_row_13);
        QHBoxLayout *setting_row_14 = new QHBoxLayout();
        setting_row_14->addWidget(wirefmt_arg,1);
        setting_row_14->addWidget(wirefmt_options,1);
        settings_layout->addLayout(setting_row_14);
        QHBoxLayout *setting_row_15 = new QHBoxLayout();
        setting_row_15->addWidget(setup_arg,1);
        setting_row_15->addWidget(setup_val,1);
        settings_layout->addLayout(setting_row_15);
        QHBoxLayout *setting_row_16 = new QHBoxLayout();
        setting_row_16->addWidget(continue_arg,1);
        setting_row_16->addWidget(continue_state,1);
        settings_layout->addLayout(setting_row_16);
        QHBoxLayout *setting_row_17 = new QHBoxLayout();
        setting_row_17->addWidget(skip_lo_arg,1);
        setting_row_17->addWidget(skip_lo_state,1);
        settings_layout->addLayout(setting_row_17);
        QHBoxLayout *setting_row_18 = new QHBoxLayout();
        setting_row_18->addWidget(additional_args,1);
        setting_row_18->addWidget(args_val,1);
        settings_layout->addLayout(setting_row_18);
        merged_layout->addLayout(settings_layout);
        merged_layout->addWidget(run_button);
    #endif

    //merged_layout->setSizeConstraint(QLayout::SetMinimumSize);
    QWidget *widget = new QWidget();
    widget->setLayout(merged_layout);
    //widget->adjustSize();
    setCentralWidget(widget);
    QSize current_window_size = this->size();
    window_width = current_window_size.width();
    window_height = current_window_size.height();
    setGeometry(int((screen_ptr->size().width()-window_width)/2), int((screen_ptr->size().height()-window_height)/2 - 100), window_width, window_height);

}

void MainWindow::button_clicked() {
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());
    QString id = senderButton->objectName();
    if (id == "refresh") {
        qDebug() << "refresh";
        available_ports = usb_device.list_devices(); 
        get_ports(available_ports);       
    } else if (id == "connect") {
        if(!is_usb_connected){            
            for(const auto &port : available_ports){
                if(port.description() == devices->currentText()){
                    if(port.vendorIdentifier() == VENDOR_ID_PASS){
                        usb_port_name = port.portName();
                        usb_device.open_port(usb_port_name);
                        is_usb_connected = true;
                        refresh_button->setEnabled(false);                                                
                        set_enabled_rf_buttons(true);
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
            refresh_button->setEnabled(true);
            auto_switch_start_button->setEnabled(false);
            auto_switch_stop_button->setEnabled(false);
            set_enabled_rf_buttons(false);
            uncheck_rf_buttons();
            is_sending_data = false;
            if(timer->isActive()){
                timer->stop();
                qDebug() << "Stopped sending data";
            }
            devices->setDisabled(false);
        }
    } else if (id == "auto_switch_start"){
        if(!switching_time->text().isEmpty() && !is_sending_data){
            
            final_auto_switch_ports_data.clear();
            for(const auto &item: tmp_auto_switch_ports_data){
                if(item != "0") final_auto_switch_ports_data.push_back(item);
            }
            qDebug() << (final_auto_switch_ports_data.size()!=0);
            if(final_auto_switch_ports_data.size() != 0){
                set_enabled_rf_buttons(false);
                auto_switch_interval = switching_time->text().toInt();
                timer->setInterval(auto_switch_interval);
                is_sending_data = true;
                timer->start();
                qDebug() << "Started sending data";
            }
            
        }
    } else if (id == "auto_switch_stop"){
        if(is_sending_data){
            set_enabled_rf_buttons(true);
            is_sending_data = false;
            timer->stop();
            qDebug() << "Stopped sending data";
        }
    } else if (id == "run") {

        #ifdef RX_SAMPLES_TO_FILE
            get_ports(available_ports);
            if(!uhd_thread.isRunning()){
                uhd_thread.set_input_args(get_settings());
                uhd_thread.start();
            }
        #endif
    }
}

void MainWindow::checkbox_marked(int state){
    QCheckBox *senderCheckbox = qobject_cast<QCheckBox *>(sender());
    QString id = senderCheckbox->objectName();
    if (id == "auto_switch_mode"){
        if(state == Qt::Checked){
            is_auto_switch_mode_selected = true;
            switching_time->setEnabled(true); 
            auto_switch_start_button->setEnabled(true);
            auto_switch_stop_button->setEnabled(true);         
            tmp_auto_switch_ports_data = {"0", "0", "0", "0"};
        } else {
            is_auto_switch_mode_selected = false;
            switching_time->setEnabled(false);
            auto_switch_start_button->setEnabled(false);
            auto_switch_stop_button->setEnabled(false);
        }
        uncheck_rf_buttons();
    }
}

void MainWindow::button_toggled(bool checked){
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());
    QString id = senderButton->objectName();
    if (id == "rf1") {
        if(checked){
            if (!is_auto_switch_mode_selected){
                qDebug() << "rf1";
                usb_device.write_data("1");
                rf2_button->setChecked(false);
                rf3_button->setChecked(false);
                rf4_button->setChecked(false);          
            } else{
                tmp_auto_switch_ports_data[0] = "1";
            }
        } else {
            if (is_auto_switch_mode_selected) tmp_auto_switch_ports_data[0] = "0";
        }        
    } else if (id == "rf2") {
        if(checked){
            if (!is_auto_switch_mode_selected){
                qDebug() << "rf2";
                usb_device.write_data("2"); 
                rf1_button->setChecked(false);
                rf3_button->setChecked(false);
                rf4_button->setChecked(false);         
            } else{
                tmp_auto_switch_ports_data[1] = "2";
            }
        } else {
            if (is_auto_switch_mode_selected) tmp_auto_switch_ports_data[1] = "0";
        }
    } else if (id == "rf3") {
        if(checked){
            if (!is_auto_switch_mode_selected){
                qDebug() << "rf3";
                usb_device.write_data("3"); 
                rf1_button->setChecked(false);
                rf2_button->setChecked(false);
                rf4_button->setChecked(false);        
            } else{
                tmp_auto_switch_ports_data[2] = "3";
            }
        } else {
            if (is_auto_switch_mode_selected) tmp_auto_switch_ports_data[2] = "0";
        }
    } else if (id == "rf4") {
        if(checked){
            if (!is_auto_switch_mode_selected){
                qDebug() << "rf4";
                usb_device.write_data("4"); 
                rf1_button->setChecked(false);
                rf2_button->setChecked(false);
                rf3_button->setChecked(false);          
            } else{
                tmp_auto_switch_ports_data[3] = "4";
            }
        } else {
            if (is_auto_switch_mode_selected) tmp_auto_switch_ports_data[3] = "0";
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

void MainWindow::uncheck_rf_buttons(){
    rf1_button->setChecked(false);
    rf2_button->setChecked(false);
    rf3_button->setChecked(false);
    rf4_button->setChecked(false);
}

void MainWindow::set_enabled_rf_buttons(bool state){
    rf1_button->setEnabled(state);
    rf2_button->setEnabled(state);
    rf3_button->setEnabled(state);
    rf4_button->setEnabled(state);
}

void MainWindow::write_multi_data(){
    static int index = 0;
    if (usb_device.isOpen()){
        usb_device.write_data(final_auto_switch_ports_data[index]);
    } else {
        qDebug() << "Not opened yet";
    }
    if (index < final_auto_switch_ports_data.size()-1){
        index ++;
    } else {
        index = 0;
    }

}
