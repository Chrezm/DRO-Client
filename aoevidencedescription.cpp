#include "aoevidencedescription.h"

AOEvidenceDescription::AOEvidenceDescription(QWidget *parent)
    : QPlainTextEdit(parent)
{
  this->setReadOnly(true);

  // connect(this, SIGNAL(returnPressed()), this, SLOT(on_enter_pressed()));
}

void AOEvidenceDescription::mouseDoubleClickEvent(QMouseEvent *e)
{
  QPlainTextEdit::mouseDoubleClickEvent(e);

  this->setReadOnly(false);
}

void AOEvidenceDescription::on_enter_pressed() { this->setReadOnly(true); }
