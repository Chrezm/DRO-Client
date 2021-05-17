#include "aoapplication.h"

#include "courtroom.h"
#include "debug_functions.h"
#include "lobby.h"
#include "networkmanager.h"

#include "aoconfig.h"
#include "aoconfigpanel.h"

#include <QDebug>
#include <QFileInfo>
#include <QRect>
#include <QRegularExpression>

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
#include <QDesktopWidget>
#else
#include <QScreen>
#endif

AOApplication::AOApplication(int &argc, char **argv) : QApplication(argc, argv)
{
  net_manager = new NetworkManager(this);
  connect(net_manager, SIGNAL(ms_connect_finished(bool, bool)), SLOT(ms_connect_finished(bool, bool)));

  ao_config = new AOConfig(this);
  connect(ao_config, SIGNAL(theme_changed(QString)), this, SLOT(on_config_theme_changed()));
  connect(ao_config, SIGNAL(gamemode_changed(QString)), this, SLOT(on_config_gamemode_changed()));
  connect(ao_config, SIGNAL(timeofday_changed(QString)), this, SLOT(on_config_timeofday_changed()));

  ao_config_panel = new AOConfigPanel(this);
  connect(ao_config_panel, SIGNAL(reload_theme()), this, SLOT(on_config_reload_theme_requested()));
  connect(this, SIGNAL(reload_theme()), ao_config_panel, SLOT(on_config_reload_theme_requested()));
  ao_config_panel->hide();

  dr_discord = new DRDiscord(this);
  dr_discord->set_presence(ao_config->discord_presence());
  dr_discord->set_hide_server(ao_config->discord_hide_server());
  dr_discord->set_hide_character(ao_config->discord_hide_character());
  connect(ao_config, SIGNAL(discord_presence_changed(bool)), dr_discord, SLOT(set_presence(bool)));
  connect(ao_config, SIGNAL(discord_hide_server_changed(bool)), dr_discord, SLOT(set_hide_server(bool)));
  connect(ao_config, SIGNAL(discord_hide_character_changed(bool)), dr_discord, SLOT(set_hide_character(bool)));
}

AOApplication::~AOApplication()
{
  destruct_lobby();
  destruct_courtroom();
}

void AOApplication::construct_lobby()
{
  if (lobby_constructed)
  {
    qDebug() << "W: lobby was attempted constructed when it already exists";
    return;
  }

  m_lobby = new Lobby(this);
  lobby_constructed = true;

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
  QRect screen_geometry = QApplication::desktop()->screenGeometry();
#else
  QScreen *screen = QApplication::screenAt(m_lobby->pos());
  if (screen == nullptr)
    return;
  QRect screen_geometry = screen->geometry();
#endif
  int x = (screen_geometry.width() - m_lobby->width()) / 2;
  int y = (screen_geometry.height() - m_lobby->height()) / 2;
  m_lobby->move(x, y);

  m_lobby->show();

  dr_discord->set_state(DRDiscord::State::Idle);
  dr_discord->clear_server_name();
}

void AOApplication::destruct_lobby()
{
  if (!lobby_constructed)
  {
    qDebug() << "W: lobby was attempted destructed when it did not exist";
    return;
  }

  delete m_lobby;
  lobby_constructed = false;
}

void AOApplication::construct_courtroom()
{
  if (courtroom_constructed)
  {
    qDebug() << "W: courtroom was attempted constructed when it already exists";
    return;
  }

  m_courtroom = new Courtroom(this);
  connect(m_courtroom, SIGNAL(closing()), this, SLOT(on_courtroom_closing()));
  connect(m_courtroom, SIGNAL(destroyed()), this, SLOT(on_courtroom_destroyed()));
  courtroom_constructed = true;

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
  QRect screen_geometry = QApplication::desktop()->screenGeometry();
#else
  QScreen *screen = QApplication::screenAt(m_courtroom->pos());
  if (screen == nullptr)
    return;
  QRect screen_geometry = screen->geometry();
#endif
  int x = (screen_geometry.width() - m_courtroom->width()) / 2;
  int y = (screen_geometry.height() - m_courtroom->height()) / 2;
  m_courtroom->move(x, y);
}

