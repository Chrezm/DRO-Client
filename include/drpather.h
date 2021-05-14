#pragma once

#include <QString>

class DRPather
{
public:
  /* @brief Gets the directory containing the base folder and the application.
   *
   * This function is a system-independent QDir::currentPath() system-independent. The only edge case really is MacOS,
   * where due to the way applications are bundled, makes QDir::currentPath() return nonsensical values by default and
   * QCoreApplication::applicationDirPath() returns the directory to the internal executable of the bundle rather
   * than the bundle location itself.
   *
   * @return Directory.
   */
  static QString get_application_path();
};
