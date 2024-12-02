#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>


namespace Ui {
//class MainWindow;
class keyboard;
}

class keyboard : public QWidget
{
    Q_OBJECT

public:
    explicit keyboard(QWidget *parent = nullptr);
    ~keyboard();
    Ui::keyboard *ui;

private slots:
    void on_k_1_clicked();

    void on_k_2_clicked();

    void on_k_5_clicked();

    void on_k_3_clicked();

    void on_k_4_clicked();

    void on_k_6_clicked();

    void on_k_7_clicked();

    void on_k_8_clicked();

    void on_k_9_clicked();

    void on_k_0_clicked();



    void on_swi_2_clicked();
    void on_swi_clicked();
};

#endif // KEYBOARD_H
