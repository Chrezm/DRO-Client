#include "courtroom.h"

#include "aoapplication.h"
#include "datatypes.h"
#include "debug_functions.h"
#include "file_functions.h"
#include "hardware_functions.h"
#include "lobby.h"

#include <QBrush>
#include <QDebug>
#include <QFileDialog>
#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QInputDialog>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QRegExp>
#include <QScrollBar>
#include <QTextCharFormat>
#include <QTime>

#include "networkmanager.h"

Courtroom::Courtroom(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;
  ao_config = new AOConfig(this);

  create_widgets();
  connect_widgets();

  set_widgets();
  set_char_select();
  set_widget_names();
}

Courtroom::~Courtroom()
{
  stop_all_audio();
}

void Courtroom::enter_courtroom(int p_cid)
{
  bool changed_character = (m_cid != p_cid);
  m_cid = p_cid;

  QString l_chr_name;

  if (is_spectating())
  {
    ao_app->discord->clear_character_name();
    ao_config->clear_showname_placeholder();
  }
  else
  {
    l_chr_name = ao_app->get_char_name(char_list.at(m_cid).name);
    const QString l_ini_showname = ao_app->get_showname(l_chr_name);
    const QString l_final_showname = l_ini_showname.trimmed().isEmpty() ? l_chr_name : l_ini_showname;
    ao_app->discord->set_character_name(l_final_showname);
    ao_config->set_showname_placeholder(l_final_showname);

    if (ao_app->m_FL_chrini_enabled)
    {
      QStringList l_content{l_chr_name, l_final_showname};
      AOPacket *l_packet = new AOPacket("chrini", l_content);
      ao_app->send_server_packet(l_packet);
    }
  }
  current_char = l_chr_name;
  m_emote_list = ao_app->get_emote_list(current_char);

  m_current_emote_id = 0;
  current_emote_page = 0;

  if (is_spectating())
    ui_emotes->hide();
  else
    ui_emotes->show();

  set_emote_page();
  set_emote_dropdown();
  ui_pre->setChecked(ui_pre || ao_config->always_pre_enabled());

  current_evidence_page = 0;
  current_evidence = 0;

  m_shout_state = 0;
  m_shout_current = 0;
  m_effect_state = 0;
  m_effect_current = 0;
  m_wtce_current = 0;
  reset_wtce_buttons();

  // setup chat
  on_chat_config_changed();

  set_evidence_page();

  // Refresh character position. If the character was changed, use the new
  // position, otherwise use the old one. Even if the else is useless now (it
  // can be omitted), I am keeping it in case we expand set_character_position
  // to do more.
  if (changed_character)
    set_character_position(ao_app->get_char_side(l_chr_name));
  else
    set_character_position(ui_pos_dropdown->currentText());

  // Update widgets first, then check if everything is valid
  // This will also handle showing the correct shouts, effects and wtce buttons,
  // and cycling through them if the buttons that are supposed to be displayed
  // do not exist It will also take care of free blocks

  set_widgets();

  check_shouts();
  if (m_shout_current < shouts_enabled.length() && !shouts_enabled[m_shout_current])
    cycle_shout(1);

  check_effects();
  if (m_effect_current < effects_enabled.length() && !effects_enabled[m_effect_current])
    cycle_effect(1);

  check_wtce();
  if (is_judge && (m_wtce_current < wtce_enabled.length() && !wtce_enabled[m_wtce_current]))
    cycle_wtce(1);

  check_free_blocks();

  ui_flip->show();

  list_music();
  list_areas();
  update_sfx_list();
  select_default_sfx();

  // unmute audio
  suppress_audio(false);

  testimony_in_progress = false;

  // ui_server_chatlog->setHtml(ui_server_chatlog->toHtml());

  ui_char_select_background->hide();

  ui_ic_chat_message->setEnabled(!is_spectating());
  ui_ic_chat_message->setFocus();

  for (int i = 0; i < ui_timers.length(); ++i)
    ui_timers[i]->redraw();

  set_widget_names();
  set_widget_layers();
}

void Courtroom::done_received()
{
  m_cid = -1;

  suppress_audio(true);

  set_char_select_page();

  set_mute_list();

  set_char_select();

  show();

  ui_spectator->show();
}

void Courtroom::set_window_title(QString p_title)
{
  this->setWindowTitle(p_title);
}

void Courtroom::set_scene()
{
  if (testimony_in_progress)
    show_testimony();

  // witness is default if pos is invalid
  QString f_background = "witnessempty";
  QString f_desk_image = "stand";
  QString f_desk_mod = m_chatmessage[CMDeskModifier];
  QString f_side = m_chatmessage[CMPosition];
  QString ini_path = ao_app->get_background_path("backgrounds.ini");

  if (file_exists(ini_path))
  {
    f_background = ao_app->read_ini(f_side, ini_path);
    f_desk_image = ao_app->read_ini(f_side + "_desk", ini_path);

    if (f_desk_mod == "0") // keeping a bit of the functionality for now
    {
      ui_vp_desk->hide();
    }
  }
  else
  {
    if (f_side == "def")
    {
      f_background = "defenseempty";
      f_desk_image = "defensedesk";
    }
    else if (f_side == "pro")
    {
      f_background = "prosecutorempty";
      f_desk_image = "prosecutiondesk";
    }
    else if (f_side == "jud")
    {
      f_background = "judgestand";
      f_desk_image = "judgedesk";
    }
    else if (f_side == "hld")
    {
      f_background = "helperstand";
      f_desk_image = "helperdesk";
    }
    else if (f_side == "hlp")
    {
      f_background = "prohelperstand";
      f_desk_image = "prohelperdesk";
    }
    else
    {
      f_desk_image = "stand";
    }

    bool has_all_desks = true;
    QStringList alldesks{"defensedesk", "prosecutiondesk", "stand"};
    for (const QString &desk : alldesks)
    {
      QString full_path = ao_app->find_asset_path({get_background_path(desk)}, animated_or_static_extensions());
      if (full_path.isEmpty())
      {
        has_all_desks = false;
        break;
      }
    }

    if (f_desk_mod == "0" || (f_desk_mod != "1" && (f_side == "jud" || f_side == "hld" || f_side == "hlp")))
      ui_vp_desk->hide();
    else if (!has_all_desks)
      ui_vp_desk->hide();
    else
      ui_vp_desk->show();
  }

  ui_vp_background->set_image(f_background);
  ui_vp_desk->set_image(f_desk_image);
}

void Courtroom::set_taken(int n_char, bool p_taken)
{
  if (n_char >= char_list.size())
  {
    qDebug() << "W: set_taken attempted to set an index bigger than char_list size";
    return;
  }

  char_type f_char;
  f_char.name = char_list.at(n_char).name;
  f_char.description = char_list.at(n_char).description;
  f_char.taken = p_taken;
  f_char.evidence_string = char_list.at(n_char).evidence_string;

  char_list.replace(n_char, f_char);
}

void Courtroom::set_background(QString p_background)
{
  testimony_in_progress = false;
  current_background = p_background;
}

void Courtroom::handle_music_anim()
{
  QString file_a = design_ini;
  QString file_b = fonts_ini;
  pos_size_type res_a = ao_app->get_element_dimensions("music_name", file_a);
  pos_size_type res_b = ao_app->get_element_dimensions("music_area", file_a);
  float speed = static_cast<float>(ao_app->get_font_property("music_name_speed", file_b));

  QFont f_font = ui_vp_music_name->font();
  QFontMetrics fm(f_font);
  int dist;
  if (ao_app->read_theme_ini_bool("enable_const_music_speed", cc_config_ini))
    dist = res_b.width;
  else
    dist = fm.horizontalAdvance(ui_vp_music_name->toPlainText());
  int time = static_cast<int>(1000000 * dist / speed);
  music_anim->setLoopCount(-1);
  music_anim->setDuration(time);
  music_anim->setStartValue(QRect(res_b.width + res_b.x, res_a.y, res_a.width, res_a.height));
  music_anim->setEndValue(QRect(-dist + res_a.x, res_a.y, res_a.width, res_a.height));
  music_anim->start();
}

void Courtroom::handle_clock(QString time)
{
  current_clock = time.toInt();
  if (current_clock < 0 || current_clock > 23)
    current_clock = -1;
  qInfo() << QString("Clock time changed to %1").arg(current_clock);

  ui_vp_clock->hide();

  if (current_clock == -1)
  {
    qInfo() << "Unknown time; no asset to be used.";
    return;
  }

  qDebug() << "Displaying clock asset...";
  QString clock_filename = "hours/" + QString::number(current_clock);
  const QString asset_path = ao_app->find_theme_asset_path(clock_filename, animated_or_static_extensions());
  if (asset_path.isEmpty())
  {
    qDebug() << "Asset not found; aborting.";
    return;
  }
  ui_vp_clock->play(clock_filename);
  ui_vp_clock->show();
}

