#include "courtroom.h"

#include "aoapplication.h"
#include "lobby.h"
#include "hardware_functions.h"
#include "file_functions.h"
#include "datatypes.h"
#include "debug_functions.h"

#include <QDebug>
#include <QScrollBar>
#include <QRegExp>
#include <QBrush>
#include <QTextCharFormat>
#include <QFont>
#include <QTime>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

void Courtroom::create_widgets()
{
  keepalive_timer = new QTimer(this);
  keepalive_timer->start(60000);

  chat_tick_timer = new QTimer(this);

  text_delay_timer = new QTimer(this);
  text_delay_timer->setSingleShot(true);

  sfx_delay_timer = new QTimer(this);
  sfx_delay_timer->setSingleShot(true);

  realization_timer = new QTimer(this);
  realization_timer->setSingleShot(true);

  testimony_show_timer = new QTimer(this);
  testimony_show_timer->setSingleShot(true);

  testimony_hide_timer = new QTimer(this);
  testimony_hide_timer->setSingleShot(true);

  char_button_mapper = new QSignalMapper(this);

  m_blip_player = new AOBlipPlayer(this, ao_app);
  m_blip_player->set_volume(0);
  m_sfx_player = new AOSfxPlayer(this, ao_app);
  m_sfx_player->set_volume(0);
  m_shout_player = new AOShoutPlayer(this, ao_app);
  m_shout_player->set_volume(0);
  m_mod_player = new AOSfxPlayer(this, ao_app);
  m_mod_player->set_volume(50);
  m_cycle_player = new AOSfxPlayer(this, ao_app);
  m_cycle_player->set_volume(0);
  m_music_player = new AOMusicPlayer(this, ao_app);
  m_music_player->set_volume(0);

  ui_background = new AOImage(this, ao_app);

  ui_viewport = new QWidget(this);
  ui_vp_background = new AOScene(ui_viewport, ao_app);
  ui_vp_speedlines = new AOMovie(ui_viewport, ao_app);
  ui_vp_speedlines->set_play_once(false);
  ui_vp_player_char = new AOCharMovie(ui_viewport, ao_app);
  ui_vp_desk = new AOScene(ui_viewport, ao_app);

  ui_vp_music_display_a = new AOImage(this, ao_app);
  ui_vp_music_display_b = new AOImage(this, ao_app);
  ui_vp_music_area = new QWidget(ui_vp_music_display_a);
  ui_vp_music_name = new QTextEdit(ui_vp_music_area);
  ui_vp_music_name->setText("DANGANRONPA ONLINE");
  ui_vp_music_name->setFrameStyle(QFrame::NoFrame);
  ui_vp_music_name->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_music_name->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_music_name->setReadOnly(true);
  music_anim = new QPropertyAnimation(ui_vp_music_name, "geometry", this);

  ui_vp_clock = new AOMovie(this, ao_app);
  ui_vp_clock->set_play_once(true);

  ui_vp_evidence_display = new AOEvidenceDisplay(this, ao_app);

  ui_vp_chatbox = new AOImage(this, ao_app);
  ui_vp_showname = new QLabel(ui_vp_chatbox);
  ui_vp_message = new QTextEdit(ui_vp_chatbox);
  ui_vp_message->setFrameStyle(QFrame::NoFrame);
  ui_vp_message->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setReadOnly(true);

  ui_vp_showname_image = new AOImage(this, ao_app);

  ui_vp_testimony = new AOImage(this, ao_app);
  ui_vp_effect = new AOMovie(this, ao_app);
  ui_vp_wtce = new AOMovie(this, ao_app);
  ui_vp_objection = new AOMovie(this, ao_app);

  ui_ic_chatlog = new QTextEdit(this);
  ui_ic_chatlog->setReadOnly(true);

  ui_ms_chatlog = new AOTextArea(this);
  ui_ms_chatlog->setReadOnly(true);
  ui_ms_chatlog->setOpenExternalLinks(true);
  ui_ms_chatlog->hide();

  ui_server_chatlog = new AOTextArea(this);
  ui_server_chatlog->setReadOnly(true);
  ui_server_chatlog->setOpenExternalLinks(true);

  ui_mute_list = new QListWidget(this);
  ui_area_list = new QListWidget(this);
  ui_music_list = new QListWidget(this);
  ui_sfx_list = new QListWidget(this);

  ui_ic_chat_message = new QLineEdit(this);
  ui_ic_chat_message->setFrame(false);

  ui_muted = new AOImage(ui_ic_chat_message, ao_app);
  ui_muted->hide();

  ui_ooc_chat_message = new QLineEdit(this);
  ui_ooc_chat_message->setFrame(false);

  ui_ooc_chat_name = new QLineEdit(this);
  ui_ooc_chat_name->setFrame(false);
  ui_ooc_chat_name->setPlaceholderText("Name");

  //ui_area_password = new QLineEdit(this);
  //ui_area_password->setFrame(false);
  ui_music_search = new QLineEdit(this);
  ui_music_search->setFrame(false);

  ui_sfx_search = new QLineEdit(this);
  ui_sfx_search->setFrame(false);

  ui_note_area = new AONoteArea(this, ao_app);
  ui_note_area->add_button = new AOButton(ui_note_area, ao_app);
  ui_note_area->m_layout = new QVBoxLayout(ui_note_area);
  note_scroll_area = new QScrollArea(this);

  note_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  note_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  note_scroll_area->setWidgetResizable(false);

  ui_set_notes = new AOButton(this, ao_app);

  construct_emotes();

  ui_defense_bar = new AOImage(this, ao_app);
  ui_prosecution_bar = new  AOImage(this, ao_app);

  ui_music_label = new AOLabel(this, ao_app);
  ui_sfx_label = new AOLabel(this, ao_app);
  ui_blip_label = new AOLabel(this, ao_app);

  load_shouts(); // Reads from theme, deletes old shouts if needed and creates new ones

  ui_shout_up = new AOButton(this, ao_app);
  ui_shout_up->setProperty("cycle_id", 1);
  ui_shout_down = new AOButton(this, ao_app);
  ui_shout_down->setProperty("cycle_id", 0);

  load_effects();

  ui_effect_down = new AOButton(this, ao_app);
  ui_effect_down->setProperty("cycle_id", 2);
  ui_effect_up = new AOButton(this, ao_app);
  ui_effect_up->setProperty("cycle_id", 3);

  load_wtce();

  ui_wtce_up = new AOButton(this, ao_app);
  ui_wtce_up->setProperty("cycle_id", 5);
  ui_wtce_down = new AOButton(this, ao_app);
  ui_wtce_down->setProperty("cycle_id", 4);

  ui_ooc_toggle = new AOButton(this, ao_app);

  ui_change_character = new AOButton(this, ao_app);
  ui_reload_theme = new AOButton(this, ao_app);
  ui_call_mod = new AOButton(this, ao_app);
  ui_switch_area_music = new AOButton(this, ao_app);

  ui_theme_list = new QComboBox(this);
  ui_confirm_theme = new AOButton(this, ao_app);
  ui_note_button = new AOButton(this, ao_app);

  ui_label_images.resize(7);
  for(int i = 0; i < ui_label_images.size(); ++i)
  {
    ui_label_images[i] = new AOImage(this, ao_app);
  }

  ui_pre = new QCheckBox(this);
  ui_pre->setText("Pre");
  ui_flip = new QCheckBox(this);
  ui_flip->setText("Flip");
  ui_flip->hide();
  ui_guard = new QCheckBox(this);
  ui_guard->setText("Guard");
  ui_guard->hide();
  ui_hidden = new QCheckBox(this);
  ui_hidden->setText("Hidden");

  // filling vectors with existing label/checkbox pointers
  ui_checks.push_back(ui_pre);
  ui_checks.push_back(ui_flip);
  ui_checks.push_back(ui_guard);
  ui_checks.push_back(ui_hidden);
  ui_labels.push_back(ui_music_label);
  ui_labels.push_back(ui_sfx_label);
  ui_labels.push_back(ui_blip_label);
  //

  ui_mute = new AOButton(this, ao_app);

  ui_defense_plus = new AOButton(this, ao_app);
  ui_defense_minus = new AOButton(this, ao_app);

  ui_prosecution_plus = new AOButton(this, ao_app);
  ui_prosecution_minus = new AOButton(this, ao_app);

  ui_text_color = new QComboBox(this);
  ui_text_color->addItem("White");
  ui_text_color->addItem("Green");
  ui_text_color->addItem("Red");
  ui_text_color->addItem("Orange");
  ui_text_color->addItem("Blue");
  if (ao_app->yellow_text_enabled)
    ui_text_color->addItem("Yellow");

  ui_music_slider = new QSlider(Qt::Horizontal, this);
  ui_music_slider->setRange(0, 100);
  ui_music_slider->setValue(ao_app->get_default_music());

  ui_sfx_slider = new QSlider(Qt::Horizontal, this);
  ui_sfx_slider->setRange(0, 100);
  ui_sfx_slider->setValue(ao_app->get_default_sfx());

  ui_blip_slider = new QSlider(Qt::Horizontal, this);
  ui_blip_slider->setRange(0, 100);
  ui_blip_slider->setValue(ao_app->get_default_blip());

  ui_evidence_button = new AOButton(this, ao_app);

  ui_vp_notepad_image = new AOImage(this, ao_app);
  ui_vp_notepad = new QTextEdit(this);
  ui_vp_notepad->setFrameStyle(QFrame::NoFrame);

  ui_timers.resize(1);
  ui_timers[0] = new AOTimer(this, ao_app);

  construct_evidence();

  load_free_blocks(); // Done last so they are guaranteed to be at bottom

  construct_char_select();
}

