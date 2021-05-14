#ifndef AOCONFIG_H
#define AOCONFIG_H

#include "datatypes.h"

#include <QObject>

#include <optional>

class AOConfig : public QObject
{
  Q_OBJECT

public:
  AOConfig(QObject *p_parent = nullptr);
  ~AOConfig();

  // generic getters
  QString get_string(QString p_name, QString p_default = nullptr) const;
  bool get_bool(QString p_name, bool p_default = false) const;
  int get_number(QString p_name, int p_default = 0) const;

  // getters
  bool autosave() const;
  QString username() const;
  QString showname() const;
  QString showname_placeholder() const;
  QString callwords() const;
  bool server_alerts_enabled() const;
  bool discord_presence() const;
  bool discord_hide_server() const;
  bool discord_hide_character() const;
  QString theme() const;
  QString gamemode() const;
  bool manual_gamemode_enabled() const;
  QString timeofday() const;
  bool manual_timeofday_enabled() const;
  bool always_pre_enabled() const;
  int chat_tick_interval() const;
  int log_max_lines() const;
  bool log_display_timestamp_enabled() const;
  bool log_display_self_highlight_enabled() const;
  bool log_display_empty_messages_enabled() const;
  bool log_is_topdown_enabled() const;
  bool log_format_use_newline_enabled() const;
  bool log_display_music_switch_enabled() const;
  bool log_is_recording_enabled() const;

  // audio
  std::optional<QString> favorite_device_driver() const;
  int master_volume() const;
  bool suppress_background_audio() const;
  int system_volume() const;
  int effect_volume() const;
  bool effect_ignore_suppression() const;
  int music_volume() const;
  bool music_ignore_suppression() const;
  int blip_volume() const;
  bool blip_ignore_suppression() const;
  int blip_rate() const;
  bool blank_blips_enabled() const;

  // io
public slots:
  void save_file();

  // setters
public slots:
  void set_autosave(bool p_enabled);
  void set_username(QString p_string);
  void set_showname(QString p_string);
  void set_showname_placeholder(QString p_string);
  void clear_showname_placeholder();
  void set_callwords(QString p_string);
  void set_server_alerts(bool p_enabled);
  void set_discord_presence(const bool p_enabled);
  void set_discord_hide_server(const bool p_enabled);
  void set_discord_hide_character(const bool p_enabled);
  void set_theme(QString p_string);
  void set_gamemode(QString p_string);
  void set_manual_gamemode(bool p_enabled);
  void set_timeofday(QString p_string);
  void set_manual_timeofday(bool p_enabled);
  void set_always_pre(bool p_enabled);
  void set_chat_tick_interval(int p_number);
  void set_log_max_lines(int p_number);
  void set_log_display_timestamp(bool p_enabled);
  void set_log_display_self_highlight(bool p_enabled);
  void set_log_display_empty_messages(bool p_enabled);
  void set_log_is_topdown(bool p_enabled);
  void set_log_format_use_newline(bool p_enabled);
  void set_log_display_music_switch(bool p_enabled);
  void set_log_is_recording(bool p_enabled);
  void set_suppress_background_audio(bool p_enabled);

  // audio
  void set_favorite_device_driver(QString p_device_driver);
  void set_master_volume(int p_number);
  void set_system_volume(int p_number);
  void set_effect_volume(int p_number);
  void set_effect_ignore_suppression(bool p_enabled);
  void set_music_volume(int p_number);
  void set_music_ignore_suppression(bool p_enabled);
  void set_blip_volume(int p_number);
  void set_blip_ignore_suppression(bool p_enabled);
  void set_blip_rate(int p_number);
  void set_blank_blips(bool p_enabled);

  // signals
signals:
  // meta
  void autosave_changed(bool);

  // general
  void username_changed(QString);
  void callwords_changed(QString);
  void server_alerts_changed(bool);
  void discord_presence_changed(bool);
  void discord_hide_server_changed(bool);
  void discord_hide_character_changed(bool);

  // game
  void theme_changed(QString);
  void gamemode_changed(QString);
  void manual_gamemode_changed(bool);
  void timeofday_changed(QString);
  void manual_timeofday_changed(bool);
  void showname_changed(QString);
  void showname_placeholder_changed(QString);
  void always_pre_changed(bool);
  void chat_tick_interval_changed(int);

  // log
  void log_max_lines_changed(int);
  void log_display_timestamp_changed(bool);
  void log_display_self_highlight_changed(bool);
  void log_display_empty_messages_changed(bool);
  void log_is_topdown_changed(bool);
  void log_format_use_newline_changed(bool);
  void log_display_music_switch_changed(bool);
  void log_is_recording_changed(bool);

  // audio
  void favorite_device_changed(QString);
  void master_volume_changed(int);
  void suppress_background_audio_changed(bool);
  void system_volume_changed(int);
  void effect_volume_changed(int);
  void effect_ignore_suppression_changed(bool);
  void music_volume_changed(int);
  void music_ignore_suppression_changed(bool);
  void blip_volume_changed(int);
  void blip_ignore_suppression_changed(bool);
  void blip_rate_changed(int);
  void blank_blips_changed(bool);
};

#endif // AOCONFIG_H
