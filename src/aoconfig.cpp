#include "aoconfig.h"
#include "datatypes.h"
#include "draudioengine.h"
#include "drpather.h"

// qt
#include <QApplication>
#include <QDebug>
#include <QPointer>
#include <QSettings>
#include <QSharedPointer>
#include <QVector>

/*!
    We have to suffer through a lot of boilerplate code
    but hey, when has ao2 ever cared?
    Wait, am I using the term wrong? Nice.
*/
class AOConfigPrivate : public QObject
{
  Q_OBJECT

public:
  AOConfigPrivate();
  ~AOConfigPrivate();

  // setters
public slots:
  void read_file();
  void save_file();

private:
  void invoke_signal(QString p_method_name, QGenericArgument p_arg1 = QGenericArgument(nullptr));
  void update_favorite_device();

private slots:
  void on_application_state_changed(Qt::ApplicationState p_state);

private:
  friend class AOConfig;

  QSettings cfg;
  // hate me more
  QVector<QObject *> children;

  // data
  bool autosave;
  QString username;
  QString callwords;
  bool server_alerts;
  bool discord_presence = false;
  bool discord_hide_server = false;
  bool discord_hide_character = false;
  QString theme;
  QString gamemode;
  bool manual_gamemode;
  QString timeofday;
  bool manual_timeofday;
  QString showname;
  QString showname_placeholder;
  bool always_pre;
  int chat_tick_interval;
  int log_max_lines;
  bool log_display_timestamp;
  bool log_display_self_highlight;
  bool log_display_empty_messages;
  bool log_is_topdown;
  bool log_format_use_newline;
  bool log_display_music_switch;
  bool log_is_recording;

  // audio
  std::optional<QString> favorite_device_driver;
  int master_volume;
  bool suppress_background_audio;
  int system_volume;
  int effect_volume;
  bool effect_ignore_suppression;
  int music_volume;
  bool music_ignore_suppression;
  int blip_volume;
  bool blip_ignore_suppression;
  int blip_rate;
  bool blank_blips;

  // audio sync
  DRAudioEngine *audio_engine = nullptr;
};

AOConfigPrivate::AOConfigPrivate()
    : QObject(nullptr), cfg(DRPather::get_application_path() + "/base/config.ini", QSettings::IniFormat),
      audio_engine(new DRAudioEngine(this))
{
  Q_ASSERT_X(qApp, "initialization", "QGuiApplication is required");
  connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this,
          SLOT(on_application_state_changed(Qt::ApplicationState)));

  read_file();
}

AOConfigPrivate::~AOConfigPrivate()
{
  if (autosave)
  {
    save_file();
  }
}

void AOConfigPrivate::read_file()
{
  autosave = cfg.value("autosave", true).toBool();
  username = cfg.value("username").toString();
  showname = cfg.value("showname").toString();
  callwords = cfg.value("callwords").toString();
  server_alerts = cfg.value("server_alerts", true).toBool();

  discord_presence = cfg.value("discord_presence", true).toBool();
  discord_hide_server = cfg.value("discord_hide_server", false).toBool();
  discord_hide_character = cfg.value("discord_hide_character", false).toBool();

  theme = cfg.value("theme").toString();
  if (theme.trimmed().isEmpty())
    theme = "default";

  gamemode = cfg.value("gamemode").toString();
  manual_gamemode = cfg.value("manual_gamemode", false).toBool();
  timeofday = cfg.value("timeofday", "").toString();
  manual_timeofday = cfg.value("manual_timeofday", false).toBool();
  always_pre = cfg.value("always_pre", true).toBool();
  chat_tick_interval = cfg.value("chat_tick_interval", 60).toInt();
  log_max_lines = cfg.value("chatlog_limit", 200).toInt();
  log_is_topdown = cfg.value("chatlog_scrolldown", true).toBool();
  log_display_timestamp = cfg.value("chatlog_display_timestamp", true).toBool();
  log_display_self_highlight = cfg.value("chatlog_display_self_highlight", true).toBool();
  log_display_empty_messages = cfg.value("chatlog_display_empty_messages", false).toBool();
  log_format_use_newline = cfg.value("chatlog_newline", false).toBool();
  log_display_music_switch = cfg.value("music_change_log", true).toBool();
  log_is_recording = cfg.value("enable_logging", true).toBool();

  if (cfg.contains("favorite_device_driver"))
    favorite_device_driver = cfg.value("favorite_device_driver").toString();

  suppress_background_audio = cfg.value("suppress_background_audio").toBool();
  master_volume = cfg.value("default_master", 50).toInt();
  system_volume = cfg.value("default_system", 50).toInt();
  effect_volume = cfg.value("default_sfx", 50).toInt();
  effect_ignore_suppression = cfg.value("effect_ignore_suppression", false).toBool();
  music_volume = cfg.value("default_music", 50).toInt();
  music_ignore_suppression = cfg.value("music_ignore_suppression", false).toBool();
  blip_volume = cfg.value("default_blip", 50).toInt();
  blip_ignore_suppression = cfg.value("blip_ignore_suppression", false).toBool();
  blip_rate = cfg.value("blip_rate", 1000000000).toInt();
  blank_blips = cfg.value("blank_blips").toBool();

  // audio update
  audio_engine->set_volume(master_volume);
  audio_engine->get_family(DRAudio::Family::FSystem)->set_volume(system_volume);
  audio_engine->get_family(DRAudio::Family::FEffect)->set_volume(effect_volume);
  audio_engine->get_family(DRAudio::Family::FEffect)->set_ignore_suppression(effect_ignore_suppression);
  audio_engine->get_family(DRAudio::Family::FMusic)->set_volume(music_volume);
  audio_engine->get_family(DRAudio::Family::FMusic)->set_ignore_suppression(effect_ignore_suppression);
  audio_engine->get_family(DRAudio::Family::FBlip)->set_volume(blip_volume);
  audio_engine->get_family(DRAudio::Family::FBlip)->set_ignore_suppression(effect_ignore_suppression);

  // audio device
  update_favorite_device();
}