void Courtroom::connect_widgets()
{
  connect(keepalive_timer, SIGNAL(timeout()), this, SLOT(ping_server()));

  connect(ui_vp_objection, SIGNAL(done()), this, SLOT(objection_done()));
  connect(ui_vp_player_char, SIGNAL(done()), this, SLOT(preanim_done()));

  connect(text_delay_timer, SIGNAL(timeout()), this, SLOT(start_chat_ticking()));
  connect(sfx_delay_timer, SIGNAL(timeout()), this, SLOT(play_sfx()));

  connect(chat_tick_timer, SIGNAL(timeout()), this, SLOT(chat_tick()));

  connect(realization_timer, SIGNAL(timeout()), this, SLOT(realization_done()));

  connect(testimony_show_timer, SIGNAL(timeout()), this, SLOT(hide_testimony()));
  connect(testimony_hide_timer, SIGNAL(timeout()), this, SLOT(show_testimony()));

  connect(ui_emote_left, SIGNAL(clicked()), this, SLOT(on_emote_left_clicked()));
  connect(ui_emote_right, SIGNAL(clicked()), this, SLOT(on_emote_right_clicked()));

  connect(ui_emote_dropdown, SIGNAL(activated(int)), this, SLOT(on_emote_dropdown_changed(int)));
  connect(ui_pos_dropdown, SIGNAL(activated(int)), this, SLOT(on_pos_dropdown_changed(int)));

  connect(ui_mute_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_mute_list_clicked(QModelIndex)));

  connect(ui_ic_chat_message, SIGNAL(returnPressed()), this, SLOT(on_chat_return_pressed()));

  connect(ui_ooc_chat_message, SIGNAL(returnPressed()), this, SLOT(on_ooc_return_pressed()));

  connect(ui_music_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_music_list_clicked()));
  connect(ui_area_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_area_list_clicked()));
  connect(ui_music_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_music_list_double_clicked(QModelIndex)));
  connect(ui_area_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_area_list_double_clicked(QModelIndex)));

  // connect events for shout/effect/wtce buttons happen in load_shouts(), load_effects(), load_wtce()
  connect(ui_shout_up, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));
  connect(ui_shout_down, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));

  connect(ui_effect_up, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));
  connect(ui_effect_down, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));

  connect(ui_wtce_up, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));
  connect(ui_wtce_down, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));

  connect(ui_mute, SIGNAL(clicked()), this, SLOT(on_mute_clicked()));

  connect(ui_defense_minus, SIGNAL(clicked()), this, SLOT(on_defense_minus_clicked()));
  connect(ui_defense_plus, SIGNAL(clicked()), this, SLOT(on_defense_plus_clicked()));
  connect(ui_prosecution_minus, SIGNAL(clicked()), this, SLOT(on_prosecution_minus_clicked()));
  connect(ui_prosecution_plus, SIGNAL(clicked()), this, SLOT(on_prosecution_plus_clicked()));

  connect(ui_text_color, SIGNAL(currentIndexChanged(int)), this, SLOT(on_text_color_changed(int)));

  connect(ui_music_slider, SIGNAL(valueChanged(int)), this, SLOT(on_music_slider_moved(int)));
  connect(ui_sfx_slider, SIGNAL(valueChanged(int)), this, SLOT(on_sfx_slider_moved(int)));
  connect(ui_blip_slider, SIGNAL(valueChanged(int)), this, SLOT(on_blip_slider_moved(int)));

  connect(ui_ooc_toggle, SIGNAL(clicked()), this, SLOT(on_ooc_toggle_clicked()));

  connect(ui_music_search, SIGNAL(textChanged(QString)), this, SLOT(on_music_search_edited(QString)));
  connect(ui_sfx_search, SIGNAL(textChanged(QString)), this, SLOT(on_sfx_search_edited(QString)));

  connect(ui_change_character, SIGNAL(clicked()), this, SLOT(on_change_character_clicked()));
  connect(ui_reload_theme, SIGNAL(clicked()), this, SLOT(on_reload_theme_clicked()));
  connect(ui_call_mod, SIGNAL(clicked()), this, SLOT(on_call_mod_clicked()));

  connect(ui_switch_area_music, SIGNAL(clicked()), this, SLOT(on_switch_area_music_clicked()));

  connect(ui_confirm_theme, SIGNAL(clicked()), this, SLOT(on_confirm_theme_clicked()));
  connect(ui_note_button, SIGNAL(clicked()), this, SLOT(on_note_button_clicked()));

  connect(ui_vp_notepad, SIGNAL(textChanged()), this, SLOT(on_note_text_changed()));

  connect(ui_pre, SIGNAL(clicked()), this, SLOT(on_pre_clicked()));
  connect(ui_flip, SIGNAL(clicked()), this, SLOT(on_flip_clicked()));
  connect(ui_guard, SIGNAL(clicked()), this, SLOT(on_guard_clicked()));

  connect(ui_hidden, SIGNAL(clicked()), this, SLOT(on_hidden_clicked()));

  connect(ui_sfx_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_sfx_list_clicked()));

  connect(ui_evidence_button, SIGNAL(clicked()), this, SLOT(on_evidence_button_clicked()));

  connect(ui_note_area->add_button, SIGNAL(clicked(bool)), this, SLOT(on_add_button_clicked()));
  connect(ui_set_notes, SIGNAL(clicked(bool)), this, SLOT(on_set_notes_clicked()));
}

