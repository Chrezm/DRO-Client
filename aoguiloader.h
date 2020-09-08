#ifndef AOGUILOADER_H
#define AOGUILOADER_H
// qt
#include <QUiLoader>
#include <QWidget>

// we could make a smarter system but let's keep it simple ;)
#define AO_GUI_WIDGET(p_type, p_name) findChild<p_type *>(p_name)

class AOGuiLoader : public QUiLoader
{
  Q_OBJECT

public:
  AOGuiLoader(QObject *p_parent = nullptr);

  QWidget *load_from_file(QString p_file_path, QWidget *p_parent = nullptr);
};

#endif
