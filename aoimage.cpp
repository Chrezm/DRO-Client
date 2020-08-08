#include "file_functions.h"

#include "aoimage.h"

#include <QDebug>

AOImage::AOImage(QWidget *parent, AOApplication *p_ao_app) : QLabel(parent)
{
  ao_app = p_ao_app;
}

void AOImage::set_image(QString p_image)
{
  QString f_path = ao_app->get_image_path(p_image);
  AOPixmap f_pixmap(f_path);
  this->setPixmap(f_pixmap->scaled(this->width(), this->height(), Qt::IgnoreAspectRatio));

  // Store final path if the path exists
  if (file_exists(f_path))
    image_path = f_path;
  else
    image_path = "";
}

void AOImage::set_image_from_path(QString p_path)
{
  QString default_path = ao_app->get_default_theme_path() + "chatmed.png";

  QString final_path;

  if (file_exists(p_path))
    final_path = p_path;
  else
    final_path = default_path;

  AOPixmap f_pixmap(final_path);
  this->setPixmap(f_pixmap->scaled(this->width(), this->height(), Qt::IgnoreAspectRatio));

  // Store final path if the path exists
  if (file_exists(final_path))
    image_path = final_path;
  else
    image_path = "";
}
