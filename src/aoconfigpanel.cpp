#include "aoconfigpanel.h"
#include "datatypes.h"
#include "drpather.h"

// qt
#include <QDebug>
#include <QDir>

// src
#include "aoapplication.h" // ruined

AOConfigPanel::AOConfigPanel(AOApplication *p_ao_app, QWidget *p_parent)
    : QWidget(p_parent), m_config(new AOConfig(this)), m_engine(new DRAudioEngine(this))
{
  ao_app = p_ao_app;

  setWindowTitle(tr("Config"));
  setWindowFlag(Qt::WindowMinMaxButtonsHint, false);

  AOGuiLoader loader;
  loader.load_from_file(":res/ui/config_panel.ui", this);

  // tab
  QTabWidget *tab_widget = AO_GUI_WIDGET(QTabWidget, "tab_widget");
  setFocusProxy(tab_widget);
  tab_widget->setCurrentIndex(0);

  // behaviour
  w_save = AO_GUI_WIDGET(QPushButton, "save");
  w_close = AO_GUI_WIDGET(QPushButton, "close");
  w_autosave = AO_GUI_WIDGET(QCheckBox, "autosave");

  // general
  w_username = AO_GUI_WIDGET(QLineEdit, "username");
  w_callwords = AO_GUI_WIDGET(QLineEdit, "callwords");
  w_server_alerts = AO_GUI_WIDGET(QCheckBox, "server_alerts");
  w_discord_presence = AO_GUI_WIDGET(QGroupBox, "discord_presence");
  w_discord_hide_server = AO_GUI_WIDGET(QCheckBox, "discord_hide_server");
  w_discord_hide_character = AO_GUI_WIDGET(QCheckBox, "discord_hide_character");

  // game
  w_theme = AO_GUI_WIDGET(QComboBox, "theme");
  w_reload_theme = AO_GUI_WIDGET(QPushButton, "theme_reload");
  w_gamemode = AO_GUI_WIDGET(QComboBox, "gamemode");
  w_manual_gamemode = AO_GUI_WIDGET(QCheckBox, "manual_gamemode");
  w_timeofday = AO_GUI_WIDGET(QComboBox, "timeofday");
  w_manual_timeofday = AO_GUI_WIDGET(QCheckBox, "manual_timeofday");
  w_showname = AO_GUI_WIDGET(QLineEdit, "showname");
  w_always_pre = AO_GUI_WIDGET(QCheckBox, "always_pre");
  w_chat_tick_interval = AO_GUI_WIDGET(QSpinBox, "chat_tick_interval");

  // IC Chatlog
  w_log_max_lines = AO_GUI_WIDGET(QSpinBox, "log_length");
  w_log_display_timestamp = AO_GUI_WIDGET(QCheckBox, "log_display_timestamp");
  w_log_display_self_highlight = AO_GUI_WIDGET(QCheckBox, "log_display_self_highlight");
  w_log_format_use_newline = AO_GUI_WIDGET(QCheckBox, "log_format_use_newline");
  w_log_display_empty_messages = AO_GUI_WIDGET(QCheckBox, "log_display_empty_messages");
  w_log_display_music_switch = AO_GUI_WIDGET(QCheckBox, "log_display_music_switch");
  w_log_orientation_top_down = AO_GUI_WIDGET(QRadioButton, "log_orientation_top_down");
  w_log_orientation_bottom_up = AO_GUI_WIDGET(QRadioButton, "log_orientation_bottom_up");
  w_log_is_recording = AO_GUI_WIDGET(QCheckBox, "log_recording");

  // audio
  w_device = AO_GUI_WIDGET(QComboBox, "device");
  w_favorite_device = AO_GUI_WIDGET(QCheckBox, "favorite_device");
  w_master = AO_GUI_WIDGET(QSlider, "master");
  w_master_value = AO_GUI_WIDGET(QLabel, "master_value");
  w_suppress_background_audio = AO_GUI_WIDGET(QGroupBox, "suppress_background_audio");
  w_system = AO_GUI_WIDGET(QSlider, "system");
  w_system_value = AO_GUI_WIDGET(QLabel, "system_value");
  w_effect = AO_GUI_WIDGET(QSlider, "effect");
  w_effect_ignore_suppression = AO_GUI_WIDGET(QCheckBox, "effect_ignore_suppression");
  w_effect_value = AO_GUI_WIDGET(QLabel, "effect_value");
  w_music = AO_GUI_WIDGET(QSlider, "music");
  w_music_ignore_suppression = AO_GUI_WIDGET(QCheckBox, "music_ignore_suppression");
  w_music_value = AO_GUI_WIDGET(QLabel, "music_value");
  w_blip = AO_GUI_WIDGET(QSlider, "blip");
  w_blip_ignore_suppression = AO_GUI_WIDGET(QCheckBox, "blip_ignore_suppression");
  w_blip_value = AO_GUI_WIDGET(QLabel, "blip_value");
  w_blip_rate = AO_GUI_WIDGET(QSpinBox, "blip_rate");
  w_blank_blips = AO_GUI_WIDGET(QCheckBox, "blank_blips");

  // themes
  refresh_theme_list();
  refresh_gamemode_list();
  refresh_timeofday_list();

  // input
  // meta
  connect(m_config, SIGNAL(autosave_changed(bool)), w_autosave, SLOT(setChecked(bool)));

  // general
  connect(m_config, SIGNAL(username_changed(QString)), w_username, SLOT(setText(QString)));
  connect(m_config, SIGNAL(callwords_changed(QString)), w_callwords, SLOT(setText(QString)));
  connect(m_config, SIGNAL(server_alerts_changed(bool)), w_server_alerts, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(discord_presence_changed(bool)), w_discord_presence, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(discord_hide_server_changed(bool)), w_discord_hide_server, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(discord_hide_character_changed(bool)), w_discord_hide_character, SLOT(setChecked(bool)));

  // game
  connect(m_config, SIGNAL(theme_changed(QString)), w_theme, SLOT(setCurrentText(QString)));
  connect(m_config, SIGNAL(gamemode_changed(QString)), w_gamemode, SLOT(setCurrentText(QString)));
  connect(m_config, SIGNAL(manual_gamemode_changed(bool)), w_manual_gamemode, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(timeofday_changed(QString)), w_timeofday, SLOT(setCurrentText(QString)));
  connect(m_config, SIGNAL(manual_timeofday_changed(bool)), w_manual_timeofday, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(showname_changed(QString)), w_showname, SLOT(setText(QString)));
  connect(m_config, SIGNAL(showname_placeholder_changed(QString)), this,
          SLOT(on_showname_placeholder_changed(QString)));
  connect(m_config, SIGNAL(always_pre_changed(bool)), w_always_pre, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(chat_tick_interval_changed(int)), w_chat_tick_interval, SLOT(setValue(int)));

  // log
  connect(m_config, SIGNAL(log_max_lines_changed(int)), w_log_max_lines, SLOT(setValue(int)));
  connect(m_config, SIGNAL(log_display_timestamp_changed(bool)), w_log_display_timestamp, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(log_display_self_highlight_changed(bool)), w_log_display_self_highlight,
          SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(log_format_use_newline_changed(bool)), w_log_format_use_newline, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(log_display_empty_messages_changed(bool)), w_log_display_empty_messages,
          SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(log_display_music_switch_changed(bool)), w_log_display_music_switch, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(log_is_topdown_changed(bool)), this, SLOT(on_log_is_topdown_changed(bool)));
  connect(m_config, SIGNAL(log_is_recording_changed(bool)), w_log_is_recording, SLOT(setChecked(bool)));

  // audio
  connect(m_config, SIGNAL(master_volume_changed(int)), w_master, SLOT(setValue(int)));
  connect(m_config, SIGNAL(suppress_background_audio_changed(bool)), w_suppress_background_audio,
          SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(system_volume_changed(int)), w_system, SLOT(setValue(int)));
  connect(m_config, SIGNAL(effect_volume_changed(int)), w_effect, SLOT(setValue(int)));
  connect(m_config, SIGNAL(effect_ignore_suppression_changed(bool)), w_effect_ignore_suppression,
          SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(music_volume_changed(int)), w_music, SLOT(setValue(int)));
  connect(m_config, SIGNAL(music_ignore_suppression_changed(bool)), w_music_ignore_suppression, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(blip_volume_changed(int)), w_blip, SLOT(setValue(int)));
  connect(m_config, SIGNAL(blip_ignore_suppression_changed(bool)), w_blip_ignore_suppression, SLOT(setChecked(bool)));
  connect(m_config, SIGNAL(blip_rate_changed(int)), w_blip_rate, SLOT(setValue(int)));
  connect(m_config, SIGNAL(blank_blips_changed(bool)), w_blank_blips, SLOT(setChecked(bool)));

  connect(m_engine, SIGNAL(device_changed(DRAudioDevice)), this, SLOT(on_audio_device_changed(DRAudioDevice)));
  connect(m_engine, SIGNAL(device_list_changed(QList<DRAudioDevice>)), this,
          SLOT(on_audio_device_list_changed(QList<DRAudioDevice>)));
  connect(m_engine, SIGNAL(favorite_device_changed(DRAudioDevice)), this,
          SLOT(on_favorite_audio_device_changed(DRAudioDevice)));

  // output
  // meta
  connect(w_close, SIGNAL(clicked()), this, SLOT(close()));
  connect(w_save, SIGNAL(clicked()), m_config, SLOT(save_file()));
  connect(w_autosave, SIGNAL(toggled(bool)), m_config, SLOT(set_autosave(bool)));

  // general
  connect(w_username, SIGNAL(editingFinished()), this, SLOT(username_editing_finished()));
  connect(w_callwords, SIGNAL(editingFinished()), this, SLOT(callwords_editing_finished()));
  connect(w_server_alerts, SIGNAL(toggled(bool)), m_config, SLOT(set_server_alerts(bool)));
  connect(w_discord_presence, SIGNAL(toggled(bool)), m_config, SLOT(set_discord_presence(bool)));
  connect(w_discord_hide_server, SIGNAL(toggled(bool)), m_config, SLOT(set_discord_hide_server(bool)));
  connect(w_discord_hide_character, SIGNAL(toggled(bool)), m_config, SLOT(set_discord_hide_character(bool)));

  // game
  connect(w_theme, SIGNAL(currentIndexChanged(QString)), m_config, SLOT(set_theme(QString)));
  connect(w_reload_theme, SIGNAL(clicked()), this, SLOT(on_reload_theme_clicked()));
  connect(w_gamemode, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_gamemode_index_changed(QString)));
  connect(w_manual_gamemode, SIGNAL(toggled(bool)), m_config, SLOT(set_manual_gamemode(bool)));
  connect(w_timeofday, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_timeofday_index_changed(QString)));
  connect(w_manual_timeofday, SIGNAL(toggled(bool)), m_config, SLOT(set_manual_timeofday(bool)));
  connect(w_showname, SIGNAL(editingFinished()), this, SLOT(showname_editing_finished()));
  connect(w_always_pre, SIGNAL(toggled(bool)), m_config, SLOT(set_always_pre(bool)));
  connect(w_chat_tick_interval, SIGNAL(valueChanged(int)), m_config, SLOT(set_chat_tick_interval(int)));

  // out, log
  connect(w_log_max_lines, SIGNAL(valueChanged(int)), m_config, SLOT(set_log_max_lines(int)));
  connect(w_log_display_timestamp, SIGNAL(toggled(bool)), m_config, SLOT(set_log_display_timestamp(bool)));
  connect(w_log_display_self_highlight, SIGNAL(toggled(bool)), m_config, SLOT(set_log_display_self_highlight(bool)));
  connect(w_log_format_use_newline, SIGNAL(toggled(bool)), m_config, SLOT(set_log_format_use_newline(bool)));
  connect(w_log_display_empty_messages, SIGNAL(toggled(bool)), m_config, SLOT(set_log_display_empty_messages(bool)));
  connect(w_log_display_music_switch, SIGNAL(toggled(bool)), m_config, SLOT(set_log_display_music_switch(bool)));
  connect(w_log_orientation_top_down, SIGNAL(toggled(bool)), m_config, SLOT(set_log_is_topdown(bool)));
  connect(w_log_is_recording, SIGNAL(toggled(bool)), m_config, SLOT(set_log_is_recording(bool)));
  connect(w_suppress_background_audio, SIGNAL(toggled(bool)), m_config, SLOT(set_suppress_background_audio(bool)));
  connect(w_device, SIGNAL(currentIndexChanged(int)), this, SLOT(on_device_current_index_changed(int)));
  connect(w_master, SIGNAL(valueChanged(int)), m_config, SLOT(set_master_volume(int)));
  connect(w_master, SIGNAL(valueChanged(int)), this, SLOT(on_master_value_changed(int)));
  connect(w_system, SIGNAL(valueChanged(int)), m_config, SLOT(set_system_volume(int)));
  connect(w_system, SIGNAL(valueChanged(int)), this, SLOT(on_system_value_changed(int)));
  connect(w_effect, SIGNAL(valueChanged(int)), m_config, SLOT(set_effect_volume(int)));
  connect(w_effect, SIGNAL(valueChanged(int)), this, SLOT(on_effect_value_changed(int)));
  connect(w_effect_ignore_suppression, SIGNAL(toggled(bool)), m_config, SLOT(set_effect_ignore_suppression(bool)));
  connect(w_music, SIGNAL(valueChanged(int)), m_config, SLOT(set_music_volume(int)));
  connect(w_music, SIGNAL(valueChanged(int)), this, SLOT(on_music_value_changed(int)));
  connect(w_music_ignore_suppression, SIGNAL(toggled(bool)), m_config, SLOT(set_music_ignore_suppression(bool)));
  connect(w_blip, SIGNAL(valueChanged(int)), m_config, SLOT(set_blip_volume(int)));
  connect(w_blip, SIGNAL(valueChanged(int)), this, SLOT(on_blip_value_changed(int)));
  connect(w_blip_ignore_suppression, SIGNAL(toggled(bool)), m_config, SLOT(set_blip_ignore_suppression(bool)));
  connect(w_blip_rate, SIGNAL(valueChanged(int)), m_config, SLOT(set_blip_rate(int)));
  connect(w_blank_blips, SIGNAL(toggled(bool)), m_config, SLOT(set_blank_blips(bool)));

  // set values
  // meta
  w_autosave->setChecked(m_config->autosave());

  // general
  w_username->setText(m_config->username());
  w_callwords->setText(m_config->callwords());
  w_server_alerts->setChecked(m_config->server_alerts_enabled());

  // game
  w_theme->setCurrentText(m_config->theme());
  w_gamemode->setCurrentText(m_config->gamemode());
  w_manual_gamemode->setChecked(m_config->manual_gamemode_enabled());
  w_timeofday->setCurrentText(m_config->timeofday());
  w_manual_timeofday->setChecked(m_config->manual_timeofday_enabled());
  w_showname->setText(m_config->showname());
  on_showname_placeholder_changed(m_config->showname_placeholder());
  w_always_pre->setChecked(m_config->always_pre_enabled());
  w_chat_tick_interval->setValue(m_config->chat_tick_interval());

  // log
  w_log_max_lines->setValue(m_config->log_max_lines());

  if (m_config->log_is_topdown_enabled())
  {
    w_log_orientation_top_down->setChecked(true);
  }
  else
  {
    w_log_orientation_bottom_up->setChecked(true);
  }

  w_log_display_timestamp->setChecked(m_config->log_display_timestamp_enabled());
  w_log_display_self_highlight->setChecked(m_config->log_display_self_highlight_enabled());
  w_log_format_use_newline->setChecked(m_config->log_format_use_newline_enabled());
  w_log_display_empty_messages->setChecked(m_config->log_display_empty_messages_enabled());
  w_log_display_music_switch->setChecked(m_config->log_display_music_switch_enabled());
  w_log_is_recording->setChecked(m_config->log_is_recording_enabled());

  w_discord_presence->setChecked(m_config->discord_presence());
  w_discord_hide_server->setChecked(m_config->discord_hide_server());
  w_discord_hide_character->setChecked(m_config->discord_hide_character());

  // audio
  update_audio_device_list();
  w_master->setValue(m_config->master_volume());
  w_suppress_background_audio->setChecked(m_config->suppress_background_audio());
  w_system->setValue(m_config->system_volume());
  w_effect->setValue(m_config->effect_volume());
  w_effect_ignore_suppression->setChecked(m_config->effect_ignore_suppression());
  w_music->setValue(m_config->music_volume());
  w_music_ignore_suppression->setChecked(m_config->music_ignore_suppression());
  w_blip->setValue(m_config->blip_volume());
  w_blip_ignore_suppression->setChecked(m_config->blip_ignore_suppression());
  w_blip_rate->setValue(m_config->blip_rate());
  w_blank_blips->setChecked(m_config->blank_blips_enabled());

  // Widget enabling connections
  w_gamemode->setEnabled(m_config->manual_gamemode_enabled());
  w_timeofday->setEnabled(m_config->manual_timeofday_enabled());
  // The manual gamemode checkbox enables browsing the gamemode combox
  // similarly with time of day
  connect(m_config, SIGNAL(manual_gamemode_changed(bool)), w_gamemode, SLOT(setEnabled(bool)));
  connect(m_config, SIGNAL(manual_timeofday_changed(bool)), w_timeofday, SLOT(setEnabled(bool)));
}

