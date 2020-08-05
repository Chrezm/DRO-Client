#include "aopixmap.h"

AOPixmap::AOPixmap(QPixmap p_pixmap) : m_pixmap(p_pixmap)
{

}

AOPixmap::AOPixmap(QString p_file_path) : m_pixmap(p_file_path)
{

}

QPixmap *AOPixmap::operator ->()
{
    if (m_pixmap.isNull())
    {
        m_pixmap = QPixmap(1, 1);
        m_pixmap.fill(Qt::transparent);
    }

    return &m_pixmap;
}
