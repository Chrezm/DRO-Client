#include "aoapplication.h"

#include "file_functions.h"

#include "aoconfig.h"

#include <QColor>
#include <QDebug>
#include <QSettings>
#include <QStringList>
#include <QTextStream>
#include <QVector>

QString AOApplication::get_theme()
{
  return config->theme();
}

QString AOApplication::get_gamemode()
{
  return config->gamemode();
}

QString AOApplication::get_timeofday()
{
  return config->timeofday();
}

int AOApplication::read_blip_rate()
{
  return config->blip_rate();
}

bool AOApplication::read_chatlog_newline()
{
  return config->log_format_use_newline_enabled();
}

int AOApplication::get_default_music()
{
  return config->music_volume();
}

int AOApplication::get_default_sfx()
{
  return config->effect_volume();
}

int AOApplication::get_default_blip()
{
  return config->blip_volume();
}

QStringList AOApplication::get_callwords()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
  return config->callwords().split(" ", QString::SkipEmptyParts);
#else
  return config->callwords().split(" ", Qt::SkipEmptyParts);
#endif
}

QString AOApplication::read_note(QString filename)
{
  QFile note_txt(filename);

  if (!note_txt.open(QIODevice::ReadOnly | QFile::Text))
  {
    qDebug() << "Couldn't open" << filename;
    return "";
  }

  QTextStream in(&note_txt);
  QString text = in.readAll();
  note_txt.close();
  return text;
}

void AOApplication::write_note(QString p_text, QString p_file)
{
  QFile f_log(p_file);
  if (f_log.open(QIODevice::WriteOnly | QFile::Text))
  {
    QTextStream out(&f_log);

    out << p_text;

    f_log.flush();
    f_log.close();
  }
}

void AOApplication::append_note(QString p_line, QString p_file)
{
  QFile f_log(p_file);
  if (f_log.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    QTextStream out(&f_log);

    out << p_line << "\r\n";

    f_log.flush();
    f_log.close();
  }
}

void AOApplication::write_to_serverlist_txt(QString p_line)
{
  QFile serverlist_txt;
  QString serverlist_txt_path = get_base_path() + "serverlist.txt";

  serverlist_txt.setFileName(serverlist_txt_path);

  if (!serverlist_txt.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    return;
  }

  QTextStream out(&serverlist_txt);

  out << "\r\n" << p_line;

  serverlist_txt.close();
}

QVector<server_type> AOApplication::read_serverlist_txt()
{
  QVector<server_type> f_server_list;

  QFile serverlist_txt;
  QString serverlist_txt_path = get_base_path() + "serverlist.txt";

  serverlist_txt.setFileName(serverlist_txt_path);

  if (!serverlist_txt.open(QIODevice::ReadOnly))
  {
    return f_server_list;
  }

  QTextStream in(&serverlist_txt);

  while (!in.atEnd())
  {
    QString line = in.readLine();
    server_type f_server;
    QStringList line_contents = line.split(":");

    if (line_contents.size() < 3)
      continue;

    f_server.ip = line_contents.at(0);
    f_server.port = line_contents.at(1).toInt();
    f_server.name = line_contents.at(2);
    f_server.desc = "";

    f_server_list.append(f_server);
  }

  return f_server_list;
}

QString AOApplication::read_ini(QString p_identifier, QString p_path)
{
  QFile ini;
  ini.setFileName(p_path);
  if (!ini.open(QIODevice::ReadOnly))
    return "";

  QTextStream in(&ini);
  QString result = "";
  while (!in.atEnd())
  {
    QString f_line = in.readLine().trimmed();

    if (!f_line.startsWith(p_identifier))
      continue;

    QStringList line_elements = f_line.split("=");

    if (line_elements.at(0).trimmed() != p_identifier)
      continue;
    if (line_elements.size() < 2)
      continue;
    result = line_elements.at(1).trimmed();
    break;
  }

  ini.close();
  return result;
}

QPoint AOApplication::get_button_spacing(QString p_identifier, QString p_file)
{
  QPoint return_value;
  return_value.setX(0);
  return_value.setY(0);

  QString f_result = read_theme_ini(p_identifier, p_file);
  if (f_result.isEmpty())
    return return_value;

  QStringList sub_line_elements = f_result.split(",");
  if (sub_line_elements.size() < 2)
    return return_value;

  return_value.setX(sub_line_elements.at(0).toInt());
  return_value.setY(sub_line_elements.at(1).toInt());

  return return_value;
}

