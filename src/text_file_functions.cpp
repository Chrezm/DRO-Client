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
  QStringList r_sfx_list;

  QStringList l_file_list;
  l_file_list.append(get_base_path() + "configs/sounds.ini");
  for (const QString &i_chr : get_char_include_tree(get_current_char()))
    l_file_list.append(get_character_path(i_chr, "sounds.ini"));

  for (const QString &i_file_path : qAsConst(l_file_list))
  {
    QFile l_file(i_file_path);
    if (l_file.open(QIODevice::ReadOnly))
    {
      QTextStream l_in(&l_file);
      while (!l_in.atEnd())
        r_sfx_list.append(l_in.readLine());
    }
  }

  return r_sfx_list;
}

// returns whatever is to the right of "search_line =" within target_tag and
// terminator_tag, trimmed returns the empty string if the search line couldnt
// be found

QVariant AOApplication::read_char_ini(QString p_chr, QString p_group, QString p_key, QVariant p_def)
{
  QSettings s(get_character_path(p_chr, "char.ini"), QSettings::IniFormat);
  s.setIniCodec("UTF-8");
  s.beginGroup(p_group);
  return s.value(p_key, p_def);
}

QVariant AOApplication::read_char_ini(QString p_chr, QString p_group, QString p_key)
{
  return read_char_ini(p_chr, p_group, p_key, QVariant());
}

QString AOApplication::get_char_name(QString p_chr)
{
  return read_char_ini(p_chr, "options", "name", p_chr).toString();
}

#include <QQueue>

QStringList AOApplication::get_char_include(QString p_chr)
{
  QStringList r_list;

  QStringList l_queue{p_chr};
  while (!l_queue.isEmpty())
  {
    const QString l_target_chr = l_queue.takeFirst().trimmed();
    if (r_list.contains(l_target_chr) || l_target_chr.isEmpty())
      continue;
    r_list.append(l_target_chr);
    l_queue.append(read_char_ini(l_target_chr, "options", "include").toStringList());
  }
  r_list.removeAll(p_chr);

  return r_list;
}

QStringList AOApplication::get_char_include_tree(QString p_chr)
{
  QStringList r_list = get_char_include(p_chr);
  r_list.prepend(p_chr);
  return r_list;
}

QString AOApplication::get_showname(QString p_chr)
{
  return read_char_ini(p_chr, "options", "showname", p_chr).toString();
}

QString AOApplication::get_char_side(QString p_chr)
{
  return read_char_ini(p_chr, "options", "side", "wit").toString();
}

QString AOApplication::get_gender(QString p_chr)
{
  return read_char_ini(p_chr, "options", "gender", "male").toString();
}

QString AOApplication::get_chat(QString p_chr)
{
  return read_char_ini(p_chr, "options", "chat").toString().toLower();
}

