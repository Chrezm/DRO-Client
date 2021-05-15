#ifndef AOEVIDENCEDISPLAY_H
#define AOEVIDENCEDISPLAY_H

#include <QLabel>
#include <QMovie>

#include "aoapplication.h"
#include "aopixmap.h"
#include "aosfxplayer.h"

class AOEvidenceDisplay : public QLabel
{
  Q_OBJECT

public:
  AOEvidenceDisplay(QWidget *p_parent, AOApplication *p_ao_app);

  void show_evidence(QString p_evidence_image, bool is_left_side);
  void reset();

private:
  AOApplication *ao_app = nullptr;
  QMovie *evidence_movie = nullptr;
  QLabel *evidence_icon = nullptr;
  AOSfxPlayer *sfx_player = nullptr;

private slots:
  void frame_change(int p_frame);
};

#endif // AOEVIDENCEDISPLAY_H