pos_size_type AOApplication::get_element_dimensions(QString p_identifier, QString p_file)
{
  pos_size_type return_value;
  return_value.x = 0;
  return_value.y = 0;
  return_value.width = -1;
  return_value.height = -1;

  QString f_result = read_theme_ini(p_identifier, p_file);
  if (f_result.isEmpty())
    return return_value;

  QStringList sub_line_elements = f_result.split(",");

  if (sub_line_elements.size() < 4)
    return return_value;

  return_value.x = sub_line_elements.at(0).toInt();
  return_value.y = sub_line_elements.at(1).toInt();
  return_value.width = sub_line_elements.at(2).toInt();
  return_value.height = sub_line_elements.at(3).toInt();

  return return_value;
}

int AOApplication::get_font_property(QString p_identifier, QString p_file)
{
  QString f_result = read_theme_ini(p_identifier, p_file);

  if (f_result.isEmpty())
    return 0;
  return f_result.toInt();
}

QColor AOApplication::get_color(QString p_identifier, QString p_file)
{
  QColor return_color(255, 255, 255);

  QString f_result = read_theme_ini(p_identifier, p_file);
  if (f_result.isEmpty())
    return return_color;

  QStringList color_list = f_result.split(",");

  if (color_list.size() < 3)
    return return_color;

  return_color.setRed(color_list.at(0).toInt());
  return_color.setGreen(color_list.at(1).toInt());
  return_color.setBlue(color_list.at(2).toInt());

  return return_color;
}

QString AOApplication::get_font_name(QString p_identifier, QString p_file)
{
  QString f_result = read_theme_ini(p_identifier, p_file);

  if (f_result.isEmpty())
    qDebug() << "Failure retreiving font name";

  return f_result;
}

QString AOApplication::get_sfx(QString p_identifier)
{
  return read_theme_ini(p_identifier, "courtroom_sounds.ini");
}

QString AOApplication::get_stylesheet(QString target_tag, QString p_file)
{
  // File lookup order
  // 1. In the theme folder (gamemode-timeofday/main/default), look for
  // `p_file`.

  QString path = find_theme_asset_path(p_file);
  if (path.isEmpty())
    return "";

  QFile design_ini(path);
  if (!design_ini.open(QIODevice::ReadOnly))
    return "";

  QTextStream in(&design_ini);
  QString f_text;
  bool tag_found = false;

  while (!in.atEnd())
  {
    QString line = in.readLine();
    if (line.startsWith(target_tag, Qt::CaseInsensitive))
    {
      tag_found = true;
      continue;
    }

    if (tag_found)
    {
      if ((line.startsWith("[") && line.endsWith("]")))
        break;
      f_text.append(line);
    }
  }

  design_ini.close();
  return f_text; // This is the empty string if no appends took place
}

QMap<DR::Color, DR::ColorInfo> AOApplication::get_chatmessage_colors()
{
  QMap<DR::Color, DR::ColorInfo> color_map = DR::get_default_color_map();

  // File lookup order
  // 1. In the theme folder (gamemode-timeofday/main/default), look for
  // "courtroom_text_colors.ini".

  const QString file_name = "courtroom_text_colors.ini";
  QString path = find_theme_asset_path(file_name);
  if (path.isEmpty())
  {
    qInfo().noquote()
        << QString("[color] theme %1 is missing file: %2, using default colors instead").arg(get_theme(), file_name);
    return color_map;
  }
  qInfo().noquote() << QString("[color] loading colors for theme %1").arg(get_theme());

  QSettings color_settings(path, QSettings::IniFormat);

  QMap<QString, DR::ColorInfo> color_replacement_map;
  for (QString &i_group : color_settings.childGroups())
  {
    const QString lower_name = i_group.toLower();

    color_settings.beginGroup(i_group);
    if (!color_settings.contains("code"))
      continue;
    const QString code = color_settings.value("code").toString().toLower();

    if (!QColor::isValidColor(code))
    {
      qWarning().noquote() << QString("[color] for color %1: color code is invalid: %2").arg(lower_name, code);
      continue;
    }

    if (color_replacement_map.contains(lower_name))
      qWarning().noquote() << QString("[color] color %1 is already defined, replacing anyway").arg(lower_name);
    color_replacement_map[lower_name].code = code.toLower();
    color_settings.endGroup();
  }

  // replace the data in the map we will return
  for (DR::ColorInfo &color_info : color_map)
  {
    const QString lower_name = color_info.name.toLower();
    if (!color_replacement_map.contains(lower_name))
      continue;
    color_info.code = color_replacement_map[lower_name].code;
  }

  return color_map;
}

