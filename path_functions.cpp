#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

#ifdef BASE_OVERRIDE
#include "base_override.h"
#endif
QString base_path = "";

QString AOApplication::get_base_path()
{
    if (base_path == "")
    {
#ifdef BASE_OVERRIDE
  base_path = base_override;
#elif defined(ANDROID)
        QString sdcard_storage = getenv("SECONDARY_STORAGE");
        if (dir_exists(sdcard_storage + "/AO2/")){
            base_path = sdcard_storage + "/AO2/";
        }else{
            QString external_storage = getenv("EXTERNAL_STORAGE");
            base_path = external_storage + "/AO2/";
        }
#else
  base_path = QDir::currentPath() + "/base/";
#endif
}
    return base_path;
}

QString AOApplication::get_data_path()
{
  return get_base_path() + "data/";
}

QString AOApplication::get_theme_path()
{
  return get_base_path() + "themes/" + current_theme.toLower() + "/";
}

QString AOApplication::get_theme_variant_path()
{
  return get_base_path() + "themes/" + current_theme.toLower() + "/"
      + theme_variant.toLower() + "/";
}

QString AOApplication::get_default_theme_path()
{
  return get_base_path() + "themes/default/";
}

QString AOApplication::get_character_path(QString p_character)
{
  return get_base_path() + "characters/" + p_character.toLower() + "/";
}

QString AOApplication::get_demothings_path()
{
  QString default_path = "misc/demothings/";
  QString alt_path = "misc/RosterImages";
  if (dir_exists(default_path))
    return get_base_path() + default_path;
  else if (dir_exists(alt_path))
    return get_base_path() + alt_path;
  else
    return get_base_path() + default_path;
}
QString AOApplication::get_sounds_path()
{
  return get_base_path() + "sounds/general/";
}
QString AOApplication::get_music_path(QString p_song)
{
  return get_base_path() + "sounds/music/" + p_song.toLower();
}

QString AOApplication::get_background_path()
{
  if (courtroom_constructed)
    return w_courtroom->get_background_path();
  //this function being called when the courtroom isn't constructed makes no sense
  return "";
}

QString AOApplication::get_default_background_path()
{
  return get_base_path() + "background/gs4/";
}

QString AOApplication::get_evidence_path()
{
    QString default_path = "evidence/";
    QString alt_path = "items/";
    if (dir_exists(default_path))
      return get_base_path() + default_path;
    else if (dir_exists(alt_path))
      return get_base_path() + alt_path;
    else
      return get_base_path() + default_path;
}

QString Courtroom::get_background_path()
{
  return ao_app->get_base_path() + "background/" + current_background.toLower() + "/";
}

QString Courtroom::get_default_background_path()
{
  return ao_app->get_base_path() + "background/gs4/";
}