void Courtroom::handle_gamemode(QString gamemode)
{
  // only manual gamemode changes are allowed
  if (ao_app->get_manual_gamemode_enabled())
    return;
  if (ao_app->get_gamemode() == gamemode)
    return;
  ao_app->set_gamemode(gamemode);
  on_app_reload_theme_requested();
}

void Courtroom::handle_timeofday(QString timeofday)
{
  // only manual gamemode changes are allowed
  if (ao_app->get_manual_timeofday_enabled())
    return;
  if (ao_app->get_timeofday() == timeofday)
    return;
  ao_app->set_timeofday(timeofday);
  on_app_reload_theme_requested();
}

void Courtroom::list_music()
{
  ui_music_list->clear();

  QString f_file = design_ini;

  QBrush found_brush(ao_app->get_color("found_song_color", f_file));
  QBrush missing_brush(ao_app->get_color("missing_song_color", f_file));

  int n_listed_songs = 0;

  for (int n_song = 0; n_song < music_list.size(); ++n_song)
  {
    QString i_song = music_list.at(n_song);

    if (i_song.toLower().contains(ui_music_search->text().toLower()))
    {
      ui_music_list->addItem(i_song);

      QString song_root = ao_app->get_music_path(i_song);
      QString song_path = ao_app->find_asset_path({song_root}, audio_extensions());

      if (!song_path.isEmpty())
        ui_music_list->item(n_listed_songs)->setBackground(found_brush);
      else
        ui_music_list->item(n_listed_songs)->setBackground(missing_brush);

      ++n_listed_songs;
    }
  }
}

void Courtroom::list_areas()
{
  ui_area_list->clear();
  //  area_names.clear();

  QString f_file = "courtroom_design.ini";

  QBrush default_color(ao_app->get_color("area_free_color", f_file));

  int n_listed_areas = 0;

  for (int n_area = 0; n_area < area_list.size(); ++n_area)
  {
    QString i_area = "";

    i_area.append(area_list.at(n_area));

    if (i_area.toLower().contains(ui_music_search->text().toLower()))
    {
      ui_area_list->addItem(i_area);
      ui_area_list->item(n_listed_areas)->setBackground(default_color);
      ++n_listed_areas;
    }
  }
}

QString Courtroom::current_sfx_file()
{
  QListWidgetItem *l_item = ui_sfx_list->currentItem();
  if (l_item == nullptr)
    return nullptr;
  const QString l_file = m_sfx_list.at(l_item->data(Qt::UserRole).toInt()).file;
  return l_file == m_sfx_default_file ? get_current_emote().sound_file : l_file;
}

void Courtroom::update_sfx_list()
{
  // colors
  m_sfx_color_found = ao_app->get_color("found_song_color", design_ini);
  m_sfx_color_missing = ao_app->get_color("missing_song_color", design_ini);

  // items
  m_sfx_list.clear();
  m_sfx_list.append(DR::SFX("Default", m_sfx_default_file));
  m_sfx_list.append(DR::SFX("Silence", nullptr));

  const QStringList l_sfx_list = ao_app->get_sfx_list();
  for (const QString &i_sfx_line : l_sfx_list)
  {
    const QStringList l_sfx_entry = i_sfx_line.split("=", DR::SkipEmptyParts);

    const QString l_name = l_sfx_entry.at(l_sfx_entry.size() - 1).trimmed();
    const QString l_file = QString(l_sfx_entry.size() >= 2 ? l_sfx_entry.at(0) : nullptr).trimmed();
    const bool l_is_found = !ao_app->find_asset_path({ao_app->get_sounds_path(l_file)}, audio_extensions()).isEmpty();
    m_sfx_list.append(DR::SFX(l_name, l_file, l_is_found));
  }

  update_sfx_widget_list();
}

void Courtroom::update_sfx_widget_list()
{
  QSignalBlocker l_blocker(ui_sfx_list);
  ui_sfx_list->clear();

  const QString l_name_filter = ui_sfx_search->text();
  for (int i = 0; i < m_sfx_list.length(); ++i)
  {
    const DR::SFX &i_sfx = m_sfx_list.at(i);
    if (!i_sfx.name.contains(l_name_filter, Qt::CaseInsensitive))
      continue;
    QListWidgetItem *l_item = new QListWidgetItem;
    l_item->setText(i_sfx.name);
    l_item->setData(Qt::UserRole, i);
    ui_sfx_list->addItem(l_item);
  }

  on_sfx_widget_list_row_changed();
}

void Courtroom::select_default_sfx()
{
  if (ui_sfx_list->count() == 0)
    return;
  ui_sfx_list->setCurrentRow(0);
}

void Courtroom::clear_sfx_selection()
{
  ui_sfx_list->setCurrentRow(-1);
}

void Courtroom::on_sfx_search_editing_finished()
{
  update_sfx_list();
}

void Courtroom::on_sfx_widget_list_row_changed()
{
  const int p_current_row = ui_sfx_list->currentRow();

  for (int i = 0; i < ui_sfx_list->count(); ++i)
  {
    QListWidgetItem *l_item = ui_sfx_list->item(i);
    const bool l_is_found = m_sfx_list.at(l_item->data(Qt::UserRole).toInt()).is_found;

    QColor i_color = l_is_found ? m_sfx_color_found : m_sfx_color_missing;
    if (i == p_current_row)
    {
      ui_pre->setChecked(ui_pre->isChecked() || l_is_found);

      // Calculate the amount of lightness it would take to light up the row. We
      // also limit it to 1.0, as giving lightness values above 1.0 to QColor does
      // nothing. +0.4 is just an arbitrarily chosen number.
      const double l_final_lightness = qMin(1.0, i_color.lightnessF() + 0.4);

      // This is just the reverse of the above, basically. We set the colour, and we
      // set the brush to have that colour.
      i_color.setHslF(i_color.hueF(), i_color.saturationF(), l_final_lightness);
    }

    l_item->setBackground(i_color);
  }
  ui_ic_chat_message->setFocus();
}

void Courtroom::list_note_files()
{
  QString f_config = ao_app->get_base_path() + file_select_ini;
  QFile f_file(f_config);
  if (!f_file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Couldn't open" << f_config;
    return;
  }

  note_list.clear();

  QString f_filestring = "";
  QString f_filename = "";

  QTextStream in(&f_file);

  QVBoxLayout *f_layout = ui_note_area->m_layout;

  while (!in.atEnd())
  {
    QString line = in.readLine().trimmed();

    QStringList f_contents = line.split("=");
    if (f_contents.size() < 2)
      continue;

    int f_index = f_contents.at(0).toInt();
    f_filestring = f_filename = f_contents.at(1).trimmed();

    if (f_contents.size() > 2)
      f_filename = f_contents.at(2).trimmed();

    while (f_index >= f_layout->count())
      on_add_button_clicked();

    AONotePicker *f_notepicker = static_cast<AONotePicker *>(f_layout->itemAt(f_index)->widget());
    f_notepicker->m_line->setText(f_filename);
    f_notepicker->real_file = f_filestring;
  }
}

void Courtroom::load_note()
{
  // Do not attempt to load anything if no file was chosen. This makes it so
  // that notepad text is kept in client if the user has decided not to choose a
  // file to save to. Of course, this is ephimeral storage, it will be erased
  // after the client closes or when the user decides to load a file.
  if (current_file.isEmpty())
    return;
  QString f_text = ao_app->read_note(current_file);
  ui_vp_notepad->setText(f_text);
}

void Courtroom::save_note()
{

  QString f_text = ui_vp_notepad->toPlainText();

  ao_app->write_note(f_text, current_file);
}

void Courtroom::save_textlog(QString p_text)
{
  QString f_file = ao_app->get_base_path() + icchatlogsfilename;

  ao_app->append_note("[" + QTime::currentTime().toString() + "]" + p_text, f_file);
}

void Courtroom::append_server_chatmessage(QString p_name, QString p_message)
{
  ui_server_chatlog->append_chatmessage(p_name, p_message);
  if (ao_config->log_is_recording_enabled())
    save_textlog("(OOC)" + p_name + ": " + p_message);
}

void Courtroom::on_showname_changed(QString p_showname)
{
  ui_ic_chat_showname->setText(p_showname);
  send_showname_packet(p_showname);
}

void Courtroom::send_showname_packet(QString p_showname)
{
  if (ao_app->m_FL_showname_enabled)
  {
    QStringList l_content = {p_showname};
    ao_app->send_server_packet(new AOPacket("SN", l_content));
  }
  else
  {
    send_ooc_packet(ao_config->username(), QString("/showname %1").arg(p_showname));
  }
}