QVector<QStringList> AOApplication::get_highlight_colors()
{
  // File lookup order
  // 1. In the theme folder (gamemode-timeofday/main/default), look for
  // "courtroom_config.ini".

  QString path = find_theme_asset_path("courtroom_config.ini");
  if (path.isEmpty())
    return QVector<QStringList>();

  QFile design_ini(path);
  if (!design_ini.open(QIODevice::ReadOnly))
    return QVector<QStringList>();

  QTextStream in(&design_ini);
  bool tag_found = false;
  QVector<QStringList> f_vec;

  while (!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("[HIGHLIGHTS]", Qt::CaseInsensitive))
    {
      tag_found = true;
      continue;
    }

    if (tag_found)
    {
      if ((line.startsWith("[") && line.endsWith("]")))
        break;
      // Syntax
      // OpenercharCloserchar = Color, Shown
      // Shown is 1 if the character should be displayed in IC, 0 otherwise.
      // If not present, assume 1.
      QString chars = line.split("=")[0].trimmed();
      QString chars_parameters = line.mid(line.indexOf("=") + 1);
      QStringList parameters = chars_parameters.split(",");
      for (int i = 0; i < parameters.size(); i++)
        parameters[i] = parameters[i].trimmed();
      if (parameters.size() == 1)
        parameters.append("1");
      f_vec.append({chars, parameters[0], parameters[1]});
    }
  }

  design_ini.close();
  return f_vec; // Could be an empty vector if no appends were made
}

QString AOApplication::get_spbutton(QString p_tag, int index)
{
  // File lookup order
  // 1. In the theme folder (gamemode-timeofday/main/default), look for
  // "courtroom_config.ini".

  QString path = find_theme_asset_path("courtroom_config.ini");
  if (path.isEmpty())
    return "";

  QFile design_ini(path);
  if (!design_ini.open(QIODevice::ReadOnly))
    return "";

  QTextStream in(&design_ini);
  bool tag_found = false;
  QString res;

  while (!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith(p_tag, Qt::CaseInsensitive))
    {
      tag_found = true;
      continue;
    }

    if (tag_found)
    {
      if ((line.startsWith("[") && line.endsWith("]")))
        break;

      QStringList line_contents = line.split("=");
      if (line_contents.at(0).trimmed() == QString::number(index))
        res = line_contents.at(1);
    }
  }

  design_ini.close();
  return res; // Could be the empty string if no matches were found.
}

QStringList AOApplication::get_effect(int index)
{
  // File lookup order
  // 1. In the theme folder (gamemode-timeofday/main/default), look for
  // "courtroom_config.ini".

  QString path = find_theme_asset_path("courtroom_config.ini");
  if (path.isEmpty())
    return QStringList();

  QFile design_ini(path);
  if (!design_ini.open(QIODevice::ReadOnly))
    return QStringList();

  QTextStream in(&design_ini);
  bool tag_found = false;
  QStringList res;

  while (!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("[EFFECTS]", Qt::CaseInsensitive))
    {
      tag_found = true;
      continue;
    }

    if (tag_found)
    {
      if ((line.startsWith("[") && line.endsWith("]")))
        break;

      QStringList line_contents = line.split("=");
      if (line_contents.at(0).trimmed() == QString::number(index))
        res = line_contents.at(1).split(",");

      if (res.size() == 1)
        res.append("1");
    }
  }

  design_ini.close();
  return res;
}

QStringList AOApplication::get_sfx_list()
{
  QStringList return_value;
  QFile base_sfx_list_ini;
  QFile char_sfx_list_ini;

  base_sfx_list_ini.setFileName(get_base_path() + "configs/sounds.ini");
  char_sfx_list_ini.setFileName(get_character_path(get_current_char(), "sounds.ini"));

  if (!char_sfx_list_ini.open(QIODevice::ReadOnly) && !base_sfx_list_ini.open(QIODevice::ReadOnly))
  {
    return return_value;
  }

  QTextStream in_a(&base_sfx_list_ini);
  QTextStream in_b(&char_sfx_list_ini);

  while (!in_a.atEnd())
  {
    QString line = in_a.readLine();
    return_value.append(line);
  }

  while (!in_b.atEnd())
  {
    QString line = in_b.readLine();
    return_value.append(line);
  }

  return return_value;
}

// returns whatever is to the right of "search_line =" within target_tag and
// terminator_tag, trimmed returns the empty string if the search line couldnt
// be found
QString AOApplication::read_char_ini(QString p_char, QString p_search_line, QString target_tag, QString terminator_tag)
{
  QString char_ini_path = get_character_path(p_char, "char.ini");

  QFile char_ini;

  char_ini.setFileName(char_ini_path);

  if (!char_ini.open(QIODevice::ReadOnly))
    return "";

  QTextStream in(&char_ini);

  bool tag_found = false;

  while (!in.atEnd())
  {
    QString line = in.readLine();

    if (QString::compare(line, terminator_tag, Qt::CaseInsensitive) == 0)
      break;

    if (line.startsWith(target_tag, Qt::CaseInsensitive))
    {
      tag_found = true;
      continue;
    }

    if (!line.startsWith(p_search_line, Qt::CaseInsensitive))
      continue;

    QStringList line_elements = line.split("=");

    if (QString::compare(line_elements.at(0).trimmed(), p_search_line, Qt::CaseInsensitive) != 0)
      continue;

    if (line_elements.size() < 2)
      continue;

    if (tag_found)
    {
      char_ini.close();
      return line_elements.at(1).trimmed();
    }
  }

  char_ini.close();
  return "";
}

