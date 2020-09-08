#ifndef AONOTEPICKER_HPP
#define AONOTEPICKER_HPP

#include "aobutton.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

class AONotePicker : public QLabel
{
  Q_OBJECT

public:
  AONotePicker(QWidget *p_parent, AOApplication *p_ao_app);

  QLineEdit *m_line;
  AOButton *m_button;
  AOButton *m_delete_button;
  AOButton *m_hover;
  QHBoxLayout *m_layout;
  QString real_file = "";

private:
  AOApplication *ao_app;
};

#endif // AONOTEPICKER_HPP
