#include "aolabel.h"

#include "file_functions.h"

AOLabel::AOLabel(QWidget *parent, AOApplication *p_ao_app) : QLabel(parent)
{
  ao_app = p_ao_app;
}

void AOLabel::set_image(QString p_image)
{
  QString f_path = ao_app->get_image_path(p_image);
  setStyleSheet("border-image:url(\"" + f_path + "\")");
}
