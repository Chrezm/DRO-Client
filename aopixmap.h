#ifndef AOPIXMAP_H
#define AOPIXMAP_H
// qt
#include <QPixmap>

class AOPixmap
{
public:
    AOPixmap(QPixmap p_pixmap);
    AOPixmap(QString p_file_path);

    QPixmap scaleToSize(QSize p_size);

private:
    QPixmap m_pixmap;
};

#endif
