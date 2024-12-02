#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "keyboard.h"
#include "ui_keyboard.h"
#include "keyboard_eng.h"
#include "ui_keyboard_eng.h"
#include <QDebug>
#include <QIODevice>
#include <QApplication>
#include <QRegularExpression>
#include <QProcess>

MainWindow *P_main;

QList<int> list_led_state;
int mqtt_ipc_connected = 0;
bool checkWlan0Status();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    P_main = this;
    ui->setupUi(this);

    list_led_state << 0 << 0 << 0;

    myuart = new QSerialPort(this);

    my_kb = new keyboard(this);
    my_kb->setGeometry(400,130,290,140);
    my_kb->hide();
    my_kb_eng = new keyboard_eng(this);
    my_kb_eng->setGeometry(400,130,290,140);
    my_kb_eng->hide();
    connect(myuart, &QSerialPort::readyRead, this, &MainWindow::uart_ready_read);

    Socket_client = new QTcpSocket(this);

    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface interface, list) {
        QList<QNetworkAddressEntry> entryList = interface.addressEntries();
        foreach (QNetworkAddressEntry entry, entryList) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                ui->textBrowser->append("host ip: " + entry.ip().toString());
                IPlist<<entry.ip();
            }
        }
    }
    connect(Socket_client, &QTcpSocket::connected, this, &MainWindow::socket_connected);
    connect(Socket_client, &QTcpSocket::disconnected, this, &MainWindow::socket_disconnected);
    connect(Socket_client, &QTcpSocket::readyRead, this, &MainWindow::socket_readready);
    connect(Socket_client, &QTcpSocket::stateChanged, this, &MainWindow::socket_stateChanged);
    connect(Socket_client, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, \
            &MainWindow::socket_error);
    p_timer = new QTimer(this);
    connect(p_timer, &QTimer::timeout, this, &MainWindow::timer_timeout);
    mqtt_timer = new QTimer(this);
    connect(mqtt_timer, &QTimer::timeout, this, &MainWindow::mqtt_timer_timeout);
    mqtt_timer->start(5000);

    //ui->led_wifi->setPixmap(QPixmap(QString::fromUtf8(":/../resource/WIFI_connnect.png")));
    QPixmap c = QPixmap(QString::fromUtf8(":/resource/WIFI_disconnnect.png"));
    ui->led1->setPixmap(QPixmap(QString::fromUtf8(":/resource/led_on.png")));
    ui->led_wifi->setPixmap(c);

}

MainWindow::~MainWindow()
{
    delete ui;
}





bool checkWlan0Status() {
    QProcess process;
    process.start("ifconfig wlan0");
    process.waitForFinished();

    QByteArray output = process.readAllStandardOutput();
    if (output.contains("inet")) {
        // 网卡正常工作
        qDebug() << "wlan0 is up";
        return true;
    } else {
        // 网卡没有正常工作
        qDebug() << "wlan0 is down";
        return false;
    }
}


void MainWindow::on_pushButton_5_clicked(bool checked)
{
    if(checked == true)
    {
        my_kb->show();
    }
    else
    {
        my_kb->hide();
        my_kb_eng->hide();
    }
}

void MainWindow::on_pushButton_mqtt_clicked(bool checked)
{
    if(checked == true && IPlist.length() != 0)
    {
        Socket_client->connectToHost(IPlist[0],8888);
        ui->label_mqtt->setPixmap(QPixmap(QString::fromUtf8(":/resource/status_on.png")));
    }
    else
    {
        Socket_client->abort();
        p_timer->stop();
        mqtt_timer->stop();
        ui->label_mqtt->setPixmap(QPixmap(QString::fromUtf8(":/resource/status_off.png")));
    }
}

void MainWindow::on_con_uart_clicked(bool checked)
{
    if(checked == true)
    {
        myuart->setPortName(ui->comboBox_port->currentText());
        myuart->setBaudRate(ui->comboBox_baud->currentText().toInt());

        switch (ui->comboBox_databit->currentText().toInt()) {
        case 6:
            this->myuart->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            this->myuart->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            this->myuart->setDataBits(QSerialPort::Data8);
            break;
        default:
            break;
        }

        switch (ui->comboBox_parity->currentIndex()) {
        case 0:
            this->myuart->setParity(QSerialPort::NoParity);
            break;
        case 1:
            this->myuart->setParity(QSerialPort::EvenParity);
            break;
        case 2:
            this->myuart->setParity(QSerialPort::OddParity);
            break;
        default:
            break;
        }

        switch (ui->comboBox_stopbit->currentIndex()) {
        case 0:
            this->myuart->setStopBits(QSerialPort::OneStop);
            break;
        case 1:
            this->myuart->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            break;
        }

        myuart->setFlowControl(QSerialPort::NoFlowControl);
        if(!myuart->open(QIODevice::ReadWrite))
        {
            ui->textBrowser->append("DEBUG: can not open the port");
            ui->label_uart->setPixmap(QPixmap(QString::fromUtf8(":/resource/status_off.png")));
        }
        else
        {
            ui->textBrowser->append("DEBUG: open the port success");
            ui->label_uart->setPixmap(QPixmap(QString::fromUtf8(":/resource/status_on.png")));
        }
    }
    else
    {
        myuart->close();
        ui->textBrowser->append("DEBUG: the port closed");
        ui->label_uart->setPixmap(QPixmap(QString::fromUtf8(":/resource/status_off.png")));
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->comboBox_send->currentText() == "UART")
    {
        QString str_head = "UART SEND: ";
        buf_uart_send = ui->lineEdit->text().toUtf8();
        str_head = str_head + QString(buf_uart_send);
        ui->textBrowser->append(str_head);
        myuart->write(buf_uart_send);
    }
    else if(ui->comboBox_send->currentText() == "MQTT")
    {
        QString str_head = "MQTT SEND: ";
        buf_uart_send = ui->lineEdit->text().toUtf8();
        str_head = str_head + QString(buf_uart_send);
        ui->textBrowser->append(str_head);
        /*MQTT SEND*/
        Socket_client->write(buf_uart_send);
    }
    else
    {
        //CMD CONFIG
    }
}

