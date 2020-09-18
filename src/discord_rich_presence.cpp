#include "discord_rich_presence.h"

#include <cstring>
#include <ctime>

#include <QDebug>

namespace AttorneyOnline
{

Discord::Discord()
{
  //  DiscordEventHandlers handlers;
  //  std::memset(&handlers, 0, sizeof(handlers));
  //  handlers = {};
  //  handlers.ready = [] {
  //    qInfo() << "Discord RPC ready";
  //  };
  //  handlers.disconnected = [](int errorCode, const char* message) {
  //    qInfo() << "Discord RPC disconnected! " << message;
  //  };
  //  handlers.errored = [](int errorCode, const char* message) {
  //    qWarning() << "Discord RPC errored out! " << message;
  //  };
  //  qInfo() << "Initializing Discord RPC";
  //  Discord_Initialize(APPLICATION_ID1, &handlers, 1, nullptr);

  start(APPLICATION_ID[0]);
}

void Discord::start(const char *APPLICATION_ID)
{
  DiscordEventHandlers handlers;
  std::memset(&handlers, 0, sizeof(handlers));
  handlers = {};
  handlers.ready = [](const DiscordUser *user) {
    qInfo() << "Discord RPC ready for" << user->username;
  };
  handlers.disconnected = [](int errorCode, const char *message) {
    qInfo() << "Discord RPC disconnected! " << message;
  };
  handlers.errored = [](int errorCode, const char *message) {
    qWarning() << "Discord RPC errored out! " << message;
  };
  qInfo() << "Initializing Discord RPC";
  Discord_Initialize(APPLICATION_ID, &handlers, 1, nullptr);
}

Discord::~Discord()
{
  Discord_Shutdown();
}

void Discord::restart(const char *APPLICATION_ID)
{
  Discord_Shutdown();
  start(APPLICATION_ID);
}

void Discord::toggle(int p_index)
{
  if (p_index >= 0 && p_index < 2)
  {
    if (p_index != m_index)
    {
      restart(APPLICATION_ID[p_index]);
      m_index = p_index;
    }
  }
  else
    qDebug() << p_index << "is not a valid APPLICATION_ID Index";
}

void Discord::state_lobby()
{
  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "danganronpa_online";
  presence.largeImageText = "Sore Wa Chigau Yo!";
  presence.instance = 1;

  presence.state = "In Lobby";
  presence.details = "Idle";
  Discord_UpdatePresence(&presence);
}

void Discord::state_server(std::string name, std::string server_id)
{
  qDebug() << "Discord RPC: Setting server state";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "danganronpa_online";
  presence.largeImageText = "Sore Wa Chigau Yo!";
  presence.instance = 1;

  auto timestamp = static_cast<int64_t>(std::time(nullptr));

  presence.state = "In a Server";
  presence.details = name.c_str();
  presence.matchSecret = server_id.c_str();
  presence.startTimestamp = this->timestamp;

  this->server_id = server_id;
  this->server_name = name;
  this->timestamp = timestamp;
  Discord_UpdatePresence(&presence);
}

void Discord::state_character(std::string name)
{
  auto name_internal =
      QString(name.c_str()).toLower().replace(' ', '_').toStdString();
  auto name_friendly = QString(name.c_str()).replace('_', ' ').toStdString();
  const std::string playing_as = "Playing as " + name_friendly;
  qDebug() << "Discord RPC: Setting character state (" << playing_as.c_str()
           << ")";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = name_internal.c_str();
  presence.instance = 1;
  presence.details = this->server_name.c_str();
  presence.matchSecret = this->server_id.c_str();
  presence.startTimestamp = this->timestamp;

  presence.state = playing_as.c_str();
  //  presence.smallImageKey = "danganronpa_online";
  presence.smallImageKey = "danganronpa_online";
  presence.smallImageText = "Danganronpa Online";
  Discord_UpdatePresence(&presence);
}

void Discord::state_spectate()
{
  qDebug() << "Discord RPC: Setting specator state";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "danganronpa_online";
  presence.largeImageText = "Sore Wa Chigau Yo!";
  presence.instance = 1;
  presence.details = this->server_name.c_str();
  presence.matchSecret = this->server_id.c_str();
  presence.startTimestamp = this->timestamp;

  presence.state = "Spectating";
  Discord_UpdatePresence(&presence);
}

} // namespace AttorneyOnline
