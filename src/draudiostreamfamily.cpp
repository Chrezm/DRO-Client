#include "draudiostreamfamily.h"

#include "draudioengine.h"

#include <QDebug>

DRAudioStreamFamily::DRAudioStreamFamily(DRAudio::Family p_family) : m_family(p_family)
{}

int32_t DRAudioStreamFamily::get_capacity()
{
  return m_capacity;
}

DRAudio::Options DRAudioStreamFamily::get_options()
{
  return m_options;
}

bool DRAudioStreamFamily::is_suppressed()
{
  return m_options.testFlag(DRAudio::OSuppressed);
}

bool DRAudioStreamFamily::is_ignore_suppression()
{
  return m_options.testFlag(DRAudio::OIgnoreSuppression);
}

DRAudioStreamFamily::iterator DRAudioStreamFamily::begin()
{
  return m_stream_list.begin();
}

DRAudioStreamFamily::iterator DRAudioStreamFamily::end()
{
  return m_stream_list.end();
}

void DRAudioStreamFamily::set_capacity(int32_t p_capacity)
{
  p_capacity = std::max(p_capacity, 0);

  if (m_capacity == p_capacity)
    return;

  m_capacity = p_capacity;
  adjust_capacity();
  Q_EMIT capacity_changed(m_capacity);
}

void DRAudioStreamFamily::set_options(DRAudio::Options p_options)
{
  if (m_options == p_options)
    return;

  m_options = p_options;
  adjust_options();
  Q_EMIT options_changed(m_options);
}

void DRAudioStreamFamily::set_volume(int32_t p_volume)
{
  p_volume = std::clamp(p_volume, 0, 100);

  if (m_volume == p_volume)
    return;

  m_volume = p_volume;
  adjust_volume();
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

std::optional<DRAudioStreamFamily::stream_ptr> DRAudioStreamFamily::create_stream(QString p_file)
{
  stream_ptr stream(new DRAudioStream(m_family));

  if (auto err = stream->set_file(p_file); err)
  {
    qWarning() << err->get_error();
    return std::nullopt;
  }

  m_stream_list.append(stream);
  adjust_capacity();

  stream->set_volume(calculate_volume());
  connect(stream.get(), SIGNAL(finished()), this, SLOT(on_stream_stopped()));

  return stream;
}

std::optional<DRAudioStreamFamily::stream_ptr> DRAudioStreamFamily::play_stream(QString p_file)
{
  std::optional<stream_ptr> r_stream = create_stream(p_file);
  if (r_stream.has_value())
  {
    auto stream = r_stream.value();
    qWarning() << "playing" << stream->get_file().value();
    stream->play();
  }
  return r_stream;
}

QVector<DRAudioStreamFamily::stream_ptr> DRAudioStreamFamily::get_stream_list()
{
  return m_stream_list;
}

int32_t DRAudioStreamFamily::calculate_volume()
{
  float volume = float(m_volume) * 0.01f;

  if (!is_ignore_suppression() && (is_suppressed() || DRAudioEngine::is_suppressed()))
  {
    volume = 0;
  }
  else
  {
    // master volume adjustment
    volume = volume * (float(DRAudioEngine::get_volume()) * 0.01f);
  }

  return volume * 100.f;
}

void DRAudioStreamFamily::adjust_capacity()
{
  if (m_capacity == 0)
    return;
  while (m_capacity < m_stream_list.length())
    m_stream_list.removeFirst();
}

void DRAudioStreamFamily::adjust_options()
{
  // suppressed
  adjust_volume();
}

void DRAudioStreamFamily::adjust_volume()
{
  const float volume = calculate_volume();
  for (auto &stream : m_stream_list)
    stream->set_volume(volume);
}

void DRAudioStreamFamily::on_stream_stopped()
{
  DRAudioStream *invoker = dynamic_cast<DRAudioStream *>(sender());
  if (invoker == nullptr)
    return;

  decltype(m_stream_list) new_stream_list;
  for (auto &i_stream : m_stream_list)
  {
    if (i_stream.get() == invoker)
      continue;
    new_stream_list.append(std::move(i_stream));
  }
  m_stream_list = std::move(new_stream_list);
}