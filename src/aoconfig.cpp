#include "aoconfig.h"

// qt
#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QVector>

/*!
    We have to suffer through a lot of boilerplate code
    but hey, when has ao2 ever cared?
    Wait, am I using the term wrong? Nice.
*/
class AOConfigPrivate : public QObject
{
  Q_OBJECT

  friend AOConfig;

  QSettings cfg;
  // hate me more
  QVector<QObject *> parents;

  // data
  bool autosave;
  QString username;
  QString callwords;
  QString theme;
  QString gamemode;
  bool manual_gamemode;
  QString timeofday;
  bool manual_timeofday;
  bool server_alerts;
  bool always_pre;
  int chat_tick_interval;
  int log_max_lines;
  bool log_is_topdown;
  bool log_uses_newline;
  bool log_music;
  bool log_is_recording;
  int effects_volume;
  int system_volume;
  int music_volume;
  int blips_volume;
  int blip_rate;
  bool blank_blips;

public:
  AOConfigPrivate()
      : QObject(qApp),
        cfg(QDir::currentPath() + "/base/config.ini", QSettings::IniFormat)
  {
    read_file();
  }
  ~AOConfigPrivate()
  {
    if (autosave)
    {
      save_file();
    }
  }

  // setters
public slots:
  void set_autosave(bool p_enabled)
  {
    if (autosave == p_enabled)
      return;
    autosave = p_enabled;
    invoke_parents("autosave_changed", Q_ARG(bool, p_enabled));
  }
  void set_username(QString p_string)
  {
    if (username == p_string)
      return;
    username = p_string;
    invoke_parents("username_changed", Q_ARG(QString, p_string));
  }
  void set_callwords(QString p_string)
  {
    if (callwords == p_string)
      return;
    callwords = p_string;
    invoke_parents("callwords_changed", Q_ARG(QString, p_string));
  }
  void set_server_alerts(bool p_enabled)
  {
    if (server_alerts == p_enabled)
      return;
    server_alerts = p_enabled;
    invoke_parents("server_alerts_changed", Q_ARG(bool, p_enabled));
  }
  void set_theme(QString p_string)
  {
    if (theme == p_string)
      return;
    theme = p_string;
    invoke_parents("theme_changed", Q_ARG(QString, p_string));
  }
  void set_gamemode(QString p_string)
  {
    if (gamemode == p_string)
      return;
    gamemode = p_string;
    invoke_parents("gamemode_changed", Q_ARG(QString, p_string));
  }
  void set_manual_gamemode(bool p_enabled)
  {
    if (manual_gamemode == p_enabled)
      return;
    manual_gamemode = p_enabled;
    invoke_parents("manual_gamemode_changed", Q_ARG(bool, p_enabled));
  }
  void set_timeofday(QString p_string)
  {
    if (timeofday == p_string)
      return;
    timeofday = p_string;
    invoke_parents("timeofday_changed", Q_ARG(QString, p_string));
  }
  void set_manual_timeofday(bool p_enabled)
  {
    if (manual_timeofday == p_enabled)
      return;
    manual_timeofday = p_enabled;
    invoke_parents("manual_timeofday_changed", Q_ARG(bool, p_enabled));
  }
  void set_always_pre(bool p_enabled)
  {
    if (always_pre == p_enabled)
      return;
    always_pre = p_enabled;
    invoke_parents("always_pre_changed", Q_ARG(bool, p_enabled));
  }
  void set_chat_tick_interval(int p_number)
  {
    if (chat_tick_interval == p_number)
      return;
    chat_tick_interval = p_number;
    invoke_parents("chat_tick_interval_changed", Q_ARG(int, p_number));
  }
  void set_log_max_lines(int p_number)
  {
    if (log_max_lines == p_number)
      return;
    log_max_lines = p_number;
    invoke_parents("log_max_lines_changed", Q_ARG(int, p_number));
  }
  void set_log_is_topdown(bool p_enabled)
  {
    if (log_is_topdown == p_enabled)
      return;
    log_is_topdown = p_enabled;
    invoke_parents("log_is_topdown_changed", Q_ARG(bool, p_enabled));
  }
  void set_log_uses_newline(bool p_enabled)
  {
    if (log_uses_newline == p_enabled)
      return;
    log_uses_newline = p_enabled;
    invoke_parents("log_uses_newline_changed", Q_ARG(bool, p_enabled));
  }
  void set_log_music(bool p_enabled)
  {
    if (log_music == p_enabled)
      return;
    log_music = p_enabled;
    invoke_parents("log_music_changed", Q_ARG(bool, p_enabled));
  }
  void set_log_is_recording(bool p_enabled)
  {
    if (log_is_recording == p_enabled)
      return;
    log_is_recording = p_enabled;
    invoke_parents("log_is_recording_changed", Q_ARG(bool, p_enabled));
  }
  void set_effects_volume(int p_number)
  {
    if (effects_volume == p_number)
      return;
    effects_volume = p_number;
    invoke_parents("effects_volume_changed", Q_ARG(int, p_number));
  }
  void set_system_volume(int p_number)
  {
    if (system_volume == p_number)
      return;
    system_volume = p_number;
    invoke_parents("system_volume_changed", Q_ARG(int, p_number));
  }
  void set_music_volume(int p_number)
  {
    if (music_volume == p_number)
      return;
    music_volume = p_number;
    invoke_parents("music_volume_changed", Q_ARG(int, p_number));
  }
  void set_blips_volume(int p_number)
  {
    if (blips_volume == p_number)
      return;
    blips_volume = p_number;
    invoke_parents("blips_volume_changed", Q_ARG(int, p_number));
  }
  void set_blip_rate(int p_number)
  {
    if (blip_rate == p_number)
      return;
    blip_rate = p_number;
    invoke_parents("blip_rate_changed", Q_ARG(int, p_number));
  }
  void set_blank_blips(bool p_enabled)
  {
    if (blank_blips == p_enabled)
      return;
    blank_blips = p_enabled;
    invoke_parents("blank_blips_changed", Q_ARG(bool, p_enabled));
  }
  void read_file()
  {
    autosave = cfg.value("autosave", true).toBool();
    username = cfg.value("username").toString();
    callwords = cfg.value("callwords").toString();
    server_alerts = cfg.value("server_alerts", true).toBool();

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
    log_uses_newline = cfg.value("chatlog_newline", false).toBool();
    log_music = cfg.value("music_change_log", true).toBool();
    log_is_recording = cfg.value("enable_logging", true).toBool();
    effects_volume = cfg.value("default_sfx", 50).toInt();
    system_volume = cfg.value("default_system", 50).toInt();
    music_volume = cfg.value("default_music", 50).toInt();
    blips_volume = cfg.value("default_blip", 50).toInt();
    blip_rate = cfg.value("blip_rate", 1000000000).toInt();
    blank_blips = cfg.value("blank_blips").toBool();
  }
  void save_file()
  {
    cfg.setValue("autosave", autosave);
    cfg.setValue("username", username);
    cfg.setValue("callwords", callwords);
    cfg.setValue("server_alerts", server_alerts);
    cfg.setValue("theme", theme);
    cfg.setValue("gamemode", gamemode);
    cfg.setValue("manual_gamemode", manual_gamemode);
    cfg.setValue("timeofday", timeofday);
    cfg.setValue("manual_timeofday", manual_timeofday);
    cfg.setValue("always_pre", always_pre);
    cfg.setValue("chat_tick_interval", chat_tick_interval);
    cfg.setValue("chatlog_limit", log_max_lines);
    cfg.setValue("chatlog_scrolldown", log_is_topdown);
    cfg.setValue("chatlog_newline", log_uses_newline);
    cfg.setValue("music_change_log", log_music);
    cfg.setValue("enable_logging", log_is_recording);
    cfg.setValue("default_sfx", effects_volume);
    cfg.setValue("default_system", system_volume);
    cfg.setValue("default_music", music_volume);
    cfg.setValue("default_blip", blips_volume);
    cfg.setValue("blip_rate", blip_rate);
    cfg.setValue("blank_blips", blank_blips);
    cfg.sync();
  }

private:
  void invoke_parents(QString p_method_name,
                      QGenericArgument p_arg1 = QGenericArgument(nullptr))
  {
    for (QObject *i_parent : parents)
    {
      QMetaObject::invokeMethod(i_parent, p_method_name.toStdString().c_str(),
                                p_arg1);
    }
  }
};

