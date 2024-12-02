#include "keyboard.h"
#include "ui_keyboard.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"



keyboard::keyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::keyboard)
{
    ui->setupUi(this);
}

keyboard::~keyboard()
{
    delete ui;
}

void keyboard::on_k_1_clicked()
{
    P_main->ui->lineEdit->insert("1");
}

void keyboard::on_k_2_clicked()
{
    P_main->ui->lineEdit->insert("2");
}

void keyboard::on_k_5_clicked()
{
    P_main->ui->lineEdit->insert("3");
}

void keyboard::on_k_3_clicked()
{
    P_main->ui->lineEdit->insert("4");
}

void keyboard::on_k_4_clicked()
{
    P_main->ui->lineEdit->insert("5");
}

void keyboard::on_k_6_clicked()
{
    P_main->ui->lineEdit->insert("6");
}

void keyboard::on_k_7_clicked()
{
    P_main->ui->lineEdit->insert("7");
}

void keyboard::on_k_8_clicked()
{
    P_main->ui->lineEdit->insert("8");
}

void keyboard::on_k_9_clicked()
{
    P_main->ui->lineEdit->insert("9");
}

void keyboard::on_k_0_clicked()
{
    P_main->ui->lineEdit->insert("0");
}

void keyboard::on_swi_2_clicked()
{
    P_main->ui->lineEdit->backspace();
}

void keyboard::on_swi_clicked()
{
    P_main->my_kb->hide();
    P_main->my_kb_eng->show();
}