QString AOApplication::get_char_name(QString p_char)
{
  QString f_result = read_char_ini(p_char, "name", "[Options]", "[Time]");

  if (f_result == "")
    return p_char;
  else
    return f_result;
}

QString AOApplication::get_showname(QString p_char)
{
  QString f_result = read_showname(p_char);
  if (f_result == "")
    f_result = read_char_ini(p_char, "showname", "[Options]", "[Time]");

  if (f_result == "")
    return p_char;
  else
    return f_result;
}

QString AOApplication::read_showname(QString p_char)
{
  QString f_filename = get_base_path() + "configs/shownames.ini";
  QFile f_file(f_filename);
  if (!f_file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Error reading" << f_filename;
    return "";
  }

  QTextStream in(&f_file);
  while (!in.atEnd())
  {
    QString f_line = in.readLine();
    if (!f_line.startsWith(p_char))
      continue;

    QStringList line_elements = f_line.split("=");
    if (line_elements.at(0).trimmed() == p_char)
      return line_elements.at(1).trimmed();
  }
  return "";
}

QString AOApplication::get_char_side(QString p_char)
{
  QString f_result = read_char_ini(p_char, "side", "[Options]", "[Time]");

  if (f_result == "")
    return "wit";
  else
    return f_result;
}

QString AOApplication::get_gender(QString p_char)
{
  QString f_result = read_char_ini(p_char, "gender", "[Options]", "[Time]");

  if (f_result == "")
    return "male";
  else
    return f_result;
}

QString AOApplication::get_chat(QString p_char)
{
  QString f_result = read_char_ini(p_char, "chat", "[Options]", "[Time]");

  // handling the correct order of chat is a bit complicated, we let the caller
  // do it
  return f_result.toLower();
}

int AOApplication::get_emote_number(QString p_char)
{
  QString f_result = read_char_ini(p_char, "number", "[Emotions]", "[Offsets]");

  if (f_result == "")
    return 0;
  else
    return f_result.toInt();
}

QString AOApplication::get_emote_comment(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[Offsets]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  else
    return result_contents.at(0);
}

QString AOApplication::get_pre_emote(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[Offsets]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "";
  }
  else
    return result_contents.at(1);
}

QString AOApplication::get_emote(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[Offsets]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  else
    return result_contents.at(2);
}

int AOApplication::get_emote_mod(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[Offsets]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << QString::number(p_emote);
    return 0;
  }
  else
    return result_contents.at(3).toInt();
}

int AOApplication::get_desk_mod(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[Emotions]", "[Offsets]");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 5)
    return -1;

  QString string_result = result_contents.at(4);
  if (string_result == "")
    return -1;

  else
    return string_result.toInt();
}

QStringList AOApplication::get_effect_offset(QString p_char, int p_effect)
{
  QStringList f_result = read_char_ini(p_char, QString::number(p_effect), "[Offsets]", "[Overlay]").split(",");

  if (f_result.size() < 2)
    return decltype(f_result){0, 0};

  return f_result;
}

QStringList AOApplication::get_overlay(QString p_char, int p_effect)
{
  QStringList f_result = read_char_ini(p_char, QString::number(p_effect), "[Overlay]", "[SoundN]").split("#");

  if (f_result.size() < 2)
    f_result.push_back("");

  return f_result;
}

QString AOApplication::get_sfx_name(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[SoundN]", "[SoundT]");

  if (f_result == "")
    return "1";
  else
    return f_result;
}

int AOApplication::get_sfx_delay(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "[SoundT]", "[TextDelay]");

  if (f_result == "")
    return 1;
  else
    return f_result.toInt();
}

int AOApplication::get_text_delay(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "[TextDelay]", "END_OF_FILE");

  if (f_result == "")
    return -1;
  else
    return f_result.toInt();
}

bool AOApplication::get_blank_blip()
{
  return config->blank_blips_enabled();
}

QString AOApplication::read_theme_ini(QString p_identifier, QString p_file)
{
  // File lookup order
  // 1. In the theme folder (gamemode-timeofday/main/default), look for
  // `p_identifier`.
  QString path = find_theme_asset_path(p_file);
  if (path.isEmpty())
    return "";

  return read_ini(p_identifier, path); // Could be the empty string
}