/*!
 * private classes are cool
 */
static AOConfigPrivate *d = nullptr;

AOConfig::AOConfig(QObject *p_parent) : QObject(p_parent)
{
  // init if not created yet
  if (d == nullptr)
  {
    d = new AOConfigPrivate;
  }

  // ao2 is the pinnacle of thread security
  d->parents.append(this);
}

AOConfig::~AOConfig()
{
  // totally safe!
  d->parents.removeAll(this);
}

QString AOConfig::get_string(QString p_name, QString p_default)
{
  return d->cfg.value(p_name, p_default).toString();
}

bool AOConfig::get_bool(QString p_name, bool p_default)
{
  return d->cfg.value(p_name, p_default).toBool();
}

int AOConfig::get_number(QString p_name, int p_default)
{
  return d->cfg.value(p_name, p_default).toInt();
}

bool AOConfig::autosave()
{
  return d->autosave;
}

QString AOConfig::username()
{
  return d->username;
}

QString AOConfig::callwords()
{
  return d->callwords;
}

QString AOConfig::theme()
{
  return d->theme;
}

QString AOConfig::gamemode()
{
  return d->gamemode;
}

bool AOConfig::manual_gamemode_enabled()
{
  return d->manual_gamemode;
}

QString AOConfig::timeofday()
{
  return d->timeofday;
}

