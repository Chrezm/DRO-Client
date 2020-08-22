#include "aoapplication.h"

#include "lobby.h"
#include "courtroom.h"
#include "networkmanager.h"
#include "debug_functions.h"

#include "aoconfig.h"
#include "aoconfigpanel.h"

#include <QDebug>
#include <QRect>
#include <QDesktopWidget>

AOApplication::AOApplication(int &argc, char **argv) : QApplication(argc, argv)
{
    discord = new AttorneyOnline::Discord();

    net_manager = new NetworkManager(this);
    connect(net_manager, SIGNAL(ms_connect_finished(bool, bool)), SLOT(ms_connect_finished(bool, bool)));

    config = new AOConfig(this);
    connect(config, SIGNAL(theme_changed(QString)), this, SLOT(on_config_theme_changed()));

    config_panel = new AOConfigPanel;
    connect(config_panel, SIGNAL(reload_theme()), this, SLOT(on_config_reload_theme_requested()));
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
  int x = (screenGeometry.width()-w_lobby->width()) / 2;
  int y = (screenGeometry.height()-w_lobby->height()) / 2;
  w_lobby->move(x, y);

  discord->state_lobby();

  w_lobby->show();
}

void AOApplication::destruct_lobby()
{
  if(!lobby_constructed)
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
  connect(w_courtroom, SIGNAL(destroyed()), this, SLOT(on_courtroom_destroyed()));
  courtroom_constructed = true;

  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  int x = (screenGeometry.width()-w_courtroom->width()) / 2;
  int y = (screenGeometry.height()-w_courtroom->height()) / 2;
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
    return QString::number(RELEASE) + "." + QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION);
}

void AOApplication::set_theme_variant(QString p_variant)
{
    m_theme_variant = p_variant;
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
        int x = (screenGeometry.width()-config_panel->width()) / 2;
        int y = (screenGeometry.height()-config_panel->height()) / 2;
        config_panel->setFocus();
        config_panel->raise();
        config_panel->move(x, y);
    }
}

bool AOApplication::get_always_pre_enabled()
{
    return config->get_bool("always_pre", true);
}

bool AOApplication::get_first_person_enabled()
{
    return config->get_bool("first_person", false);
}

bool AOApplication::get_chatlog_scrolldown()
{
    return config->log_goes_downward_enabled();
}

int AOApplication::get_chatlog_max_lines()
{
    return config->log_max_lines();
}

int AOApplication::get_chat_tick_interval()
{
    return config->get_number("chat_tick_interval", 60);
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
    return config->get_bool("music_change_log", true);
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
      w_lobby->append_error("Error connecting to master server. Will try again in "
                          + QString::number(net_manager->ms_reconnect_delay_ms / 1000.f) + " seconds.");
    }
    else
    {
      call_error("There was an error connecting to the master server.\n"
                 "We deploy multiple master servers to mitigate any possible downtime, "
                 "but the client appears to have exhausted all possible methods of finding "
                 "and connecting to one.\n"
                 "Please check your Internet connection and firewall, and please try again.");
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
