#include <QList>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <iostream>
#include <fstream>
#include <QByteArray>
#include <cstdio>

CannulaDriver::CannulaDriver() {

    setName("Cannula Driver");

    // Get a list of available ports
    avail_ports = QSerialPortInfo::availablePorts();

    for (int k=0; k < avail_ports.length(); k++){
        if(avail_ports[k].portName() == "ttyACM0"){
            portIndex = k;
        }
    }

    connect(&port, &QSerialPort::errorOccurred, this, &CannulaDriver::serial_port_error_handler);
    connect(&port, &QSerialPort::readyRead, this, &CannulaDriver::read_serial_port);

    // Set port settings
    port.setPort(avail_ports[portIndex]);
    port.setDataBits(QSerialPort::Data8);
    port.setStopBits(QSerialPort::OneStop);
    port.setBaudRate(QSerialPort::Baud9600);

    enabled = false;

}

CannulaDriver::~CannulaDriver(){
    port.close();
    enabled = false;
    //delete UI;
}

bool CannulaDriver::enable(){
    if(isInError)
        return false;

    // Open port
    if(port.open(QIODevice::ReadWrite)){
        std::cout << "Port opened" << std::endl;
        enabled = true;
        return true;
    }
    return false;
}

bool CannulaDriver::disable(){
    enabled = false;
    port.close();
    return (!port.isOpen());
}

void CannulaDriver::moveRelative(){
    char message [50];
    sprintf(message, "<Rel, %f, %f>", step, speed);
    port.write(message);
}

void CannulaDriver::moveAbsolute(){
    char message [50];
    sprintf(message, "<Abs, %f, %f>", step, speed);
    port.write(message);
}

void CannulaDriver::stopDriver(){
    port.write("<Stop, 0.0, 0.0>");
}

void CannulaDriver::serial_port_error_handler() {

    QSerialPort::SerialPortError error = port.error();

    // Set Cannula Driver to Error state
    isInError = true;

    // Provide a message based on the error that occurred
    if (error == QSerialPort::NoError) {
        return; // There was no error so no need to show anything
    }
    if (error == QSerialPort::DeviceNotFoundError) {
        std::cout << "Serial Port Error: Device Not Found Error!" << std::endl;
    }
    else if (error == QSerialPort::PermissionError) {
        std::cout << "Serial Port Error: Permission Error!" << std::endl;
    }
    else if (error == QSerialPort::OpenError) {
        std::cout << "Serial Port Error: Open Error!" << std::endl;
    }
    else if (error == QSerialPort::NotOpenError) {
        std::cout << "Serial Port Error: Not Open Error!" << std::endl;
    }
    else if (error == QSerialPort::ParityError) {
        std::cout << "Serial Port Error: Parity Error!" << std::endl;
    }
    else if (error == QSerialPort::FramingError) {
        std::cout << "Serial Port Error: Framing Error!" << std::endl;
    }
    else if (error == QSerialPort::BreakConditionError) {
        std::cout << "Serial Port Error: Break Condition Error!" << std::endl;
    }
    else if (error == QSerialPort::WriteError) {
        std::cout << "Serial Port Error: Write Error!" << std::endl;
    }
    else if (error == QSerialPort::ReadError) {
        std::cout << "Serial Port Error: Read Error!" << std::endl;
    }
    else if (error == QSerialPort::ResourceError) {
        std::cout << "Serial Port Error: Resource Error!" << std::endl;
    }
    else if (error == QSerialPort::UnsupportedOperationError) {
        std::cout << "Serial Port Error: Unsupported Operation Error!" << std::endl;
    }
    else if (error == QSerialPort::TimeoutError) {
        std::cout << "Serial Port Error: Timeout Error!" << std::endl;
    }
    else if (error == QSerialPort::UnknownError) {
        std::cout << "Serial Port Error: Unknown Error!" << std::endl;
    }

}

void CannulaDriver::read_serial_port(){
    while(port.canReadLine()){
        QByteArray responseData = port.readLine();
        QString response = QString::fromUtf8(responseData);
        currentDeg = response.toDouble();
    }
}

void CannulaDriver::updatePosition(){
    distance = currentDeg/stageDistToDeg;
    dist_box->setText(QStringLiteral("%1").arg(distance));
}
