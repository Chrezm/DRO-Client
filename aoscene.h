#ifndef AOSCENE_H
#define AOSCENE_H

#include <QLabel>

class Courtroom;
class AOApplication;

class AOScene : public QLabel
{
  Q_OBJECT

public:
  AOScene(QWidget *parent, AOApplication *p_ao_app);

  void set_image(QString p_image);
  void combo_resize(QSize p_size);

private:
  AOApplication *ao_app = nullptr;
  QMovie *m_reader = nullptr;
};

#endif // AOSCENE_H