void MainWindow::on_pushButton_scan_clicked()
{
    int i = 0;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        i++;
        this->ui->comboBox_port->addItem(info.portName());
    }
    if(i == 0)
    {
        this->ui->comboBox_port->addItem("None");
        this->ui->comboBox_port->setEnabled(false);
        this->ui->con_uart->setEnabled(false);
    }
    else
    {
        this->ui->comboBox_port->setEnabled(true);
        this->ui->con_uart->setEnabled(true);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    QApplication::quit();
}


void MainWindow::uart_ready_read()
{
    QString str_head = "UART REC: ";
    buf_uart_rec = myuart->readAll();
    ui->textBrowser->append(str_head + QString::fromUtf8(buf_uart_rec));
    /*
     * prase code
    */
    QRegularExpression regex("led(\\d+):(on|off)");
    QRegularExpressionMatch match = regex.match(QString::fromUtf8(buf_uart_rec));

    if (match.hasMatch()) {
        if(mqtt_ipc_connected == 1) Socket_client->write(buf_uart_rec);

        int device = match.captured(1).toInt();  // 设备号
        QString status = match.captured(2);  // 状态 (on/off)
        switch (device) {
        case 0:
               if(status == "on")
               {
                   list_led_state[0] = 1;
                   ui->led1->setPixmap(QPixmap(QString::fromUtf8(":/resource/led_on.png")));
               }
               else
               {
                   list_led_state[0] = 0;
                   ui->led1->setPixmap(QPixmap(QString::fromUtf8(":/resource/led_off.png")));
               }
            break;
        case 1:
            if(status == "on")
            {
                list_led_state[1] = 1;
                ui->led1_2->setPixmap(QPixmap(QString::fromUtf8(":/resource/led_on.png")));
            }
            else
            {
                list_led_state[1] = 0;
                ui->led1_2->setPixmap(QPixmap(QString::fromUtf8(":/resource/led_off.png")));
            }
         break;
        case 2:
            if(status == "on")
            {
                list_led_state[2] = 1;
                ui->led1_3->setPixmap(QPixmap(QString::fromUtf8(":/resource/led_on.png")));
            }
            else
            {
                list_led_state[2] = 0;
                ui->led1_3->setPixmap(QPixmap(QString::fromUtf8(":/resource/led_off.png")));
            }
         break;
        default:
            break;
        }
        qDebug() << "设备号:" << device << ", 状态:" << status;
    } else {
        qDebug() << "无法解析数据:" << QString::fromUtf8(buf_uart_rec);
    }
}

void MainWindow::socket_connected()
{
    p_timer->stop();    
    mqtt_ipc_connected = 1;
}

void MainWindow::socket_disconnected()
{
/**/
    mqtt_ipc_connected = 0;
}

void MainWindow::socket_readready()
{
    QString str_head = "MQTT REC: ";
    buf_uart_rec = Socket_client->readAll();
    ui->textBrowser->append(str_head + QString::fromUtf8(buf_uart_rec));
}

void MainWindow::socket_stateChanged(QAbstractSocket::SocketState sta1)
{

}

void MainWindow::socket_error(QAbstractSocket::SocketError err1)
{
    Socket_client->abort();
    ui->textBrowser->append("DEBUG: Socket connect failed retrying ...");
    p_timer->start(5000);
    mqtt_timer->stop();
    mqtt_ipc_connected = 0;
}

void MainWindow::timer_timeout()
{
    Socket_client->connectToHost(IPlist[0],8888);
}

void MainWindow::mqtt_timer_timeout()
{
    if(checkWlan0Status() == true)
    {
        mqtt_timer->stop();
        ui->pushButton_mqtt->setEnabled(true);
        ui->led_wifi->setPixmap(QPixmap(QString::fromUtf8(":/resource/WIFI_connect.png")));
    }
}