void AOConfigPrivate::save_file()
{
  cfg.setValue("autosave", autosave);
  cfg.setValue("username", username);
  cfg.setValue("showname", showname);
  cfg.setValue("callwords", callwords);
  cfg.setValue("server_alerts", server_alerts);

  cfg.setValue("discord_presence", discord_presence);
  cfg.setValue("discord_hide_server", discord_hide_server);
  cfg.setValue("discord_hide_character", discord_hide_character);

  cfg.setValue("theme", theme);
  cfg.setValue("gamemode", gamemode);
  cfg.setValue("manual_gamemode", manual_gamemode);
  cfg.setValue("timeofday", timeofday);
  cfg.setValue("manual_timeofday", manual_timeofday);
  cfg.setValue("always_pre", always_pre);
  cfg.setValue("chat_tick_interval", chat_tick_interval);
  cfg.setValue("chatlog_limit", log_max_lines);
  cfg.setValue("chatlog_display_timestamp", log_display_timestamp);
  cfg.setValue("chatlog_display_self_highlight", log_display_self_highlight);
  cfg.setValue("chatlog_newline", log_format_use_newline);
  cfg.setValue("chatlog_display_empty_messages", log_display_empty_messages);
  cfg.setValue("music_change_log", log_display_music_switch);
  cfg.setValue("chatlog_scrolldown", log_is_topdown);
  cfg.setValue("enable_logging", log_is_recording);

  // audio
  if (favorite_device_driver.has_value())
    cfg.setValue("favorite_device_driver", favorite_device_driver.value());

  cfg.setValue("suppress_background_audio", suppress_background_audio);
  cfg.setValue("default_master", master_volume);
  cfg.setValue("default_system", system_volume);
  cfg.setValue("default_sfx", effect_volume);
  cfg.setValue("effect_ignore_suppression", effect_ignore_suppression);
  cfg.setValue("default_music", music_volume);
  cfg.setValue("music_ignore_suppression", music_ignore_suppression);
  cfg.setValue("default_blip", blip_volume);
  cfg.setValue("blip_ignore_suppression", blip_ignore_suppression);
  cfg.setValue("blip_rate", blip_rate);
  cfg.setValue("blank_blips", blank_blips);
  cfg.sync();
}

void AOConfigPrivate::invoke_signal(QString p_method_name, QGenericArgument p_arg1)
{
  for (QObject *i_child : qAsConst(children))
  {
    QMetaObject::invokeMethod(i_child, p_method_name.toStdString().c_str(), p_arg1);
  }
}

void AOConfigPrivate::update_favorite_device()
{
  if (!favorite_device_driver.has_value())
    return;
  audio_engine->set_favorite_device_by_driver(favorite_device_driver.value());
}

