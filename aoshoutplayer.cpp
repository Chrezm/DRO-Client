#include "aoshoutplayer.hpp"
#include "file_functions.h"

#include <QDebug>

AOShoutPlayer::AOShoutPlayer(QObject *p_parent, AOApplication *p_ao_app)
    : AOAbstractPlayer(p_parent, p_ao_app)
{}

void AOShoutPlayer::play(QString p_name, QString p_char)
{
    QString f_file;

    QString char_path = ao_app->get_character_path(p_char) + p_name.toLower();
    QString theme_path = ao_app->get_image_path(p_name.toLower());

    qDebug() << char_path;
    qDebug() << theme_path;

    if(file_exists(char_path))
      f_file = char_path;
    else if (file_exists(theme_path))
      f_file = theme_path;
    else
      f_file = "";

    try {
        AOBassHandle *handle = new AOBassHandle(f_file, true, this);
        connect(this, &AOShoutPlayer::new_volume, handle, &AOBassHandle::set_volume);
        connect(this, &AOShoutPlayer::stopping, handle, &AOBassHandle::stop);
        handle->set_volume(get_volume());
        handle->play();
    } catch(const std::exception &e_exception) {
        qDebug() << e_exception.what();
    }
}

void AOShoutPlayer::stop()
{
    emit stopping();
}
