#ifndef AOCONFIGPANEL_H
#define AOCONFIGPANEL_H

#include "aoconfig.h"
#include "aoguiloader.h"
#include "draudioengine.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QWidget>

class AOApplication;

class AOConfigPanel : public QWidget
{
  Q_OBJECT

public:
  AOConfigPanel(AOApplication *p_ao_app, QWidget *p_parent = nullptr);

public slots:
  void on_config_reload_theme_requested();

protected:
  void showEvent(QShowEvent *event);

signals:
  void reload_theme();

private:
  void refresh_theme_list();
  void refresh_gamemode_list();
  void refresh_timeofday_list();
  void update_audio_device_list();

private slots:
  void on_reload_theme_clicked();
  void on_gamemode_index_changed(QString p_text);
  void on_timeofday_index_changed(QString p_text);
  void on_showname_placeholder_changed(QString p_text);
  void on_log_is_topdown_changed(bool p_enabled);
  void on_device_current_index_changed(int p_index);
  void on_audio_device_changed(DRAudioDevice p_device);
  void on_favorite_audio_device_changed(DRAudioDevice p_device);
  void on_audio_device_list_changed(QList<DRAudioDevice> p_device_list);
  void on_master_value_changed(int p_num);
  void on_system_value_changed(int p_num);
  void on_effect_value_changed(int p_num);
  void on_music_value_changed(int p_num);
  void on_blip_value_changed(int p_num);

private:
  // FIXME This dependency shouldn't have come to exist.
  AOApplication *ao_app = nullptr;

  // driver
  AOConfig *m_config = nullptr;
  DRAudioEngine *m_engine = nullptr;

  // behaviour
  QPushButton *w_save = nullptr;
  QPushButton *w_close = nullptr;
  QCheckBox *w_autosave = nullptr;

  // general
  QLineEdit *w_username = nullptr;
  QLineEdit *w_callwords = nullptr;
  QCheckBox *w_server_alerts = nullptr;

  QGroupBox *w_discord_presence = nullptr;
  QCheckBox *w_discord_hide_server = nullptr;
  QCheckBox *w_discord_hide_character = nullptr;

  // game
  QComboBox *w_theme = nullptr;
  QPushButton *w_reload_theme = nullptr;
  QComboBox *w_gamemode = nullptr;
  QCheckBox *w_manual_gamemode = nullptr;
  QComboBox *w_timeofday = nullptr;
  QCheckBox *w_manual_timeofday = nullptr;
  QLineEdit *w_showname = nullptr;
  QCheckBox *w_always_pre = nullptr;
  QSpinBox *w_chat_tick_interval = nullptr;

  // IC Chatlog
  QSpinBox *w_log_max_lines = nullptr;
  QCheckBox *w_log_display_timestamp = nullptr;
  QCheckBox *w_log_display_self_highlight = nullptr;
  QCheckBox *w_log_format_use_newline = nullptr;
  QCheckBox *w_log_display_empty_messages = nullptr;
  QCheckBox *w_log_display_music_switch = nullptr;
  QRadioButton *w_log_orientation_top_down = nullptr;
  QRadioButton *w_log_orientation_bottom_up = nullptr;
  QCheckBox *w_log_is_recording = nullptr;

  // audio
  QComboBox *w_device = nullptr;
  QCheckBox *w_favorite_device = nullptr;
  QSlider *w_master = nullptr;
  QLabel *w_master_value = nullptr;
  QGroupBox *w_suppress_background_audio = nullptr;
  QSlider *w_system = nullptr;
  QLabel *w_system_value = nullptr;
  QSlider *w_effect = nullptr;
  QCheckBox *w_effect_ignore_suppression = nullptr;
  QLabel *w_effect_value = nullptr;
  QSlider *w_music = nullptr;
  QCheckBox *w_music_ignore_suppression = nullptr;
  QLabel *w_music_value = nullptr;
  QSlider *w_blip = nullptr;
  QCheckBox *w_blip_ignore_suppression = nullptr;
  QLabel *w_blip_value = nullptr;
  QSpinBox *w_blip_rate = nullptr;
  QCheckBox *w_blank_blips = nullptr;

private slots:
  void username_editing_finished();
  void showname_editing_finished();
  void callwords_editing_finished();
};

#endif // AOCONFIGPANEL_H
