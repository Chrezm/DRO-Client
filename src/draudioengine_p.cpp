#include "draudioengine_p.h"

/*!
    I could've avoided making this necessary but I didn't
    want to make the code extra complex.

    So here, instead have this very convuloted workaround
    instead.
*/
#include "draudioengine.h"

#include <QDebug>
#include <QGuiApplication>

static const int EVENT_TIMER_INTERVAL_DEFAULT = 100;

DRAudioEnginePrivate::DRAudioEnginePrivate() : QObject(nullptr), event_timer(new QTimer)
{
}

DRAudioEnginePrivate::~DRAudioEnginePrivate()
{
  event_timer->stop();

  BASS_Free();
}

void DRAudioEnginePrivate::invoke_signal(QString p_method_name, QGenericArgument p_arg1)
{
  // Does this do anything? children seems to always be an empty QObjectList
  for (QObject *i_child : children)
  {
    QMetaObject::invokeMethod(i_child, p_method_name.toStdString().c_str(), p_arg1);
  }
}

void DRAudioEnginePrivate::update_device()
{
  // cache the stream's position before we make the switch
  for (DRAudioStreamFamily::ptr &i_family : family_map)
    for (DRAudioStream::ptr &i_stream : i_family->get_stream_list())
      i_stream->cache_position();

  if (previous_device.has_value())
  {
    qInfo().noquote() << QString("Shutting down current device: %1").arg(previous_device->get_name());
    if (BASS_Free() == FALSE)
      qCritical() << DRAudioError(QString("Failed to free: %1").arg(DRAudio::get_last_bass_error())).what();
    previous_device.reset();
  }

  if (!device.has_value())
  {
    qCritical() << DRAudioError("Failed to initialize, device not set!").what();
    return;
  }

  qInfo().noquote() << QString("Initializing device: %1").arg(device->get_name());
  if (BASS_Init(device->get_id().value_or(0), 48000, BASS_DEVICE_LATENCY, 0, NULL) == FALSE)
  {
    qCritical() << DRAudioError(QString("Failed to initialize: %1").arg(DRAudio::get_last_bass_error())).what();
    return;
  }

  qInfo() << "Switching to initiliazed device";
  if (BASS_SetDevice(device->get_id().value_or(0)) == FALSE)
  {
    qCritical() << DRAudioError(QString("Failed to set device: %1").arg(DRAudio::get_last_bass_error())).what();
    return;
  }

  for (DRAudioStreamFamily::ptr &i_family : family_map)
    i_family->update_device();
}

void DRAudioEnginePrivate::update_device_list()
{
  QVector<DRAudioDevice> updated_device_list;

  {
    BASS_DEVICEINFO device_info;
    for (int device = 1; BASS_GetDeviceInfo(device, &device_info); ++device)
      updated_device_list.append(DRAudioDevice(device, device_info));
  }

  bool is_changed = false;
  for (DRAudioDevice &device : updated_device_list)
  {
    if (device_map.contains(device.get_driver()))
    {
      if (device_map[device.get_driver()].merge(device))
        is_changed = true;
      continue;
    }

    device_map.insert(device.get_driver(), device);
    is_changed = true;
  }

  if (!is_changed)
    return;
  check_device = true;
  Q_EMIT invoke_signal("device_list_changed", Q_ARG(QList<DRAudioDevice>, device_map.values()));
}

void DRAudioEnginePrivate::update_options()
{
  update_volume();
}

void DRAudioEnginePrivate::update_volume()
{
  for (auto &i_group : family_map)
    i_group->update_volume();
}

void DRAudioEnginePrivate::check_stream_error()
{
  event_timer->start(EVENT_TIMER_INTERVAL_DEFAULT);
  check_device = true;
}

void DRAudioEnginePrivate::on_event_tick()
{
  update_device_list();

  if (device.has_value() && !check_device)
    return;
  check_device = false;

  std::optional<DRAudioDevice> target_device;
  for (DRAudioDevice &i_device : device_map)
  {
    if (!i_device.is_enabled())
      continue;

    if (favorite_device && favorite_device.value().get_driver() == i_device.get_driver())
    {
      // if our favorite device doesn't have an id then
      // it was most likely set through the driver
      // in which case we need to update it
      if (!favorite_device->get_id().has_value())
      {
        favorite_device.reset();
        engine->set_favorite_device(i_device);
      }

      target_device = i_device;
      break;
    }

    if (!target_device.has_value() || i_device.is_default())
      target_device = i_device;
  }

  if (!target_device.has_value())
  {
#ifdef QT_DEBUG
    qWarning().noquote() << DRAudioError(QString("NO DEVICE AVAILABLE!")).what();
#endif
    return;
  }
  engine->set_device(target_device.value());
}
