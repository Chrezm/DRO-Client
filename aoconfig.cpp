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
    QString username;
    QString callwords;
    QString theme;
    bool always_pre;
    int chat_tick_interval;
    int log_max_lines;
    bool log_goes_downward;
    bool log_uses_newline;
    bool log_music;
    bool log_is_recording;
    int effects_volume;
    int system_volume;
    int music_volume;
    int blips_volume;
    int blip_rate;
    int blank_blips;

public:
    AOConfigPrivate() : QObject(qApp), cfg(QDir::currentPath() + "/base/config.ini", QSettings::IniFormat)
    {
        read_file();
    }
    ~AOConfigPrivate()
    {
        save_file();
    }

    // setters
public slots:
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
    void set_theme(QString p_string)
    {
        if (theme == p_string)
            return;
        theme = p_string;
        invoke_parents("theme_changed", Q_ARG(QString, p_string));
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
    void set_log_goes_downward(bool p_enabled)
    {
        if (log_goes_downward == p_enabled)
            return;
        log_goes_downward = p_enabled;
        invoke_parents("log_goes_downward_changed", Q_ARG(bool, p_enabled));
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
        username           = cfg.value("username").toString();
        callwords          = cfg.value("callwords").toString();
        theme              = cfg.value("theme", "default").toString();
        always_pre         = cfg.value("always_pre", true).toBool();
        chat_tick_interval = cfg.value("chat_tick_interval", 60).toInt();
        log_max_lines      = cfg.value("chatlog_limit", 200).toInt();
        log_goes_downward  = cfg.value("chatlog_scrolldown", true).toBool();
        log_uses_newline   = cfg.value("chatlog_newline", false).toBool();
        log_music          = cfg.value("music_change_log", true).toBool();
        log_is_recording   = cfg.value("enable_logging", true).toBool();
        effects_volume     = cfg.value("default_sfx", 50).toInt();
        system_volume      = cfg.value("default_system", 50).toInt();
        music_volume       = cfg.value("default_music", 50).toInt();
        blips_volume       = cfg.value("default_blip", 50).toInt();
        blip_rate          = cfg.value("blip_rate", 1000000000).toInt();
        blank_blips        = cfg.value("blank_blips").toBool();
    }
    void save_file()
    {
        cfg.setValue("username", username);
        cfg.setValue("callwords", callwords);
        cfg.setValue("theme", theme);
        cfg.setValue("always_pre", always_pre);
        cfg.setValue("chat_tick_interval", chat_tick_interval);
        cfg.setValue("chatlog_limit", log_max_lines);
        cfg.setValue("chatlog_scrolldown", log_goes_downward);
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
    void invoke_parents(QString p_method_name, QGenericArgument p_arg1 = QGenericArgument(nullptr))
    {
        for (QObject *i_parent : parents)
        {
            QMetaObject::invokeMethod(i_parent, p_method_name.toStdString().c_str(), p_arg1);
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

bool AOConfig::log_goes_downward_enabled()
{
    return d->log_goes_downward;
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

void AOConfig::set_log_goes_downward(bool p_enabled)
{
    d->set_log_goes_downward(p_enabled);
}

void AOConfig::set_log_goes_downward(int p_state)
{
    set_log_goes_downward(p_state == Qt::Checked);
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