void Courtroom::reset_widget_names()
{
    // Assign names to the default widgets
    widget_names = {
        {"courtroom", this},
            {"viewport", ui_viewport},
                {"background", ui_vp_background}, //*
                {"speedlines", ui_vp_speedlines}, //*
                {"player_char", ui_vp_player_char}, //*
                {"desk", ui_vp_desk}, //*
            {"music_display_a", ui_vp_music_display_a},
            {"music_display_b", ui_vp_music_display_b},
            {"music_area", ui_vp_music_area},
            {"music_name", ui_vp_music_name},
            // music_anim
            {"clock", ui_vp_clock},
            // ui_vp_evidence_display
            {"ao2_chatbox", ui_vp_chatbox},
                {"showname", ui_vp_showname},
                {"message", ui_vp_message},
            {"showname_image", ui_vp_showname_image},
            {"vp_testimony", ui_vp_testimony},
            {"vp_effect", ui_vp_effect},
            {"vp_wtce", ui_vp_wtce},
            {"vp_objection", ui_vp_objection},
            {"ic_chatlog", ui_ic_chatlog},
            {"ms_chatlog", ui_ms_chatlog},
            {"server_chatlog", ui_server_chatlog},
            {"mute_list", ui_mute_list},
            {"area_list", ui_area_list},
            {"music_list", ui_music_list},
            {"sfx_list", ui_sfx_list},
            {"ao2_ic_chat_message", ui_ic_chat_message},
            // ui_muted
            {"ooc_chat_message", ui_ooc_chat_message},
            {"ooc_chat_name", ui_ooc_chat_name},
            {"music_search", ui_music_search},
            {"sfx_search", ui_sfx_search},
            {"note_scroll_area", note_scroll_area},
                {"note_area", ui_note_area},
            // add_button
            // m_layout
            {"set_notes_button", ui_set_notes},
            {"emotes", ui_emotes},
            {"emote_left", ui_emote_left},
            {"emote_right", ui_emote_right},
            {"emote_dropdown", ui_emote_dropdown},
            {"pos_dropdown", ui_pos_dropdown},
            {"defense_bar", ui_defense_bar},
            {"prosecution_bar", ui_prosecution_bar},
            {"music_label", ui_music_label},
            {"sfx_label", ui_sfx_label},
            {"blip_label", ui_blip_label},
            // Each ui_shouts[i]
            {"shout_up", ui_shout_up},
            {"shout_down", ui_shout_down},
            // Each ui_effects[i]
            {"effect_down", ui_effect_down},
            {"effect_up", ui_effect_up},
            // Each ui_wtce[i]
            {"wtce_up", ui_wtce_up},
            {"wtce_down", ui_wtce_down},
            {"ooc_toggle", ui_ooc_toggle},
            {"change_character", ui_change_character},
            {"reload_theme", ui_reload_theme},
            {"call_mod", ui_call_mod},
            {"switch_area_music", ui_switch_area_music},
            {"theme_list", ui_theme_list},
            {"confirm_theme", ui_confirm_theme},
            {"note_button", ui_note_button},
            // Each ui_label_images[i]
            {"pre", ui_pre},
            {"flip", ui_flip},
            {"guard", ui_guard},
            {"hidden", ui_hidden},
            {"mute_button", ui_mute},
            {"defense_plus", ui_defense_plus},
            {"defense_minus", ui_defense_minus},
            {"prosecution_plus", ui_prosecution_plus},
            {"prosecution_minus", ui_prosecution_minus},
            {"text_color", ui_text_color},
            {"music_slider", ui_music_slider},
            {"sfx_slider", ui_sfx_slider},
            {"blip_slider", ui_blip_slider},
            {"evidence_button", ui_evidence_button},
            {"notepad_image", ui_vp_notepad_image},
            {"notepad", ui_vp_notepad},
            // Each ui_timers[i]
            {"evidence_background", ui_evidence},
                {"evidence_buttons", ui_evidence_buttons},
            {"char_select", ui_char_select_background},
                {"back_to_lobby", ui_back_to_lobby},
                {"char_password", ui_char_password},
                {"char_buttons", ui_char_buttons},
                {"char_select_left", ui_char_select_left},
                {"char_select_right", ui_char_select_right},
                {"spectator", ui_spectator},
    };
}

void Courtroom::insert_widget_name(QString p_widget_name, QWidget *p_widget)
{
    // insert entry
    widget_names[p_widget_name] = p_widget;
    // set name
    p_widget->setObjectName(p_widget_name);
}


void Courtroom::insert_widget_names(QVector<QString> &p_widget_names, QVector<QWidget*> &p_widgets)
{
    for (int i = 0; i < p_widgets.length(); ++i)
        insert_widget_name(p_widget_names[i], p_widgets[i]);
}

void Courtroom::set_widget_names()
{
    // Assign names to the default widgets
    reset_widget_names();

    // set existing widget names
    for (QString widget_name : widget_names.keys())
        widget_names[widget_name]->setObjectName(widget_name);

    // setup table of widgets and names
    insert_widget_names(effect_names, ui_effects);
    insert_widget_names(free_block_names, ui_free_blocks);
    insert_widget_names(shout_names, ui_shouts);
    insert_widget_names(wtce_names, ui_wtce);

    // timers are special children
    QVector<QString> timer_names;
    for (int i = 0; i < ui_timers.length(); ++i)
        timer_names.append("timer_" + QString::number(i));
    insert_widget_names(timer_names, ui_timers);
}

