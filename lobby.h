#ifndef LOBBY_H
#define LOBBY_H

#include "aobutton.h"
#include "aoimage.h"
#include "aopacket.h"
#include "aotextarea.h"

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QTextBrowser>

class AOApplication;

class Lobby : public QMainWindow
{
  Q_OBJECT

public:
  Lobby(AOApplication *p_ao_app);

  void set_widgets();
  void list_servers();
  void list_favorites();
  void append_chatmessage(QString f_name, QString f_message);
  void append_error(QString f_message);
  void set_choose_a_server();
  void set_player_count(int players_online, int max_players);
  void set_loading_text(QString p_text);
  void set_stylesheet(QWidget *widget, QString target_tag);
  void set_stylesheets();
  void set_fonts();
  void set_font(QWidget *widget, QString p_identifier);
  void set_qtextedit_font(QTextEdit *widget, QString p_identifier);
  void show_loading_overlay()
  {
    ui_loading_background->show();
  }
  void hide_loading_overlay()
  {
    ui_loading_background->hide();
  }
  QString get_chatlog();
  int get_selected_server();

  void set_loading_value(int p_value);

  bool public_servers_selected = true;

private:
  AOApplication *ao_app = nullptr;

  AOImage *ui_background;

  AOButton *ui_public_servers;
  AOButton *ui_favorites;

  AOButton *ui_refresh;
  AOButton *ui_add_to_fav;
  AOButton *ui_connect;

  QTextEdit *ui_version;
  AOButton *ui_about;

  QListWidget *ui_server_list;

  QTextEdit *ui_player_count;
  AOTextArea *ui_description;

  AOTextArea *ui_chatbox;

  QLineEdit *ui_chatname;
  QLineEdit *ui_chatmessage;

  AOImage *ui_loading_background;
  QTextEdit *ui_loading_text;
  QProgressBar *ui_progress_bar;
  AOButton *ui_cancel;

  server_type f_last_server;

  void set_size_and_pos(QWidget *p_widget, QString p_identifier);

private slots:
  void on_public_servers_clicked();
  void on_favorites_clicked();

  void on_refresh_pressed();
  void on_refresh_released();
  void on_add_to_fav_pressed();
  void on_add_to_fav_released();
  void on_connect_pressed();
  void on_connect_released();
  void on_about_clicked();
  void on_server_list_clicked(QModelIndex p_model);
  void on_chatfield_return_pressed();
};

#endif // LOBBY_H
