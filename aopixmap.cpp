#include "aopixmap.h"

AOPixmap::AOPixmap(QPixmap p_pixmap) : m_pixmap(p_pixmap)
{
  if (m_pixmap.isNull())
  {
    m_pixmap = QPixmap(1, 1);
    m_pixmap.fill(Qt::transparent);
  }
}

AOPixmap::AOPixmap(QString p_file_path) : AOPixmap(QPixmap(p_file_path)) {}

QPixmap AOPixmap::scaleToSize(QSize p_size)
{
  bool f_is_pixmap_larger =
      m_pixmap.width() > p_size.width() || m_pixmap.height() > p_size.height();
  return m_pixmap.scaled(p_size, Qt::IgnoreAspectRatio,
                         f_is_pixmap_larger ? Qt::SmoothTransformation
                                            : Qt::FastTransformation);
}
