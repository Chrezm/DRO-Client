#include "drpather.h"

#include <QCoreApplication>
#include <QDir>

/* @brief Gets the directory containing the base folder and the application.
 *
 * This function is a system-independent QDir::currentPath() system-independent. The only edge case really is MacOS,
 * where due to the way applications are bundled, makes QDir::currentPath() return nonsensical values by default and
 * QCoreApplication::applicationDirPath() returns the directory to the internal executable of the bundle rather
 * than the bundle location itself.
 *
 * @return Directory.
 */
QString DRPather::get_application_path()
{
#ifdef Q_OS_MACOS
  QDir l_mac_path(QCoreApplication::applicationDirPath());
  for (int i = 0; i < 3; ++i) // equivalent of "/../../.."
    l_mac_path.cdUp();
  return l_mac_path.canonicalPath();
#else
  return QDir::currentPath();
#endif
}