void Courtroom::set_widget_layers()
{
    QStringList paths{
        ao_app->get_theme_variant_path() + "courtroom_layers.ini",
        ao_app->get_theme_path() + "courtroom_layers.ini",
        ao_app->get_default_theme_path() + "courtroom_layers.ini",
    };

    // needed to avoid cyclic parenting
    QStringList recorded_widgets;

    // read the entire thing
    for (QString path : paths)
    {
        QFile layer_ini(path);

        if (layer_ini.open(QFile::ReadOnly))
        {
            QTextStream in(&layer_ini);

            // current parent's name
            QString parent_name = "courtroom";
            // the courtroom is ALWAYS going to be recorded
            recorded_widgets.append(parent_name);

            while (!in.atEnd())
            {
                QString line = in.readLine().trimmed();

                // skip if line is empty
                if (line.isEmpty())
                    continue;

                // revert to default parent if we encounter an end scope
                if (line.startsWith("[\\"))
                {
                    parent_name = "courtroom";
                }
                // is this a parent?
                else if (line.startsWith("["))
                {
                    // update the current parent
                    parent_name = line.remove(0, 1).chopped(1).trimmed();
                }
                // if this is not a parent, it's a child
                else
                {
                    // if the child is already known, skip
                    if (recorded_widgets.contains(line))
                        continue;
                    // make the child known
                    recorded_widgets.append(line);

                    // attach the children to the parents'
                    QWidget *child = widget_names[line];
                    // if child is null, then it does not exist
                    if (!child)
                        continue;

                    QWidget *parent = widget_names[parent_name];
                    // if parent is null, attach main parent
                    if (!parent)
                        parent = widget_names["courtroom"];

                    // set child to parent
                    bool was_visible = child->isVisible();
                    child->setParent(parent);
                    child->raise();

                    // Readjust visibility in case this changed after the widget changed parent
                    // I don't know why, I don't want to know why, I shouldn't
                    // have to wonder why, but for whatever reason these stupid
                    // panels aren't laying out correctly unless we do this terribleness
                    if (child->isVisible() != was_visible)
                        child->setVisible(was_visible);
                }
            }

            // break the loop, we have found a proper file
            break;
        }
    }
}

void Courtroom::set_widgets()
{
  blip_rate = ao_app->read_blip_rate();
  blank_blip = ao_app->get_blank_blip();

  QString filename = design_ini;
  pos_size_type f_courtroom = ao_app->get_element_dimensions("courtroom", filename);

  if (f_courtroom.width < 0 || f_courtroom.height < 0)
  {
    qDebug() << "W: did not find courtroom width or height in " << filename;

    this->resize(714, 668);
  }
  else
  {
    m_courtroom_width = f_courtroom.width;
    m_courtroom_height = f_courtroom.height;

    this->resize(f_courtroom.width, f_courtroom.height);
  }

  ui_background->move(0, 0);
  ui_background->resize(m_courtroom_width, m_courtroom_height);
  ui_background->set_image("courtroombackground.png");

  set_size_and_pos(ui_viewport, "viewport");

  ui_vp_background->move(0, 0);
  ui_vp_background->resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_speedlines->move(0, 0);
  ui_vp_speedlines->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_player_char->move(0, 0);
  ui_vp_player_char->combo_resize(ui_viewport->width(), ui_viewport->height());

  //the AO2 desk element
  ui_vp_desk->move(0, 0);
  ui_vp_desk->resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_evidence_display->move(0, 0);
  ui_vp_evidence_display->resize(ui_viewport->width(), ui_viewport->height());

  set_size_and_pos(ui_vp_notepad_image, "notepad_image");
  ui_vp_notepad_image->set_image("notepad_image.png");
  ui_vp_notepad_image->hide();

  set_size_and_pos(ui_vp_notepad, "notepad");
  ui_vp_notepad->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_notepad->verticalScrollBar()->hide();
  ui_vp_notepad->verticalScrollBar()->resize(0, 0);
  ui_vp_notepad->hide();

  set_size_and_pos(ui_vp_showname, "showname");

  set_size_and_pos(ui_vp_showname_image, "showname_image");
  ui_vp_showname_image->hide();

  set_size_and_pos(ui_vp_message, "message");
  ui_vp_message->setTextInteractionFlags(Qt::NoTextInteraction);
  ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                               "color: white");

  ui_vp_testimony->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_testimony->resize(ui_viewport->width(), ui_viewport->height());
  ui_vp_testimony->set_image("testimony.png");
  ui_vp_testimony->hide();

  ui_vp_effect->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_effect->resize(ui_viewport->width(), ui_viewport->height());
  ui_vp_effect->hide();

  ui_vp_wtce->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_wtce->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_objection->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_objection->combo_resize(ui_viewport->width(), ui_viewport->height());

  set_size_and_pos(ui_ic_chatlog, "ic_chatlog");

  set_size_and_pos(ui_ms_chatlog, "ms_chatlog");

  set_size_and_pos(ui_server_chatlog, "server_chatlog");

  set_size_and_pos(ui_mute_list, "mute_list");
  ui_mute_list->hide();

  set_size_and_pos(ui_area_list, "area_list");
  ui_area_list->hide();
//  ui_area_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_music_list, "music_list");

  set_size_and_pos(ui_sfx_list, "sfx_list");

  set_size_and_pos(ui_ic_chat_message, "ao2_ic_chat_message");
  set_size_and_pos(ui_vp_chatbox, "ao2_chatbox");

  set_size_and_pos(ui_vp_music_area, "music_area");
  ui_vp_music_area->show();
  set_size_and_pos(ui_vp_music_name, "music_name");

  set_size_and_pos(ui_vp_music_display_a, "music_display_a");
  ui_vp_music_display_a->set_image("music_display_a.png");
  ui_vp_music_display_a->show();

  set_size_and_pos(ui_vp_music_display_b, "music_display_b");
  ui_vp_music_display_b->set_image("music_display_b.png");
  ui_vp_music_display_b->show();

  set_size_and_pos(ui_vp_clock, "clock");
  ui_vp_clock->show();

  ui_ic_chat_message->setStyleSheet("QLineEdit{background-color: rgba(100, 100, 100, 255);}");

  ui_vp_chatbox->set_image("chatmed.png");
  ui_vp_chatbox->hide();

  ui_muted->resize(ui_ic_chat_message->width(), ui_ic_chat_message->height());
  ui_muted->set_image("muted.png");

  set_size_and_pos(ui_ooc_chat_message, "ooc_chat_message");
  ui_ooc_chat_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_ooc_chat_name, "ooc_chat_name");
  ui_ooc_chat_name->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  //set_size_and_pos(ui_area_password, "area_password");
  set_size_and_pos(ui_music_search, "music_search");

  set_size_and_pos(ui_sfx_search, "sfx_search");

  // char select
  reconstruct_char_select();

  // emotes
  set_size_and_pos(ui_emotes, "emotes");
  reconstruct_emotes();

  set_size_and_pos(ui_emote_left, "emote_left");
  ui_emote_left->set_image("arrow_left.png");

  set_size_and_pos(ui_emote_right, "emote_right");
  ui_emote_right->set_image("arrow_right.png");

  set_size_and_pos(ui_emote_dropdown, "emote_dropdown");

  set_size_and_pos(ui_pos_dropdown, "pos_dropdown");

  set_size_and_pos(ui_defense_bar, "defense_bar");
  ui_defense_bar->set_image("defensebar" + QString::number(defense_bar_state) + ".png");

  set_size_and_pos(ui_prosecution_bar, "prosecution_bar");
  ui_prosecution_bar->set_image("prosecutionbar" + QString::number(prosecution_bar_state) + ".png");

  set_size_and_pos(ui_music_label, "music_label");
  ui_music_label->setText("Music");
  set_size_and_pos(ui_sfx_label, "sfx_label");
  ui_sfx_label->setText("Sfx");
  set_size_and_pos(ui_blip_label, "blip_label");
  ui_blip_label->setText("Blips");

