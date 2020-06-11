#ifndef AOBUTTON_H
#define AOBUTTON_H

#include "aoapplication.h"

#include <QPushButton>

class AOButton : public QPushButton
{
  Q_OBJECT

public:
  AOButton(QWidget *parent, AOApplication *p_ao_app);
  void set_image(QString p_image);

  AOApplication *ao_app = nullptr;
  QString image_path = "";
};

#endif // AOBUTTON_H
