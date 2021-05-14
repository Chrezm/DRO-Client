#include "file_functions.h"

#include <QDir>
#include <QFileInfo>

QStringList animated_or_static_extensions()
{
  return QStringList{".webp", ".apng", ".gif", ".png"};
}

QStringList animated_extensions()
{
  return QStringList{".webp", ".apng", ".gif"};
}

QStringList audio_extensions()
{
  return QStringList{"", ".wav", ".ogg", ".opus", ".mp3"};
}

bool file_exists(QString file_path)
{
  QFileInfo check_file(file_path);
  return check_file.exists() && check_file.isFile();
}

bool dir_exists(QString dir_path)
{
  QDir check_dir(dir_path);
  return check_dir.exists();
}
