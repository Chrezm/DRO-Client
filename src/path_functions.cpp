#include "aoapplication.h"

#include "courtroom.h"
#include "drpather.h"
#include "file_functions.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>

// Copied over from Vanilla.
// As said in the comments there, this is a *super broad* definition.
// Linux is guaranteed to be case-sensitive, however, in case of Mac, it...
// depends. On Mac, it can be toggled. So, should the time ever come to that,
// manually define CASE_SENSITIVE_FILESYSTEM if you're working on a Mac that
// has, well, a case-sensitive filesystem.
#ifdef Q_OS_LINUX
#define CASE_SENSITIVE_FILESYSTEM
#endif

QString AOApplication::get_base_path()
{
  return DRPather::get_application_path() + "/base/";
}

QString AOApplication::get_data_path()
{
  return get_base_path() + "data/";
}

QString AOApplication::get_character_folder_path(QString p_chr)
{
  QString r_path = get_base_path() + "characters/" + p_chr;
  return get_case_sensitive_path(r_path);
}

QString AOApplication::get_character_path(QString p_chr, QString p_file)
{
  const QString r_path = get_character_folder_path(p_chr) + "/" + p_file;
  return get_case_sensitive_path(r_path);
}

QString AOApplication::get_sounds_path(QString p_file)
{
  QString path = get_base_path() + "sounds/general/" + p_file;
  return get_case_sensitive_path(path);
}

QString AOApplication::get_music_path(QString p_song)
{
  QString path = get_base_path() + "sounds/music/" + p_song;
  return get_case_sensitive_path(path);
}

QString AOApplication::get_background_path(QString p_file)
{
  if (courtroom_constructed)
  {
    return get_case_sensitive_path(w_courtroom->get_background_path(p_file));
  }
  // this function being called when the courtroom isn't constructed makes no
  // sense
  return "";
}

QString AOApplication::get_default_background_path(QString p_file)
{
  QString path = get_base_path() + "background/gs4/" + p_file;
  return get_case_sensitive_path(path);
}

QString AOApplication::get_evidence_path(QString p_file)
{
  QString default_path = get_case_sensitive_path(get_base_path() + "evidence/" + p_file);
  QString alt_path = get_case_sensitive_path(get_base_path() + "items/" + p_file);

  if (QFile(default_path).exists())
    return default_path;
  else
    return alt_path;
}

QString Courtroom::get_background_path(QString p_file)
{
  return ao_app->get_base_path() + "background/" + current_background + "/" + p_file;
}

#ifndef CASE_SENSITIVE_FILESYSTEM
QString AOApplication::get_case_sensitive_path(QString p_file)
{
  return p_file.replace("//", "/");
}
#else
QString AOApplication::get_case_sensitive_path(QString p_file)
{
  // First, check to see if the file already exists as it is.
  if (QFile(p_file).exists())
    return p_file;

  QFileInfo file(p_file);

  QString file_basename = file.fileName();
  QString file_parent_dir = get_case_sensitive_path(file.absolutePath());

  // Second, does it exist in the new parent directory?
  if (QFile(file_parent_dir + "/" + file_basename).exists())
    return file_parent_dir + "/" + file_basename;

  // In case it doesn't, look through the entries in the parent directory, and
  // try and find it based on a case-insensitive regex search.
  // Note also the fixed string search here. This is so that, for example, music
  // files with parentheses don't get interpreted as grouping for a regex
  // search.
  QRegExp file_rx = QRegExp(file_basename, Qt::CaseInsensitive, QRegExp::FixedString);
  QStringList files = QDir(file_parent_dir).entryList();

  int result = files.indexOf(file_rx);

  if (result != -1)
    return file_parent_dir + "/" + files.at(result);

  // If nothing is found, we let the caller handle that case.
  return file_parent_dir + "/" + file_basename;
}
#endif

QString AOApplication::find_asset_path(QStringList p_root_list, QStringList p_ext_list)
{
  for (QString &i_root : p_root_list)
  {
    // We can assume that possible_exts will only be populated with hardcoded strings.
    // Therefore, the only place where sanitize_path could catch something bad is in the root.
    // So, check that now, so we don't need to check later.
    if (sanitize_path(i_root).isEmpty())
      continue;

    // Check if parent folder actually exists. If it does not, none of the following files would exist
    if (!dir_exists(QFileInfo(i_root).absolutePath()))
      continue;

    for (QString &i_ext : p_ext_list)
    {
      QString full_path = get_case_sensitive_path(i_root + i_ext);
      if (file_exists(full_path))
        return full_path;
    }
  }

  return nullptr;
}

QString AOApplication::find_theme_asset_path(QString p_file, QStringList p_ext_list)
{
  QStringList l_path_list;

  // Only add gamemode and/or time of day if non empty.
  const QString l_gamemode = config->gamemode();
  const QString l_timeofday = config->timeofday();
  const QString l_theme_root = get_base_path() + "themes/" + config->theme();

  if (!l_gamemode.isEmpty())
  {
    if (!l_timeofday.isEmpty())
      l_path_list.append(l_theme_root + "/gamemodes/" + l_gamemode + "/times/" + l_timeofday + "/" + p_file);
    l_path_list.append(l_theme_root + "/gamemodes/" + l_gamemode + "/" + p_file);
  }

  if (!l_timeofday.isEmpty())
    l_path_list.append(l_theme_root + "/times/" + l_timeofday + "/" + p_file);

  l_path_list.append(l_theme_root + "/" + p_file);

  // Check if default folder exists. We do this here as it is cheaper than doing it in find_asset_path
  // (as we know there should not be capitalization or folder jumping shenanigans here.
  const QString l_default_theme_path = get_base_path() + "themes/default/";
  if (dir_exists(l_default_theme_path))
    l_path_list.append(l_default_theme_path + p_file);

  return find_asset_path(l_path_list, p_ext_list);
}