//  set_size_and_pos(ui_shouts[0], "hold_it");
//  ui_shouts[0]->show();
//  set_size_and_pos(ui_shouts[1], "objection");
//  ui_shouts[1]->show();
//  set_size_and_pos(ui_shouts[2], "take_that");
//  ui_shouts[2]->show();
//  set_size_and_pos(ui_shouts[3], "custom_objection");
//  set_size_and_pos(ui_shouts[4], "got_it");
//  set_size_and_pos(ui_shouts[5], "cross_swords");
//  set_size_and_pos(ui_shouts[6], "counter_alt");
  for(int i = 0; i < shout_names.size(); ++i)
  {
    set_size_and_pos(ui_shouts[i], shout_names[i]);
  }
//  ui_shouts[0]->show();
//  ui_shouts[1]->show();
//  ui_shouts[2]->show();
  reset_shout_buttons();

  set_size_and_pos(ui_shout_up, "shout_up");
  ui_shout_up->set_image("shoutup.png");
  ui_shout_up->hide();
  set_size_and_pos(ui_shout_down, "shout_down");
  ui_shout_down->set_image("shoutdown.png");
  ui_shout_down->hide();

  // courtroom_config.ini necessary + check for crash
  if (ao_app->read_theme_ini("enable_single_shout", cc_config_ini) == "true" && ui_shouts.size() > 0)
  {
    for(auto & shout : ui_shouts) move_widget(shout, "bullet");

    set_shouts();

    ui_shout_up->show();
    ui_shout_down->show();
  }

//  set_size_and_pos(ui_effects[0], "effect_flash");
//  set_size_and_pos(ui_effects[1], "effect_gloom");
//  set_size_and_pos(ui_effects[2], "effect_question");
//  set_size_and_pos(ui_effects[3], "effect_pow");
  for(int i = 0; i < effect_names.size(); ++i)
  {
    set_size_and_pos(ui_effects[i], effect_names[i]);
  }
  reset_effect_buttons();

  set_size_and_pos(ui_effect_up, "effect_up");
  ui_effect_up->set_image("effectup.png");
  ui_effect_up->hide();
  set_size_and_pos(ui_effect_down, "effect_down");
  ui_effect_down->set_image("effectdown.png");
  ui_effect_down->hide();

  if (ao_app->read_theme_ini("enable_single_effect", cc_config_ini) == "true" && ui_effects.size() > 0) // check to prevent crashing
  {
    for(auto & effect  : ui_effects) move_widget(effect, "effect");

    set_effects();

    ui_effect_up->show();
    ui_effect_down->show();
  }

  set_size_and_pos(ui_wtce_up, "wtce_up");
  ui_wtce_up->set_image("wtceup.png");
  ui_wtce_up->hide();
  set_size_and_pos(ui_wtce_down, "wtce_down");
  ui_wtce_down->set_image("wtcedown.png");
  ui_wtce_down->hide();

  for(int i = 0; i < wtce_names.size(); ++i)
  {
      set_size_and_pos(ui_wtce[i], wtce_names[i]);
  }

  if (ao_app->read_theme_ini("enable_single_wtce", cc_config_ini) == "true" ) // courtroom_config.ini necessary
  {
    for(auto & wtce : ui_wtce)
        move_widget(wtce, "wtce");
    qDebug() << "AA: single wtce";
  }
  set_judge_wtce();

  // this will reset the image
  reset_judge_wtce_buttons();

  for(int i = 0; i < free_block_names.size(); ++i)
  {
    set_size_and_pos(ui_free_blocks[i], free_block_names[i]);
  }
  set_free_blocks();

  set_size_and_pos(ui_ooc_toggle, "ooc_toggle");
  ui_ooc_toggle->setText("Server");

  set_size_and_pos(ui_call_mod, "call_mod");

  set_size_and_pos(ui_change_character, "change_character");
  set_size_and_pos(ui_reload_theme, "reload_theme");
  set_size_and_pos(ui_call_mod, "call_mod");
  set_size_and_pos(ui_confirm_theme, "confirm_theme");
  set_size_and_pos(ui_note_button, "note_button");

  set_size_and_pos(ui_switch_area_music, "switch_area_music");

  // Set the default values for the buttons, then try and determine if they should be replaced by images
  ui_change_character->setText("Change character");
  ui_change_character->setStyleSheet("");

  ui_reload_theme->setText("Reload theme");
  ui_reload_theme->setStyleSheet("");

  ui_call_mod->setText("Call mod");
  ui_call_mod->setStyleSheet("");

  ui_switch_area_music->setText("A/M");
  ui_switch_area_music->setStyleSheet("");

  ui_confirm_theme->setText("^");
  ui_confirm_theme->setStyleSheet("");

  ui_note_button->setText("><:");
  ui_note_button->setStyleSheet("");

  if (ao_app->read_theme_ini("enable_button_images", cc_config_ini) == "true")
  {
    // Set files, ask questions later
    // set_image first tries the theme variant folder, then the theme folder, then falls back to the default theme
    ui_change_character->set_image("changecharacter.png");
    if (!ui_change_character->image_path.isEmpty())
      ui_change_character->setText("");

    ui_reload_theme->set_image("reloadtheme.png");
    if (!ui_reload_theme->image_path.isEmpty())
      ui_reload_theme->setText("");

    ui_call_mod->set_image("callmod.png");
    if (!ui_call_mod->image_path.isEmpty())
      ui_call_mod->setText("");

    ui_switch_area_music->set_image("switch_area_music.png");
    if (!ui_switch_area_music->image_path.isEmpty())
      ui_switch_area_music->setText("");

    ui_confirm_theme->set_image("confirmtheme.png");
    if (!ui_confirm_theme->image_path.isEmpty())
      ui_confirm_theme->setText("");

    ui_note_button->set_image("notebutton.png");
    if (!ui_note_button->image_path.isEmpty())
      ui_note_button->setText("");
  }

  set_size_and_pos(ui_theme_list, "theme_list");

  set_size_and_pos(ui_pre, "pre");
  ui_pre->setText("Pre");

  set_size_and_pos(ui_flip, "flip");

  set_size_and_pos(ui_guard, "guard");

  set_size_and_pos(ui_hidden, "hidden");

  for(int i = 0; i < ui_label_images.size(); ++i)
  {
    set_size_and_pos(ui_label_images[i], label_images[i].toLower() + "_image");
  }

  if (ao_app->read_theme_ini("enable_label_images", cc_config_ini) == "true")
  {
    for (int i = 0 ; i < ui_checks.size(); ++i) // loop through checks
    {
      QString image = label_images[i].toLower() + ".png";
      ui_label_images[i]->set_image(image);

      if (!ui_label_images[i]->image_path.isEmpty())
        ui_checks[i]->setText("");
      else
        ui_checks[i]->setText(label_images[i]);
    }

    for(int i = 0 ; i < ui_labels.size(); ++i) // now through labels..........
    {
      int j = i + ui_checks.size();
      QString image = label_images[j].toLower() + ".png";
      ui_label_images[j]->set_image(image);

      if (!ui_label_images[j]->image_path.isEmpty())
        ui_labels[i]->setText("");
      else
        ui_labels[i]->setText(label_images[j]);
    }
  }
  else
  {
    for(int i = 0; i < ui_checks.size(); ++i) //same thing
    {
      ui_checks[i]->setText(label_images[i]);
      ui_label_images[i]->set_image("");
    }

    for(int i = 0; i < ui_labels.size(); ++i) //same thing
    {
      int j = i + ui_checks.size();
      ui_labels[i]->setText(label_images[j]);
      ui_label_images[j]->set_image("");
    }
  }

  set_size_and_pos(ui_mute, "mute_button");
  ui_mute->set_image("mute.png");

  set_size_and_pos(ui_defense_plus, "defense_plus");
  ui_defense_plus->set_image("defplus.png");

  set_size_and_pos(ui_defense_minus, "defense_minus");
  ui_defense_minus->set_image("defminus.png");

  set_size_and_pos(ui_prosecution_plus, "prosecution_plus");
  ui_prosecution_plus->set_image("proplus.png");

  set_size_and_pos(ui_prosecution_minus, "prosecution_minus");
  ui_prosecution_minus->set_image("prominus.png");

  set_size_and_pos(ui_text_color, "text_color");

  set_size_and_pos(ui_music_slider, "music_slider");
  set_size_and_pos(ui_sfx_slider, "sfx_slider");
  set_size_and_pos(ui_blip_slider, "blip_slider");

  set_size_and_pos(ui_evidence_button, "evidence_button");
  ui_evidence_button->set_image("evidencebutton.png");

  set_size_and_pos(ui_evidence, "evidence_background");
  ui_evidence->set_image("evidencebackground.png");

  set_size_and_pos(ui_evidence_name, "evidence_name");

  set_size_and_pos(ui_evidence_buttons, "evidence_buttons");

  set_size_and_pos(ui_evidence_left, "evidence_left");
  ui_evidence_left->set_image("arrow_left.png");

  set_size_and_pos(ui_evidence_right, "evidence_right");
  ui_evidence_right->set_image("arrow_right.png");

  set_size_and_pos(ui_evidence_present, "evidence_present");
  ui_evidence_present->set_image("present_disabled.png");

  set_size_and_pos(ui_evidence_overlay, "evidence_overlay");
  ui_evidence_overlay->set_image("evidenceoverlay.png");

  set_size_and_pos(ui_evidence_delete, "evidence_delete");
  ui_evidence_delete->set_image("deleteevidence.png");

  set_size_and_pos(ui_evidence_image_name, "evidence_image_name");

  set_size_and_pos(ui_evidence_image_button, "evidence_image_button");

  set_size_and_pos(ui_evidence_x, "evidence_x");
  ui_evidence_x->set_image("evidencex.png");

  set_size_and_pos(ui_evidence_description, "evidence_description");

  ui_char_button_selector->set_image("char_selector.png");
  ui_char_button_selector->hide();

  set_size_and_pos(ui_back_to_lobby, "back_to_lobby");
  ui_back_to_lobby->setText("Back to Lobby");

  set_size_and_pos(ui_char_password, "char_password");

  set_size_and_pos(ui_char_buttons, "char_buttons");

  set_size_and_pos(ui_char_select_left, "char_select_left");
  ui_char_select_left->set_image("arrow_left.png");

  set_size_and_pos(ui_char_select_right, "char_select_right");
  ui_char_select_right->set_image("arrow_right.png");

  set_size_and_pos(ui_spectator, "spectator");

  handle_music_anim();

  set_size_and_pos(ui_set_notes, "set_notes_button");
  ui_set_notes->set_image("set_notes.png");
  ui_note_area->m_layout->setSpacing(10);
  set_size_and_pos(ui_note_area, "note_area");
  set_size_and_pos(note_scroll_area, "note_area");
  note_scroll_area->setWidget(ui_note_area);
  ui_note_area->set_image("note_area.png");
  ui_note_area->add_button->set_image("add_button.png");
  ui_note_area->add_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  ui_note_area->setLayout(ui_note_area->m_layout);
  ui_note_area->show();
  note_scroll_area->hide();

  list_note_files();

  if(!contains_add_button)
  {
    ui_note_area->m_layout->addWidget(ui_note_area->add_button);
    contains_add_button = true;
  }

  timer_number = adapt_numbered_items(ui_timers, "timer_number", "timer");
  set_dropdowns();
  set_fonts();
}

