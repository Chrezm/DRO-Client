#define NOMINMAX

#include "draudiostreamfamily.h"

#include "draudioengine.h"

#include <QDebug>

DRAudioStreamFamily::DRAudioStreamFamily(DRAudio::Family p_family) : m_family(p_family)
{
}

int32_t DRAudioStreamFamily::get_capacity() const
{
  return m_capacity;
}

DRAudio::Options DRAudioStreamFamily::get_options() const
{
  return m_options;
}

bool DRAudioStreamFamily::is_suppressed() const
{
  return m_options.testFlag(DRAudio::OSuppressed);
}

bool DRAudioStreamFamily::is_ignore_suppression() const
{
  return m_options.testFlag(DRAudio::OIgnoreSuppression);
}

void DRAudioStreamFamily::set_capacity(int32_t p_capacity)
{
  p_capacity = std::max(p_capacity, 0);

  if (m_capacity == p_capacity)
    return;

  m_capacity = p_capacity;
  update_capacity();
  Q_EMIT capacity_changed(m_capacity);
}

void DRAudioStreamFamily::set_options(DRAudio::Options p_options)
{
  if (m_options == p_options)
    return;

  m_options = p_options;
  update_options();
  Q_EMIT options_changed(m_options);
}

void DRAudioStreamFamily::set_volume(int32_t p_volume)
{
  p_volume = std::clamp(p_volume, 0, 100);

  if (m_volume == p_volume)
    return;

  m_volume = p_volume;
  update_volume();
  Q_EMIT volume_changed(m_volume);
}

void DRAudioStreamFamily::set_suppressed(bool p_enabled)
{
  DRAudio::Options options = m_options;
  options.setFlag(DRAudio::OSuppressed, p_enabled);
  set_options(options);
}

void DRAudioStreamFamily::set_ignore_suppression(bool p_enabled)
{
  DRAudio::Options options = m_options;
  options.setFlag(DRAudio::OIgnoreSuppression, p_enabled);
  set_options(options);
}

std::optional<DRAudioStream::ptr> DRAudioStreamFamily::create_stream(QString p_file)
{
  DRAudioStream::ptr stream(new DRAudioStream(m_family));

  if (auto err = stream->set_file(p_file); err)
  {
    qWarning() << err->what();
    return std::nullopt;
  }

  m_stream_list.append(stream);
  update_capacity();
  stream->set_volume(calculate_volume());
  connect(stream.get(), SIGNAL(finished()), this, SLOT(on_stream_finished()));

  return stream;
}

std::optional<DRAudioStream::ptr> DRAudioStreamFamily::play_stream(QString p_file)
{
  std::optional<DRAudioStream::ptr> r_stream = create_stream(p_file);
  if (r_stream.has_value())
  {
    auto stream = r_stream.value();
    qWarning() << "playing" << stream->get_file().value();
    stream->play();
  }
  return r_stream;
}

DRAudioStreamFamily::stream_list DRAudioStreamFamily::get_stream_list() const
{
  return m_stream_list;
}

float DRAudioStreamFamily::calculate_volume()
{
  float volume = float(m_volume) * 0.01f;

  if (!is_ignore_suppression() && (is_suppressed() || DRAudioEngine::is_suppressed()))
  {
    volume = 0;
  }
  else
  {
    // master volume adjustment
    volume *= (float(DRAudioEngine::get_volume()) * 0.01f);
  }

  return volume * 100.0f;
}

void DRAudioStreamFamily::update_device()
{
  for (DRAudioStream::ptr i_stream : qAsConst(m_stream_list))
    i_stream->update_device();
}

void DRAudioStreamFamily::update_capacity()
{
  if (m_capacity == 0)
    return;
  while (m_capacity < m_stream_list.length())
    m_stream_list.removeFirst();
}

void DRAudioStreamFamily::update_options()
{
  // suppressed
  update_volume();
}

void DRAudioStreamFamily::update_volume()
{
  const float volume = calculate_volume();
  for (auto &stream : m_stream_list)
    stream->set_volume(volume);
}

void DRAudioStreamFamily::on_stream_finished()
{
  DRAudioStream *invoker = dynamic_cast<DRAudioStream *>(sender());
  if (invoker == nullptr)
    return;

  if (auto file = invoker->get_file(); file)
    qInfo() << "removing" << file.value();
  else
    qWarning() << "removing unspecified stream";

  stream_list new_stream_list;
  for (auto &i_stream : m_stream_list)
  {
    if (i_stream.get() == invoker)
      continue;
    new_stream_list.append(std::move(i_stream));
  }
  m_stream_list = std::move(new_stream_list);
}
