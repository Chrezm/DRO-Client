#ifndef AOCONFIGPANEL_H
#define AOCONFIGPANEL_H
// qt
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QWidget>
// src
#include "aoconfig.h"
#include "aoguiloader.h"

class AOConfigPanel : public QWidget
{
  Q_OBJECT

  AOConfig *m_config = nullptr;

  // general
  QLineEdit *w_username = nullptr;
  QLineEdit *w_callwords = nullptr;
  QComboBox *w_theme = nullptr;
  QPushButton *w_reload_theme = nullptr;
  QComboBox *w_theme_variant = nullptr;
  QCheckBox *w_always_pre = nullptr;
  QSpinBox *w_chat_tick_interval = nullptr;
  QCheckBox *w_server_alerts = nullptr;

  // IC Chatlog
  QSpinBox *w_log_max_lines = nullptr;
  QCheckBox *w_log_uses_newline = nullptr;
  QCheckBox *w_log_goes_downward = nullptr;
  QCheckBox *w_log_music = nullptr;
  QCheckBox *w_log_is_recording = nullptr;

  // audio
  QSlider *w_effects = nullptr;
  QLabel *w_effects_value = nullptr;
  QSlider *w_system = nullptr;
  QLabel *w_system_value = nullptr;
  QSlider *w_music = nullptr;
  QLabel *w_music_value = nullptr;
  QSlider *w_blips = nullptr;
  QLabel *w_blips_value = nullptr;
  QSpinBox *w_blip_rate = nullptr;
  QCheckBox *w_blank_blips = nullptr;

  // save
  QPushButton *w_save = nullptr;

public:
  AOConfigPanel(QWidget *p_parent = nullptr);

public slots:
  void on_config_reload_theme_requested();

signals:
  void reload_theme();

private:
  void refresh_theme_list();
  void refresh_theme_variant_list();

private slots:
  void on_reload_theme_clicked();
  void on_effects_value_changed(int p_num);
  void on_system_value_changed(int p_num);
  void on_music_value_changed(int p_num);
  void on_blips_value_changed(int p_num);
};

#endif // AOCONFIGPANEL_H