bool AOConfig::manual_timeofday_enabled()
{
  return d->manual_timeofday;
}

bool AOConfig::server_alerts_enabled()
{
  return d->server_alerts;
}

bool AOConfig::always_pre_enabled()
{
  return d->always_pre;
}

int AOConfig::chat_tick_interval()
{
  return d->chat_tick_interval;
}
int AOConfig::log_max_lines()
{
  return d->log_max_lines;
}

bool AOConfig::log_is_topdown_enabled()
{
  return d->log_is_topdown;
}

bool AOConfig::log_uses_newline_enabled()
{
  return d->log_uses_newline;
}

bool AOConfig::log_music_enabled()
{
  return d->log_music;
}

bool AOConfig::log_is_recording_enabled()
{
  return d->log_is_recording;
}

int AOConfig::effects_volume()
{
  return d->effects_volume;
}

int AOConfig::system_volume()
{
  return d->system_volume;
}

int AOConfig::music_volume()
{
  return d->music_volume;
}

int AOConfig::blips_volume()
{
  return d->blips_volume;
}

int AOConfig::blip_rate()
{
  return d->blip_rate;
}

bool AOConfig::blank_blips_enabled()
{
  return d->blank_blips;
}

void AOConfig::set_autosave(bool p_enabled)
{
  d->set_autosave(p_enabled);
}

void AOConfig::set_autosave(int p_state)
{
  set_autosave(p_state == Qt::Checked);
}

void AOConfig::set_username(QString p_string)
{
  d->set_username(p_string);
}

void AOConfig::set_callwords(QString p_string)
{
  d->set_callwords(p_string);
}

void AOConfig::set_theme(QString p_string)
{
  d->set_theme(p_string);
}

void AOConfig::set_gamemode(QString p_string)
{
  d->set_gamemode(p_string);
}

void AOConfig::set_manual_gamemode(int p_state)
{
  set_manual_gamemode(p_state == Qt::Checked);
}

void AOConfig::set_manual_gamemode(bool p_enabled)
{
  d->set_manual_gamemode(p_enabled);
}

void AOConfig::set_timeofday(QString p_string)
{
  d->set_timeofday(p_string);
}

void AOConfig::set_manual_timeofday(int p_state)
{
  set_manual_timeofday(p_state == Qt::Checked);
}

void AOConfig::set_manual_timeofday(bool p_enabled)
{
  d->set_manual_timeofday(p_enabled);
}

void AOConfig::set_server_alerts(int p_state)
{
  set_server_alerts(p_state == Qt::Checked);
}

void AOConfig::set_server_alerts(bool p_enabled)
{
  d->set_server_alerts(p_enabled);
}

void AOConfig::set_always_pre(int p_state)
{
  set_always_pre(p_state == Qt::Checked);
}

void AOConfig::set_always_pre(bool p_enabled)
{
  d->set_always_pre(p_enabled);
}

void AOConfig::set_chat_tick_interval(int p_number)
{
  d->set_chat_tick_interval(p_number);
}

void AOConfig::set_log_max_lines(int p_number)
{
  d->set_log_max_lines(p_number);
}

void AOConfig::set_log_is_topdown(bool p_enabled)
{
  d->set_log_is_topdown(p_enabled);
}

void AOConfig::set_log_is_topdown(int p_state)
{
  set_log_is_topdown(p_state == Qt::Checked);
}

void AOConfig::set_log_uses_newline(bool p_enabled)
{
  d->set_log_uses_newline(p_enabled);
}

void AOConfig::set_log_uses_newline(int p_state)
{
  set_log_uses_newline(p_state == Qt::Checked);
}

void AOConfig::set_log_music(bool p_enabled)
{
  d->set_log_music(p_enabled);
}

void AOConfig::set_log_music(int p_state)
{
  set_log_music(p_state == Qt::Checked);
}

void AOConfig::set_log_is_recording(bool p_enabled)
{
  d->set_log_is_recording(p_enabled);
}

void AOConfig::set_log_is_recording(int p_state)
{
  set_log_is_recording(p_state == Qt::Checked);
}

void AOConfig::set_effects_volume(int p_number)
{
  d->set_effects_volume(p_number);
}

void AOConfig::set_system_volume(int p_number)
{
  d->set_system_volume(p_number);
}

void AOConfig::set_music_volume(int p_number)
{
  d->set_music_volume(p_number);
}

void AOConfig::set_blips_volume(int p_number)
{
  d->set_blips_volume(p_number);
}

void AOConfig::set_blip_rate(int p_number)
{
  d->set_blip_rate(p_number);
}

void AOConfig::set_blank_blips(bool p_enabled)
{
  d->set_blank_blips(p_enabled);
}

void AOConfig::set_blank_blips(int p_state)
{
  set_blank_blips(p_state == Qt::Checked);
}

void AOConfig::save_file()
{
  d->save_file();
}

// moc
#include "aoconfig.moc"
