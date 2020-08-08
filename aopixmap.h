#pragma once
// qt
#include <QPixmap>

class AOPixmap
{
public:
    AOPixmap(QPixmap p_pixmap);
    AOPixmap(QString p_file_path);

    QPixmap *operator ->();

private:
    QPixmap m_pixmap;
};
