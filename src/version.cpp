#include "version.h"

int get_release_version()
{
  return 1;
}

int get_major_version()
{
  return 0;
}

int get_minor_version()
{
  return 0;
}

QString get_version_string()
{
  return QString::number(get_release_version()) + "." + QString::number(get_major_version()) + "." +
         QString::number(get_minor_version());
}
