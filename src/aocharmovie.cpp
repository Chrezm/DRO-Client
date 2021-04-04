#include "aocharmovie.h"

#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"
#include "misc_functions.h"

#include <QDebug>
#include <QImageReader>

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  // center even images, not just text
  setAlignment(Qt::AlignCenter);

  m_movie = new QMovie(this);

  m_frame_timer = new QTimer(this);
  m_frame_timer->setSingleShot(true);

  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(on_frame_changed(int)));
  connect(m_frame_timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
}

void AOCharMovie::play(QString p_chr, QString p_emote, QString p_emote_prefix, bool p_play_once)
{
  // Asset lookup order
  // 1. In the character folder, look for
  // `p_emote_prefix+p_emote` + extensions in `exts` in order
  // 2. In the character folder, look for
  // `p_emote` + extensions in `exts` in order
  // 3. In the theme folder (gamemode-timeofday/main/default), look for
  // "placeholder" + extensions in `exts` in order

  QString target_path = ao_app->find_asset_path(
      {
          ao_app->get_character_path(p_chr, p_emote_prefix + p_emote),
          ao_app->get_character_path(p_chr, p_emote),
      },
      animated_or_static_extensions());
  if (target_path.isEmpty())
    target_path = ao_app->find_theme_asset_path("placeholder", animated_extensions());

  stop();
  m_movie->setFileName(target_path);
  m_play_once = p_play_once;
  m_movie->start();
}

void AOCharMovie::play(QString p_chr, QString p_emote, bool p_play_once)
{
  play(p_chr, p_emote, QString(), p_play_once);
}

bool AOCharMovie::play_pre(QString p_chr, QString p_emote)
{
  QString f_file_path = ao_app->get_character_path(p_chr, p_emote);
  bool f_file_exist = false;

  { // figure out what extension the animation is using
    QString f_source_path = ao_app->get_character_path(p_chr, p_emote);
    for (QString &i_ext : animated_or_static_extensions())
    {
      QString f_target_path = ao_app->get_case_sensitive_path(f_source_path + i_ext);
      if (file_exists(f_target_path))
      {
        f_file_path = f_target_path;
        f_file_exist = true;
        break;
      }
    }
  }

  // play if it exist
  if (f_file_exist)
  {
    play(p_chr, p_emote, true);
  }

  return f_file_exist;
}

void AOCharMovie::play_talking(QString p_chr, QString p_emote)
{
  play(p_chr, p_emote, "(b)", false);
}

void AOCharMovie::play_idle(QString p_chr, QString p_emote)
{
  play(p_chr, p_emote, "(a)", false);
}

void AOCharMovie::set_mirror_enabled(bool p_enabled)
{
  m_mirror = p_enabled;
}

void AOCharMovie::stop()
{
  // for all intents and purposes, stopping is the same as hiding. at no point
  // do we want a frozen gif to display
  m_movie->stop();
  m_frame_timer->stop();
  m_current_frame = QImage();
  paint_frame();
}

void AOCharMovie::combo_resize(QSize p_size)
{
  resize(p_size);
  paint_frame();
}

void AOCharMovie::paint_frame()
{
  AOPixmap f_pixmap(QPixmap::fromImage(m_movie->currentImage().mirrored(m_mirror, false)));
  this->setPixmap(f_pixmap.scale_to_height(this->size()));
}

void AOCharMovie::on_frame_changed(int p_frame_num)
{
  m_current_frame = m_movie->currentImage();

  paint_frame();

  if (m_play_once)
  {
    const int f_frame_count = m_movie->frameCount();
    if (f_frame_count == 0 || p_frame_num == (f_frame_count - 1))
    {
      int f_frame_delay = m_movie->nextFrameDelay();
      if (f_frame_delay < 0)
        f_frame_delay = 0;
      m_movie->stop();
      m_frame_timer->start(f_frame_delay);
    }
  }
}

void AOCharMovie::on_timer_timeout()
{
  Q_EMIT done();
}
