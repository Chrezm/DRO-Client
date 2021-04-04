#include "draudiostream.h"

#include "draudioengine.h"
#include "draudiostreamfamily.h"

#include <QDebug>
#include <QFileInfo>

DRAudioStream::DRAudioStream(DRAudio::Family p_family) : m_family(p_family)
{
  connect(this, &DRAudioStream::device_error, this, &DRAudioStream::on_device_error, Qt::QueuedConnection);
}

DRAudioStream::~DRAudioStream()
{
  if (m_hstream.has_value())
  {
    const HSTREAM hstream = m_hstream.value();

    while (!m_hsync_stack.empty())
    {
      BASS_ChannelRemoveSync(hstream, m_hsync_stack.top().sync);
      m_hsync_stack.pop();
    }

    BASS_StreamFree(hstream);
  }
}

DRAudio::Family DRAudioStream::get_family() const
{
  return m_family;
}

std::optional<QString> DRAudioStream::get_file() const
{
  return m_file;
}

bool DRAudioStream::is_playing() const
{
  if (!m_hstream.has_value())
    return false;
  return BASS_ChannelIsActive(m_hstream.value()) == BASS_ACTIVE_PLAYING;
}

void DRAudioStream::play()
{
  if (!m_hstream.has_value())
    return;
  const BOOL result = BASS_ChannelPlay(m_hstream.value(), FALSE);
  if (result == FALSE)
  {
    qWarning() << DRAudioError(
                      QString("failed to play file %1: %2").arg(m_file.value(), DRAudio::get_last_bass_error()))
                      .what();
    Q_EMIT finished();
  }
}

void DRAudioStream::stop()
{
  if (!m_hstream.has_value())
    return;
  BASS_ChannelStop(m_hstream.value());
  Q_EMIT finished();
}

std::optional<DRAudioError> DRAudioStream::set_file(QString p_file)
{
  if (m_file.has_value())
    return DRAudioError("file already set");

  const QFileInfo file(p_file);
  if (!file.exists())
    return DRAudioError(QString("file does not exist: %1").arg(p_file.isEmpty() ? "<empty>" : p_file));

  if (m_file == p_file)
    return std::nullopt;

  m_file = p_file;

  HSTREAM stream_handle = BASS_StreamCreateFile(FALSE, m_file->utf16(), 0, 0, BASS_UNICODE | BASS_ASYNCFILE);
  if (stream_handle == 0)
    return DRAudioError(QString("failed to create stream for file %1: %2").arg(p_file, DRAudio::get_last_bass_error()));
  m_hstream = stream_handle;

  // bass events
  for (const DWORD type : {BASS_SYNC_END, BASS_SYNC_DEV_FAIL})
  {
    const DWORD final_type = type == BASS_SYNC_DEV_FAIL ? type | BASS_SYNC_MIXTIME : type;
    HSYNC sync_handle = BASS_ChannelSetSync(stream_handle, final_type, 0, &DRAudioStream::on_sync_callback, this);
    if (sync_handle == 0)
      return DRAudioError(
          QString("failed to create sync %1 for file %2: %3").arg(type).arg(p_file, DRAudio::get_last_bass_error()));
    m_hsync_stack.push(DRAudioStreamSync{sync_handle, type});
  }

  Q_EMIT file_changed(p_file);
  return std::nullopt;
}

void DRAudioStream::set_volume(float p_volume)
{
  if (!m_hstream.has_value())
    return;
  m_volume = p_volume;
  BASS_ChannelSetAttribute(m_hstream.value(), BASS_ATTRIB_VOL, float(p_volume) * 0.01f);
}

#include <QTimer>

void DRAudioStream::on_sync_callback(HSYNC hsync, DWORD ch, DWORD data, void *userdata)
{
  Q_UNUSED(hsync)
  Q_UNUSED(ch)
  Q_UNUSED(data)

  /*
   * BASS only provides what we fed it when we first created our synch even if
   * the pointer we provided get deleted mid-way through the program lifetime
   */
  DRAudioStream *self = static_cast<DRAudioStream *>(userdata);
  for (auto &v : self->m_hsync_stack)
  {
    if (v.sync != hsync)
      continue;

    switch (v.type)
    {
    case BASS_SYNC_END:
      Q_EMIT self->finished();
      break;

    case BASS_SYNC_DEV_FAIL:
      Q_EMIT self->device_error(QPrivateSignal());
      break;
    }
  }
}

void DRAudioStream::cache_position()
{
  if (!m_hstream.has_value())
    return;
  if (m_position.has_value())
    return;
  m_position = BASS_ChannelGetPosition(m_hstream.value(), BASS_POS_BYTE);
  BASS_ChannelStop(m_hstream.value());
}

void DRAudioStream::on_device_error()
{
  cache_position();
  DRAudioEngine::check_stream_error();
}

void DRAudioStream::update_device()
{
  if (!m_hstream.has_value())
  {
    Q_EMIT finished();
    return;
  }

  if (is_playing())
    return;
  const QString file = m_file.value();

  m_file.reset();
  m_hstream.reset();
  m_hsync_stack.clear();

  blockSignals(true);
  set_file(file);
  set_volume(m_volume);
  blockSignals(false);

  if (m_position.has_value())
  {
    if (BASS_ChannelSetPosition(m_hstream.value(), m_position.value(), BASS_POS_BYTE) == FALSE)
      qWarning() << DRAudioError(QString("failed to set position for %1: %2").arg(file, DRAudio::get_last_bass_error()))
                        .what();
    m_position.reset();
  }

  play();
}
