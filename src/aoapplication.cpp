#include "aoapplication.h"

#include "courtroom.h"
#include "debug_functions.h"
#include "lobby.h"
#include "networkmanager.h"

#include "aoconfig.h"
#include "aoconfigpanel.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QRect>

AOApplication::AOApplication(int &argc, char **argv) : QApplication(argc, argv)
{
  discord = new AttorneyOnline::Discord();

  net_manager = new NetworkManager(this);
  connect(net_manager, SIGNAL(ms_connect_finished(bool, bool)),
          SLOT(ms_connect_finished(bool, bool)));

  config = new AOConfig(this);
  connect(config, SIGNAL(theme_changed(QString)), this,
          SLOT(on_config_theme_changed()));
  connect(config, SIGNAL(gamemode_changed(QString)), this,
          SLOT(on_config_gamemode_changed()));
  connect(config, SIGNAL(timeofday_changed(QString)), this,
          SLOT(on_config_timeofday_changed()));

  config_panel = new AOConfigPanel(this);
  connect(config_panel, SIGNAL(reload_theme()), this,
          SLOT(on_config_reload_theme_requested()));
  connect(this, SIGNAL(reload_theme()), config_panel,
          SLOT(on_config_reload_theme_requested()));
  config_panel->hide();
}

AOApplication::~AOApplication()
{
  destruct_lobby();
  destruct_courtroom();
  delete discord;
  delete config_panel;
}

void AOApplication::construct_lobby()
{
  if (lobby_constructed)
  {
    qDebug() << "W: lobby was attempted constructed when it already exists";
    return;
  }

  w_lobby = new Lobby(this);
  lobby_constructed = true;

  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  int x = (screenGeometry.width() - w_lobby->width()) / 2;
  int y = (screenGeometry.height() - w_lobby->height()) / 2;
  w_lobby->move(x, y);

  discord->state_lobby();

  w_lobby->show();
}

void AOApplication::destruct_lobby()
{
  if (!lobby_constructed)
  {
    qDebug() << "W: lobby was attempted destructed when it did not exist";
    return;
  }

  delete w_lobby;
  lobby_constructed = false;
}

void AOApplication::construct_courtroom()
{
  if (courtroom_constructed)
  {
    qDebug() << "W: courtroom was attempted constructed when it already exists";
    return;
  }

  w_courtroom = new Courtroom(this);
  connect(w_courtroom, SIGNAL(closing()), this, SLOT(on_courtroom_closing()));
  connect(w_courtroom, SIGNAL(destroyed()), this,
          SLOT(on_courtroom_destroyed()));
  courtroom_constructed = true;

  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  int x = (screenGeometry.width() - w_courtroom->width()) / 2;
  int y = (screenGeometry.height() - w_courtroom->height()) / 2;
  w_courtroom->move(x, y);
}

void AOApplication::destruct_courtroom()
{
  // destruct courtroom
  if (courtroom_constructed)
  {
    delete w_courtroom;
    courtroom_constructed = false;
  }
  else
  {
    qDebug() << "W: courtroom was attempted destructed when it did not exist";
  }

  // gracefully close our connection to the current server
  net_manager->disconnect_from_server();
}

QString AOApplication::get_version_string()
{
  return QString::number(RELEASE) + "." + QString::number(MAJOR_VERSION) + "." +
         QString::number(MINOR_VERSION);
}

void AOApplication::set_gamemode(QString p_gamemode)
{
  config->set_gamemode(p_gamemode);
  emit reload_theme();
}

void AOApplication::set_timeofday(QString p_timeofday)
{
  config->set_timeofday(p_timeofday);
  emit reload_theme();
}

void AOApplication::on_config_theme_changed()
{
  emit reload_theme();
}

void AOApplication::on_config_reload_theme_requested()
{
  emit reload_theme();
}

void AOApplication::on_config_gamemode_changed()
{
  emit reload_theme();
}

void AOApplication::on_config_timeofday_changed()
{
  emit reload_theme();
}

void AOApplication::set_favorite_list()
{
  favorite_list = read_serverlist_txt();
}

QString AOApplication::get_current_char()
{
  if (courtroom_constructed)
    return w_courtroom->get_current_char();
  else
    return "";
}

void AOApplication::toggle_config_panel()
{
  config_panel->setVisible(!config_panel->isVisible());
  if (config_panel->isVisible())
  {
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - config_panel->width()) / 2;
    int y = (screenGeometry.height() - config_panel->height()) / 2;
    config_panel->setFocus();
    config_panel->raise();
    config_panel->move(x, y);
  }
}

bool AOApplication::get_server_alerts_enabled()
{
  return config->server_alerts_enabled();
}

bool AOApplication::get_manual_gamemode_enabled()
{
  return config->manual_gamemode_enabled();
}

bool AOApplication::get_manual_timeofday_enabled()
{
  return config->manual_timeofday_enabled();
}

bool AOApplication::get_always_pre_enabled()
{
  return config->always_pre_enabled();
}

bool AOApplication::get_first_person_enabled()
{
  return config->get_bool("first_person", false);
}
bool AOApplication::get_chatlog_scrolldown()
{
  return config->log_is_topdown_enabled();
}

int AOApplication::get_chatlog_max_lines()
{
  return config->log_max_lines();
}

int AOApplication::get_chat_tick_interval()
{
  return config->chat_tick_interval();
}

bool AOApplication::get_chatlog_newline()
{
  return config->log_uses_newline_enabled();
}

bool AOApplication::get_enable_logging_enabled()
{
  return config->log_is_recording_enabled();
}

bool AOApplication::get_music_change_log_enabled()
{
  return config->log_music_enabled();
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

void AOApplication::server_disconnected()
{
  if (!courtroom_constructed)
    return;
  call_notice("Disconnected from server.");
  construct_lobby();
  destruct_courtroom();
}

void AOApplication::loading_cancelled()
{
  destruct_courtroom();

  w_lobby->hide_loading_overlay();
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
      w_lobby->append_error(
          "Error connecting to master server. Will try again in " +
          QString::number(net_manager->ms_reconnect_delay_ms / 1000.f) +
          " seconds.");
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
  config_panel->hide();
}

void AOApplication::on_courtroom_destroyed()
{
  config_panel->hide();
}

void AOApplication::recursive_resize(QResizeEvent *event, QWidget *widget)
{
  // call_notice("Started recursive resize");
  QSize old_size = event->oldSize();
  QSize new_size = event->size();
  if (old_size.height() <= 0 || old_size.width() <= 0)
    return;
  if (new_size.height() <= 0 || new_size.width() <= 0)
    return;

  float y_ratio = new_size.height() / (float)old_size.height();
  float x_ratio = new_size.width() / (float)old_size.width();

  // call_notice("About to move and resize");
  // call_notice(QString::number(widget->x() * x_ratio));
  // call_notice(QString::number(widget->y() * y_ratio));
  // call_notice(QString::number(widget->width() * x_ratio));
  // call_notice(QString::number(widget->height() * y_ratio));

  widget->move(widget->x() * x_ratio, widget->y() * y_ratio);
  widget->resize(widget->width() * x_ratio, widget->height() * y_ratio);

  for (QWidget *child : widget->findChildren<QWidget *>())
    recursive_resize(event, child);
  // call_notice("Done moving and resizing");
}
