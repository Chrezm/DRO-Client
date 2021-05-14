#ifndef AOAPPLICATION_H
#define AOAPPLICATION_H

#include "aopacket.h"
#include "datatypes.h"
#include "drdiscord.h"

#include <QApplication>
#include <QFile>
#include <QVector>

class NetworkManager;
class Lobby;
class Courtroom;
class AOConfig;
class AOConfigPanel;

class AOApplication : public QApplication
{
  Q_OBJECT

public:
  AOApplication(int &argc, char **argv);
  ~AOApplication();

  NetworkManager *net_manager = nullptr;
  Lobby *w_lobby = nullptr;
  Courtroom *w_courtroom = nullptr;
  DRDiscord *discord = nullptr;
  AOConfig *config = nullptr;
  AOConfigPanel *config_panel = nullptr;

  bool lobby_constructed = false;
  bool courtroom_constructed = false;

  void construct_lobby();
  void destruct_lobby();

  void construct_courtroom();
  void destruct_courtroom();

  void ms_packet_received(AOPacket *p_packet);
  void server_packet_received(AOPacket *p_packet);

  void send_ms_packet(AOPacket *p_packet);
  void send_server_packet(AOPacket *p_packet, bool encoded = true);

  ///////////////server metadata////////////////
#ifdef DRO_ACKMS // TODO WARNING remove entire block on 1.0.0 release

  bool m_FL_ackMS_enabled = false;
#endif
  bool m_FL_showname_enabled = false;
  bool m_FL_chrini_enabled = false;

  ///////////////loading info///////////////////

  // player number, it's hardly used but might be needed for some old servers
  int s_pv = 0;

  QString server_software;

  int char_list_size = 0;
  int loaded_chars = 0;
  int evidence_list_size = 0;
  int loaded_evidence = 0;
  int music_list_size = 0;
  int loaded_music = 0;

  bool courtroom_loaded = false;

  //////////////////versioning///////////////

  int get_release()
  {
    return RELEASE;
  }
  int get_major_version()
  {
    return MAJOR_VERSION;
  }
  int get_minor_version()
  {
    return MINOR_VERSION;
  }
  QString get_version_string();

  ///////////////////////////////////////////

  void set_favorite_list();
  QVector<server_type> &get_favorite_list()
  {
    return favorite_list;
  }
  void add_favorite_server(int p_server);

  void set_server_list();
  QVector<server_type> &get_server_list()
  {
    return server_list;
  }

  // reads the theme from config.ini and sets it accordingly
  void set_theme_name(QString p_name);

  // Returns the character the player has currently selected
  QString get_current_char();

  // implementation in path_functions.cpp
  QString get_base_path();
  QString get_data_path();
  QString get_character_folder_path(QString character);
  QString get_character_path(QString p_character, QString p_file);
  // QString get_demothings_path();
  QString get_sounds_path(QString p_file);
  QString get_music_path(QString p_song);
  QString get_background_path(QString p_file);
  QString get_default_background_path(QString p_file);
  QString get_evidence_path(QString p_file);

  /**
   * @brief Check the path for various known exploits.
   *
   * In order:
   * - Directory traversal (most commonly: "../" jumps)
   * @param p_file The path to check.
   * @return A sanitized path. If any check fails, the path returned is an empty string. The sanitized path does not
   * necessarily exist.
   */
  QString sanitize_path(QString p_file);

  /**
   * @brief Returns the first case-sensitive file that is the combination of one
   * of the given root and extensions, or empty string if no combination exists.
   *
   * @details A root is matched to all given extensions in order before
   * continuing to the next root.
   *
   * @param possible_roots The potential roots the filepath could have.
   * Case-insensitive.
   * @param possible_exts The potential extensions the filepath could have.
   * Case-insensitive.
   *
   * @return The first case-sensitive root+extension path for which a file
   * exists, or an empty string, if not one does.
   */
  QString find_asset_path(QStringList possible_roots, QStringList possible_exts = {""});

