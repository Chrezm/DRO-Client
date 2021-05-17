#include "file_functions.h"

#include "aoimagedisplay.h"

#include <QDebug>

/*!
 * @class AOImageDisplay
 * @brief Represents a static theme-dependent image.
 */
AOImageDisplay::AOImageDisplay(QWidget *parent, AOApplication *p_ao_app) : QLabel(parent)
{
  ao_app = p_ao_app;
}

void AOImageDisplay::set_image(QString p_image)
{
  QString f_path = ao_app->find_theme_asset_path(p_image);
  AOPixmap f_pixmap(f_path);
  this->setPixmap(f_pixmap.scale(size()));

  // Store final path if the path exists
  if (file_exists(f_path))
    image_path = f_path;
  else
    image_path = "";
}

void AOImageDisplay::set_image_from_path(QString p_path)
{
  QString default_path = ao_app->find_theme_asset_path("chatmed.png");

  QString final_path;

  if (file_exists(p_path))
    final_path = p_path;
  else
    final_path = default_path;

  AOPixmap f_pixmap(final_path);
  this->setPixmap(f_pixmap.scale(size()));

  // Store final path if the path exists
  if (file_exists(final_path))
    image_path = final_path;
  else
    image_path = "";
}