void AOConfigPrivate::on_application_state_changed(Qt::ApplicationState p_state)
{
  audio_engine->set_suppressed(suppress_background_audio && p_state != Qt::ApplicationActive);
}

// AOConfig ////////////////////////////////////////////////////////////////////

/*!
 * private classes are cool
 */
namespace
{
static QSharedPointer<AOConfigPrivate> d;
}

AOConfig::AOConfig(QObject *p_parent) : QObject(p_parent)
{
  // init if not created yet
  if (d == nullptr)
  {
    Q_ASSERT_X(qApp, "initialization", "QGuiApplication is required");
    d = QSharedPointer<AOConfigPrivate>(new AOConfigPrivate);
  }

  // ao2 is the pinnacle of thread security
  d->children.append(this);
}

AOConfig::~AOConfig()
{
  // totally safe!
  d->children.removeAll(this);
}

QString AOConfig::get_string(QString p_name, QString p_default) const
{
  return d->cfg.value(p_name, p_default).toString();
}

bool AOConfig::get_bool(QString p_name, bool p_default) const
{
  return d->cfg.value(p_name, p_default).toBool();
}

int AOConfig::get_number(QString p_name, int p_default) const
{
  return d->cfg.value(p_name, p_default).toInt();
}

bool AOConfig::autosave() const
{
  return d->autosave;
}

QString AOConfig::username() const
{
  return d->username;
}

QString AOConfig::showname() const
{
  return d->showname;
}

QString AOConfig::showname_placeholder() const
{
  return d->showname_placeholder;
}

QString AOConfig::callwords() const
{
  return d->callwords;
}

bool AOConfig::server_alerts_enabled() const
{
  return d->server_alerts;
}

bool AOConfig::discord_presence() const
{
  return d->discord_presence;
}

bool AOConfig::discord_hide_server() const
{
  return d->discord_hide_server;
}

bool AOConfig::discord_hide_character() const
{
  return d->discord_hide_character;
}

/**
 * @brief Return the current theme name.
 * @return Name of current theme.
 */
QString AOConfig::theme() const
{
  return d->theme;
}

/**
 * @brief Return the current gamemode. If no gamemode is set, return the
 * empty string.
 *
 * @return Current gamemode, or empty string if not set.
 */
QString AOConfig::gamemode() const
{
  return d->gamemode;
}

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
bool AOConfig::manual_gamemode_enabled() const
{
  return d->manual_gamemode;
}

/**
 * @brief Returns the current time of day. If no time of day is set, return
 * the empty string.
 *
 * @return Current gamemode, or empty string if not set.
 */
QString AOConfig::timeofday() const
{
  return d->timeofday;
}

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
bool AOConfig::manual_timeofday_enabled() const
{
  return d->manual_timeofday;
}

bool AOConfig::always_pre_enabled() const
{
  return d->always_pre;
}

int AOConfig::chat_tick_interval() const
{
  return d->chat_tick_interval;
}

int AOConfig::log_max_lines() const
{
  return d->log_max_lines;
}

bool AOConfig::log_display_timestamp_enabled() const
{
  return d->log_display_timestamp;
}

bool AOConfig::log_display_self_highlight_enabled() const
{
  return d->log_display_self_highlight;
}

bool AOConfig::log_display_empty_messages_enabled() const
{
  return d->log_display_empty_messages;
}

bool AOConfig::log_is_topdown_enabled() const
{
  return d->log_is_topdown;
}

bool AOConfig::log_format_use_newline_enabled() const
{
  return d->log_format_use_newline;
}

bool AOConfig::log_display_music_switch_enabled() const
{
  return d->log_display_music_switch;
}

bool AOConfig::log_is_recording_enabled() const
{
  return d->log_is_recording;
}

std::optional<QString> AOConfig::favorite_device_driver() const
{
  return d->favorite_device_driver;
}

int AOConfig::master_volume() const
{
  return d->master_volume;
}

bool AOConfig::suppress_background_audio() const
{
  return d->suppress_background_audio;
}

int AOConfig::system_volume() const
{
  return d->system_volume;
}
int AOConfig::effect_volume() const
{
  return d->effect_volume;
}

bool AOConfig::effect_ignore_suppression() const
{
  return d->effect_ignore_suppression;
}

int AOConfig::music_volume() const
{
  return d->music_volume;
}

bool AOConfig::music_ignore_suppression() const
{
  return d->music_ignore_suppression;
}

