#include "aoapplication.h"

#include "courtroom.h"
#include "debug_functions.h"
#include "file_functions.h"
#include "hardware_functions.h"
#include "lobby.h"
#include "networkmanager.h"
#include "version.h"

#include <QCryptographicHash>
#include <QDebug>

void AOApplication::ms_packet_received(AOPacket *p_packet)
{
  p_packet->net_decode();

  QString header = p_packet->get_header();
  QStringList f_contents = p_packet->get_contents();

  if (header != "CHECK")
    qDebug() << "R(ms):" << p_packet->to_string();

  if (header == "ALL")
  {
    server_list.clear();

    for (const QString &i_string : qAsConst(p_packet->get_contents()))
    {
      server_type f_server;
      QStringList sub_contents = i_string.split("&");

      if (sub_contents.size() < 4)
      {
        qDebug() << "W: malformed packet";
        continue;
      }

      f_server.name = sub_contents.at(0);
      f_server.desc = sub_contents.at(1);
      f_server.ip = sub_contents.at(2);
      f_server.port = sub_contents.at(3).toInt();

      server_list.append(f_server);
    }

    if (lobby_constructed)
    {
      m_lobby->list_servers();
    }
  }
  else if (header == "CT")
  {
    QString f_name, f_message;

    if (f_contents.size() == 1)
    {
      f_name = "";
      f_message = f_contents.at(0);
    }
    else if (f_contents.size() >= 2)
    {
      f_name = f_contents.at(0);
      f_message = f_contents.at(1);
    }
    else
      goto end;

    if (lobby_constructed)
    {
      m_lobby->append_chatmessage(f_name, f_message);
    }
  }
  else if (header == "AO2CHECK")
  {
    send_ms_packet(new AOPacket("ID#DRO#" + get_version_string() + "#%"));
    send_ms_packet(new AOPacket("HI#" + get_hdid() + "#%"));

    if (f_contents.size() < 1)
      goto end;

    QStringList version_contents = f_contents.at(0).split(".");

    if (version_contents.size() < 3)
      goto end;

    int f_release = version_contents.at(0).toInt();
    int f_major = version_contents.at(1).toInt();
    int f_minor = version_contents.at(2).toInt();

    if (get_release_version() > f_release)
      goto end;
    else if (get_release_version() == f_release)
    {
      if (get_major_version() > f_major)
        goto end;
      else if (get_major_version() == f_major)
      {
        if (get_minor_version() >= f_minor)
          goto end;
      }
    }

    call_notice("Outdated version! Your version: " + get_version_string() +
                "\nPlease go to aceattorneyonline.com to update.");
    destruct_courtroom();
    destruct_lobby();
  }

end:
  delete p_packet;
}

