#include "aocharmovie.h"

#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"
#include "misc_functions.h"

#include <QDebug>
#include <QImageReader>

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app)
    : QLabel(p_parent)
{
  ao_app = p_ao_app;

  m_reader = new QMovie(this);

  m_frame_timer = new QTimer(this);
  m_frame_timer->setSingleShot(true);

  connect(m_reader, SIGNAL(frameChanged(int)), this,
          SLOT(on_frame_changed(int)));
  connect(m_frame_timer, SIGNAL(timeout()), this, SLOT(timer_done()));
}

void AOCharMovie::play(QString p_char, QString p_emote, QString p_emote_prefix,
                       bool p_visible)
{
  QString target_path;
  QStringList f_paths{
      ao_app->get_character_path(p_char, p_emote_prefix + p_emote), // .gif
      ao_app->get_character_path(p_char, p_emote),                  // .png
      ao_app->get_theme_variant_path("placeholder"),                // .gif
      ao_app->get_theme_path("placeholder"),                        // .gif
      ao_app->get_default_theme_path("placeholder")                 // .gif
  };

  for (auto &f_file : f_paths)
  {
    bool found = false;
    for (auto &ext : QStringList{".webp", ".apng", ".gif", ".png"})
    {
      QString fullPath = f_file + ext;
      found = file_exists(fullPath);
      if (found)
      {
        target_path = fullPath;
        break;
      }
    }

    if (found)
      break;
  }

  show();
  if (!p_visible)
    hide();

  movie_frames.clear();
  QImageReader *reader = new QImageReader(target_path);
  for (int i = 0; i < reader->imageCount(); ++i)
  { // optimize can be better, but I'll just keep it like that for now
    QImage f_image = reader->read();
    if (m_mirror)
      f_image = f_image.mirrored(true, false);
    movie_frames.append(f_image);
  }
  delete reader;

  m_reader->stop();
  this->clear();
  m_reader->setFileName(target_path);
  m_reader->start();
}

bool AOCharMovie::play_pre(QString p_char, QString p_emote, bool show)
{
  QString f_file_path = ao_app->get_character_path(p_char, p_emote);
  bool f_file_exist = false;

  { // figure out what extension the animation is using
    QString f_source_path = ao_app->get_character_path(p_char, p_emote);
    for (QString &i_ext : QStringList{".webp", ".apng", ".gif", ".png"})
    {
      QString f_target_path = f_source_path + i_ext;
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
    m_reader->stop();
    this->clear();
    m_play_once = true;
    m_reader->setFileName(f_file_path);
    play(p_char, p_emote, "", show);
  }

  return f_file_exist;
}

void AOCharMovie::play_talking(QString p_char, QString p_emote, bool p_visible)
{
  QString gif_path = ao_app->get_character_path(p_char, "(b)" + p_emote);

  m_reader->stop();
  this->clear();
  m_play_once = false;
  m_reader->setFileName(gif_path);
  play(p_char, p_emote, "(b)", p_visible);
}

void AOCharMovie::play_idle(QString p_char, QString p_emote, bool p_visible)
{
  QString gif_path = ao_app->get_character_path(p_char, "(a)" + p_emote);

  this->clear();
  m_reader->stop();
  m_play_once = false;
  m_reader->setFileName(gif_path);
  play(p_char, p_emote, "(a)", p_visible);
}

void AOCharMovie::set_mirror_enabled(bool p_enable)
{
  m_mirror = p_enable;
}

void AOCharMovie::stop()
{
  // for all intents and purposes, stopping is the same as hiding. at no point
  // do we want a frozen gif to display
  m_reader->stop();
  m_frame_timer->stop();
  this->hide();
}

void AOCharMovie::combo_resize(QSize p_size)
{
  resize(p_size);
  m_reader->stop();
  m_frame_timer->stop();
  m_reader->setScaledSize(p_size);
  m_reader->start();
}

void AOCharMovie::on_frame_changed(int p_frame_num)
{
  if (movie_frames.size() > p_frame_num)
  {
    AOPixmap f_pixmap = QPixmap::fromImage(movie_frames.at(p_frame_num));
    this->setPixmap(f_pixmap.scale_to_size(this->size()));
  }

  // pre-anim only
  if (m_play_once)
  {
    int f_frame_count = m_reader->frameCount();
    if (f_frame_count == 0 || p_frame_num == (f_frame_count - 1))
    {
      int f_frame_delay = m_reader->nextFrameDelay();
      if (f_frame_delay < 0)
        f_frame_delay = 0;
      m_frame_timer->start(f_frame_delay);
      m_reader->stop();
    }
  }
}

void AOCharMovie::timer_done()
{
  done();
}