  /**
   * @brief Returns the first case-sensitive file in the theme folder that is
   * of the form name+extension, or empty string if it fails.
   *
   * @details The p_exts list is browsed in order. A name+extension file is
   * searched in order in the following directories before checking the next
   * extension:
   * 1. The current time of day folder in the current gamemode folder
   * 2. The current gamemode folder
   * 3. The current time of day folder
   * 4. The current theme folder.
   * The first path that is matched is the one that is returned. If no file
   * is found at all, it returns an empty string.
   *
   * @param p_name Name of the file to look for. Case-insensitive.
   * @param p_exts The potential extensions the filepath could have.
   * Case-insensitive.
   *
   * @return The first case-sensitive root+extension path that corresponds to an
   * actual file, or an empty string, if not one does.
   */
  QString find_theme_asset_path(QString p_root, QStringList p_exts = {""});

  /**
   * @brief Returns the 'correct' path for the file given as the parameter by
   * trying to match the case of the actual path.
   *
   * @details This function is mostly used on case-sensitive file systems, like
   * ext4, generally used on Linux. On FAT, there is no difference between
   * "file" and "FILE". On ext4, those are two different files. This results in
   * assets that are detected correctly on Windows not being detected on Linux.
   *
   * For this reason, the implementation of this function is system-dependent:
   * on case-insensitive systems, it just returns the parameter itself.
   *
   * @param p_file The path whose casing must be checked against the actual
   * directory structure.
   *
   * @return The parameter path with fixed casing.
   */
  QString get_case_sensitive_path(QString p_file);

  ////// Functions for accessing the config panel //////

  void toggle_config_panel();

  ////// Functions for reading and writing files //////
  // Implementations file_functions.cpp

  // Returns text from note file
  QString read_note(QString filename);

  // Returns the blip rate from config.ini
  int read_blip_rate();

  // returns whatever we want newlines or ':' to be appended in front of names
  // in the ic chat log
  bool read_chatlog_newline();

  // returns the user name
  QString get_username();

  // returns a list of call words
  QStringList get_callwords();

  /**
   * @brief Return the current theme name.
   * @return Name of current theme.
   */
  QString get_theme();

  /**
   * @brief Return the current gamemode. If no gamemode is set, return the
   * empty string.
   *
   * @return Current gamemode, or empty string if not set.
   */
  QString get_gamemode();

  /**
   * @brief Returns the current manual gamemode status.
   *
   * @details If true, a player can change gamemodes manually and their client
   * will ignore orders to change gamemode from the server. If false, neither is
   * possible and the client will follow orders from the server to change
   * gamemode.
   *
   * @return Current manual gamemode status.
   */
  bool get_manual_gamemode_enabled();

  /**
   * @brief Returns the current time of day. If no time of day is set, return
   * the empty string.
   *
   * @return Current gamemode, or empty string if not set.
   */
  QString get_timeofday();

  /**
   * @brief Returns the current manual time of day status.
   *
   * @details If true, a player can change time of day manually and their client
   * will ignore orders to change time of day from the server. If false, neither
   * is possible and the client will follow orders from the server to change
   * time of day.
   *
   * @return Current manual time of day status.
   */
  bool get_manual_timeofday_enabled();

  // returns whether server alerts (ones that trigger a client alert other than
  // callwords) should actually tigger a server alert or not
  bool get_server_alerts_enabled();

  // returns whatever preanimations should always play or not
  bool get_always_pre_enabled();

  // returns whatever the client should simulate first person dialog
  bool get_first_person_enabled();

  // returns if chatlog goes downward
  bool get_chatlog_scrolldown();

  int get_chatlog_max_lines();

  int get_chat_tick_interval();

  bool get_chatlog_newline();

  bool get_enable_logging_enabled();

  // Returns the value of default_sfx in config.ini
  int get_default_sfx();

  // Returns the value of default_music in config.ini
  int get_default_music();

  // returns whatever music is logged within the chatlog
  bool get_music_change_log_enabled();

  // Returns the value of default_blip in config.ini
  int get_default_blip();

  // Returns true if blank blips is enabled in config.ini and false otherwise
  bool get_blank_blip();

  // TODO document what this does
  QStringList get_sfx_list();

  // Appends the argument string to serverlist.txt
  void write_to_serverlist_txt(QString p_line);

  // Writes to note file
  void write_note(QString p_text, QString filename);

  // appends to note file
  void append_note(QString p_line, QString filename);

  // Overwrites config.ini with new theme
  void write_theme(QString theme);

  // Set the gamemode
  void set_gamemode(QString m_gamemode);

