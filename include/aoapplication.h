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

  AOConfig *ao_config = nullptr;
  AOConfigPanel *ao_config_panel = nullptr;
  DRDiscord *dr_discord = nullptr;

  NetworkManager *net_manager = nullptr;

  Lobby *m_lobby = nullptr;
  bool lobby_constructed = false;

  Courtroom *m_courtroom = nullptr;
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

  ///////////////////////////////////////////

  void set_favorite_list();
  QVector<server_type> &get_favorite_list();
  void add_favorite_server(int p_server);

  QVector<server_type> &get_server_list();

  // Returns the character the player has currently selected
  QString get_current_char();

  // implementation in path_functions.cpp
  QString get_base_path();
  QString get_character_folder_path(QString character);
  QString get_character_path(QString p_character, QString p_file);
  // QString get_demothings_path();
  QString get_sounds_path(QString p_file);
  QString get_music_path(QString p_song);
  QString get_background_path(QString p_file);
  QString get_default_background_path(QString p_file);
  QString get_evidence_path(QString p_file);

  QString sanitize_path(QString p_file);

  QString find_asset_path(QStringList possible_roots, QStringList possible_exts = {""});
  QString find_theme_asset_path(QString p_root, QStringList p_exts = {""});

  QString get_case_sensitive_path(QString p_file);

  ////// Functions for accessing the config panel //////
  void toggle_config_panel();

  ////// Functions for reading and writing files //////
  // Implementations file_functions.cpp

  // Returns text from note file
  QString read_note(QString filename);

  // returns a list of call words
  QStringList get_callwords();

  // returns whatever the client should simulate first person dialog
  bool get_first_person_enabled();

  // TODO document what this does
  QStringList get_sfx_list();

  // Appends the argument string to serverlist.txt
  void write_to_serverlist_txt(QString p_line);

  // Writes to note file
  void write_note(QString p_text, QString filename);

  // appends to note file
  void append_note(QString p_line, QString filename);

  // Returns the contents of serverlist.txt
  QVector<server_type> read_serverlist_txt();

  QString read_ini(QString p_identifier, QString p_path);

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
