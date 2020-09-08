#ifndef AOPIXMAP_H
#define AOPIXMAP_H
// qt
#include <QPixmap>

class AOPixmap
{
public:
  AOPixmap(QPixmap p_pixmap = QPixmap());
  AOPixmap(QString p_file_path);

  void clear();

  QPixmap scale_to_size(QSize p_size);

private:
  QPixmap m_pixmap;
};

#endif