  // Set the time of day
  void set_timeofday(QString m_timeofday);

  // Returns the contents of serverlist.txt
  QVector<server_type> read_serverlist_txt();

  /**
   * @brief Reads p_path and returns the value associated with key
   * p_identifier. If the file or key do not exist, return empty.
   *
   * @param p_identifier Key to look for.
   * @param p_path Full path to ini file
   * @return Value associated with key, or empty if not found.
   */
  QString read_ini(QString p_identifier, QString p_path);

  /**
   * @brief Searches p_file in theme folder and returns the value associated
   * with key p_identifier. If the file or key do not exist, return empty.
   *
   * @details p_file is looked for in the following directories. The earliest
   * directory where it is found is the one that is considered.
   * 1. The current time of day folder in the current gamemode folder
   * 2. The current gamemode folder
   * 3. The current time of day folder
   * 4. The current theme folder.
   * 5. The default theme folder.
   *
   * @param p_identifier Key to look for.
   * @param p_file Name of file+ini to look for.
   * @return Value associated with key, or empty if not found.
   */
  QString read_theme_ini(QString p_identifier, QString p_file);

  bool read_theme_ini_bool(QString p_identifier, QString p_file);

  int read_theme_ini_int(QString p_identifier, QString p_file);

  // Returns the coordinates of widget with p_identifier from p_file
  QPoint get_button_spacing(QString p_identifier, QString p_file);

  // Returns the dimensions of widget with specified identifier from p_file
  pos_size_type get_element_dimensions(QString p_identifier, QString p_file);

  // Returns the value of font property p_identifier from p_file
  int get_font_property(QString p_identifier, QString p_file);

  // Returns the name of the font with p_identifier from p_file
  QString get_font_name(QString p_identifier, QString p_file);

  // Returns the color with p_identifier from p_file
  QColor get_color(QString p_identifier, QString p_file);

  // Returns the sfx with p_identifier from sounds.ini in the current theme path
  QString get_sfx(QString p_identifier);

  // Returns the value of p_search_line within target_tag and terminator_tag

  QVariant read_char_ini(QString character, QString group, QString key, QVariant default_value);
  QVariant read_char_ini(QString character, QString group, QString key);

  // Returns the text between target_tag and terminator_tag in p_file
  QString get_stylesheet(QString target_tag, QString p_file);

  // Returns string list (color name to color HEX)
  QMap<DR::Color, DR::ColorInfo> get_chatmessage_colors();

  // Returns string list (highlight character to color name)
  QVector<QStringList> get_highlight_colors();

  // Returns special button on cc_config according to index
  QString get_spbutton(QString p_tag, int index);

  // Returns effect on cc_config according to index
  QStringList get_effect(int index);

  // Returns wtce on cc_config according to index
  QStringList get_wtce(int index);

  // Returns the side of the p_char character from that characters ini file
  QString get_char_side(QString p_char);

  // Returns the showname from the ini of p_char
  QString get_showname(QString p_char);

  // Returns showname from showname.ini
  QString read_showname(QString p_char);

  // Returns the value of chat from the specific p_char's ini file
  QString get_chat(QString p_char);

  // Returns the name of p_char
  QString get_char_name(QString p_char);

  QStringList get_char_include(QString character);

  QStringList get_char_include_tree(QString character);

  // Returns p_char's gender
  QString get_gender(QString p_char);

  QVector<DREmote> get_emote_list(QString p_chr);

  // Returns x,y offset for effect p_effect
  QStringList get_effect_offset(QString p_char, int p_effect);

  // Returns overlay at p_effect in char_path/overlay
  QStringList get_overlay(QString p_char, int p_effect);

signals:
  void reload_theme();

private:
  const int RELEASE = 1;
  const int MAJOR_VERSION = 0;
  const int MINOR_VERSION = 0;

  QVector<server_type> server_list;
  QVector<server_type> favorite_list;

private slots:
  void ms_connect_finished(bool connected, bool will_retry);
  void on_courtroom_closing();
  void on_courtroom_destroyed();
  void on_config_theme_changed();
  void on_config_reload_theme_requested();
  void on_config_gamemode_changed();
  void on_config_timeofday_changed();

public slots:
  void server_disconnected();
  void loading_cancelled();
};

#endif // AOAPPLICATION_H