bool Courtroom::is_spectating()
{
  return m_cid == -1;
}

void Courtroom::on_showname_placeholder_changed(QString p_showname_placeholder)
{
  const QString l_showname(p_showname_placeholder.trimmed().isEmpty() ? "Showname" : p_showname_placeholder);
  ui_ic_chat_showname->setPlaceholderText(l_showname);
  ui_ic_chat_showname->setToolTip(l_showname);
}

void Courtroom::on_ic_message_return_pressed()
{
  if (ui_ic_chat_message->text() == "" || is_client_muted)
    return;

  if ((anim_state < 3 || text_state < 2) && m_shout_state == 0)
    return;

  if (!m_showname_sent)
  {
    m_showname_sent = true;
    send_showname_packet(ao_config->showname());
  }
  // MS#
  // deskmod#
  // pre-emote#
  // character#
  // emote#
  // message#
  // side#
  // sfx-name#
  // emote_modifier#
  // char_id#
  // sfx_delay#
  // objection_modifier#
  // evidence#
  // placeholder#
  // realization#
  // text_color#%

  QStringList packet_contents;

  const DREmote &l_emote = get_emote(m_current_emote_id);

  const QString l_desk_modifier =
      l_emote.desk_modifier == -1 ? QString("chat") : QString::number(l_emote.desk_modifier);
  packet_contents.append(l_desk_modifier);

  packet_contents.append(l_emote.anim);

  packet_contents.append(current_char);

  if (ui_hidden->isChecked())
    packet_contents.append("../../misc/blank");
  else
    packet_contents.append(l_emote.dialog);

  packet_contents.append(ui_ic_chat_message->text());

  const QString l_side = ao_app->get_char_side(current_char);
  packet_contents.append(l_side);

  // sfx file
  const QString l_sound_file = current_sfx_file();
  packet_contents.append(l_sound_file.isEmpty() ? "0" : l_sound_file);
  // TODO remove empty string workaround for pre-DRO 1.0.0

  int l_emote_modifier = l_emote.modifier;
  // needed or else legacy won't understand what we're saying
  if (m_shout_state > 0)
  {
    if (l_emote_modifier == 5)
      l_emote_modifier = 6;
    else
      l_emote_modifier = 2;
  }
  else if (ui_pre->isChecked())
  {
    if (l_emote_modifier == 0)
      l_emote_modifier = 1;
  }
  else
  {
    if (l_emote_modifier == 1)
      l_emote_modifier = 0;
    else if (l_emote_modifier == 4)
      l_emote_modifier = 5;
  }

  packet_contents.append(QString::number(l_emote_modifier));
  packet_contents.append(QString::number(m_cid));

  if (l_emote.sound_file == current_sfx_file())
    packet_contents.append(QString::number(l_emote.sound_delay));
  else
    packet_contents.append("0");

  QString f_obj_state;

  if (m_shout_state < 0)
    f_obj_state = "0";
  else
    f_obj_state = QString::number(m_shout_state);

  packet_contents.append(f_obj_state);

  if (is_presenting_evidence)
    // the evidence index is shifted by 1 because 0 is no evidence per legacy
    // standards besides, older clients crash if we pass -1
    packet_contents.append(QString::number(current_evidence + 1));
  else
    packet_contents.append("0");

  QString f_flip = ui_flip->isChecked() ? "1" : "0";
  packet_contents.append(f_flip);

  packet_contents.append(QString::number(m_effect_state));

  QString f_text_color;

  if (m_text_color < 0)
    f_text_color = "0";
  else if (m_text_color > ui_text_color->count())
    f_text_color = "0";
  else
    f_text_color = QString::number(m_text_color);

  packet_contents.append(f_text_color);
  ao_app->send_server_packet(new AOPacket("MS", packet_contents));
}

void Courtroom::handle_acknowledged_ms()
{
  ui_ic_chat_message->clear();

  // reset states
  ui_pre->setChecked(ao_config->always_pre_enabled());

  reset_shout_buttons();
  reset_effect_buttons();
  reset_wtce_buttons();
  clear_sfx_selection();

  is_presenting_evidence = false;
  ui_evidence_present->set_image("present_disabled.png");
}

void Courtroom::handle_chatmessage(QStringList p_contents)
{
  if (p_contents.size() < 15)
    return;
  else if (p_contents.size() == 15)
    p_contents.append(QString());

  for (int i = 0; i < chatmessage_size; ++i)
    m_chatmessage[i] = p_contents[i];

  int f_char_id = m_chatmessage[CMChrId].toInt();

  if (f_char_id == -1)
  {
    is_system_speaking = true;
    m_chatmessage[CMChrId] = "0";
    f_char_id = 0;
  }
  else
    is_system_speaking = false;

  if (f_char_id < 0 || f_char_id >= char_list.size())
    return;

  if (mute_map.value(m_chatmessage[CMChrId].toInt()))
    return;

  chatmessage_is_empty = m_chatmessage[CMMessage] == " " || m_chatmessage[CMMessage] == "";
  m_msg_is_first_person = false;

  // reset our ui state if client just spoke
  if (m_cid == f_char_id && is_system_speaking == false)
  {
#ifdef DRO_ACKMS // TODO WARNING remove entire block on 1.0.0 release
    // If the server does not have the feature of acknowledging our MS
    // messages, assume in this if that the message is proof the server
    // acknowledged our message. It is not quite the same, as it is
    // possible the server crafted a message with the same char_id
    // as the client, but the client did not send that message, but it is
    // the best we can do.
    if (!ao_app->m_FL_ackMS_enabled)
    {
      handle_acknowledged_ms();
    }

    // If the server does have the feature of acknowledging our MS messages,
    // it will have sent an ackMS packet prior to the MS one, so chat would
    // have been cleared and thus we need not do anything else.
#endif

    // update first person mode status
    m_msg_is_first_person = ao_app->get_first_person_enabled();
  }

  QString f_showname;
  qDebug() << "handle_chatmessage";

  // We actually DO wanna fail here if the showname is empty but the system is
  // speaking.
  // Having an empty showname for system is actually what we expect.
  if (m_chatmessage[CMShowName].isEmpty() && !is_system_speaking)
  {
    f_showname = ao_app->get_showname(char_list.at(f_char_id).name);
  }
  else
  {
    f_showname = m_chatmessage[CMShowName];
  }

  m_effects_player->stop_all();

  text_state = 0;
  anim_state = 0;
  ui_vp_objection->stop();
  chat_tick_timer->stop();
  ui_vp_evidence_display->reset();

  // reset effect
  ui_vp_effect->stop();

  if (is_system_speaking)
    append_system_text(f_showname, m_chatmessage[CMMessage]);
  else
    append_ic_text(f_showname, m_chatmessage[CMMessage], false, false, f_char_id == m_cid);

  if (ao_config->log_is_recording_enabled() && (!chatmessage_is_empty || !is_system_speaking))
  {
    save_textlog(f_showname + ": " + m_chatmessage[CMMessage]);
  }

  int objection_mod = m_chatmessage[CMShoutModifier].toInt();
  QString f_char = m_chatmessage[CMChrName];

  // if an objection is used
  if (objection_mod > 0)
  {
    int emote_mod = m_chatmessage[CMEmoteModifier].toInt();
    if (emote_mod == 0)
      m_chatmessage[CMEmoteModifier] = 1;

    // handles cases 1-8 (5-8 are DRO only)
    if (objection_mod >= 1 && objection_mod <= ui_shouts.size() && ui_shouts.size() > 0) // check to prevent crashing
    {
      ui_vp_objection->play_interjection(f_char, shout_names.at(objection_mod - 1));
      m_shouts_player->play(f_char, shout_names.at(objection_mod - 1));
    }
    else
      qDebug() << "W: Shout identifier unknown" << objection_mod;
  }
  else
    handle_chatmessage_2();
}

void Courtroom::objection_done()
{
  handle_chatmessage_2();
}

