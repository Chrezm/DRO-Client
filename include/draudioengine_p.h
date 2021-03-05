#pragma once

#include "draudio.h"
#include "draudiostreamfamily.h"

#include <QFlags>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QTimer>

#include <optional>

class DRAudioEngine;
class DRAudioEngineData;

class DRAudioEnginePrivate : public QObject
{
  Q_OBJECT

public:
  using ptr = QPointer<DRAudioEnginePrivate>;

  DRAudioEnginePrivate();
  ~DRAudioEnginePrivate();

  void invoke_signal(QString p_method_name, QGenericArgument p_arg1 = QGenericArgument(nullptr));

private:
  friend class DRAudioEngine;
  friend class DRAudioEngineData;

  void update_device();
  void update_device_list();
  void update_options();
  void update_volume();

  void check_stream_error();

  QObjectList children;
  QPointer<DRAudioEngine> engine;

  QPointer<QTimer> event_timer;
  bool check_device = false;

  std::optional<DRAudioDevice> device;
  std::optional<DRAudioDevice> previous_device;
  std::optional<DRAudioDevice> favorite_device;

  int32_t volume = 0;
  DRAudio::Options options;

  QMap<QString, DRAudioDevice> device_map;
  QMap<DRAudio::Family, DRAudioStreamFamily::ptr> family_map;

private slots:
  void on_event_tick();
};