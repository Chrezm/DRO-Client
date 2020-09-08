#include <QDebug>

#include "aoevidencedisplay.h"

#include "datatypes.h"
#include "file_functions.h"
#include "misc_functions.h"

AOEvidenceDisplay::AOEvidenceDisplay(QWidget *p_parent, AOApplication *p_ao_app)
    : QLabel(p_parent)
{
  ao_app = p_ao_app;

  evidence_movie = new QMovie(this);
  evidence_icon = new QLabel(this);
  sfx_player = new AOSfxPlayer(this, ao_app);

  connect(evidence_movie, SIGNAL(frameChanged(int)), this,
          SLOT(frame_change(int)));
}

void AOEvidenceDisplay::show_evidence(QString p_evidence_image,
                                      bool is_left_side)
{
  this->reset();

  QString f_evidence_path = ao_app->get_evidence_path(p_evidence_image);

  AOPixmap f_pixmap(f_evidence_path);

  QString final_gif_path;
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

  pos_size_type icon_dimensions =
      ao_app->get_element_dimensions(icon_identifier, "courtroom_design.ini");

  evidence_icon->move(icon_dimensions.x, icon_dimensions.y);
  evidence_icon->resize(icon_dimensions.width, icon_dimensions.height);
  evidence_icon->setPixmap(f_pixmap.scale_to_size(evidence_icon->size()));

  QString f_path = ao_app->get_image_path(gif_name);
  evidence_movie->setFileName(f_path);
  if (evidence_movie->frameCount() < 1)
    return;

  this->setMovie(evidence_movie);

  evidence_movie->start();
  sfx_player->play(ao_app->get_sfx("evidence_present"));
}

void AOEvidenceDisplay::frame_change(int p_frame)
{
  if (p_frame == (evidence_movie->frameCount() - 1))
  {
    // we need this or else the last frame wont show
    delay(evidence_movie->nextFrameDelay());

    evidence_movie->stop();
    this->clear();

    evidence_icon->show();
  }
}

void AOEvidenceDisplay::reset()
{
  sfx_player->stop();
  evidence_movie->stop();
  evidence_icon->hide();
  this->clear();
}

QLabel *AOEvidenceDisplay::get_evidence_icon()
{
  return evidence_icon;
}