void AOConfigPanel::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);

  if (isVisible())
  {
    // refresh theme, gamemode and time of day comboboxes
    refresh_theme_list();
    refresh_gamemode_list();
    refresh_timeofday_list();
  }
}

void AOConfigPanel::refresh_theme_list()
{
  const QString p_prev_text = w_theme->currentText();

  // block signals
  w_theme->blockSignals(true);
  w_theme->clear();

  // themes
  const QString path = DRPather::get_application_path() + "/base/themes";
  const QStringList folders = QDir(ao_app->get_case_sensitive_path(path)).entryList(QDir::Dirs);
  for (const QString &i_folder : folders)
  {
    if (i_folder == "." || i_folder == "..")
      continue;
    w_theme->addItem(i_folder);
  }

  // restore previous selection
  w_theme->setCurrentText(p_prev_text);

  // unblock
  w_theme->blockSignals(false);
}

void AOConfigPanel::refresh_gamemode_list()
{
  const QString p_prev_text = w_gamemode->currentText();

  // block signals
  w_gamemode->blockSignals(true);
  w_gamemode->clear();

  // add empty entry indicating no gamemode chosen
  w_gamemode->addItem("<default>");
  // gamemodes
  QString path = DRPather::get_application_path()  + "/base/themes/" + m_config->theme() + "/gamemodes/";
  const QStringList folders = QDir(ao_app->get_case_sensitive_path(path)).entryList(QDir::Dirs);
  for (const QString &i_folder : folders)
  {
    if (i_folder == "." || i_folder == "..")
      continue;
    w_gamemode->addItem(i_folder, i_folder);
  }

  // restore previous selection
  w_gamemode->setCurrentText(p_prev_text);

  // unblock
  w_gamemode->blockSignals(false);
}