void Courtroom::handle_chatmessage_2() // handles IC
{
  ui_vp_speedlines->stop();
  ui_vp_player_char->stop();

  qDebug() << "handle_chatmessage_2";

  if (shout_delayed_reload_theme)
  {
    shout_delayed_reload_theme = false;
    on_app_reload_theme_requested();
  }

  QString real_name = char_list.at(m_chatmessage[CMChrId].toInt()).name;

  QString f_showname;

  if (m_chatmessage[CMShowName].isEmpty())
  {
    f_showname = ao_app->get_showname(real_name);
  }
  else
  {
    f_showname = m_chatmessage[CMShowName];
  }

  ui_vp_showname->setText(f_showname);

  ui_vp_message->clear();
  ui_vp_chatbox->hide();
  ui_vp_showname_image->hide();

  QString l_chatbox_name = ao_app->get_chat(m_chatmessage[CMChrName]);

  if (l_chatbox_name.isEmpty())
  {
    l_chatbox_name = "chatmed.png";

    if (ao_config->log_display_self_highlight_enabled() && m_chatmessage[CMChrId].toInt() == m_cid)
    {
      const QString l_chatbox_self_name = "chatbox_self.png";
      if (file_exists(ao_app->find_theme_asset_path(l_chatbox_self_name)))
        l_chatbox_name = l_chatbox_self_name;
    }

    ui_vp_chatbox->set_image(l_chatbox_name);
  }
  else
  {
    QString chatbox_path = ao_app->get_base_path() + "misc/" + l_chatbox_name + ".png";
    ui_vp_chatbox->set_image_from_path(chatbox_path);
  }

  if (m_msg_is_first_person == false)
  {
    set_scene();
  }

  int emote_mod = m_chatmessage[CMEmoteModifier].toInt();

  if (m_chatmessage[CMFlipState].toInt() == 1)
    ui_vp_player_char->set_mirror_enabled(true);
  else
    ui_vp_player_char->set_mirror_enabled(false);

  switch (emote_mod)
  {
  case 1:
  case 2:
  case 6:
    play_preanim();
    break;
  default:
    qDebug() << "W: invalid emote mod: " << QString::number(emote_mod);
    // intentional fallthru
  case 0:
  case 5:
    handle_chatmessage_3();
  }
}

void Courtroom::handle_chatmessage_3()
{
  qDebug() << "handle_chatmessage_3";

  setup_chat();
  int f_evi_id = m_chatmessage[CMEvidenceId].toInt();
  QString f_side = m_chatmessage[CMPosition];

  if (f_evi_id > 0 && f_evi_id <= local_evidence_list.size())
  {
    // shifted by 1 because 0 is no evidence per legacy standards
    QString f_image = local_evidence_list.at(f_evi_id - 1).image;
    // def jud and hlp should display the evidence icon on the RIGHT side
    bool is_left_side = !(f_side == "def" || f_side == "hlp" || f_side == "jud");
    ui_vp_evidence_display->show_evidence(f_image, is_left_side);
  }

  int emote_mod = m_chatmessage[CMEmoteModifier].toInt();

  if (emote_mod == 5 || emote_mod == 6)
  {
    QString side = m_chatmessage[CMPosition];
    ui_vp_desk->hide();

    if (side == "pro" || side == "hlp" || side == "wit")
      ui_vp_speedlines->play("prosecution_speedlines");
    else
      ui_vp_speedlines->play("defense_speedlines");
  }

  int f_anim_state = 0;
  // BLUE is from an enum in datatypes.h
  bool text_is_blue = m_chatmessage[CMTextColor].toInt() == DR::CBlue;

  if (!text_is_blue && text_state == 1)
    // talking
    f_anim_state = 2;
  else
    // idle
    f_anim_state = 3;

  if (f_anim_state <= anim_state)
    return;

  ui_vp_player_char->stop();
  const QString f_char = m_chatmessage[CMChrName];
  const QString f_emote = m_chatmessage[CMEmote];
  const bool l_hide_emote = (f_emote == "../../misc/blank");

  QString path;
  if (!chatmessage_is_empty && ao_app->read_theme_ini_bool("enable_showname_image", cc_config_ini))
  {
    // Asset lookup order
    // 1. In the theme folder (gamemode-timeofday/main/default), in the character
    // folder, look for "showname" + extensions in `exts` in order
    // 2. In the character folder, look for
    // "showname" + extensions in `exts` in order

    path = ao_app->find_theme_asset_path("characters/" + f_char + "/showname", {".png"});
    if (path.isEmpty())
      path = ao_app->find_asset_path({ao_app->get_character_path(f_char, "showname")}, {".png"});
  }

  // Path may be empty if
  // 1. Chat message was empty
  // 2. Enable showname images was false
  // 3. No valid showname image was found
  if (!path.isEmpty())
  {
    ui_vp_showname->hide();
    ui_vp_showname_image->set_image_from_path(path);
    ui_vp_showname_image->show();
  }
  else
  {
    ui_vp_showname->show();
    ui_vp_showname_image->hide();
  }

  ui_vp_player_char->setHidden(l_hide_emote);
  switch (f_anim_state)
  {
  case 2:
    if (m_msg_is_first_person == false)
    {
      ui_vp_player_char->play_talk(f_char, f_emote);
    }
    anim_state = 2;
    break;
  default:
    qDebug() << "W: invalid anim_state: " << f_anim_state;
    [[fallthrough]];
  case 3:
    if (m_msg_is_first_person == false)
    {
      ui_vp_player_char->play_idle(f_char, f_emote);
    }
    anim_state = 3;
    break;
  }

  int effect = m_chatmessage[CMEffectState].toInt();
  QStringList offset = ao_app->get_effect_offset(f_char, effect);
  ui_vp_effect->move(ui_viewport->x() + offset.at(0).toInt(), ui_viewport->y() + offset.at(1).toInt());

  QStringList overlay = ao_app->get_overlay(f_char, effect);
  QString overlay_name = overlay.at(0);
  QString overlay_sfx = overlay.at(1);

  if (effect > 0 && effect <= ui_effects.size() && effect_names.size() > 0) // check to prevent crashing
  {
    QString s_eff = effect_names.at(effect - 1);
    QStringList f_eff = ao_app->get_effect(effect);

    bool once = f_eff.at(1).trimmed().toInt();

    if (overlay_sfx == "")
      overlay_sfx = ao_app->get_sfx(s_eff);
    m_effects_player->play_effect(overlay_sfx);
    ui_vp_effect->set_play_once(once);
    if (overlay_name == "")
      overlay_name = s_eff;
    ui_vp_effect->play(overlay_name, f_char);
  }

  QString f_message = m_chatmessage[CMMessage];
  QStringList callwords = ao_app->get_callwords();

  for (const QString &word : callwords)
  {
    if (f_message.contains(word, Qt::CaseInsensitive))
    {
      m_system_player->play(ao_app->get_sfx("word_call"));
      ao_app->alert(this);
      const QString name = "CLIENT";
      const QString message =
          ui_vp_showname->toPlainText() + " has called you via your callword \"" + word + "\": \"" + f_message + "\"";
      ui_server_chatlog->append_chatmessage(name, message);
      if (ao_config->log_is_recording_enabled())
        save_textlog("(OOC)" + name + ": " + message);
      break;
    }
  }

  chat_tick_timer->start(ao_app->get_chat_tick_interval());
}

void Courtroom::on_chat_config_changed()
{
  update_ic_log(true);
}

