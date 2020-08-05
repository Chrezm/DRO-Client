#include "aocharmovie.h"

#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"
#include "misc_functions.h"

#include <QDebug>
#include <QImageReader>

AOCharMovie::AOCharMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
    ao_app = p_ao_app;

    m_movie = new QMovie(this);

    preanim_timer = new QTimer(this);
    preanim_timer->setSingleShot(true);

    connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
    connect(preanim_timer, SIGNAL(timeout()), this, SLOT(timer_done()));
}

void AOCharMovie::play(QString p_char, QString p_emote, QString emote_prefix, bool show)
{
    QString gif_path;
    QStringList f_vec;
    QStringList f_paths{
        ao_app->get_character_path(p_char) + emote_prefix + p_emote.toLower(), // .gif
        ao_app->get_character_path(p_char) + p_emote.toLower(),                // .png
        ao_app->get_theme_variant_path() + "placeholder",                      // .gif
        ao_app->get_theme_path() + "placeholder",                              // .gif
        ao_app->get_default_theme_path() + "placeholder"                       // .gif
    };

    for (auto &f_file : f_paths)
    {
        bool found = false;
        for (auto &ext : decltype(f_vec){".apng", ".gif", ".png"})
        {
            QString fullPath = f_file + ext;
            found            = file_exists(fullPath);
            if (found)
            {
                gif_path = fullPath;
                break;
            }
        }

        if (found)
            break;
    }

    m_movie->stop();
    m_movie->setFileName(gif_path);

    QImageReader *reader = new QImageReader(gif_path);

    movie_frames.clear();
    QImage f_image = reader->read();
    while (!f_image.isNull())
    {
        if (m_flipped)
            movie_frames.append(f_image.mirrored(true, false));
        else
            movie_frames.append(f_image);
        f_image = reader->read();
    }

    delete reader;

    this->show();
    if (!show) this->hide();

    m_movie->start();
}

void AOCharMovie::play_pre(QString p_char, QString p_emote, int duration, bool show)
{
    QString gif_path = ao_app->get_character_path(p_char) + p_emote.toLower();

    m_movie->stop();
    this->clear();
    m_movie->setFileName(gif_path);
    m_movie->jumpToFrame(0);

    int full_duration = duration * time_mod;
    int real_duration = 0;

    play_once = false;

    for (int n_frame = 0; n_frame < m_movie->frameCount(); ++n_frame)
    {
        real_duration += m_movie->nextFrameDelay();
        m_movie->jumpToFrame(n_frame + 1);
    }
    qDebug() << "full_duration: " << full_duration;
    qDebug() << "real_duration: " << real_duration;

    double percentage_modifier = 100.0;

    if (real_duration != 0 && duration != 0)
    {
        double modifier     = full_duration / static_cast<double>(real_duration);
        percentage_modifier = 100 / modifier;

        if (percentage_modifier > 100.0)
            percentage_modifier = 100.0;
    }
    qDebug() << "% mod: " << percentage_modifier;

    if (full_duration == 0 || full_duration >= real_duration)
    {
        play_once = true;
    }
    else
    {
        play_once = false;
        preanim_timer->start(full_duration);
    }

    m_movie->setSpeed(static_cast<int>(percentage_modifier));
    play(p_char, p_emote, "", show);
}

void AOCharMovie::play_talking(QString p_char, QString p_emote, bool show)
{
    QString gif_path = ao_app->get_character_path(p_char) + "(b)" + p_emote.toLower();

    m_movie->stop();
    this->clear();
    m_movie->setFileName(gif_path);

    play_once = false;
    m_movie->setSpeed(100);
    play(p_char, p_emote, "(b)", show);
}

void AOCharMovie::play_idle(QString p_char, QString p_emote, bool show)
{
    QString gif_path = ao_app->get_character_path(p_char) + "(a)" + p_emote.toLower();

    m_movie->stop();
    this->clear();
    m_movie->setFileName(gif_path);

    play_once = false;
    m_movie->setSpeed(100);
    play(p_char, p_emote, "(a)", show);
}

void AOCharMovie::stop()
{
    //for all intents and purposes, stopping is the same as hiding. at no point do we want a frozen gif to display
    m_movie->stop();
    preanim_timer->stop();
    this->hide();
}

void AOCharMovie::combo_resize(int w, int h)
{
    QSize f_size(w, h);
    this->resize(f_size);
    m_movie->setScaledSize(f_size);
}

void AOCharMovie::frame_change(int n_frame)
{
    if (movie_frames.size() > n_frame)
    {
        AOPixmap f_pixmap = QPixmap::fromImage(movie_frames.at(n_frame));
        this->setPixmap(f_pixmap->scaled(this->width(), this->height()));
    }

    if (m_movie->frameCount() - 1 == n_frame && play_once)
    {
        preanim_timer->start(m_movie->nextFrameDelay());
        m_movie->stop();
    }
}

void AOCharMovie::timer_done()
{
    done();
}
