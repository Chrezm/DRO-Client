#ifndef DISCORD_RICH_PRESENCE_H
#define DISCORD_RICH_PRESENCE_H
// 3rd
#include <discord_rpc.h>
// std
#include <string>

#include "datatypes.h"

namespace AttorneyOnline
{

class Discord
{
private:
  const char *APPLICATION_ID[2] = {
      "538080629535801347",
      "538080629535801347",
  }; // insert second one here blah blah
  int m_index = 0;
  std::string server_name, server_id;
  int64_t timestamp;
  DR::DiscordRichPresenceStyle style;
  DiscordRichPresence current_presence;
  void refresh_presence(DiscordRichPresence * = nullptr);

public:
  Discord();
  ~Discord();

  void state_lobby();
  void state_server(std::string name, std::string server_id);
  void state_character(std::string name);
  void state_spectate();
  void start(const char *APPLICATION_ID);
  void restart(const char *APPLICATION_ID);
  void toggle(int p_index);
  void set_style(DR::DiscordRichPresenceStyle new_style);
};

} // namespace AttorneyOnline
#endif // DISCORD_RICH_PRESENCE_H