void Courtroom::update_ic_log(bool p_reset_log)
{
  // resize if needed
  const int record_count = m_ic_record_list.length() + m_ic_record_queue.length();
  if (record_count > ao_config->log_max_lines())
    m_ic_record_list = m_ic_record_list.mid(record_count - ao_config->log_max_lines());

  if (p_reset_log)
  {
    // Turn off auto align. That is because we are going to be performing a lot of text change operations
    // but we don't necessarily care the intermediate states are not aligned
    ui_ic_chatlog->set_auto_align(false);
    // we need all recordings
    QQueue<DR::ChatRecord> new_queue;
    while (!m_ic_record_list.isEmpty())
      new_queue.append(m_ic_record_list.takeFirst());
    new_queue.append(m_ic_record_queue);
    m_ic_record_queue = std::move(new_queue);

    // clear log
    ui_ic_chatlog->clear();
  }

  // prepare the formats we need
  // default color
  QColor default_color = ao_app->get_color("ic_chatlog_color", fonts_ini);
  QColor not_found_color = QColor(255, 255, 255);

  QTextCharFormat name_format = ui_ic_chatlog->currentCharFormat();
  if (ao_app->get_font_property("ic_chatlog_bold", fonts_ini))
    name_format.setFontWeight(QFont::Bold);
  else
    name_format.setFontWeight(QFont::Normal);

  QColor showname_color = ao_app->get_color("ic_chatlog_showname_color", fonts_ini);
  if (showname_color == not_found_color)
    showname_color = default_color;
  name_format.setForeground(showname_color);

  QTextCharFormat selfname_format = name_format;

  if (ao_config->log_display_self_highlight_enabled())
  {
    QColor selfname_color = ao_app->get_color("ic_chatlog_selfname_color", fonts_ini);
    if (selfname_color == not_found_color)
      selfname_color = showname_color;
    selfname_format.setForeground(selfname_color);
  }

  QTextCharFormat line_format = ui_ic_chatlog->currentCharFormat();
  line_format.setFontWeight(QFont::Normal);
  QColor message_color = ao_app->get_color("ic_chatlog_message_color", fonts_ini);
  if (message_color == not_found_color)
    message_color = default_color;
  line_format.setForeground(message_color);

  QTextCharFormat system_format = ui_ic_chatlog->currentCharFormat();
  system_format.setFontWeight(QFont::Normal);
  QColor system_color = ao_app->get_color("ic_chatlog_system_color", fonts_ini);
  if (system_color == not_found_color)
    system_color = not_found_color;
  system_format.setForeground(system_color);

  // need vscroll bar for cache
  QScrollBar *vscrollbar = ui_ic_chatlog->verticalScrollBar();

  // format values
  const bool chatlog_scrolldown = ao_config->log_is_topdown_enabled();
  const bool chatlog_newline = ao_config->log_format_use_newline_enabled();

  // cache previous values
  const QTextCursor prev_cursor = ui_ic_chatlog->textCursor();
  const int scroll_pos = vscrollbar->value();
  const bool is_scrolled =
      chatlog_scrolldown ? scroll_pos == vscrollbar->maximum() : scroll_pos == vscrollbar->minimum();

  // recover cursor
  QTextCursor cursor = ui_ic_chatlog->textCursor();
  // figure out if we need to move up or down
  const QTextCursor::MoveOperation move_type = chatlog_scrolldown ? QTextCursor::End : QTextCursor::Start;

  while (!m_ic_record_queue.isEmpty())
  {
    DR::ChatRecord record = m_ic_record_queue.takeFirst();
    m_ic_record_list.append(record);
    const QTextCharFormat l_record_name_format = record.is_self() ? selfname_format : name_format;

    if (record.get_message().trimmed().isEmpty() && !ao_config->log_display_empty_messages_enabled())
      continue;

    if (record.is_music() && !ao_config->log_display_music_switch_enabled())
      continue;

    // move cursor
    cursor.movePosition(move_type);

    const QString record_end = (QString(QChar::LineFeed) + (chatlog_newline ? QString(QChar::LineFeed) : ""));

    if (ao_config->log_display_timestamp_enabled())
      cursor.insertText(QString("[%1] ").arg(record.get_timestamp().toString("hh:mm")), l_record_name_format);

    if (record.is_system())
    {
      cursor.insertText(record.get_message() + record_end, system_format);
    }
    else
    {
      QString separator;
      if (chatlog_newline)
        separator = QString(QChar::LineFeed);
      else if (!record.is_music())
        separator = ": ";
      else
        separator = " ";
      cursor.insertText(record.get_name() + separator, l_record_name_format);
      cursor.insertText(record.get_message() + record_end, line_format);
    }
  }

  // figure out the number of blocks we need overall
  // this is always going to amount to at least the current length of records
  int block_count = m_ic_record_list.length() + 1; // there's always one extra block
  // to do that, we need to go through the records
  for (DR::ChatRecord &record : m_ic_record_list)
    if (!record.is_system())
      if (chatlog_newline)
        block_count += 2; // if newline is actived, it always inserts two extra
                          // newlines; therefor two paragraphs

  // there's always one extra block count, so deduce one from block_count
  int blocks_to_delete = ui_ic_chatlog->document()->blockCount() - block_count;

  // the orientation at which we need to delete from
  const QTextCursor::MoveOperation start_location = chatlog_scrolldown ? QTextCursor::Start : QTextCursor::End;
  const QTextCursor::MoveOperation block_orientation =
      chatlog_scrolldown ? QTextCursor::NextBlock : QTextCursor::PreviousBlock;

  /* Blocks appear like this
   * textQChar(0x2029)
   * additionaltextQChar(0x2029)
   * moretextQChar(0x2029)
   * where QChar(0x2029) is the paragraph break block.
   * Do note that the above example has FOUR blocks: text, additionaltext,
   * moretext, and an empty block. That is because QTextCursor separates blocks
   * by paragraph break block (which is why the above code has a -1) and does
   * not consider this break character as part of the block (which is why we
   * move Left in the loop, to 'be in the block'). Finally, BlockUnderCursor
   * does NOT select the break character, so we deleteChar after removing the
   * selection to remove the straggling newline.
   * */

  // move our cursor at the start
  cursor.movePosition(start_location);

  // move the cursor around, depending on the orientation we need
  for (int i = 0; i < blocks_to_delete; ++i)
    cursor.movePosition(block_orientation, QTextCursor::KeepAnchor);

  // now that everything is selected, delete it
  cursor.removeSelectedText();

  /*
   * However, if we do this, we also remove the last newline of the last block
   * that remains, which will make it difficult to append new blocks to
   * it/figure out the amount of blocks if we have a scroll up log, so we add it
   * again if we removed any break characters at all
   * */
  if (!chatlog_scrolldown && blocks_to_delete > 0)
    cursor.insertBlock();

  /*
   * Unfortunately, the simplest alternative, that is, move cursor to the last
   * block, remove the block under it and delete the last char does not work, as
   * this also removes the last character of the block that remains. That's why
   * we have to do this whole complicated process.
   * */
  if (prev_cursor.hasSelection() || !is_scrolled)
  {
    // restore previous selection and vscrollbar
    ui_ic_chatlog->setTextCursor(prev_cursor);
    vscrollbar->setValue(scroll_pos);
  }
  // scroll up/down depending on context
  else
  {
    ui_ic_chatlog->moveCursor(move_type);
    vscrollbar->setValue(chatlog_scrolldown ? vscrollbar->maximum() : vscrollbar->minimum());
  }

  if (p_reset_log)
  {
    // We are done updating the IC chat log, now do all alignment computations
    ui_ic_chatlog->set_auto_align(true);
  }
}

void Courtroom::append_ic_text(QString p_name, QString p_line, bool p_system, bool p_music, bool p_self)
{
  if (p_name.trimmed().isEmpty())
    p_name = "Anonymous";

  if (p_line.trimmed().isEmpty())
    p_line = p_line.trimmed();

  DR::ChatRecord new_record(p_name, p_line);
  new_record.set_music(p_music);
  new_record.set_system(p_system);
  new_record.set_self(p_self);
  m_ic_record_queue.append(new_record);
  update_ic_log(false);
}

void Courtroom::append_system_text(QString p_showname, QString p_line)
{
  if (chatmessage_is_empty)
    return;

  append_ic_text(p_showname, p_line, true, false, false);
}

void Courtroom::play_preanim()
{
  // all time values in char.inis are multiplied by a constant(time_mod) to get
  // the actual time
  int sfx_delay = m_chatmessage[CMSoundDelay].toInt() * 60;
  sfx_delay_timer->start(sfx_delay);

  QString f_preanim = m_chatmessage[CMPreAnim];

  if (f_preanim.trimmed() == "-")
  {
    // no animation, continue
    preanim_done();
    return;
  }

  QString f_char = m_chatmessage[CMChrName];
  // set state
  anim_state = 1;

  if (m_msg_is_first_person == false)
  {
    QString f_anim_path = ao_app->get_character_path(f_char, f_preanim);
    if (ui_vp_player_char->play_pre(f_char, f_preanim))
    {
      qDebug() << "Playing" << f_anim_path;

      // finished
      return;
    }
    else
    {
      qDebug() << "could not find " + f_anim_path;
    }
  }

  // no animation, continue
  preanim_done();
}

void Courtroom::preanim_done()
{
  handle_chatmessage_3();
}

void Courtroom::realization_done()
{
  ui_vp_effect->stop();
}

void Courtroom::setup_chat()
{
  ui_vp_message->clear();

  set_text_color();
  rainbow_counter = 0;
  // we need to ensure that the text isn't already ticking because this function
  // can be called by two logic paths
  if (text_state != 0)
    return;

  if (chatmessage_is_empty)
  {
    // since the message is empty, it's technically done ticking
    text_state = 2;
    return;
  }

  ui_vp_chatbox->show();

  tick_pos = 0;
  blip_pos = 0;

  // Cache these so chat_tick performs better
  m_chatbox_message_outline = (ao_app->get_font_property("message_outline", fonts_ini) == 1);
  m_chatbox_message_enable_highlighting = (ao_app->read_theme_ini_bool("enable_highlighting", cc_config_ini));
  m_chatbox_message_highlight_colors = ao_app->get_highlight_colors();

  QString f_gender = ao_app->get_gender(m_chatmessage[CMChrName]);

  m_blips_player->set_blips("sfx-blip" + f_gender + ".wav");

  // means text is currently ticking
  text_state = 1;
}

