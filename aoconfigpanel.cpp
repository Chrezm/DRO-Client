#include "aoconfigpanel.h"
// qt
#include <QDebug>
#include <QDir>

AOConfigPanel::AOConfigPanel(QWidget *p_parent) : QWidget(p_parent), m_config(new AOConfig(this))
{
    setWindowTitle(tr("Config"));
    setWindowFlag(Qt::WindowMinMaxButtonsHint, false);

    AOGuiLoader loader;
    loader.load_from_file(":res/ui/config_panel.ui", this);

    // tab
    setFocusProxy(AO_GUI_WIDGET(QTabWidget, "tab_widget"));

    // general
    w_username = AO_GUI_WIDGET(QLineEdit, "username");
    w_callwords = AO_GUI_WIDGET(QLineEdit, "callwords");
    w_theme = AO_GUI_WIDGET(QComboBox, "theme");
    w_reload_theme = AO_GUI_WIDGET(QPushButton, "theme_reload");
    w_log_max_lines = AO_GUI_WIDGET(QSpinBox, "log_length");
    w_log_uses_newline = AO_GUI_WIDGET(QCheckBox, "log_newline");
    w_log_goes_downward = AO_GUI_WIDGET(QCheckBox, "log_downward");
    w_log_is_recording = AO_GUI_WIDGET(QCheckBox, "log_recording");

    // audio
    w_music = AO_GUI_WIDGET(QSlider, "music");
    w_music_value = AO_GUI_WIDGET(QLabel, "music_value");
    w_effects = AO_GUI_WIDGET(QSlider, "effects");
    w_effects_value = AO_GUI_WIDGET(QLabel, "effects_value");
    w_blips = AO_GUI_WIDGET(QSlider, "blips");
    w_blips_value = AO_GUI_WIDGET(QLabel, "blips_value");
    w_blip_rate = AO_GUI_WIDGET(QSpinBox, "blip_rate");
    w_blank_blips = AO_GUI_WIDGET(QCheckBox, "blank_blips");

    // themes
    for (QString i_folder : QDir(QDir::currentPath() + "/base/themes").entryList())
    {
        if (i_folder == "." || i_folder == "..")
            continue;
        w_theme->addItem(i_folder);
    }

    // input
    connect(m_config, SIGNAL(username_changed(QString)), w_username, SLOT(setText(QString)));
    connect(m_config, SIGNAL(callwords_changed(QString)), w_callwords, SLOT(setText(QString)));
    connect(m_config, SIGNAL(theme_changed(QString)), w_theme, SLOT(setCurrentText(QString)));
    connect(m_config, SIGNAL(log_max_lines_changed(int)), w_log_max_lines, SLOT(setValue(int)));
    connect(m_config, SIGNAL(log_goes_downward_changed(bool)), w_log_goes_downward, SLOT(setChecked(bool)));
    connect(m_config, SIGNAL(log_uses_newline_changed(bool)), w_log_uses_newline, SLOT(setChecked(bool)));
    connect(m_config, SIGNAL(log_is_recording_changed(bool)), w_log_is_recording, SLOT(setChecked(bool)));
    connect(m_config, SIGNAL(effects_volume_changed(int)), w_effects, SLOT(setValue(int)));
    connect(m_config, SIGNAL(music_volume_changed(int)), w_music, SLOT(setValue(int)));
    connect(m_config, SIGNAL(blips_volume_changed(int)), w_blips, SLOT(setValue(int)));
    connect(m_config, SIGNAL(blip_rate_changed(int)), w_blip_rate, SLOT(setValue(int)));
    connect(m_config, SIGNAL(blank_blips_changed(bool)), w_blank_blips, SLOT(setChecked(bool)));

    // output
    connect(w_username, SIGNAL(textEdited(QString)), m_config, SLOT(set_username(QString)));
    connect(w_callwords, SIGNAL(textEdited(QString)), m_config, SLOT(set_callwords(QString)));
    connect(w_theme, SIGNAL(currentIndexChanged(QString)), m_config, SLOT(set_theme(QString)));
    connect(w_reload_theme, SIGNAL(clicked()), this, SLOT(on_reload_theme_clicked()));
    connect(w_log_max_lines, SIGNAL(valueChanged(int)), m_config, SLOT(set_log_max_lines(int)));
    connect(w_log_goes_downward, SIGNAL(stateChanged(int)), m_config, SLOT(set_log_goes_downward(int)));
    connect(w_log_uses_newline, SIGNAL(stateChanged(int)), m_config, SLOT(set_log_uses_newline(int)));
    connect(w_log_is_recording, SIGNAL(stateChanged(int)), m_config, SLOT(set_log_is_recording(int)));
    connect(w_effects, SIGNAL(valueChanged(int)), m_config, SLOT(set_effects_volume(int)));
    connect(w_effects, SIGNAL(valueChanged(int)), this, SLOT(on_effects_value_changed(int)));
    connect(w_music, SIGNAL(valueChanged(int)), m_config, SLOT(set_music_volume(int)));
    connect(w_music, SIGNAL(valueChanged(int)), this, SLOT(on_music_value_changed(int)));
    connect(w_blips, SIGNAL(valueChanged(int)), m_config, SLOT(set_blips_volume(int)));
    connect(w_blips, SIGNAL(valueChanged(int)), this, SLOT(on_blips_value_changed(int)));
    connect(w_blip_rate, SIGNAL(valueChanged(int)), m_config, SLOT(set_blip_rate(int)));
    connect(w_blank_blips, SIGNAL(stateChanged(int)), m_config, SLOT(set_blank_blips(int)));

    // set values
    w_username->setText(m_config->username());
    w_callwords->setText(m_config->callwords());
    w_theme->setCurrentText(m_config->theme());
    w_reload_theme->hide();
    w_log_max_lines->setValue(m_config->log_max_lines());
    w_log_is_recording->setChecked(m_config->log_is_recording_enabled());
    w_log_goes_downward->setChecked(m_config->log_goes_downward_enabled());
    w_log_uses_newline->setChecked(m_config->log_uses_newline_enabled());
    w_effects->setValue(m_config->effects_volume());
    w_music->setValue(m_config->music_volume());
    w_blips->setValue(m_config->blips_volume());
    w_blip_rate->setValue(m_config->blip_rate());
    w_blank_blips->setChecked(m_config->blank_blips_enabled());
}

void AOConfigPanel::on_reload_theme_clicked()
{
    qDebug() << "reload theme clicked";
    emit reload_theme();
}

void AOConfigPanel::on_effects_value_changed(int p_num)
{
    w_effects_value->setText(QString::number(p_num) + "%");
}

void AOConfigPanel::on_music_value_changed(int p_num)
{
    w_music_value->setText(QString::number(p_num) + "%");
}

void AOConfigPanel::on_blips_value_changed(int p_num)
{
    w_blips_value->setText(QString::number(p_num) + "%");
}
