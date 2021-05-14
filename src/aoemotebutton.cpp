#include "aoemotebutton.h"

#include "file_functions.h"
#include <QDebug>

AOEmoteButton::AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y) : QPushButton(p_parent)
{
  ao_app = p_ao_app;

  this->move(p_x, p_y);
  this->resize(40, 40);

  w_selected = new QLabel(this);
  w_selected->resize(size());
  w_selected->setAttribute(Qt::WA_TransparentForMouseEvents);
  w_selected->hide();

  connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
}

void AOEmoteButton::set_emote_number(int p_emote_number)
{
  m_emote_number = p_emote_number;
}

int AOEmoteButton::get_emote_number()
{
  return m_emote_number;
}

void AOEmoteButton::set_image(DREmote p_emote, bool p_enabled)
{
  QString texture_path =
      ao_app->get_character_path(p_emote.character, QString("emotions/button%1_off.png").arg(p_emote.key));

  // reset states
  w_selected->hide();

  // nested ifs are okay
  if (p_enabled)
  {
    const QString enabled_texture_path =
        ao_app->get_character_path(p_emote.character, QString("emotions/button%1_on.png").arg(p_emote.key));

    if (file_exists(enabled_texture_path))
    {
      texture_path = enabled_texture_path;
    }
    else
    {
      const QString selected_texture_path = ao_app->get_character_path(p_emote.character, "emotions/selected.png");

      if (file_exists(selected_texture_path))
      {
        w_selected->setStyleSheet(QString("border-image: url(\"%1\")").arg(selected_texture_path));
      }
      else
      {
        w_selected->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, "
                                  "y2:1, stop:0 rgba(0, 0, 0, 0), stop:1 rgba(0, 0, 0, 127)); }");
      }

      w_selected->show();
    }
  }

  const bool texture_exist = file_exists(texture_path);
  setText(texture_exist ? nullptr : p_emote.comment);
  setStyleSheet(texture_exist
                    ? QString("%1 { border-image: url(\"%2\"); }").arg(metaObject()->className(), texture_path)
                    : QString());
}

void AOEmoteButton::on_clicked()
{
  emote_clicked(m_emote_number);
}
