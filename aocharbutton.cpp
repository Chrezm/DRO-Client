#include "aocharbutton.h"

#include "file_functions.h"

#include <QFile>

AOCharButton::AOCharButton(QWidget *parent, AOApplication *p_ao_app, int x_pos,
                           int y_pos)
    : QPushButton(parent)
{
  ao_app = p_ao_app;

  this->resize(60, 60);
  this->move(x_pos, y_pos);

  ui_taken = new AOImage(this, ao_app);
  ui_taken->resize(60, 60);
  ui_taken->set_image("char_taken.png");
  ui_taken->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_taken->hide();
}

void AOCharButton::reset() { ui_taken->hide(); }

void AOCharButton::set_taken() { ui_taken->show(); }

void AOCharButton::set_image(QString p_character)
{
  QString image_path =
      ao_app->get_character_path(p_character) + "char_icon.png";
  QString legacy_path =
      ao_app->get_demothings_path() + p_character.toLower() + "_char_icon.png";
  QString alt_path =
      ao_app->get_demothings_path() + p_character.toLower() + "_off.png";

  this->setText("");

  if (file_exists(image_path))
    this->setStyleSheet("border-image:url(\"" + image_path + "\")");
  else if (file_exists(legacy_path))
  {
    this->setStyleSheet("border-image:url(\"" + legacy_path + "\")");
    // ninja optimization
    QFile::copy(legacy_path, image_path);
  }
  else
  {
    this->setStyleSheet("border-image:url()");
    this->setText(p_character);
  }
}

void AOCharButton::enterEvent(QEvent *e)
{
  setFlat(false);
  QPushButton::enterEvent(e);
  emit mouse_entered(this);
}

void AOCharButton::leaveEvent(QEvent *e)
{
  QPushButton::leaveEvent(e);
  emit mouse_left();
}
