#include "debug_functions.h"

#include <QDebug>
#include <QMessageBox>

void drMessageBox(QString p_message, bool p_error)
{
  QMessageBox l_box;
  l_box.setWindowTitle(p_error ? "Error" : "Notice");
  l_box.setIcon(p_error ? QMessageBox::Critical : QMessageBox::Information);
  l_box.setText(p_message);
  l_box.exec();
}

void call_error(QString p_message)
{
  drMessageBox(p_message, true);
  qWarning() << "ERROR" << p_message;
}

void call_notice(QString p_message)
{
  drMessageBox(p_message, false);
  qInfo() << p_message;
}
