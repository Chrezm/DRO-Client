#include "aoshoutplayer.h"

#include "draudioengine.h"
#include "file_functions.h"

#include <QDebug>

AOShoutPlayer::AOShoutPlayer(AOApplication *p_ao_app, QObject *p_parent) : AOObject(p_ao_app, p_parent)
{}

void AOShoutPlayer::play(QString p_chr, QString p_shout)
{
  QStringList l_file_list;
  for (const QString &i_chr : ao_app->get_char_include_tree(p_chr))
    l_file_list.append(ao_app->get_character_path(i_chr, p_shout));

  QString l_file = ao_app->find_asset_path(l_file_list, audio_extensions());
  if (l_file.isEmpty())
    l_file = ao_app->find_theme_asset_path(p_shout, audio_extensions());

  if (l_file.isEmpty())
    return;
  DRAudioEngine::get_family(DRAudio::Family::FEffect)->play_stream(l_file);
}
