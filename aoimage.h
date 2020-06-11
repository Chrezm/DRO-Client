//This class represents a static theme-dependent image

#ifndef AOIMAGE_H
#define AOIMAGE_H

#include "aoapplication.h"

#include <QLabel>

class AOImage : public QLabel
{
public:
  AOImage(QWidget *parent, AOApplication *p_ao_app);

  void set_image(QString p_image);
  void set_image_variant(QString p_image, QString p_variant);
  void set_image_from_path(QString p_path);
  void set_size_and_pos(QString identifier);

  AOApplication *ao_app = nullptr;
  QString image_path = "";
};

#endif // AOIMAGE_H
