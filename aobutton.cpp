#include "aobutton.h"

#include "debug_functions.h"
#include "file_functions.h"

#include <QDebug>

AOButton::AOButton(QWidget *parent, AOApplication *p_ao_app) : QPushButton(parent)
{
  ao_app = p_ao_app;
}

void AOButton::set_image(QString p_image)
{
  QString f_image_name = p_image.left(p_image.lastIndexOf(QChar('.')));
  QString some_image_path = ao_app->get_image_path(p_image);
  QString some_hover_image_path = ao_app->get_image_path(p_image + "_hover.png");

  if (file_exists(some_image_path))
  {
    image_path = some_image_path;

    if (file_exists(some_hover_image_path))
      this->setStyleSheet("QPushButton {border-image:url(\"" + some_image_path + "\");}"
                          "QPushButton:hover {border-image:url(\"" + some_hover_image_path + "\");}");
    else
      this->setStyleSheet("border-image:url(\"" + some_image_path + "\")");
  }
  else
  {
    image_path = "";
    this->setStyleSheet("border-image:url(\"" + some_image_path + "\")");
  }
}