void Courtroom::chat_tick()
{
  // note: this is called fairly often(every 60 ms when char is talking)
  // do not perform heavy operations here
  QTextCharFormat vp_message_format = ui_vp_message->currentCharFormat();
  if (m_chatbox_message_outline)
    vp_message_format.setTextOutline(QPen(Qt::black, 1));
  else
    vp_message_format.setTextOutline(Qt::NoPen);

  QString f_message = m_chatmessage[CMMessage];

  if (tick_pos >= f_message.size())
  {
    text_state = 2;
    chat_tick_timer->stop();
    anim_state = 3;

    if (m_msg_is_first_person == false)
    {
      ui_vp_player_char->play_idle(m_chatmessage[CMChrName], m_chatmessage[CMEmote]);
    }

    m_string_color = "";
    m_color_stack.clear();
  }

  else
  {
    QString f_character = f_message.at(tick_pos);

    if (f_character == " ")
      ui_vp_message->insertPlainText(" ");
    else if (m_chatmessage[CMTextColor].toInt() == DR::CRainbow)
    {
      QString html_color;

      switch (rainbow_counter)
      {
      case 0:
        html_color = "#BA1518";
        break;
      case 1:
        html_color = "#D55900";
        break;
      case 2:
        html_color = "#E7CE4E";
        break;
      case 3:
        html_color = "#65C856";
        break;
      default:
        html_color = "#1596C8";
        rainbow_counter = -1;
      }

      ++rainbow_counter;
      // Apply color to the next character
      QColor text_color;
      text_color.setNamedColor(html_color);
      vp_message_format.setForeground(text_color);

      ui_vp_message->textCursor().insertText(f_character, vp_message_format);
    }
    else if (m_chatbox_message_enable_highlighting)
    {
      bool highlight_found = false;
      bool render_character = true;
      // render_character should only be false if the character is a highlight
      // character specifically marked as a character that should not be
      // rendered.
      if (m_color_stack.isEmpty())
        m_color_stack.push("");

      for (const auto &col : m_chatbox_message_highlight_colors)
      {
        if (f_character == QString(col[0][0]) && m_string_color != col[1])
        {
          m_color_stack.push(col[1]);
          m_string_color = m_color_stack.top();
          highlight_found = true;
          render_character = (col[2] != "0");
          break;
        }
      }

      // Apply color to the next character
      if (m_string_color.isEmpty())
        vp_message_format.setForeground(m_base_string_color);
      else
      {
        QColor textColor;
        textColor.setNamedColor(m_string_color);
        vp_message_format.setForeground(textColor);
      }

      QString m_future_string_color = m_string_color;

      for (const auto &col : m_chatbox_message_highlight_colors)
      {
        if (f_character == QString(col[0][1]) && !highlight_found)
        {
          if (m_color_stack.size() > 1)
            m_color_stack.pop();
          m_future_string_color = m_color_stack.top();
          render_character = (col[2] != "0");
          break;
        }
      }

      if (render_character)
        ui_vp_message->textCursor().insertText(f_character, vp_message_format);

      m_string_color = m_future_string_color;
    }
    else
    {
      ui_vp_message->textCursor().insertText(f_character, vp_message_format);
    }

    QScrollBar *scroll = ui_vp_message->verticalScrollBar();
    scroll->setValue(scroll->maximum());

    if ((f_message.at(tick_pos) != ' ' || ao_config->blank_blips_enabled()))
    {

      if (blip_pos % ao_app->read_blip_rate() == 0)
      {
        blip_pos = 0;

        // play blip
        m_blips_player->blip_tick();
      }

      ++blip_pos;
    }

    ++tick_pos;
  }

  ui_vp_message->repaint();
}

void Courtroom::show_testimony()
{
  if (!testimony_in_progress || m_chatmessage[CMPosition] != "wit")
    return;

  ui_vp_testimony->show();

  testimony_show_timer->start(testimony_show_time);
}

void Courtroom::hide_testimony()
{
  ui_vp_testimony->hide();

  if (!testimony_in_progress)
    return;

  testimony_hide_timer->start(testimony_hide_time);
}

void Courtroom::play_sfx()
{
  const QString l_effect = m_chatmessage[CMSoundName];
  if (l_effect.isEmpty() || l_effect == "0" || l_effect == "1")
    return;
  const QString l_chr = m_chatmessage[CMChrName];
  m_effects_player->play_character_effect(l_chr, l_effect);
}

void Courtroom::set_text_color()
{
  const QMap<DR::Color, DR::ColorInfo> color_map = ao_app->get_chatmessage_colors();
  const DR::Color color = DR::Color(m_chatmessage[CMTextColor].toInt());
  const QString color_code = color_map[color_map.contains(color) ? color : DR::CDefault].code;
  ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0)");
  m_base_string_color.setNamedColor(color_code);
}

void Courtroom::set_mute(bool p_muted, int p_cid)
{
  if (p_cid != m_cid && p_cid != -1)
    return;

  if (p_muted)
    ui_muted->show();
  else
  {
    ui_muted->hide();
    ui_ic_chat_message->setFocus();
  }

  ui_muted->resize(ui_ic_chat_message->width(), ui_ic_chat_message->height());
  ui_muted->set_image("muted.png");

  is_client_muted = p_muted;
  ui_ic_chat_message->setEnabled(!p_muted);
}

void Courtroom::set_ban(int p_cid)
{
  if (p_cid != m_cid && p_cid != -1)
    return;

  call_notice("You have been banned.");

  ao_app->construct_lobby();
  ao_app->destruct_courtroom();
}

void Courtroom::handle_song(QStringList p_contents)
{
  if (p_contents.size() < 2)
    return;

  QString f_song = p_contents.at(0);
  int l_chr_id = p_contents.at(1).toInt();

  for (auto &ext : audio_extensions())
  {
    QString r_song = f_song + ext;
    QString song_path = ao_app->get_music_path(r_song);
    if (file_exists(song_path))
    {
      f_song = r_song;
      break;
    }
  }

  if (l_chr_id < 0 || l_chr_id >= char_list.size())
  {
    m_music_player->play(f_song);
  }
  else
  {
    // This 2th argument corresponds to the showname to use when displaying the
    // music change message in IC
    // Backwards compatibility is explicitly kept for older versions of
    // tsuserver that do not send such an argument by assuming an empty showname
    // If there is an empty showname, the client will use instead the default
    // showname of the character.
    QString f_showname;
    if (p_contents.size() == 3)
    {
      f_showname = p_contents.at(2);
    }
    else
    {
      f_showname = "";
    }

    QString str_char;
    if (f_showname.isEmpty())
    {
      str_char = ao_app->get_showname(char_list.at(l_chr_id).name);
    }
    else
    {
      str_char = f_showname;
    }

    if (!mute_map.value(l_chr_id))
    {
      append_ic_text(str_char, "has played a song: " + f_song, false, true, l_chr_id == m_cid);
      if (ao_config->log_is_recording_enabled())
        save_textlog(str_char + " has played a song: " + f_song);
      m_music_player->play(f_song);
    }
  }

  int pos = f_song.lastIndexOf(QChar('.'));
  QString r_song = f_song.left(pos);

  ui_vp_music_name->setText(r_song);

  handle_music_anim();
}

void Courtroom::handle_wtce(QString p_wtce)
{
  int index = p_wtce.at(p_wtce.size() - 1).digitValue();
  if (index > 0 && index < wtce_names.size() + 1 && wtce_names.size() > 0) // check to prevent crash
  {
    p_wtce.chop(1); // looking for the 'testimony' part
    if (p_wtce == "testimony")
    {
      m_effects_player->play_effect(ao_app->get_sfx(wtce_names[index - 1]));
      ui_vp_wtce->play(wtce_names[index - 1]);
      if (index == 1)
      {
        testimony_in_progress = true;
      }
      else if (index == 2)
        testimony_in_progress = false;
    }
  }
}

void Courtroom::set_hp_bar(int p_bar, int p_state)
{
  if (p_state < 0 || p_state > 10)
    return;

  if (p_bar == 1)
  {
    ui_defense_bar->set_image("defensebar" + QString::number(p_state) + ".png");
    defense_bar_state = p_state;
  }
  else if (p_bar == 2)
  {
    ui_prosecution_bar->set_image("prosecutionbar" + QString::number(p_state) + ".png");
    prosecution_bar_state = p_state;
  }
}

void Courtroom::set_character_position(QString p_pos)
{
  int index = ui_pos_dropdown->findData(p_pos);
  if (index != -1)
    ui_pos_dropdown->setCurrentIndex(index);

  // enable judge mechanics if appropriate
  set_judge_enabled(p_pos == "jud");
}

