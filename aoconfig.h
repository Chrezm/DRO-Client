#ifndef AOCONFIG_H
#define AOCONFIG_H
// qt
#include <QObject>

class AOConfig : public QObject
{
    Q_OBJECT

public:
    AOConfig(QObject *p_parent = nullptr);
    ~AOConfig();

    // generic getters
    QString get_string(QString p_name, QString p_default = nullptr);
    bool get_bool(QString p_name, bool p_default = false);
    int get_number(QString p_name, int p_default = 0);

    // getters
    QString username();
    QString callwords();
    QString theme();
    int log_max_lines();
    bool log_goes_downward_enabled();
    bool log_uses_newline_enabled();
    bool log_is_recording_enabled();
    int effects_volume();
    int system_volume();
    int music_volume();
    int blips_volume();
    int blip_rate();
    bool blank_blips_enabled();

    // io
public slots:
    void save_file();

    // setters
public slots:
    void set_username(QString p_string);
    void set_callwords(QString p_string);
    void set_theme(QString p_string);
    void set_log_max_lines(int p_number);
    void set_log_goes_downward(bool p_enabled);
    void set_log_goes_downward(int p_state);
    void set_log_uses_newline(bool p_enabled);
    void set_log_uses_newline(int p_state);
    void set_log_is_recording(bool p_enabled);
    void set_log_is_recording(int p_state);
    void set_effects_volume(int p_number);
    void set_system_volume(int p_number);
    void set_music_volume(int p_number);
    void set_blips_volume(int p_number);
    void set_blip_rate(int p_number);
    void set_blank_blips(bool p_enabled);
    void set_blank_blips(int p_state);

    // signals
signals:
    void username_changed(QString);
    void callwords_changed(QString);
    void theme_changed(QString);
    void log_max_lines_changed(int);
    void log_goes_downward_changed(bool);
    void log_uses_newline_changed(bool);
    void log_is_recording_changed(bool);
    void effects_volume_changed(int);
    void system_volume_changed(int);
    void music_volume_changed(int);
    void blips_volume_changed(int);
    void blip_rate_changed(int);
    void blank_blips_changed(bool);
};

#endif // AOCONFIG_H
