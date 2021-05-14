#pragma once

#include "aoobject.h"

class AOSfxPlayer : public AOObject
{
  Q_OBJECT

public:
  AOSfxPlayer(AOApplication *p_ao_app, QObject *p_parent = nullptr);

  void play(QString p_file);
  void play_effect(QString p_effect);
  void play_character_effect(QString p_character, QString p_effect);
  void stop_all();
};
