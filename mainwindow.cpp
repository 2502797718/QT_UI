#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "keyboard.h"
#include "ui_keyboard.h"
#include "keyboard_eng.h"
#include "ui_keyboard_eng.h"
#include <QDebug>
#include <QIODevice>
#include <QApplication>


MainWindow *P_main;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    P_main = this;
    ui->setupUi(this);


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
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_3_clicked(bool checked)
{
    if(checked == true)
    {
        ui->led1->setEnabled(true);
        ui->led1_2->setEnabled(true);
        ui->led1_3->setEnabled(true);
    }
    else
    {
        ui->led1->setEnabled(false);
        ui->led1_2->setEnabled(false);
        ui->led1_3->setEnabled(false);
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
    else
    {
        QString str_head = "MQTT SEND: ";
        buf_uart_send = ui->lineEdit->text().toUtf8();
        str_head = str_head + QString(buf_uart_send);
        ui->textBrowser->append(str_head);
        /*MQTT SEND*/
        Socket_client->write(buf_uart_send);
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
}

void MainWindow::socket_connected()
{
    p_timer->stop();
}

void MainWindow::socket_disconnected()
{
/**/
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
}

void MainWindow::timer_timeout()
{
    Socket_client->connectToHost(IPlist[0],8888);
}