void AOConfigPanel::refresh_timeofday_list()
{
  const QString p_prev_text = w_timeofday->currentText();

  // block signals
  w_timeofday->blockSignals(true);
  w_timeofday->clear();

  // add empty entry indicating no time of day chosen
  w_timeofday->addItem("<default>");

  // decide path to look for times of day. This differs whether there is a
  // gamemode chosen or not
  QString path;
  if (m_config->gamemode().isEmpty())
    path = DRPather::get_application_path() + "/base/themes/" + m_config->theme() + "/times/";
  else
    path = DRPather::get_application_path() + "/base/themes/" + m_config->theme() + "/gamemodes/" +
           m_config->gamemode() + "/times/";

  // times of day
  const QStringList folders = QDir(ao_app->get_case_sensitive_path(path)).entryList(QDir::Dirs);
  for (const QString &i_folder : folders)
  {
    if (i_folder == "." || i_folder == "..")
      continue;
    w_timeofday->addItem(i_folder, i_folder);
  }

  // restore previous selection
  w_timeofday->setCurrentText(p_prev_text);

  // unblock
  w_timeofday->blockSignals(false);
}

void AOConfigPanel::update_audio_device_list()
{
  const auto current_device = m_engine->get_device();
  const auto favorite_device = m_engine->get_favorite_device();

  QSignalBlocker device_blocker(w_device);
  w_device->clear();

  std::optional<int> current_device_index;
  {
    for (DRAudioDevice &i_device : m_engine->get_device_list())
    {
      if (!i_device.is_enabled())
        continue;
      w_device->addItem(i_device.get_name(), i_device.get_driver());
      int item_index = w_device->count() - 1;

      if (current_device.has_value() && current_device.value().get_driver() == i_device.get_driver())
        current_device_index = item_index;

      if (favorite_device.has_value() && favorite_device.value().get_driver() == i_device.get_driver())
        w_device->setItemData(item_index, QColor(Qt::green), Qt::BackgroundRole);
    }

    const bool has_items(w_device->count());
    if (!has_items)
      w_device->addItem(tr("<no sound>"));
    w_device->setEnabled(has_items);
  }

  if (current_device_index.has_value())
    w_device->setCurrentIndex(current_device_index.value());
}

