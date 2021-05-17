#pragma once

// pdir
#include "aoapplication.h"
#include "aoobject.h"
#include "draudioengine.h"

// 3rd
#include <bass.h>

// qt
#include <QDebug>
#include <QWidget>

// std
#include <optional>

const int BLIP_COUNT = 5;

class AOBlipPlayer : public AOObject
{
  Q_OBJECT

public:
  AOBlipPlayer(AOApplication *p_ao_app, QObject *p_parent = nullptr);

public slots:
  void set_blips(QString p_sfx);
  void blip_tick();

private:
  DRAudioStreamFamily::ptr m_family;
  std::optional<QString> m_name;
  std::optional<QString> m_file;
};