void Courtroom::send_ooc_packet(QString ooc_name, QString ooc_message)
{
  if (ooc_name.trimmed().isEmpty())
  {
    bool ok;
    do
    {
      ooc_name = QInputDialog::getText(this, "Enter a name",
                                       "You must have a name to talk in OOC chat. Enter a name: ", QLineEdit::Normal,
                                       "user", &ok);
    } while (ok && ooc_name.isEmpty());
    if (!ok)
      return;

    ao_config->set_username(ooc_name);
  }

  if (ooc_message.trimmed().isEmpty())
  {
    append_server_chatmessage("CLIENT", "You cannot send an empty message.");
    return;
  }

  QStringList l_content{ooc_name, ooc_message};
  ao_app->send_server_packet(new AOPacket("CT", l_content));
}

void Courtroom::mod_called(QString p_ip)
{
  ui_server_chatlog->append(p_ip);
  if (ao_app->get_server_alerts_enabled())
  {
    m_system_player->play(ao_app->get_sfx("mod_call"));
    ao_app->alert(this);
    if (ao_config->log_is_recording_enabled())
      save_textlog("(OOC)(MOD CALL)" + p_ip);
  }
}

void Courtroom::on_ic_showname_editing_finished()
{
  set_showname(ui_ic_chat_showname->text());
}

void Courtroom::set_showname(QString p_showname)
{
  ao_config->set_showname(p_showname);
}

void Courtroom::on_ooc_name_editing_finished()
{
  ao_config->set_username(ui_ooc_chat_name->text());
}

void Courtroom::on_ooc_return_pressed()
{
  const QString ooc_name = ui_ooc_chat_name->text();
  const QString ooc_message = ui_ooc_chat_message->text();

  if (ooc_message.startsWith("/rainbow") && !rainbow_appended)
  {
    ui_text_color->addItem("Rainbow");
    ui_ooc_chat_message->clear();
    rainbow_appended = true;
    return;
  }
  else if (ooc_message.startsWith("/switch_am"))
  {
    on_switch_area_music_clicked();
    ui_ooc_chat_message->clear();
    return;
  }
  else if (ooc_message.startsWith("/rollp"))
  {
    m_effects_player->play_effect(ao_app->get_sfx("dice"));
  }
  else if (ooc_message.startsWith("/roll"))
  {
    m_effects_player->play_effect(ao_app->get_sfx("dice"));
  }
  else if (ooc_message.startsWith("/coinflip"))
  {
    m_effects_player->play_effect(ao_app->get_sfx("coinflip"));
  }
  else if (ooc_message.startsWith("/tp "))
  {
    int timer_id;
    if (space_location == -1)
      timer_id = 0;
    else
      timer_id = ooc_message.mid(space_location + 1).toInt();
    resume_timer(timer_id);
  }
  else if (ooc_message.startsWith("/ts "))
  {
    // Timer set
    QStringList arguments = ooc_message.split(" ");
    int size = arguments.size();

    // Note arguments[0] == "/ts", so every index (and thus length) is off by
    // one.
    if (size > 5)
      return;

    int timer_id = (size > 1 ? arguments[1].toInt() : 0);
    int new_time = (size > 2 ? arguments[2].toInt() : 300) * 1000;
    int timestep_length = (size > 3 ? arguments[3].toDouble() : -.016) * 1000;
    int firing_interval = (size > 4 ? arguments[4].toDouble() : .016) * 1000;
    set_timer_time(timer_id, new_time);
    set_timer_timestep(timer_id, timestep_length);
    set_timer_firing(timer_id, firing_interval);
  }
  else if (ooc_message.startsWith("/tp "))
  {
    // Timer pause
    int space_location = ooc_message.indexOf(" ");

    int timer_id;
    if (space_location == -1)
      timer_id = 0;
    else
      timer_id = ooc_message.mid(space_location + 1).toInt();
    pause_timer(timer_id);
  }

  send_ooc_packet(ooc_name, ooc_message);

  ui_ooc_chat_message->clear();
  ui_ooc_chat_message->setFocus();
}

void Courtroom::on_music_search_edited()
{
  list_music();
  list_areas();
}

void Courtroom::on_pos_dropdown_changed(int p_index)
{
  ui_ic_chat_message->setFocus();

  if (p_index < 0 || p_index > 5)
    return;

  QString f_pos;

  switch (p_index)
  {
  case 0:
    f_pos = "wit";
    break;
  case 1:
    f_pos = "def";
    break;
  case 2:
    f_pos = "pro";
    break;
  case 3:
    f_pos = "jud";
    break;
  case 4:
    f_pos = "hld";
    break;
  case 5:
    f_pos = "hlp";
    break;
  default:
    f_pos = "";
  }

  if (f_pos == "" || ui_ooc_chat_name->text() == "")
    return;

  set_judge_enabled(f_pos == "jud");

  ao_app->send_server_packet(new AOPacket("CT#" + ui_ooc_chat_name->text() + "#/pos " + f_pos + "#%"));
  // Uncomment later and remove above
  // Will only work in TSDR 4.3+ servers
  // ao_app->send_server_packet(new AOPacket("SP#" + f_pos + "#%"));
}

void Courtroom::on_mute_list_item_changed(QListWidgetItem *p_item)
{
  int f_cid = -1;

  for (int n_char = 0; n_char < char_list.size(); n_char++)
  {
    if (char_list.at(n_char).name == p_item->text())
      f_cid = n_char;
  }

  if (f_cid < 0 || f_cid >= char_list.size())
  {
    qDebug() << "W: " << p_item->text() << " not present in char_list";
    return;
  }

  if (Qt::CheckState::Checked == p_item->checkState())
  {
    mute_map.insert(f_cid, true);
  }
  else
  {
    mute_map.insert(f_cid, false);
  }
}

