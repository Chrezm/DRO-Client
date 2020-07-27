#ifndef AOCHARBUTTON_H
#define AOCHARBUTTON_H

#include "aoapplication.h"
#include "aoimage.h"

#include <QPushButton>
#include <QString>
#include <QWidget>

class AOCharButton : public QPushButton
{
    Q_OBJECT

public:
    AOCharButton(QWidget *parent, AOApplication *p_ao_app, int x_pos, int y_pos);
    AOApplication *ao_app = nullptr;

    void reset();
    void set_taken();
    void set_passworded();
    void set_image(QString p_character);

signals:
    void mouse_entered(AOCharButton *p_caller);
    void mouse_left();

private:
    AOImage *ui_taken      = nullptr;
    AOImage *ui_passworded = nullptr;

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
};

#endif // AOCHARBUTTON_H
