#include "aoscene.h"
// src
#include "courtroom.h"
#include "file_functions.h"
// qt
#include <QDebug>
#include <QMovie>

AOScene::AOScene(QWidget *parent, AOApplication *p_ao_app)
    : QLabel(parent), ao_app(p_ao_app)
{
  m_reader = new QMovie(this);
  setMovie(m_reader);
}

void AOScene::set_image(QString p_image)
{
  QString target_path = ao_app->get_default_background_path(p_image);

  // background specific path
  QString background_path = ao_app->get_background_path(p_image);

  for (auto &ext : QVector<QString>{".webp", ".apng", ".gif", ".png"})
  {
    QString full_background_path =
        ao_app->get_case_sensitive_path(background_path + ext);

    if (file_exists(full_background_path))
    {
      target_path = full_background_path;
      break;
    }
  }

  // do not update the movie if we're using the same file
  if (m_reader->fileName() == target_path)
    return;

  m_reader->stop();
  delete m_reader;

  m_reader = new QMovie(this);
  m_reader->setScaledSize(size());
  m_reader->setFileName(target_path);
  setMovie(m_reader);
  m_reader->start();
}

void AOScene::combo_resize(QSize p_size)
{
  resize(p_size);
  m_reader->stop();
  m_reader->setScaledSize(p_size);
  m_reader->start();
}
