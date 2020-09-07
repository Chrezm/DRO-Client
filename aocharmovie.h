#ifndef AOCHARMOVIE_H
#define AOCHARMOVIE_H

#include "aopixmap.h"

#include <QLabel>
#include <QMovie>
#include <QTimer>

class AOApplication;

class AOCharMovie : public QLabel
{
  Q_OBJECT

public:
  AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app);

  void play(QString p_char, QString p_emote, QString emote_prefix, bool show);
  bool play_pre(QString p_char, QString p_emote, bool show);
  void play_talking(QString p_char, QString p_emote, bool show);
  void play_idle(QString p_char, QString p_emote, bool show);
  void set_mirror_enabled(bool p_enable);
  void combo_resize(QSize p_size);
  void stop();

private:
  AOApplication *ao_app = nullptr;

  QMovie *m_reader;
  QVector<QImage> movie_frames;
  QTimer *m_frame_timer;

  bool m_mirror = false;
  bool m_play_once = false;

signals:
  void done();

private slots:
  void on_frame_changed(int n_frame);
  void timer_done();
};

#endif // AOCHARMOVIE_H
