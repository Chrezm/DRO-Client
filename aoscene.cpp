#include "aoscene.h"
// src
#include "courtroom.h"
#include "file_functions.h"
// qt
#include <QDebug>
#include <QMovie>

AOScene::AOScene(QWidget *parent, AOApplication *p_ao_app)
    : QLabel(parent)
{
    ao_app = p_ao_app;
}

void AOScene::set_image(QString p_image)
{
    QString target_path = ao_app->get_default_background_path() + p_image;

    // background specific path
    QString background_path = ao_app->get_background_path() + p_image;

    for (auto &ext : QVector<QString>{".webp", ".apng", ".gif", ".png"})
    {
        QString full_background_path = background_path + ext;

        if (file_exists(full_background_path))
        {
            target_path = full_background_path;
            break;
        }
    }

    // do not update the movie if we're using the same file
    if (m_movie && m_movie->fileName() == target_path)
        return;

    // clear previous
    this->clear();

    // delete current movie
    delete m_movie;

    // create new movie to run
    m_movie = new QMovie(this);
    setMovie(m_movie);
    m_movie->setFileName(target_path);
    m_movie->setScaledSize(size());
    m_movie->start();
}
