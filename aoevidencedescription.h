#ifndef AOEVIDENCEDESCRIPTION_H
#define AOEVIDENCEDESCRIPTION_H

#include <QPlainTextEdit>

class AOEvidenceDescription : public QPlainTextEdit
{
  Q_OBJECT
public:
  AOEvidenceDescription(QWidget *parent);

protected:
  void mouseDoubleClickEvent(QMouseEvent *e);

signals:
  void double_clicked();

private slots:
  void on_enter_pressed();

};

#endif // AOEVIDENCEDESCRIPTION_H
