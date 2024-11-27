#include "usb_port.hpp"

USBPort::USBPort(QObject *parent) : QObject(parent) {
    config_port();
}

QList<QSerialPortInfo> USBPort::list_devices(){
    QList<QSerialPortInfo> available_ports = QSerialPortInfo::availablePorts();
    qDebug() << "Connected USB Devices:";

    for(const QSerialPortInfo &serialPortInfo : available_ports) {
        qDebug() << "Port Name:" << serialPortInfo.portName();
        qDebug() << "Description:" << serialPortInfo.description();
        qDebug() << "Manufacturer:" << serialPortInfo.manufacturer();
        qDebug() << "Product ID:" << serialPortInfo.productIdentifier();
        qDebug() << "Vendor ID:" << serialPortInfo.vendorIdentifier();
        qDebug() << "--------------------------------------";
    }
    return available_ports;
}

void USBPort::config_port(){
    serial_port.setBaudRate(QSerialPort::Baud9600);
    serial_port.setDataBits(QSerialPort::Data8);
    serial_port.setParity(QSerialPort::NoParity);
    serial_port.setStopBits(QSerialPort::OneStop);
    serial_port.setFlowControl(QSerialPort::NoFlowControl);
}

void USBPort::open_port(const QString &portName){
    if(!serial_port.isOpen()){
        serial_port.setPortName(portName);
        serial_port.open(QIODevice::WriteOnly);
        qDebug() << "Port opened successfully";
    }else{
        qDebug() << "Already opened";
    }
}

void USBPort::close_port(){
    if(serial_port.isOpen()){
        serial_port.close();
        qDebug() << "Port closed successfully";
    }else{
        qDebug() << "Already closed";
    }
}

void USBPort::write_data(const QByteArray &data){
    if(serial_port.isOpen()){
        qint64 state = serial_port.write(data);
        //qDebug() << state << "debug wysylania";
    }else{
        qDebug() << "Cannot send data";
    }
}

bool USBPort::isOpen(){
    return serial_port.isOpen();
}

