#ifndef AOSHOUTPLAYER_HPP
#define AOSHOUTPLAYER_HPP

#include "aoobject.h"

class AOShoutPlayer : public AOObject
{
  Q_OBJECT

public:
  AOShoutPlayer(AOApplication *p_ao_app, QObject *p_parent = nullptr);

  void play(QString character, QString shout);
};

#endif // AOSHOUTPLAYER_HPP
