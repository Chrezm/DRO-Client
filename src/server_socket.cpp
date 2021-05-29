#include "aoapplication.h"

#include "aoconfig.h"
#include "aopacket.h"
#include "courtroom.h"
#include "debug_functions.h"
#include "drdiscord.h"
#include "drserversocket.h"
#include "file_functions.h"
#include "hardware_functions.h"
#include "lobby.h"
#include "version.h"

#include <QDebug>

void AOApplication::connect_to_server(server_type p_server)
{
  m_server_socket->connect_to_server(p_server, false);
}

void AOApplication::send_server_packet(AOPacket p_packet)
{
  qDebug().noquote() << "S/S:" << p_packet.to_string();
  m_server_socket->send_packet(p_packet);
}

void AOApplication::_p_handle_server_packet(AOPacket p_packet)
{
  p_packet.net_decode();

  const QString l_header = p_packet.get_header();
  const QStringList l_content = p_packet.get_contents();

  if (l_header != "checkconnection")
    qDebug().noquote() << "S/R:" << p_packet.to_string();

  if (l_header == "decryptor")
  {
    // This packet is maintained as is for legacy purposes,
    // even though its sole argument is no longer used for anything
    // productive
    if (l_content.size() == 0)
      return;

#ifdef DRO_ACKMS // TODO WARNING remove entire block on 1.0.0 release
    feature_ackMS = false;
#endif
    feature_showname = false;
    feature_chrini = false;
    feature_chat_speed = false;

    send_server_packet(AOPacket("HI#" + get_hdid() + "#%"));
  }
  else if (l_header == "ID")
  {
    if (l_content.size() < 2)
      return;

    m_client_id = l_content.at(0).toInt();
    m_server_software = l_content.at(1);

    send_server_packet(AOPacket("ID#DRO#" + get_version_string() + "#%"));
  }
  else if (l_header == "CT")
  {
    if (l_content.size() < 2)
      return;

    if (is_courtroom_constructed)
      m_courtroom->append_server_chatmessage(l_content.at(0), l_content.at(1));
  }
  else if (l_header == "FL")
  {
#ifdef DRO_ACKMS // TODO WARNING remove entire block on 1.0.0 release
    feature_ackMS = l_content.contains("ackMS", Qt::CaseInsensitive);
#endif
    feature_showname = l_content.contains("showname", Qt::CaseInsensitive);
    feature_chrini = l_content.contains("chrini", Qt::CaseInsensitive);
    feature_chat_speed = l_content.contains("chat_speed", Qt::CaseInsensitive);
  }
  else if (l_header == "PN")
  {
    if (l_content.size() < 2)
      return;

    m_lobby->set_player_count(l_content.at(0).toInt(), l_content.at(1).toInt());
  }
  else if (l_header == "SI")
  {
    if (l_content.size() != 3)
      return;

    m_character_count = l_content.at(0).toInt();
    m_evidence_count = l_content.at(1).toInt();
    m_music_count = l_content.at(2).toInt();

    if (m_character_count < 1 || m_evidence_count < 0 || m_music_count < 0)
      return;

    m_loaded_characters = 0;
    m_loaded_evidence = 0;
    m_loaded_music = 0;

    construct_courtroom();

    is_courtroom_loaded = false;

    QString window_title = "Danganronpa Online";
    int selected_server = m_lobby->get_selected_server();

    QString server_name, server_address;
    bool is_favorite = false;
    if (m_lobby->is_public_server())
    {
      if (selected_server >= 0 && selected_server < m_server_list.size())
      {
        auto info = m_server_list.at(selected_server);
        server_name = info.name;
        server_address = info.ip + info.port;
        window_title += ": " + server_name;
      }
    }
    else
    {
      if (selected_server >= 0 && selected_server < m_favorite_server_list.size())
      {
        auto info = m_favorite_server_list.at(selected_server);
        server_name = info.name;
        server_address = info.ip + info.port;
        is_favorite = true;
        window_title += ": " + server_name;
      }
    }

    m_courtroom->set_window_title(window_title);

    m_lobby->show_loading_overlay();
    m_lobby->set_loading_text("Loading");
    m_lobby->set_loading_value(0);

    send_server_packet(AOPacket("RC#%"));

    // look for the server inside the known public list and report it
    if (is_favorite)
    {
      for (server_type &server : m_server_list)
      {
        const QString l_address = server.ip + server.port;
        if (server_address == l_address)
        {
          server_name = server.name;
          break;
        }
      }
    }
    dr_discord->set_state(DRDiscord::State::Connected);
    dr_discord->set_server_name(server_name);
  }
  else if (l_header == "CI")
  {
    if (!is_courtroom_constructed)
      return;

    for (int n_element = 0; n_element < l_content.size(); n_element += 2)
    {
      if (l_content.at(n_element).toInt() != m_loaded_characters)
        break;

      // this means we are on the last element and checking n + 1 element will
      // be game over so
      if (n_element == l_content.size() - 1)
        break;

      QStringList sub_elements = l_content.at(n_element + 1).split("&");
      if (sub_elements.size() < 2)
        break;

      char_type f_char;
      f_char.name = sub_elements.at(0);
      f_char.description = sub_elements.at(1);
      f_char.evidence_string = sub_elements.at(3);
      // temporary. the CharsCheck packet sets this properly
      f_char.taken = false;

      ++m_loaded_characters;

      m_lobby->set_loading_text("Loading chars:\n" + QString::number(m_loaded_characters) + "/" +
                                QString::number(m_character_count));

      m_courtroom->append_char(f_char);
    }

    int total_loading_size = m_character_count + m_evidence_count + m_music_count;
    int loading_value = (m_loaded_characters / static_cast<double>(total_loading_size)) * 100;
    m_lobby->set_loading_value(loading_value);

    send_server_packet(AOPacket("RE#%"));
  }
  else if (l_header == "EI")
  {
    if (!is_courtroom_constructed)
      return;

    // +1 because evidence starts at 1 rather than 0 for whatever reason
    // enjoy fanta
    if (l_content.at(0).toInt() != m_loaded_evidence + 1)
      return;

    if (l_content.size() < 2)
      return;

    QStringList sub_elements = l_content.at(1).split("&");
    if (sub_elements.size() < 4)
      return;

    evi_type f_evi;
    f_evi.name = sub_elements.at(0);
    f_evi.description = sub_elements.at(1);
    // no idea what the number at position 2 is. probably an identifier?
    f_evi.image = sub_elements.at(3);

    ++m_loaded_evidence;

    m_lobby->set_loading_text("Loading evidence:\n" + QString::number(m_loaded_evidence) + "/" +
                              QString::number(m_evidence_count));

    int total_loading_size = m_character_count + m_evidence_count + m_music_count;
    int loading_value = ((m_loaded_characters + m_loaded_evidence) / static_cast<double>(total_loading_size)) * 100;
    m_lobby->set_loading_value(loading_value);

    QString next_packet_number = QString::number(m_loaded_evidence);
    send_server_packet(AOPacket("AE#" + next_packet_number + "#%"));
  }
  else if (l_header == "CharsCheck")
  {
    if (!is_courtroom_constructed)
      return;

    for (int n_char = 0; n_char < l_content.size(); ++n_char)
    {
      if (l_content.at(n_char) == "-1")
        m_courtroom->set_taken(n_char, true);
      else
        m_courtroom->set_taken(n_char, false);
    }
  }

  else if (l_header == "SC")
  {
    if (!is_courtroom_constructed)
      return;

    for (int n_element = 0; n_element < l_content.size(); ++n_element)
    {
      QStringList sub_elements = l_content.at(n_element).split("&");

      char_type f_char;
      f_char.name = sub_elements.at(0);
      if (sub_elements.size() >= 2)
        f_char.description = sub_elements.at(1);

      // temporary. the CharsCheck packet sets this properly
      f_char.taken = false;

      ++m_loaded_characters;

      m_lobby->set_loading_text("Loading chars:\n" + QString::number(m_loaded_characters) + "/" +
                                QString::number(m_character_count));

      m_courtroom->append_char(f_char);
    }

    int total_loading_size = m_character_count + m_evidence_count + m_music_count;
    int loading_value = (m_loaded_characters / static_cast<double>(total_loading_size)) * 100;
    m_lobby->set_loading_value(loading_value);

    send_server_packet(AOPacket("RM#%"));
  }
  else if (l_header == "SM" || l_header == "FM")
  {
    if (!is_courtroom_constructed)
      return;

    QStringList l_area_list;
    QStringList l_music_list;

    for (int i = 0; i < l_content.length(); ++i)
    {
      bool l_found_music = false;

      { // look for first song
        const QString &i_value = l_content.at(i);
        for (const QString &i_ext : audio_extensions(true))
        {
          if (!i_value.endsWith(i_ext, Qt::CaseInsensitive))
            continue;
          l_found_music = true;
          break;
        }
      }

      if (!l_found_music)
        continue;
      l_area_list = l_content.mid(0, i - 1);
      l_music_list = l_content.mid(i - 1);
      break;
    }
    m_courtroom->set_area_list(l_area_list);
    m_courtroom->set_music_list(l_music_list);

    if (l_header == "SM")
    {
      m_loaded_music = m_music_count;
      m_lobby->set_loading_text("Loading music:\n" + QString::number(m_loaded_music) + "/" +
                                QString::number(m_music_count));
      int total_loading_size = m_character_count + m_evidence_count + m_music_count;
      int loading_value =
          ((m_loaded_characters + m_loaded_evidence + m_loaded_music) / static_cast<double>(total_loading_size)) * 100;
      m_lobby->set_loading_value(loading_value);
      send_server_packet(AOPacket("RD#%"));
    }
  }
  else if (l_header == "DONE")
  {
    if (!is_courtroom_constructed)
      return;

    m_courtroom->done_received();

    is_courtroom_loaded = true;

    destruct_lobby();
  }
  else if (l_header == "BN")
  {
    if (l_content.size() < 1)
      return;

    if (is_courtroom_constructed)
    {
      m_courtroom->set_background(l_content.at(0));
      m_courtroom->set_scene();
    }
  }
  else if (l_header == "chat_tick_rate")
  {
    if (is_courtroom_constructed)
      m_courtroom->set_tick_rate(l_content.isEmpty() ? std::nullopt : std::optional<int>(l_content.at(0).toInt()));
  }
  // server accepting char request(CC) packet
  else if (l_header == "PV")
  {
    if (l_content.size() < 3)
      return;

    if (is_courtroom_constructed)
      m_courtroom->enter_courtroom(l_content.at(2).toInt());
  }
  else if (l_header == "MS")
  {
    if (is_courtroom_constructed && is_courtroom_loaded)
      m_courtroom->handle_chatmessage(p_packet.get_contents());
  }
  else if (l_header == "ackMS")
  {
    if (is_courtroom_constructed && is_courtroom_loaded)
      m_courtroom->handle_acknowledged_ms();
  }
  else if (l_header == "MC")
  {
    if (is_courtroom_constructed && is_courtroom_loaded)
      m_courtroom->handle_song(p_packet.get_contents());
  }
  else if (l_header == "RT")
  {
    if (l_content.size() < 1)
      return;
    if (is_courtroom_constructed)
      m_courtroom->handle_wtce(l_content.at(0));
  }
  else if (l_header == "HP")
  {
    if (is_courtroom_constructed && l_content.size() > 1)
      m_courtroom->set_hp_bar(l_content.at(0).toInt(), l_content.at(1).toInt());
  }
  else if (l_header == "LE")
  {
    if (is_courtroom_constructed)
    {
      QVector<evi_type> f_evi_list;

      for (const QString &f_string : l_content)
      {
        QStringList sub_contents = f_string.split("&");

        if (sub_contents.size() < 3)
          continue;

        evi_type f_evi;
        f_evi.name = sub_contents.at(0);
        f_evi.description = sub_contents.at(1);
        f_evi.image = sub_contents.at(2);

        f_evi_list.append(f_evi);
      }

      m_courtroom->set_evidence_list(f_evi_list);
    }
  }
  else if (l_header == "IL")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
      m_courtroom->set_ip_list(l_content.at(0));
  }
  else if (l_header == "MU")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
      m_courtroom->set_mute(true, l_content.at(0).toInt());
  }
  else if (l_header == "UM")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
      m_courtroom->set_mute(false, l_content.at(0).toInt());
  }
  else if (l_header == "KK")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
    {
      int f_cid = m_courtroom->get_character_id();
      int remote_cid = l_content.at(0).toInt();

      if (f_cid != remote_cid && remote_cid != -1)
        return;

      call_notice("You have been kicked.");
      construct_lobby();
      destruct_courtroom();
    }
  }
  else if (l_header == "KB")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
      m_courtroom->set_ban(l_content.at(0).toInt());
  }
  else if (l_header == "BD")
  {
    call_notice("You are banned on this server.");
  }
  else if (l_header == "ZZ")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
      m_courtroom->mod_called(l_content.at(0));
  }
  else if (l_header == "CL")
  {
    qDebug() << l_content;
    m_courtroom->handle_clock(l_content.at(1));
  }
  else if (l_header == "GM")
  {
    if (l_content.length() < 1)
      return;
    if (ao_config->manual_gamemode_enabled())
      return;
    ao_config->set_gamemode(l_content.at(0));
  }
  else if (l_header == "TOD")
  {
    if (l_content.length() < 1)
      return;
    if (ao_config->manual_timeofday_enabled())
      return;
    ao_config->set_timeofday(l_content.at(0));
  }
  else if (l_header == "TR")
  {
    // Timer resume
    if (l_content.size() != 1)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    m_courtroom->resume_timer(timer_id);
  }
  else if (l_header == "TST")
  {
    // Timer set time
    if (l_content.size() != 2)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    int new_time = l_content.at(1).toInt();
    m_courtroom->set_timer_time(timer_id, new_time);
  }
  else if (l_header == "TSS")
  {
    // Timer set timeStep length
    if (l_content.size() != 2)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    int timestep_length = l_content.at(1).toInt();
    m_courtroom->set_timer_timestep(timer_id, timestep_length);
  }
  else if (l_header == "TSF")
  {
    // Timer set Firing interval
    if (l_content.size() != 2)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    int firing_interval = l_content.at(1).toInt();
    m_courtroom->set_timer_firing(timer_id, firing_interval);
  }
  else if (l_header == "TP")
  {
    // Timer pause
    if (l_content.size() != 1)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    m_courtroom->pause_timer(timer_id);
  }
  else if (l_header == "SP")
  {
    // Set position
    if (l_content.size() != 1)
      return;
    if (!is_courtroom_constructed)
      return;
    m_courtroom->set_character_position(l_content.at(0));
  }
  else if (l_header == "SN")
  {
    // Server-set showname.
    // This has priority over user-set showname.
    if (l_content.size() != 1)
      return;
    if (!is_courtroom_constructed)
      return;
    const QString l_showname = l_content.at(0);
    // By updating now, we prevent the client from sending an SN back when we later on go ahead and modify the
    // showname box.
    ao_config->set_showname(l_showname);
  }
}