void AOConfigPanel::on_reload_theme_clicked()
{
  qDebug() << "reload theme clicked";
  Q_EMIT reload_theme();
}

void AOConfigPanel::on_gamemode_index_changed(QString p_text)
{
  Q_UNUSED(p_text);
  m_config->set_gamemode(w_gamemode->currentData().toString());
}

void AOConfigPanel::on_timeofday_index_changed(QString p_text)
{
  Q_UNUSED(p_text);
  m_config->set_timeofday(w_timeofday->currentData().toString());
}

void AOConfigPanel::on_showname_placeholder_changed(QString p_text)
{
  const QString l_showname(p_text.trimmed().isEmpty() ? "Showname" : p_text);
  w_showname->setPlaceholderText(l_showname);
  w_showname->setToolTip(l_showname);
}

void AOConfigPanel::on_log_is_topdown_changed(bool p_enabled)
{
  w_log_orientation_top_down->setChecked(p_enabled);
  w_log_orientation_bottom_up->setChecked(!p_enabled);
}

void AOConfigPanel::on_device_current_index_changed(int p_index)
{
  if (p_index == -1 || p_index >= w_device->count())
    return;

  const QString target_device_driver = w_device->itemData(p_index).toString();
  for (DRAudioDevice &i_device : m_engine->get_device_list())
    if (target_device_driver == i_device.get_driver())
    {
      m_engine->set_device(i_device);
      m_engine->set_favorite_device(i_device);
      m_config->set_favorite_device_driver(i_device.get_driver());
      break;
    }
}

