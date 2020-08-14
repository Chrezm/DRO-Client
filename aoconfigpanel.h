#ifndef AOCONFIGPANEL_H
#define AOCONFIGPANEL_H
// qt
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
// src
#include "aoguiloader.h"
#include "aoconfig.h"

class AOConfigPanel : public QWidget
{
    Q_OBJECT

    AOConfig *m_config = nullptr;

    // general
    QLineEdit *w_username = nullptr;
    QLineEdit *w_callwords = nullptr;
    QComboBox *w_theme = nullptr;
    QPushButton *w_reload_theme = nullptr;
    QSpinBox *w_log_max_lines = nullptr;
    QCheckBox *w_log_uses_newline = nullptr;
    QCheckBox *w_log_goes_downward = nullptr;
    QCheckBox *w_log_is_recording = nullptr;

    // audio
    QSpinBox *w_effects = nullptr;
    QSpinBox *w_music = nullptr;
    QSpinBox *w_blips = nullptr;
    QSpinBox *w_blip_rate = nullptr;
    QCheckBox *w_blank_blips = nullptr;

public:
    AOConfigPanel(QWidget *p_parent = nullptr);

signals:
    void reload_theme();

private slots:
    void on_reload_theme_clicked();
};

#endif // AOCONFIGPANEL_H
