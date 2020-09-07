#include "aoapplication.h"
#include "courtroom.h"
#include "datatypes.h"
#include "debug_functions.h"
#include "lobby.h"
#include "networkmanager.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <cstdio>

int main(int argc, char *argv[])
{
#if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
  // High-DPI support is for Qt version >=5.6.
  // However, many Linux distros still haven't brought their stable/LTS
  // packages up to Qt 5.6, so this is conditional.
  AOApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
  AOApplication app(argc, argv);

  QPluginLoader apng("qapng");
  if (!apng.load())
  {
#ifdef QT_NO_DEBUG
    call_error("APNG plugin could not be loaded.");
#endif
  }

  app.construct_lobby();
#ifdef QT_NO_DEBUG
  app.net_manager->connect_to_master();
#endif
  app.w_lobby->show();

  return app.exec();
}