void AOConfigPanel::on_audio_device_changed(DRAudioDevice p_device)
{
  Q_UNUSED(p_device)
  update_audio_device_list();
}

void AOConfigPanel::on_favorite_audio_device_changed(DRAudioDevice p_device)
{
  Q_UNUSED(p_device)
  update_audio_device_list();
}

void AOConfigPanel::on_audio_device_list_changed(QList<DRAudioDevice> p_device_list)
{
  Q_UNUSED(p_device_list);
  update_audio_device_list();
}

void AOConfigPanel::on_master_value_changed(int p_num)
{
  w_master_value->setText(QString::number(p_num) + "%");
}

void AOConfigPanel::on_system_value_changed(int p_num)
{
  w_system_value->setText(QString::number(p_num) + "%");
}

void AOConfigPanel::on_effect_value_changed(int p_num)
{
  w_effect_value->setText(QString::number(p_num) + "%");
}

void AOConfigPanel::on_music_value_changed(int p_num)
{
  w_music_value->setText(QString::number(p_num) + "%");
}

void AOConfigPanel::on_blip_value_changed(int p_num)
{
  w_blip_value->setText(QString::number(p_num) + "%");
}

void AOConfigPanel::username_editing_finished()
{
  m_config->set_username(w_username->text());
}

void AOConfigPanel::showname_editing_finished()
{
  m_config->set_showname(w_showname->text());
}

void AOConfigPanel::callwords_editing_finished()
{
  m_config->set_callwords(w_callwords->text());
}

void AOConfigPanel::on_config_reload_theme_requested()
{
  refresh_theme_list();
  refresh_gamemode_list();
  refresh_timeofday_list();
}
