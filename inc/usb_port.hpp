#ifndef USB_PORT_HPP
#define USB_PORT_HPP

#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define VENDOR_ID_PASS 1155

class USBPort : public QObject {
    Q_OBJECT
public:
    USBPort(QObject *parent = nullptr);
    QList<QSerialPortInfo> list_devices();
    void open_port(const QString &portName);
    void close_port();
    void write_data(const QByteArray &data);
    bool isOpen();
private:    
    QSerialPort serial_port;
    void config_port();
};

#endif