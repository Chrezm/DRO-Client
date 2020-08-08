#pragma once
// qt
#include <QPixmap>

class AOPixmap
{
public:
    AOPixmap(QPixmap p_pixmap);
    AOPixmap(QString p_file_path);

    QPixmap scaleToSize(QSize p_size);

    QPixmap *operator ->();

private:
    QPixmap m_pixmap;
};
