#ifndef AOSCENE_H
#define AOSCENE_H

#include <QLabel>

class Courtroom;
class AOApplication;

class AOScene : public QLabel
{
    Q_OBJECT

public:
    explicit AOScene(QWidget *parent, AOApplication *p_ao_app);

    void set_image(QString p_image);

private:
    AOApplication *ao_app = nullptr;
    QMovie *m_movie       = nullptr;
};

#endif // AOSCENE_H
