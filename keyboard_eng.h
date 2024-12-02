#ifndef KEYBOARD_ENG_H
#define KEYBOARD_ENG_H

#include <QWidget>

namespace Ui {
class keyboard_eng;
}

class keyboard_eng : public QWidget
{
    Q_OBJECT

public:
    explicit keyboard_eng(QWidget *parent = nullptr);
    ~keyboard_eng();

private:
    Ui::keyboard_eng *ui;
    int8_t CapsLock;
private slots:
    void keypressed(void);
    void func_keypressed(void);
};

#endif // KEYBOARD_ENG_H