int AOConfig::blip_volume() const
{
  return d->blip_volume;
}

bool AOConfig::blip_ignore_suppression() const
{
  return d->blip_ignore_suppression;
}

int AOConfig::blip_rate() const
{
  return d->blip_rate;
}

bool AOConfig::blank_blips_enabled() const
{
  return d->blank_blips;
}

void AOConfig::save_file()
{
  d->save_file();
}

void AOConfig::set_autosave(bool p_enabled)
{
  if (d->autosave == p_enabled)
    return;
  d->autosave = p_enabled;
  d->invoke_signal("autosave_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_username(QString p_string)
{
  if (d->username == p_string)
    return;
  d->username = p_string;
  d->invoke_signal("username_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_showname(QString p_string)
{
  if (d->showname == p_string)
    return;
  d->showname = p_string;
  d->invoke_signal("showname_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_showname_placeholder(QString p_string)
{
  if (d->showname_placeholder == p_string)
    return;
  d->showname_placeholder = p_string;
  d->invoke_signal("showname_placeholder_changed", Q_ARG(QString, p_string));
}

void AOConfig::clear_showname_placeholder()
{
  set_showname_placeholder(nullptr);
}

void AOConfig::set_callwords(QString p_string)
{
  if (d->callwords == p_string)
    return;
  d->callwords = p_string;
  d->invoke_signal("callwords_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_server_alerts(bool p_enabled)
{
  if (d->server_alerts == p_enabled)
    return;
  d->server_alerts = p_enabled;
  d->invoke_signal("server_alerts_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_discord_presence(const bool p_enabled)
{
  if (d->discord_presence == p_enabled)
    return;
  d->discord_presence = p_enabled;
  Q_EMIT d->invoke_signal("discord_presence_changed", Q_ARG(bool, d->discord_presence));
}

void AOConfig::set_discord_hide_server(const bool p_enabled)
{
  if (d->discord_hide_server == p_enabled)
    return;
  d->discord_hide_server = p_enabled;
  Q_EMIT d->invoke_signal("discord_hide_server_changed", Q_ARG(bool, d->discord_hide_server));
}

void AOConfig::set_discord_hide_character(const bool p_enabled)
{
  if (d->discord_hide_character == p_enabled)
    return;
  d->discord_hide_character = p_enabled;
  Q_EMIT d->invoke_signal("discord_hide_character_changed", Q_ARG(bool, d->discord_hide_character));
}

void AOConfig::set_theme(QString p_string)
{
  if (d->theme == p_string)
    return;
  d->theme = p_string;
  d->invoke_signal("theme_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_gamemode(QString p_string)
{
  if (d->gamemode == p_string)
    return;
  d->gamemode = p_string;
  d->invoke_signal("gamemode_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_manual_gamemode(bool p_enabled)
{
  if (d->manual_gamemode == p_enabled)
    return;
  d->manual_gamemode = p_enabled;
  d->invoke_signal("manual_gamemode_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_timeofday(QString p_string)
{
  if (d->timeofday == p_string)
    return;
  d->timeofday = p_string;
  d->invoke_signal("timeofday_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_manual_timeofday(bool p_enabled)
{
  if (d->manual_timeofday == p_enabled)
    return;
  d->manual_timeofday = p_enabled;
  d->invoke_signal("manual_timeofday_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_always_pre(bool p_enabled)
{
  if (d->always_pre == p_enabled)
    return;
  d->always_pre = p_enabled;
  d->invoke_signal("always_pre_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_chat_tick_interval(int p_number)
{
  if (d->chat_tick_interval == p_number)
    return;
  d->chat_tick_interval = p_number;
  d->invoke_signal("chat_tick_interval_changed", Q_ARG(int, p_number));
}

void AOConfig::set_log_max_lines(int p_number)
{
  if (d->log_max_lines == p_number)
    return;
  d->log_max_lines = p_number;
  d->invoke_signal("log_max_lines_changed", Q_ARG(int, p_number));
}

void AOConfig::set_log_display_timestamp(bool p_enabled)
{
  if (d->log_display_timestamp == p_enabled)
    return;
  d->log_display_timestamp = p_enabled;
  d->invoke_signal("log_display_timestamp_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_display_self_highlight(bool p_enabled)
{
  if (d->log_display_self_highlight == p_enabled)
    return;
  d->log_display_self_highlight = p_enabled;
  d->invoke_signal("log_display_self_highlight_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_display_empty_messages(bool p_enabled)
{
  if (d->log_display_empty_messages == p_enabled)
    return;
  d->log_display_empty_messages = p_enabled;
  d->invoke_signal("log_display_empty_messages_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_format_use_newline(bool p_enabled)
{
  if (d->log_format_use_newline == p_enabled)
    return;
  d->log_format_use_newline = p_enabled;
  d->invoke_signal("log_format_use_newline_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_is_topdown(bool p_enabled)
{
  if (d->log_is_topdown == p_enabled)
    return;
  d->log_is_topdown = p_enabled;
  d->invoke_signal("log_is_topdown_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_display_music_switch(bool p_enabled)
{
  if (d->log_display_music_switch == p_enabled)
    return;
  d->log_display_music_switch = p_enabled;
  d->invoke_signal("log_display_music_switch_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_is_recording(bool p_enabled)
{
  if (d->log_is_recording == p_enabled)
    return;
  d->log_is_recording = p_enabled;
  d->invoke_signal("log_is_recording_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_master_volume(int p_number)
{
  if (d->master_volume == p_number)
    return;
  d->master_volume = p_number;
  d->audio_engine->set_volume(p_number);
  d->invoke_signal("master_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_suppress_background_audio(bool p_enabled)
{
  if (d->suppress_background_audio == p_enabled)
    return;
  d->suppress_background_audio = p_enabled;
  d->invoke_signal("suppress_background_audio_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_favorite_device_driver(QString p_device_driver)
{
  if (d->favorite_device_driver.has_value() && d->favorite_device_driver.value() == p_device_driver)
    return;
  d->favorite_device_driver = p_device_driver;
  d->update_favorite_device();
  d->invoke_signal("favorite_device_changed", Q_ARG(QString, p_device_driver));
}

void AOConfig::set_system_volume(int p_number)
{
  if (d->system_volume == p_number)
    return;
  d->system_volume = p_number;
  d->audio_engine->get_family(DRAudio::Family::FSystem)->set_volume(p_number);
  d->invoke_signal("system_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_effect_volume(int p_number)
{
  if (d->effect_volume == p_number)
    return;
  d->effect_volume = p_number;
  d->audio_engine->get_family(DRAudio::Family::FEffect)->set_volume(p_number);
  d->invoke_signal("effect_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_effect_ignore_suppression(bool p_enabled)
{
  if (d->music_ignore_suppression == p_enabled)
    return;
  d->music_ignore_suppression = p_enabled;
  d->audio_engine->get_family(DRAudio::Family::FMusic)->set_ignore_suppression(p_enabled);
  d->invoke_signal("music_ignore_suppression_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_music_volume(int p_number)
{
  if (d->music_volume == p_number)
    return;
  d->music_volume = p_number;
  d->audio_engine->get_family(DRAudio::Family::FMusic)->set_volume(p_number);
  d->invoke_signal("music_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_music_ignore_suppression(bool p_enabled)
{
  if (d->music_ignore_suppression == p_enabled)
    return;
  d->music_ignore_suppression = p_enabled;
  d->audio_engine->get_family(DRAudio::Family::FMusic)->set_ignore_suppression(p_enabled);
  d->invoke_signal("music_ignore_suppression_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_blip_volume(int p_number)
{
  if (d->blip_volume == p_number)
    return;
  d->blip_volume = p_number;
  d->audio_engine->get_family(DRAudio::Family::FBlip)->set_volume(p_number);
  d->invoke_signal("blip_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_blip_ignore_suppression(bool p_enabled)
{
  if (d->blip_ignore_suppression == p_enabled)
    return;
  d->blip_ignore_suppression = p_enabled;
  d->audio_engine->get_family(DRAudio::Family::FBlip)->set_ignore_suppression(p_enabled);
  d->invoke_signal("blip_ignore_suppression_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_blip_rate(int p_number)
{
  if (d->blip_rate == p_number)
    return;
  d->blip_rate = p_number;
  d->invoke_signal("blip_rate_changed", Q_ARG(int, p_number));
}

void AOConfig::set_blank_blips(bool p_enabled)
{
  if (d->blank_blips == p_enabled)
    return;
  d->blank_blips = p_enabled;
  d->invoke_signal("blank_blips_changed", Q_ARG(bool, p_enabled));
}

// moc
#include "aoconfig.moc"