void Courtroom::set_size_and_pos(QWidget *p_widget, QString p_identifier)
{
  QString filename = design_ini;


  pos_size_type design_ini_result = ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0)
  {
    qDebug() << "W: could not find \"" << p_identifier << "\" in " << filename;
    p_widget->hide();
  }
  else
  {
    p_widget->move(design_ini_result.x, design_ini_result.y);
    p_widget->resize(design_ini_result.width, design_ini_result.height);
  }
}

void Courtroom::move_widget(QWidget *p_widget, QString p_identifier)
{
  QString filename = design_ini;

  pos_size_type design_ini_result = ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0)
  {
    qDebug() << "W: could not find \"" << p_identifier << "\" in " << filename;
    p_widget->hide();
  }
  else
  {
    p_widget->move(design_ini_result.x, design_ini_result.y);
  }
}

template<typename T>
int Courtroom::adapt_numbered_items(QVector<T*> &item_vector, QString config_item_number,
                                      QString item_name)
{
  // &item_vector must be a vector of size at least 1!

  // Redraw the new correct number of items.
  int new_item_number = ao_app->read_theme_ini(config_item_number, cc_config_ini).toInt();
  int current_item_number = item_vector.size();
  // Note we use the fact that, if config_item_number is not there, read_theme_ini returns an
  // empty string, which .toInt() would fail to convert and by documentation transform to 0.

  // Decide what to do if the new theme has a different amount of items than the old one
  if (new_item_number < current_item_number)
  {
    // Hide old items if there are any.
    for (int i=new_item_number; i<current_item_number; i++)
    {
      item_vector[i]->hide();
    }
  }
  else if (current_item_number < new_item_number)
  {
    // Create new items
    item_vector.resize(new_item_number);
    for (int i=current_item_number; i<new_item_number; i++)
    {
      item_vector[i] = new T(this, ao_app);
      item_vector[i]->stackUnder(item_vector[i-1]);
      // index i-1 exists as i >= current_item_number == item_vector.size() >= 1
    }
  }
  // Note that by design we do *not* destroy items (or similarly, the size of item_vector
  // is non-decreasing. This is because we want to allow for items to, say, run in the
  // background as invisible.
  // With that said, we can now properly format our new items
  for (int i=0; i<new_item_number; i++)
  {
    item_vector[i]->show();
    set_size_and_pos(item_vector[i], item_name+"_"+QString::number(i));
    // Note that show is deliberately placed before set_size_and_pos
    // That is because we want to retain the functionality set_size_and_pos includes where
    // hides a widget if it is unable to find a position for it, and does not change its
    // visibility otherwise.
  }
  return new_item_number;
}