QVector<DREmote> AOApplication::get_emote_list(QString p_chr)
{
  QVector<DREmote> r_emote_list;

  QStringList l_chr_list = get_char_include(p_chr);
  l_chr_list.append(p_chr);

#ifdef QT_DEBUG
  qDebug().noquote() << QString("Compiling char.ini for character <%1>").arg(p_chr);
#endif
  for (const QString &i_chr : l_chr_list)
  {
    if (!dir_exists(get_character_folder_path(i_chr)))
    {
      qWarning().noquote()
          << QString("Parent character <%1> not found, character <%2> cannot use it.").arg(i_chr, p_chr);
      continue;
    }
#ifdef QT_DEBUG
    qDebug().noquote() << QString("Adding <%1>").arg(i_chr);
#endif

    QSettings l_chrini(get_character_path(i_chr, "char.ini"), QSettings::IniFormat);
    l_chrini.setIniCodec("UTF-8");

    QStringList l_keys;
    { // recover all numbered keys, ignore words
      l_chrini.beginGroup("emotions");
      l_keys = l_chrini.childKeys();
      l_chrini.endGroup();

      // remove keywords
      l_keys.removeAll("firstmode");
      l_keys.removeAll("number");

      // remove all negative and non-numbers
      for (int i = 0; i < l_keys.length(); ++i)
      {
        const QString &i_key = l_keys.at(i);
        bool ok = false;
        const int l_num = i_key.toInt(&ok);
        if (ok && l_num >= 0)
          continue;
        l_keys.removeAt(i--);
      }

      std::stable_sort(l_keys.begin(), l_keys.end(), [](const QString &a, const QString &b) -> bool {
        // if 0s are added at the beginning of the key, consider a whole number
        if (a.length() < b.length())
          return true;
        return a.toInt() < b.toInt();
      });
    }

    for (const QString &i_key : qAsConst(l_keys))
    {
      l_chrini.beginGroup("emotions");
      const QStringList l_emotions = l_chrini.value(i_key).toString().split("#", DR::KeepEmptyParts);
      l_chrini.endGroup();

      if (l_emotions.length() < 4)
      {
        qWarning().noquote() << QString("Emote <%2> of <%1>; emote is malformed.").arg(i_chr, i_key);
        continue;
      }
      enum EmoteField
      {
        Comment,
        Animation,
        Dialog,
        Modifier,
        DeskModifier,
      };

      DREmote l_emote;
      l_emote.key = i_key;
      l_emote.character = i_chr;
      l_emote.comment = l_emotions.at(Comment);
      l_emote.anim = l_emotions.at(Animation);
      l_emote.dialog = l_emotions.at(Dialog);
      l_emote.modifier = qMax(l_emotions.at(Modifier).toInt(), 0);
      if (DeskModifier < l_emotions.length())
        l_emote.desk_modifier = l_emotions.at(DeskModifier).toInt();

      l_chrini.beginGroup("soundn");
      l_emote.sound_file = l_chrini.value(i_key).toString();
      l_chrini.endGroup();

      l_chrini.beginGroup("soundt");
      l_emote.sound_delay = qMax(l_chrini.value(i_key).toInt(), 0);
      l_chrini.endGroup();

      // add the emote
      r_emote_list.append(l_emote);
    }
  }

  // remove duplicate emotes and bring the last one to the front
  QVector<DREmote> l_filtered_list;
  QStringList l_dialog_filter_list;
  for (auto it = r_emote_list.cbegin(); it != r_emote_list.cend(); ++it)
  {
    const DREmote &it_emote = *it;
    if (l_dialog_filter_list.contains(it_emote.dialog))
      continue;
    l_dialog_filter_list.append(it_emote.dialog);

    for (auto rit = r_emote_list.crbegin(); rit != r_emote_list.crend(); ++rit)
    {
      const DREmote &rit_emote = *rit;
      if (it_emote.dialog == rit_emote.dialog)
      {
        l_filtered_list.append(rit_emote);
        break;
      }
    }
  }
  r_emote_list = std::move(l_filtered_list);
  return r_emote_list;
}

QStringList AOApplication::get_effect_offset(QString p_chr, int p_effect)
{
  QStringList r_offset = read_char_ini(p_chr, "offsets", QString::number(p_effect)).toString().split(",");
  while (r_offset.length() < 2)
    r_offset.append(nullptr);
  return r_offset;
}

QStringList AOApplication::get_overlay(QString p_chr, int p_overlay)
{
  QStringList r_overlay = read_char_ini(p_chr, "overlay", QString::number(p_overlay)).toString().split("#");
  while (r_overlay.length() < 2)
    r_overlay.append(nullptr);
  return r_overlay;
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

bool AOApplication::read_theme_ini_bool(QString p_identifier, QString p_file)
{
  return read_theme_ini(p_identifier, p_file) == "true";
}

int AOApplication::read_theme_ini_int(QString p_identifier, QString p_file)
{
  return read_theme_ini(p_identifier, p_file).toInt();
}
