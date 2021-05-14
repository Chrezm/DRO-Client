#ifndef AOEMOTEBUTTON_H
#define AOEMOTEBUTTON_H

// src
#include "aoapplication.h"
#include "datatypes.h"

// qt
#include <QLabel>
#include <QPushButton>

class AOEmoteButton : public QPushButton
{
  Q_OBJECT

public:
  AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y);

  int get_emote_number();
  void set_emote_number(int p_emote_number);
  void set_image(DREmote emote, bool enabled);

signals:
  void emote_clicked(int p_emote_number);

private:
  AOApplication *ao_app = nullptr;
  QLabel *w_selected = nullptr;
  int m_emote_number = 0;

private slots:
  void on_clicked();
};

#endif // AOEMOTEBUTTON_H
