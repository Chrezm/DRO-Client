#ifndef AOCHARMOVIE_H
#define AOCHARMOVIE_H

#include <QMovie>
#include <QLabel>
#include <QTimer>

#include "aopixmap.h"

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

  void set_flipped(bool p_flipped) {m_flipped = p_flipped;}

  void stop();

  void combo_resize(int w, int h);

private:
  AOApplication *ao_app = nullptr;

  QMovie *m_movie;
  QVector<QImage> movie_frames;
  QTimer *m_frame_timer;

  const int time_mod = 62;

  bool m_flipped = false;

  bool play_once = true;

signals:
  void done();

private slots:
  void on_frame_changed(int n_frame);
  void timer_done();
};

#endif // AOCHARMOVIE_H
