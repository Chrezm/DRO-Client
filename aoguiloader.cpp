#include "aoguiloader.h"
// qt
#include <QFile>
#include <QVBoxLayout>

AOGuiLoader::AOGuiLoader(QObject *p_parent) : QUiLoader(p_parent)
{
  // padding
}

QWidget *AOGuiLoader::load_from_file(QString p_file_path, QWidget *p_parent)
{
  QWidget *r_widget = nullptr;

  QFile f_file(p_file_path);
  if (f_file.open(QIODevice::ReadOnly))
  {
    r_widget = load(&f_file, p_parent);

    // lazily replace the parent's layout with our own
    if (p_parent != nullptr)
    {
      QVBoxLayout *f_parent_layout = new QVBoxLayout(p_parent);
      f_parent_layout->addWidget(r_widget);
      p_parent->setLayout(f_parent_layout);
    }
  }

  return r_widget;
}