void AOApplication::server_packet_received(AOPacket *p_packet)
{
  p_packet->net_decode();

  QString header = p_packet->get_header();
  QStringList f_contents = p_packet->get_contents();
  QString f_packet = p_packet->to_string();

  if (header != "checkconnection")
    qDebug() << "R:" << f_packet;

  if (header == "decryptor")
  {
    // This packet is maintained as is for legacy purposes,
    // even though its sole argument is no longer used for anything
    // productive
    if (f_contents.size() == 0)
      goto end;

    QString f_hdid;
    f_hdid = get_hdid();

#ifdef DRO_ACKMS // TODO WARNING remove entire block on 1.0.0 release
    m_FL_ackMS_enabled = false;
#endif
    m_FL_showname_enabled = false;
    m_FL_chrini_enabled = false;

    AOPacket *hi_packet = new AOPacket("HI#" + f_hdid + "#%");
    send_server_packet(hi_packet);
  }
  else if (header == "ID")
  {
    if (f_contents.size() < 2)
      goto end;

    s_pv = f_contents.at(0).toInt();
    server_software = f_contents.at(1);

    send_server_packet(new AOPacket("ID#DRO#" + get_version_string() + "#%"));
  }
  else if (header == "CT")
  {
    if (f_contents.size() < 2)
      goto end;

    if (courtroom_constructed)
      m_courtroom->append_server_chatmessage(f_contents.at(0), f_contents.at(1));
  }
  else if (header == "FL")
  {
#ifdef DRO_ACKMS // TODO WARNING remove entire block on 1.0.0 release
    m_FL_ackMS_enabled = f_packet.contains("ackMS", Qt::CaseInsensitive);
    m_FL_showname_enabled = f_packet.contains("showname", Qt::CaseInsensitive);
    m_FL_chrini_enabled = f_packet.contains("chrini", Qt::CaseInsensitive);
#endif
  }
  else if (header == "PN")
  {
    if (f_contents.size() < 2)
      goto end;

    m_lobby->set_player_count(f_contents.at(0).toInt(), f_contents.at(1).toInt());
  }
  else if (header == "SI")
  {
    if (f_contents.size() != 3)
      goto end;

    char_list_size = f_contents.at(0).toInt();
    evidence_list_size = f_contents.at(1).toInt();
    music_list_size = f_contents.at(2).toInt();

    if (char_list_size < 1 || evidence_list_size < 0 || music_list_size < 0)
      goto end;

    loaded_chars = 0;
    loaded_evidence = 0;
    loaded_music = 0;

    construct_courtroom();

    courtroom_loaded = false;

    QString window_title = "Danganronpa Online";
    int selected_server = m_lobby->get_selected_server();

    QString server_name, server_address;
    bool is_favorite = false;
    if (m_lobby->public_servers_selected)
    {
      if (selected_server >= 0 && selected_server < server_list.size())
      {
        auto info = server_list.at(selected_server);
        server_name = info.name;
        server_address = info.ip + info.port;
        window_title += ": " + server_name;
      }
    }
    else
    {
      if (selected_server >= 0 && selected_server < favorite_list.size())
      {
        auto info = favorite_list.at(selected_server);
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

    AOPacket *f_packet = nullptr;

    f_packet = new AOPacket("RC#%");
    send_server_packet(f_packet);

    // look for the server inside the known public list and report it
    if (is_favorite)
    {
      for (server_type &server : server_list)
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
  else if (header == "CI")
  {
    if (!courtroom_constructed)
      goto end;

    for (int n_element = 0; n_element < f_contents.size(); n_element += 2)
    {
      if (f_contents.at(n_element).toInt() != loaded_chars)
        break;

      // this means we are on the last element and checking n + 1 element will
      // be game over so
      if (n_element == f_contents.size() - 1)
        break;

      QStringList sub_elements = f_contents.at(n_element + 1).split("&");
      if (sub_elements.size() < 2)
        break;

      char_type f_char;
      f_char.name = sub_elements.at(0);
      f_char.description = sub_elements.at(1);
      f_char.evidence_string = sub_elements.at(3);
      // temporary. the CharsCheck packet sets this properly
      f_char.taken = false;

      ++loaded_chars;

      m_lobby->set_loading_text("Loading chars:\n" + QString::number(loaded_chars) + "/" +
                                QString::number(char_list_size));

      m_courtroom->append_char(f_char);
    }

    int total_loading_size = char_list_size + evidence_list_size + music_list_size;
    int loading_value = (loaded_chars / static_cast<double>(total_loading_size)) * 100;
    m_lobby->set_loading_value(loading_value);

    send_server_packet(new AOPacket("RE#%"));
  }
  else if (header == "EI")
  {
    if (!courtroom_constructed)
      goto end;

    // +1 because evidence starts at 1 rather than 0 for whatever reason
    // enjoy fanta
    if (f_contents.at(0).toInt() != loaded_evidence + 1)
      goto end;

    if (f_contents.size() < 2)
      goto end;

    QStringList sub_elements = f_contents.at(1).split("&");
    if (sub_elements.size() < 4)
      goto end;

    evi_type f_evi;
    f_evi.name = sub_elements.at(0);
    f_evi.description = sub_elements.at(1);
    // no idea what the number at position 2 is. probably an identifier?
    f_evi.image = sub_elements.at(3);

    ++loaded_evidence;

    m_lobby->set_loading_text("Loading evidence:\n" + QString::number(loaded_evidence) + "/" +
                              QString::number(evidence_list_size));

    m_courtroom->append_evidence(f_evi);

    int total_loading_size = char_list_size + evidence_list_size + music_list_size;
    int loading_value = ((loaded_chars + loaded_evidence) / static_cast<double>(total_loading_size)) * 100;
    m_lobby->set_loading_value(loading_value);

    QString next_packet_number = QString::number(loaded_evidence);
    send_server_packet(new AOPacket("AE#" + next_packet_number + "#%"));
  }
  else if (header == "CharsCheck")
  {
    if (!courtroom_constructed)
      goto end;

    for (int n_char = 0; n_char < f_contents.size(); ++n_char)
    {
      if (f_contents.at(n_char) == "-1")
        m_courtroom->set_taken(n_char, true);
      else
        m_courtroom->set_taken(n_char, false);
    }
  }

  else if (header == "SC")
  {
    if (!courtroom_constructed)
      goto end;

    for (int n_element = 0; n_element < f_contents.size(); ++n_element)
    {
      QStringList sub_elements = f_contents.at(n_element).split("&");

      char_type f_char;
      f_char.name = sub_elements.at(0);
      if (sub_elements.size() >= 2)
        f_char.description = sub_elements.at(1);

      // temporary. the CharsCheck packet sets this properly
      f_char.taken = false;

      ++loaded_chars;

      m_lobby->set_loading_text("Loading chars:\n" + QString::number(loaded_chars) + "/" +
                                QString::number(char_list_size));

      m_courtroom->append_char(f_char);
    }

    int total_loading_size = char_list_size + evidence_list_size + music_list_size;
    int loading_value = (loaded_chars / static_cast<double>(total_loading_size)) * 100;
    m_lobby->set_loading_value(loading_value);

    send_server_packet(new AOPacket("RM#%"));
  }
  else if (header == "SM" || header == "FM")
  {
    if (!courtroom_constructed)
      goto end;

    QStringList l_area_list;
    QStringList l_music_list;

    for (int i = 0; i < f_contents.length(); ++i)
    {
      bool l_found_music = false;

      { // look for first song
        const QString &i_value = f_contents.at(i);
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
      l_area_list = f_contents.mid(0, i - 1);
      l_music_list = f_contents.mid(i - 1);
      break;
    }
    m_courtroom->set_area_list(l_area_list);
    m_courtroom->set_music_list(l_music_list);

    if (header == "SM")
    {
      loaded_music = music_list_size;
      m_lobby->set_loading_text("Loading music:\n" + QString::number(loaded_music) + "/" +
                                QString::number(music_list_size));
      int total_loading_size = char_list_size + evidence_list_size + music_list_size;
      int loading_value =
          ((loaded_chars + loaded_evidence + loaded_music) / static_cast<double>(total_loading_size)) * 100;
      m_lobby->set_loading_value(loading_value);
      send_server_packet(new AOPacket("RD#%"));
    }
  }
  else if (header == "DONE")
  {
    if (!courtroom_constructed)
      goto end;

    m_courtroom->done_received();

    courtroom_loaded = true;

    destruct_lobby();
  }
  else if (header == "BN")
  {
    if (f_contents.size() < 1)
      goto end;

    if (courtroom_constructed)
    {
      m_courtroom->set_background(f_contents.at(0));
      m_courtroom->set_scene();
    }
  }
  // server accepting char request(CC) packet
  else if (header == "PV")
  {
    if (f_contents.size() < 3)
      goto end;

    if (courtroom_constructed)
      m_courtroom->enter_courtroom(f_contents.at(2).toInt());
  }
  else if (header == "MS")
  {
    if (courtroom_constructed && courtroom_loaded)
      m_courtroom->handle_chatmessage(p_packet->get_contents());
  }
  else if (header == "ackMS")
  {
    if (courtroom_constructed && courtroom_loaded)
      m_courtroom->handle_acknowledged_ms();
  }
  else if (header == "MC")
  {
    if (courtroom_constructed && courtroom_loaded)
      m_courtroom->handle_song(p_packet->get_contents());
  }
  else if (header == "RT")
  {
    if (f_contents.size() < 1)
      goto end;
    if (courtroom_constructed)
      m_courtroom->handle_wtce(f_contents.at(0));
  }
  else if (header == "HP")
  {
    if (courtroom_constructed && f_contents.size() > 1)
      m_courtroom->set_hp_bar(f_contents.at(0).toInt(), f_contents.at(1).toInt());
  }
  else if (header == "LE")
  {
    if (courtroom_constructed)
    {
      QVector<evi_type> f_evi_list;

      for (const QString &f_string : f_contents)
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
  else if (header == "MU")
  {
    if (courtroom_constructed && f_contents.size() > 0)
      m_courtroom->set_mute(true, f_contents.at(0).toInt());
  }
  else if (header == "UM")
  {
    if (courtroom_constructed && f_contents.size() > 0)
      m_courtroom->set_mute(false, f_contents.at(0).toInt());
  }
  else if (header == "KK")
  {
    if (courtroom_constructed && f_contents.size() > 0)
    {
      int f_cid = m_courtroom->get_cid();
      int remote_cid = f_contents.at(0).toInt();

      if (f_cid != remote_cid && remote_cid != -1)
        goto end;

      call_notice("You have been kicked.");
      construct_lobby();
      destruct_courtroom();
    }
  }
  else if (header == "KB")
  {
    if (courtroom_constructed && f_contents.size() > 0)
      m_courtroom->set_ban(f_contents.at(0).toInt());
  }
  else if (header == "BD")
  {
    call_notice("You are banned on this server.");
  }
  else if (header == "ZZ")
  {
    if (courtroom_constructed && f_contents.size() > 0)
      m_courtroom->mod_called(f_contents.at(0));
  }
  else if (header == "CL")
  {
    m_courtroom->handle_clock(f_contents.at(1));
  }
  else if (header == "GM")
  {
    if (f_contents.length() < 1)
      goto end;
    if (ao_config->manual_gamemode_enabled())
      goto end;
    ao_config->set_gamemode(f_contents.at(0));
  }
  else if (header == "TOD")
  {
    if (f_contents.length() < 1)
      goto end;
    if (ao_config->manual_timeofday_enabled())
      goto end;
    ao_config->set_timeofday(f_contents.at(0));
  }
  else if (header == "TR")
  {
    // Timer resume
    if (f_contents.size() != 1)
      goto end;
    if (!courtroom_constructed)
      goto end;
    int timer_id = f_contents.at(0).toInt();
    m_courtroom->resume_timer(timer_id);
  }
  else if (header == "TST")
  {
    // Timer set time
    if (f_contents.size() != 2)
      goto end;
    if (!courtroom_constructed)
      goto end;
    int timer_id = f_contents.at(0).toInt();
    int new_time = f_contents.at(1).toInt();
    m_courtroom->set_timer_time(timer_id, new_time);
  }
  else if (header == "TSS")
  {
    // Timer set timeStep length
    if (f_contents.size() != 2)
      goto end;
    if (!courtroom_constructed)
      goto end;
    int timer_id = f_contents.at(0).toInt();
    int timestep_length = f_contents.at(1).toInt();
    m_courtroom->set_timer_timestep(timer_id, timestep_length);
  }
  else if (header == "TSF")
  {
    // Timer set Firing interval
    if (f_contents.size() != 2)
      goto end;
    if (!courtroom_constructed)
      goto end;
    int timer_id = f_contents.at(0).toInt();
    int firing_interval = f_contents.at(1).toInt();
    m_courtroom->set_timer_firing(timer_id, firing_interval);
  }
  else if (header == "TP")
  {
    // Timer pause
    if (f_contents.size() != 1)
      goto end;
    if (!courtroom_constructed)
      goto end;
    int timer_id = f_contents.at(0).toInt();
    m_courtroom->pause_timer(timer_id);
  }
  else if (header == "SP")
  {
    // Set position
    if (f_contents.size() != 1)
      goto end;
    if (!courtroom_constructed)
      goto end;
    m_courtroom->set_character_position(f_contents.at(0));
  }
  else if (header == "SN")
  {
    // Server-set showname.
    // This has priority over user-set showname.
    if (f_contents.size() != 1)
      goto end;
    if (!courtroom_constructed)
      goto end;
    const QString l_showname = f_contents.at(0);
    // By updating now, we prevent the client from sending an SN back when we later on go ahead and modify the
    // showname box.
    ao_config->set_showname(l_showname);
  }

end:

  delete p_packet;
}

void AOApplication::send_ms_packet(AOPacket *p_packet)
{
  p_packet->net_encode();

  QString f_packet = p_packet->to_string();

  net_manager->ship_ms_packet(f_packet);

  qDebug() << "S(ms):" << f_packet;

  delete p_packet;
}

void AOApplication::send_server_packet(AOPacket *p_packet, bool encoded)
{
  if (encoded)
    p_packet->net_encode();

  QString f_packet = p_packet->to_string();

  qDebug() << "S:" << f_packet;
  net_manager->ship_server_packet(f_packet);

  delete p_packet;
}
