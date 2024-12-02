#include "keyboard_eng.h"
#include "ui_keyboard_eng.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>

QList <QPushButton*> btn_list;


keyboard_eng::keyboard_eng(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::keyboard_eng)
{
    ui->setupUi(this);
    btn_list.append(ui->a);btn_list.append(ui->b);
    btn_list.append(ui->c);btn_list.append(ui->d);
    btn_list.append(ui->e);btn_list.append(ui->f);
    btn_list.append(ui->g);btn_list.append(ui->h);
    btn_list.append(ui->i);btn_list.append(ui->j);
    btn_list.append(ui->k);btn_list.append(ui->l);
    btn_list.append(ui->m);
    btn_list.append(ui->n);
    btn_list.append(ui->u);
    btn_list.append(ui->v);
    btn_list.append(ui->w);
    btn_list.append(ui->x);
    btn_list.append(ui->y);
    btn_list.append(ui->z);
    this->CapsLock = 0;
    foreach (const QPushButton *p, btn_list) {
        connect(p,&QPushButton::clicked,this,&keyboard_eng::keypressed);
    }
    connect(ui->backspace,&QPushButton::clicked,this,&keyboard_eng::func_keypressed);
    connect(ui->Caplock,&QPushButton::clicked,this,&keyboard_eng::func_keypressed);
    connect(ui->switch_2,&QPushButton::clicked,this,&keyboard_eng::func_keypressed);

}

keyboard_eng::~keyboard_eng()
{
    delete ui;
}

void keyboard_eng::keypressed()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    qDebug() << button->text() << endl;
    P_main->ui->lineEdit->insert(button->text());

}

void keyboard_eng::func_keypressed()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if(button->text() == "backspace")
    {
        P_main->ui->lineEdit->backspace();
    }
    else if(button->text() == "Caps")
    {
        if(this->CapsLock == 0)
        {
            foreach (QPushButton *p, btn_list) {
                p->setText(p->text().toUpper());
            }
            this->CapsLock = 1;
        }
        else
        {
            foreach (QPushButton *p, btn_list) {
                p->setText(p->text().toLower());
            }
            this->CapsLock = 0;
        }
    }
    else if(button->text() == "switch")
    {
        P_main->my_kb_eng->hide();
        P_main->my_kb->show();
    }
}