void Courtroom::on_music_list_clicked()
{
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_area_list_clicked()
{
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_music_list_double_clicked(QModelIndex p_model)
{
  if (is_client_muted)
    return;

  QString p_song = ui_music_list->item(p_model.row())->text();

  ao_app->send_server_packet(new AOPacket("MC#" + p_song + "#" + QString::number(m_cid) + "#%"), false);

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_area_list_double_clicked(QModelIndex p_model)
{
  QString p_area = ui_area_list->item(p_model.row())->text();

  ao_app->send_server_packet(new AOPacket("MC#" + p_area + "#" + QString::number(m_cid) + "#%"), false);

  ui_ic_chat_message->setFocus();
}

void Courtroom::reset_shout_buttons()
{
  for (AOButton *i_button : qAsConst(ui_shouts))
    i_button->setChecked(false);
  m_shout_state = 0;
}

void Courtroom::on_shout_button_clicked(const bool p_checked)
{
  AOButton *l_button = dynamic_cast<AOButton *>(sender());
  if (l_button == nullptr)
    return;

  bool l_ok = false;
  const int l_id = l_button->property("shout_id").toInt(&l_ok);
  if (!l_ok)
    return;

  // disable all other buttons
  for (AOButton *i_button : qAsConst(ui_shouts))
  {
    if (i_button == l_button)
      continue;
    i_button->setChecked(false);
  }
  m_shout_state = p_checked ? l_id : 0;

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_shout_button_toggled(const bool p_checked)
{
  AOButton *l_button = dynamic_cast<AOButton *>(sender());
  if (l_button == nullptr)
    return;

  const QString l_name = l_button->property("shout_name").toString();
  if (l_name.isEmpty())
    return;

  const QString l_image_name(QString("%1%2.png").arg(l_name, QString(p_checked ? "_selected" : nullptr)));
  l_button->set_image(l_image_name);
  if (ao_app->find_theme_asset_path(l_image_name).isEmpty())
    l_button->setText(l_name);
}

void Courtroom::on_cycle_clicked()
{
  AOButton *f_cycle_button = static_cast<AOButton *>(sender());
  int f_cycle_id = f_cycle_button->property("cycle_id").toInt();

  switch (f_cycle_id)
  {
  case 5:
    cycle_wtce(-1);
    break;
  case 4:
    cycle_wtce(1);
    break;
  case 3:
    cycle_effect(-1);
    break;
  case 2:
    cycle_effect(1);
    break;
  case 1:
    cycle_shout(-1);
    break;
  case 0:
    cycle_shout(1);
    break;
  default:
    break;
  }

  if (ao_app->read_theme_ini_bool("enable_cycle_ding", cc_config_ini))
    m_system_player->play(ao_app->get_sfx("cycle"));

  set_shouts();
  ui_ic_chat_message->setFocus();
}

void Courtroom::cycle_shout(int p_delta)
{
  int n = ui_shouts.size();
  m_shout_current = (m_shout_current - p_delta + n) % n;
  set_shouts();
}

void Courtroom::cycle_effect(int p_delta)
{
  int n = ui_effects.size();
  m_effect_current = (m_effect_current - p_delta + n) % n;
  set_effects();
}

void Courtroom::cycle_wtce(int p_delta)
{
  int n = ui_wtce.size();
  m_wtce_current = (m_wtce_current - p_delta + n) % n;
  set_judge_wtce();
}

void Courtroom::reset_effect_buttons()
{
  for (AOButton *i_button : qAsConst(ui_effects))
    i_button->setChecked(false);
  m_effect_state = 0;
}

void Courtroom::on_effect_button_clicked(const bool p_checked)
{
  AOButton *l_button = dynamic_cast<AOButton *>(sender());
  if (l_button == nullptr)
    return;

  bool l_ok = false;
  const int l_id = l_button->property("effect_id").toInt(&l_ok);
  if (!l_ok)
    return;

  // disable all other buttons
  for (AOButton *i_button : qAsConst(ui_effects))
  {
    if (i_button == l_button)
      continue;
    i_button->setChecked(false);
  }

  m_effect_state = p_checked ? l_id : 0;
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_effect_button_toggled(const bool p_checked)
{
  AOButton *l_button = dynamic_cast<AOButton *>(sender());
  if (l_button == nullptr)
    return;

  const QString l_name = l_button->property("effect_name").toString();
  if (l_name.isEmpty())
    return;

  const QString l_image_name(QString("%1%2.png").arg(l_name, QString(p_checked ? "_pressed" : nullptr)));
  l_button->set_image(l_image_name);
  if (ao_app->find_theme_asset_path(l_image_name).isEmpty())
    l_button->setText(l_name);
}

void Courtroom::on_mute_clicked()
{
  if (ui_mute_list->isHidden())
  {
    ui_mute_list->show();
    ui_mute->set_image("mute_pressed.png");
  }
  else
  {
    ui_mute_list->hide();
    ui_mute->set_image("mute.png");
  }
}

void Courtroom::on_defense_minus_clicked()
{
  int f_state = defense_bar_state - 1;

  if (f_state >= 0)
    ao_app->send_server_packet(new AOPacket("HP#1#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_defense_plus_clicked()
{
  int f_state = defense_bar_state + 1;

  if (f_state <= 10)
    ao_app->send_server_packet(new AOPacket("HP#1#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_prosecution_minus_clicked()
{
  int f_state = prosecution_bar_state - 1;

  if (f_state >= 0)
    ao_app->send_server_packet(new AOPacket("HP#2#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_prosecution_plus_clicked()
{
  int f_state = prosecution_bar_state + 1;

  if (f_state <= 10)
    ao_app->send_server_packet(new AOPacket("HP#2#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_text_color_changed(int p_color)
{
  m_text_color = p_color;
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_witness_testimony_clicked()
{
  if (is_client_muted)
    return;

  ao_app->send_server_packet(new AOPacket("RT#testimony1#%"));

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_cross_examination_clicked()
{
  if (is_client_muted)
    return;

  ao_app->send_server_packet(new AOPacket("RT#testimony2#%"));

  ui_ic_chat_message->setFocus();
}

void Courtroom::reset_wtce_buttons()
{
  for (int i = 0; i < wtce_names.size(); ++i)
  {
    QString wtce_file = wtce_names.at(i) + ".png";
    ui_wtce[i]->set_image(wtce_file);
    if (ao_app->find_theme_asset_path(wtce_file).isEmpty())
      ui_wtce[i]->setText(wtce_names.at(i));
    else
      ui_wtce[i]->setText("");
  }

  m_wtce_current = 0;

  // Unlike the other reset functions, the judge buttons are of immediate
  // action and thus are immediately unpressed after being pressed.
  // Therefore, we do not need to handle displaying a "_selected.png"
  // when appropriate, because there is no appropriate situation
}

void Courtroom::on_wtce_clicked()
{
  if (is_client_muted)
    return;

  AOButton *f_sig = static_cast<AOButton *>(sender());
  QString id = f_sig->property("wtce_id").toString();

  QString packet = QString("RT#testimony%1#%").arg(id);

  ao_app->send_server_packet(new AOPacket(packet));

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_change_character_clicked()
{
  suppress_audio(true);

  set_char_select();

  ui_char_select_background->show();
  ui_spectator->show();
}

void Courtroom::on_app_reload_theme_requested()
{
  // If an objection is playing, delay reload theme order to be executed
  // after objection is done
  if (ui_vp_objection->state() == QMovie::MovieState::Running)
  {
    shout_delayed_reload_theme = true;
    return;
  }

  // Otherwise carry on
  load_shouts();
  load_effects();
  load_wtce();
  load_free_blocks();

  // to update status on the background
  set_background(current_background);
  enter_courtroom(m_cid);
}

void Courtroom::on_back_to_lobby_clicked()
{
  // hide so we don't get the 'disconnected from server' prompt
  hide();

  ao_app->construct_lobby();
  ao_app->w_lobby->list_servers();
  ao_app->w_lobby->set_choose_a_server();
  ao_app->destruct_courtroom();
}

void Courtroom::on_char_select_left_clicked()
{
  --current_char_page;
  set_char_select_page();
}

void Courtroom::on_char_select_right_clicked()
{
  ++current_char_page;
  set_char_select_page();
}

void Courtroom::on_spectator_clicked()
{
  QString content = "CC#" + QString::number(ao_app->s_pv) + "#-1#" + get_hdid() + "#%";
  ao_app->send_server_packet(new AOPacket(content));
  enter_courtroom(-1);

  ui_emotes->hide();

  ui_char_select_background->hide();
}

void Courtroom::on_call_mod_clicked()
{
  QMessageBox::StandardButton reply;
  QString warning = "Are you sure you want to call a mod?\n"
                    "\n"
                    "Be prepared to explain what is happening and why they are needed when they answer.";
  reply = QMessageBox::warning(this, "Warning", warning, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

  if (reply == QMessageBox::Yes)
  {
    ao_app->send_server_packet(new AOPacket("ZZ#%"));
    qDebug() << "Called mod";
  }
  else
    qDebug() << "Did not call mod";

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_switch_area_music_clicked()
{
  if (ui_area_list->isHidden())
  {
    ui_area_list->show();
    ui_music_list->hide();
  }
  else
  {
    ui_area_list->hide();
    ui_music_list->show();
  }
}

void Courtroom::on_pre_clicked()
{
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_flip_clicked()
{
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_hidden_clicked()
{
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_evidence_button_clicked()
{
  if (ui_evidence->isHidden())
  {
    ui_evidence->show();
    ui_evidence_overlay->hide();
  }
  else
  {
    ui_evidence->hide();
  }
}

void Courtroom::on_config_panel_clicked()
{
  ao_app->toggle_config_panel();
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_note_button_clicked()
{
  if (!note_shown)
  {
    load_note();
    ui_vp_notepad_image->show();
    ui_vp_notepad->show();
    ui_vp_notepad->setFocus();
    note_shown = true;
  }
  else
  {
    save_note();
    ui_vp_notepad_image->hide();
    ui_vp_notepad->hide();
    ui_ic_chat_message->setFocus();
    note_shown = false;
  }
}

void Courtroom::on_note_text_changed()
{
  ao_app->write_note(ui_vp_notepad->toPlainText(), current_file);
}

void Courtroom::ping_server()
{
  ao_app->send_server_packet(new AOPacket("CH#" + QString::number(m_cid) + "#%"));
}

void Courtroom::closeEvent(QCloseEvent *event)
{
  Q_EMIT closing();
  QMainWindow::closeEvent(event);
}

void Courtroom::on_set_notes_clicked()
{
  if (note_scroll_area->isHidden())
    note_scroll_area->show();
  else
    note_scroll_area->hide();
}

void Courtroom::resume_timer(int timer_id)
{
  if (timer_id >= timer_number || timer_id < 0)
    return;

  ui_timers[timer_id]->resume();
}

void Courtroom::set_timer_time(int timer_id, int new_time)
{
  if (timer_id >= timer_number || timer_id < 0)
    return;

  ui_timers[timer_id]->set_time(QTime(0, 0).addMSecs(new_time));
}

void Courtroom::set_timer_timestep(int timer_id, int timestep_length)
{
  if (timer_id >= timer_number || timer_id < 0)
    return;

  ui_timers[timer_id]->set_timestep_length(timestep_length);
}

void Courtroom::set_timer_firing(int timer_id, int firing_interval)
{
  if (timer_id >= timer_number || timer_id < 0)
    return;

  ui_timers[timer_id]->set_firing_interval(firing_interval);
}

void Courtroom::pause_timer(int timer_id)
{
  if (timer_id >= timer_number || timer_id < 0)
    return;

  ui_timers[timer_id]->pause();
}