void AOApplication::destruct_courtroom()
{
  // destruct courtroom
  if (courtroom_constructed)
  {
    delete m_courtroom;
    courtroom_constructed = false;
  }
  else
  {
    qDebug() << "W: courtroom was attempted destructed when it did not exist";
  }

  // gracefully close our connection to the current server
  net_manager->disconnect_from_server();
}

void AOApplication::on_config_theme_changed()
{
  Q_EMIT reload_theme();
}

void AOApplication::on_config_reload_theme_requested()
{
  Q_EMIT reload_theme();
}

void AOApplication::on_config_gamemode_changed()
{
  Q_EMIT reload_theme();
}

void AOApplication::on_config_timeofday_changed()
{
  Q_EMIT reload_theme();
}

void AOApplication::set_favorite_list()
{
  favorite_list = read_serverlist_txt();
}

QVector<server_type> &AOApplication::get_favorite_list()
{
  return favorite_list;
}

QString AOApplication::get_current_char()
{
  if (courtroom_constructed)
    return m_courtroom->get_current_char();
  else
    return "";
}

/**
 * @brief Check the path for various known exploits.
 *
 * In order:
 * - Directory traversal (most commonly: "../" jumps)
 * @param p_file The path to check.
 * @return A sanitized path. If any check fails, the path returned is an empty string. The sanitized path does not
 * necessarily exist.
 */
QString AOApplication::sanitize_path(QString p_file)
{
  if (!p_file.contains(".."))
    return p_file;

  QStringList list = p_file.split(QRegularExpression("[\\/]"));
  while (!list.isEmpty())
    if (list.takeFirst().contains(QRegularExpression("\\.{2,}")))
      return nullptr;

  return p_file;
}

void AOApplication::toggle_config_panel()
{
  ao_config_panel->setVisible(!ao_config_panel->isVisible());
  if (ao_config_panel->isVisible())
  {
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
    QRect screen_geometry = QApplication::desktop()->screenGeometry();
#else
    QScreen *screen = QApplication::screenAt(ao_config_panel->pos());
    if (screen == nullptr)
      return;
    QRect screen_geometry = screen->geometry();
#endif
    int x = (screen_geometry.width() - ao_config_panel->width()) / 2;
    int y = (screen_geometry.height() - ao_config_panel->height()) / 2;
    ao_config_panel->setFocus();
    ao_config_panel->raise();
    ao_config_panel->move(x, y);
  }
}

bool AOApplication::get_first_person_enabled()
{
  return ao_config->get_bool("first_person", false);
}

void AOApplication::add_favorite_server(int p_server)
{
  if (p_server < 0 || p_server >= server_list.size())
    return;

  server_type fav_server = server_list.at(p_server);

  QString str_port = QString::number(fav_server.port);

  QString server_line = fav_server.ip + ":" + str_port + ":" + fav_server.name;

  write_to_serverlist_txt(server_line);
}

QVector<server_type> &AOApplication::get_server_list()
{
  return server_list;
}

void AOApplication::server_disconnected()
{
  if (!courtroom_constructed)
    return;
  m_courtroom->stop_all_audio();
  call_notice("Disconnected from server.");
  construct_lobby();
  destruct_courtroom();
}

void AOApplication::loading_cancelled()
{
  destruct_courtroom();

  m_lobby->hide_loading_overlay();
}

void AOApplication::ms_connect_finished(bool connected, bool will_retry)
{
  if (connected)
  {
    AOPacket *f_packet = new AOPacket("ALL#%");
    send_ms_packet(f_packet);
  }
  else
  {
    if (!lobby_constructed)
    {
      return;
    }
    else if (will_retry)
    {
      m_lobby->append_error("Error connecting to master server. Will try again in " +
                            QString::number(net_manager->ms_reconnect_delay_ms / 1000.f) + " seconds.");
    }
    else
    {
      call_error("There was an error connecting to the master server.\n"
                 "We deploy multiple master servers to mitigate any possible "
                 "downtime, "
                 "but the client appears to have exhausted all possible "
                 "methods of finding "
                 "and connecting to one.\n"
                 "Please check your Internet connection and firewall, and "
                 "please try again.");
    }
  }
}

void AOApplication::on_courtroom_closing()
{
  ao_config_panel->hide();
}

void AOApplication::on_courtroom_destroyed()
{
  ao_config_panel->hide();
}
