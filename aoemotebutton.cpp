#include "aoemotebutton.h"

#include "file_functions.h"
#include <QDebug>

AOEmoteButton::AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app,
                             int p_x, int p_y)
    : QPushButton(p_parent) {
  ao_app = p_ao_app;

  this->move(p_x, p_y);
  this->resize(40, 40);

  connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
}

void AOEmoteButton::set_image(QString p_char, int p_emote, QString suffix) {
  QString emotion_number = QString::number(p_emote + 1);
  QString image_path = ao_app->get_character_path(
      p_char, "emotions/ao2/button" + emotion_number + suffix);
  QString alt_path = ao_app->get_character_path(
      p_char, "emotions/button" + emotion_number + suffix);
  QString hover_path = ao_app->get_character_path(
      p_char, "emotions/hovers/button" + emotion_number + "_hover" + suffix);

  if (file_exists(image_path)) {
    this->setText("");
    this->setStyleSheet("border-image:url(\"" + image_path + "\")");
  } else if (file_exists(alt_path)) {
    this->setText("");
    if (file_exists(hover_path)) {
      this->setStyleSheet("QPushButton {border-image:url(\"" + alt_path +
                          "\");}"
                          "QPushButton:hover {border-image:url(\"" +
                          hover_path + "\");}");
    } else
      this->setStyleSheet("border-image:url(\"" + alt_path + "\")");
  } else {
    this->setText(ao_app->get_emote_comment(p_char, p_emote));
    this->setStyleSheet("border-image:url(\"\")");
  }
}

void AOEmoteButton::on_clicked() { emote_clicked(m_id); }