void Courtroom::check_effects()
{
  QString char_path = ao_app->get_character_path(current_char);
  QString theme_variant_path = ao_app->get_theme_variant_path();
  QString theme_path = ao_app->get_theme_path();
  for(int i = 0; i < ui_effects.size(); ++i)
  {
    QStringList paths{
      char_path + effect_names.at(i) + ".gif",
      theme_variant_path + effect_names.at(i) + ".gif",
      theme_variant_path + effect_names.at(i) + ".apng",
      theme_path + effect_names.at(i) + ".gif",
      theme_path + effect_names.at(i) + ".apng"
    };

    // Assume the effect does not exist until a matching file is found
    effects_enabled[i] = false;
    for (QString path : paths)
    {
      if (file_exists(path))
      {
        effects_enabled[i] = true;
        break;
      }
    }
  }
}

void Courtroom::check_free_blocks()
{
  QString char_path = ao_app->get_character_path(current_char);
  QString theme_variant_path = ao_app->get_theme_variant_path();
  QString theme_path = ao_app->get_theme_path();
  for(int i = 0; i < ui_free_blocks.size(); ++i)
  {
    QStringList paths{
      char_path + free_block_names.at(i) + ".gif",
      theme_variant_path + free_block_names.at(i) + ".gif",
      theme_variant_path + free_block_names.at(i) + ".apng",
      theme_path + free_block_names.at(i) + ".gif",
      theme_path + free_block_names.at(i) + ".apng"
    };

    // Assume the free block does not exist until a matching file is found
    free_blocks_enabled[i] = false;
    for (QString path : paths)
    {
      if (file_exists(path))
      {
        free_blocks_enabled[i] = true;
        break;
      }
    }
  }
}

void Courtroom::check_shouts()
{
  QString char_path = ao_app->get_character_path(current_char);
  QString theme_variant_path = ao_app->get_theme_variant_path();
  QString theme_path = ao_app->get_theme_path();
  for(int i = 0; i < ui_shouts.size(); ++i)
  {
    QStringList paths{
      char_path + shout_names.at(i) + ".gif",
      theme_variant_path + shout_names.at(i) + ".gif",
      theme_variant_path + shout_names.at(i) + ".apng",
      theme_path + shout_names.at(i) + ".gif",
      theme_path + shout_names.at(i) + ".apng"
    };

    // Assume the shout does not exist until a matching file is found
    shouts_enabled[i] = false;
    for (QString path : paths)
    {
      if (file_exists(path))
      {
        shouts_enabled[i] = true;
        break;
      }
    }
  }
}

void Courtroom::check_wtce()
{
  QString char_path = ao_app->get_character_path(current_char);
  QString theme_variant_path = ao_app->get_theme_variant_path();
  QString theme_path = ao_app->get_theme_path();
  for(int i = 0; i < ui_wtce.size(); ++i)
  {
    QStringList paths{
      char_path + wtce_names.at(i) + ".gif",
      theme_variant_path + wtce_names.at(i) + ".gif",
      theme_variant_path + wtce_names.at(i) + ".apng",
      theme_path + wtce_names.at(i) + ".gif",
      theme_path + wtce_names.at(i) + ".apng"
    };

    // Assume the judge button does not exist until a matching file is found
    wtce_enabled[i] = false;
    for (QString path : paths)
    {
      if (file_exists(path))
      {
        wtce_enabled[i] = true;
        break;
      }
    }
  }
}

void Courtroom::delete_widget(QWidget *p_widget)
{
    // remove the widget from recorded names
    widget_names.remove(p_widget->objectName());

    // transfer the children to our grandparent since our parent is
    // about to commit suicide
    QWidget *grand_parent = p_widget->parentWidget();
    // if we don't have a grand parent, attach ourselves to courtroom
    if (!grand_parent)
        grand_parent = this;

    // set new parent
    for (QWidget *child : p_widget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly))
        child->setParent(grand_parent);

    // delete widget
    delete p_widget;
}

void Courtroom::load_effects()
{
  // Close any existing effects to prevent memory leaks
  for (QWidget *widget : ui_effects)
      delete_widget(widget);

  // And create new effects
  int effect_number = ao_app->get_design_ini_value("effect_number", cc_config_ini);
  effects_enabled.resize(effect_number);
  ui_effects.resize(effect_number);

  for (int i=0; i<ui_effects.size(); ++i)
  {
    ui_effects[i] = new AOButton(this, ao_app);
    ui_effects[i]->setProperty("effect_id", i+1);
    ui_effects[i]->stackUnder(ui_effect_up);
    ui_effects[i]->stackUnder(ui_effect_down);
  }

  // And connect their actions
  for (auto & effect : ui_effects)
    connect(effect, SIGNAL(clicked(bool)), this, SLOT(on_effect_button_clicked()));

  // And add names
  effect_names.clear();
  for (int i=1; i<=ui_effects.size(); ++i)
  {
    QStringList names = ao_app->get_effect(i);
    if (!names.isEmpty())
    {
      QString name = names.at(0).trimmed();
      effect_names.append(name);
    }
  }
}

void Courtroom::load_free_blocks()
{
    for (QWidget *widget : ui_free_blocks)
        delete_widget(widget);

  // And create new free block buttons
  int free_block_number = ao_app->get_design_ini_value("free_block_number", cc_config_ini);
  free_blocks_enabled.resize(free_block_number);
  ui_free_blocks.resize(free_block_number);

  for (int i=0; i<ui_free_blocks.size(); ++i)
  {
    ui_free_blocks[i] = new AOMovie(this, ao_app);
    //ui_free_blocks[i]->setProperty("free_block_id", i+1);
    ui_free_blocks[i]->set_play_once(false);
    ui_free_blocks[i]->stackUnder(ui_vp_player_char);
  }

  // And add names
  free_block_names.clear();
  for (int i=1; i<=ui_free_blocks.size(); ++i)
  {
    QString name = "free_block_" + ao_app->get_spbutton("[FREE BLOCKS]", i).trimmed();
    if (!name.isEmpty())
    {
      free_block_names.append(name);
      widget_names[name] = ui_free_blocks[i-1];
      ui_free_blocks[i-1]->setObjectName(name);
    }
  }
  qDebug() << "FREE BLOCKS HERE " << free_block_names;
}

