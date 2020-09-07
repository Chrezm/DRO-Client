#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#include "aoabstractplayer.h"

class AOSfxPlayer : public AOAbstractPlayer
{
  Q_OBJECT

public:
  AOSfxPlayer(QObject *p_parent, AOApplication *p_ao_app);

  void play(QString p_file);
  void stop();
};

#endif // AOSFXPLAYER_H
