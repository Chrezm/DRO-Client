#include <QDebug>

#include "aoevidencedisplay.h"

#include "datatypes.h"
#include "file_functions.h"
#include "misc_functions.h"

AOEvidenceDisplay::AOEvidenceDisplay(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  m_movie = new QMovie(this);
  w_icon = new QLabel(this);
  dr_sfx = new AOSfxPlayer(ao_app, this);

  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
}

void AOEvidenceDisplay::show_evidence(QString p_evidence_image, bool is_left_side)
{
  this->reset();

  QString f_evidence_path = ao_app->get_evidence_path(p_evidence_image);

  AOPixmap f_pixmap(f_evidence_path);
  QString gif_name;
  QString icon_identifier;

  if (is_left_side)
  {
    icon_identifier = "left_evidence_icon";
    gif_name = "evidence_appear_left.gif";
  }
  else
  {
    icon_identifier = "right_evidence_icon";
    gif_name = "evidence_appear_right.gif";
  }

  pos_size_type icon_dimensions = ao_app->get_element_dimensions(icon_identifier, "courtroom_design.ini");

  w_icon->move(icon_dimensions.x, icon_dimensions.y);
  w_icon->resize(icon_dimensions.width, icon_dimensions.height);
  w_icon->setPixmap(f_pixmap.scale(w_icon->size()));

  QString f_path = ao_app->find_theme_asset_path(gif_name);
  m_movie->setFileName(f_path);
  if (m_movie->frameCount() < 1)
    return;

  this->setMovie(m_movie);

  m_movie->start();
  dr_sfx->play_effect(ao_app->get_sfx("evidence_present"));
}

void AOEvidenceDisplay::frame_change(int p_frame)
{
  if (p_frame == (m_movie->frameCount() - 1))
  {
    // we need this or else the last frame wont show
    delay(m_movie->nextFrameDelay());

    m_movie->stop();
    this->clear();

    w_icon->show();
  }
}

void AOEvidenceDisplay::reset()
{
  m_movie->stop();
  w_icon->hide();
  this->clear();
}