void Courtroom::load_shouts()
{
    for (QWidget *widget : ui_shouts)
        delete_widget(widget);

  // And create new shouts
  int shout_number = ao_app->get_design_ini_value("shout_number", cc_config_ini);
  shouts_enabled.resize(shout_number);
  ui_shouts.resize(shout_number);

  for (int i=0; i<ui_shouts.size(); ++i)
  {
    ui_shouts[i] = new AOButton(this, ao_app);
    ui_shouts[i]->setProperty("shout_id", i+1);
    ui_shouts[i]->stackUnder(ui_shout_up);
    ui_shouts[i]->stackUnder(ui_shout_down);
  }

  // And connect their actions
  for (auto & shout : ui_shouts)
    connect(shout, SIGNAL(clicked(bool)), this, SLOT(on_shout_clicked()));

  // And add names
  shout_names.clear();
  for (int i=1; i<=ui_shouts.size(); ++i)
  {
    QString name = ao_app->get_spbutton("[SHOUTS]", i).trimmed();
    if (!name.isEmpty())
    {
      qDebug() << "SHOUT " << name << " " << ui_shouts[i-1];
      shout_names.append(name);
      widget_names[name] = ui_shouts[i-1];
      ui_shouts[i-1]->setObjectName(name);
    }
  }
  qDebug() << widget_names;
}

void Courtroom::load_wtce()
{
    for (QWidget *widget : ui_wtce)
        delete_widget(widget);

  // And create new wtce buttons
  int wtce_number = ao_app->get_design_ini_value("wtce_number", cc_config_ini);
  wtce_enabled.resize(wtce_number);
  ui_wtce.resize(wtce_number);

  for (int i=0; i<ui_wtce.size(); ++i)
  {
    ui_wtce[i] = new AOButton(this, ao_app);
    ui_wtce[i]->setProperty("wtce_id", i+1);
    ui_wtce[i]->stackUnder(ui_wtce_up);
    ui_wtce[i]->stackUnder(ui_wtce_down);
  }

  // And connect their actions
  for (auto & wtce : ui_wtce)
    connect(wtce, SIGNAL(clicked(bool)), this, SLOT(on_wtce_clicked()));

  // And add names
  wtce_names.clear();
  for (int i=1; i<=ui_wtce.size(); ++i)
  {
    QString name = ao_app->get_spbutton("[WTCE]", i).trimmed();
    if (!name.isEmpty())
    {
      wtce_names.append(name);
      widget_names[name] = ui_wtce[i-1];
      ui_wtce[i-1]->setObjectName(name);
    }
  }
}

void Courtroom::set_shouts()
{
  for(auto & shout : ui_shouts) shout->hide();
  if (ui_shouts.size() > 0) ui_shouts[m_shout_state]->show(); // check to prevent crashing
}

void Courtroom::set_effects()
{
  for(auto & effect : ui_effects)
      effect->hide();

  // check to prevent crashing
  if (ui_effects.size() > 0)
      ui_effects[m_effect_current]->show();
}

void Courtroom::set_judge_enabled(bool p_enabled)
{
    is_judge = p_enabled;

    // set judge button visibility
    ui_defense_plus->setVisible(is_judge);
    ui_defense_minus->setVisible(is_judge);
    ui_prosecution_plus->setVisible(is_judge);
    ui_prosecution_minus->setVisible(is_judge);

    set_judge_wtce();
}

void Courtroom::set_judge_wtce()
{
    // hide all wtce before enabling visibility
    for(auto & wtce : ui_wtce)
        wtce->hide();

    // check if we use a single wtce or multiple
    const bool is_single_wtce = ao_app->read_theme_ini("enable_single_wtce", cc_config_ini) == "true";

    // update visibility for next/previous
    ui_wtce_up->setVisible(is_judge && is_single_wtce);
    ui_wtce_down->setVisible(is_judge && is_single_wtce);

    // prevent going ahead if we have no wtce
    if (!is_judge || ui_wtce.length() == 0)
        return;


    // set visibility based off parameter
    if (is_single_wtce == true)
    {
        ui_wtce[m_wtce_current]->show();
    }
    else
    {
        for (AOButton *i_wtce : ui_wtce)
            i_wtce->show();
    }
}

void Courtroom::set_free_blocks()
{
  for (int i=0; i<ui_free_blocks.size(); i++)
  {
    AOMovie* free_block = ui_free_blocks[i];
    free_block->play(free_block_names[i]);
  }
}

void Courtroom::set_dropdown(QWidget *widget, QString target_tag)
{
  QString f_file = "courtroom_stylesheets.css";
  QString style_sheet_string = ao_app->get_stylesheet(target_tag, f_file);
  if (style_sheet_string != "")
    widget->setStyleSheet(style_sheet_string);
}

void Courtroom::set_dropdowns()
{
  set_dropdown(ui_text_color, "[TEXT COLOR]");
  set_dropdown(ui_pos_dropdown, "[POS DROPDOWN]");
  set_dropdown(ui_theme_list, "[THEME LIST]");
  set_dropdown(ui_emote_dropdown, "[EMOTE DROPDOWN]");
  set_dropdown(ui_mute_list, "[MUTE LIST]");
  set_dropdown(ui_ic_chat_message, "[IC LINE]");
  set_dropdown(ui_ooc_chat_message, "[OOC LINE]");
}


void Courtroom::set_font(QWidget *widget, QString p_identifier)
{
  QString design_file = fonts_ini;
  int f_weight = ao_app->get_font_size(p_identifier, design_file);
  QString class_name = widget->metaObject()->className();

  QString font_name = ao_app->get_font_name("font_" + p_identifier, design_file);

  widget->setFont(QFont(font_name, f_weight));

  QColor f_color = ao_app->get_color(p_identifier + "_color", design_file);

  int bold = ao_app->get_font_size(p_identifier + "_bold", design_file); // is the font bold or not?

  QString is_bold = "";
  if(bold == 1) is_bold = "bold";

  QString style_sheet_string = class_name + " { background-color: rgba(0, 0, 0, 0);\n" +
                               "color: rgba(" +
                               QString::number(f_color.red()) + ", " +
                               QString::number(f_color.green()) + ", " +
                               QString::number(f_color.blue()) + ", 255);\n"
                                                                 "font: " + is_bold + "; }";

  widget->setStyleSheet(style_sheet_string);
}

void Courtroom::set_fonts()
{
  set_font(ui_vp_showname, "showname");
  set_font(ui_vp_message, "message");
  set_font(ui_ic_chatlog, "ic_chatlog");
  set_font(ui_ms_chatlog, "ms_chatlog");
  set_font(ui_server_chatlog, "server_chatlog");
  set_font(ui_music_list, "music_list");
  set_font(ui_area_list, "area_list");
  set_font(ui_sfx_list, "sfx_list");
  set_font(ui_vp_music_name, "music_name");
  set_font(ui_vp_notepad, "notepad");
  for (int i=0; i<timer_number; i++)
  {
    set_font(ui_timers[i], "timer_"+QString::number(i));
  }
}


void Courtroom::set_mute_list()
{
  mute_map.clear();

  //maps which characters are muted based on cid, none are muted by default
  for (int n_cid = 0 ; n_cid < char_list.size() ; n_cid++)
  {
    mute_map.insert(n_cid, false);
  }

  QStringList sorted_mute_list;

  for (char_type i_char : char_list)
    sorted_mute_list.append(i_char.name);

  sorted_mute_list.sort();

  for (QString i_name : sorted_mute_list)
  {
    //mute_map.insert(i_name, false);
    ui_mute_list->addItem(i_name);
  }
}
