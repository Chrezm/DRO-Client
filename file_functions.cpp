#include <QDir>
#include <QFileInfo>

#include "file_functions.h"

bool file_exists(QString file_path)
{
  QFileInfo check_file(file_path);

  return check_file.exists() && check_file.isFile();
}

QString file_exists(QString file_path, QVector<QString> p_exts)
{
  for (auto &ext : p_exts)
  {
    if (file_exists(file_path + ext))
      return ext;
  }
  return "";
}

bool dir_exists(QString dir_path)
{
  QDir check_dir(dir_path);

  return check_dir.exists();
}
