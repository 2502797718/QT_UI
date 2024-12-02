#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include "keyboard.h"
#include "keyboard_eng.h"
#include <QByteArray>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
//class keyboard;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSerialPort *myuart;
    Ui::MainWindow *ui;
    keyboard *my_kb;
    keyboard_eng *my_kb_eng;
    QByteArray buf_uart_rec;
    QByteArray buf_uart_send;

    QTcpSocket *Socket_client;
    QList<QHostAddress> IPlist;
    QTimer *p_timer;
    QTimer *mqtt_timer;
private slots:

    void on_pushButton_5_clicked(bool checked);

    void on_pushButton_mqtt_clicked(bool checked);

    void on_con_uart_clicked(bool checked);

    void on_pushButton_clicked();

    void on_pushButton_scan_clicked();

    void on_pushButton_4_clicked();
    void uart_ready_read();

    void socket_connected();
    void socket_disconnected();
    void socket_readready();
    void socket_stateChanged(QAbstractSocket::SocketState sta1);
    void socket_error(QAbstractSocket::SocketError err1);

    void timer_timeout();
    void mqtt_timer_timeout();
private:

};

extern MainWindow *P_main;
#endif // MAINWINDOW_H
