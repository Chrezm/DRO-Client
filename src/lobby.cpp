#include "lobby.h"

#include "aoapplication.h"
#include "aosfxplayer.h"
#include "debug_functions.h"
#include "drtextedit.h"
#include "networkmanager.h"

#include <QDebug>
#include <QDir>
#include <QImageReader>
#include <QMessageBox>
#include <QScrollBar>

Lobby::Lobby(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

  this->setWindowTitle("Danganronpa Online");

  ui_background = new AOImageDisplay(this, ao_app);
  ui_public_servers = new AOButton(this, ao_app);
  ui_favorites = new AOButton(this, ao_app);
  ui_refresh = new AOButton(this, ao_app);
  ui_add_to_fav = new AOButton(this, ao_app);
  ui_connect = new AOButton(this, ao_app);
  ui_version = new DRTextEdit(this);
  ui_version->setFrameStyle(QFrame::NoFrame);
  ui_version->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_version->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_version->setReadOnly(true);
  ui_about = new AOButton(this, ao_app);
  ui_server_list = new QListWidget(this);
  ui_player_count = new DRTextEdit(this);
  ui_player_count->setFrameStyle(QFrame::NoFrame);
  ui_player_count->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_player_count->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_player_count->setReadOnly(true);
  ui_description = new AOTextArea(this);
  ui_chatbox = new AOTextArea(this);
  ui_chatbox->setOpenExternalLinks(true);
  ui_chatname = new QLineEdit(this);
  ui_chatname->setPlaceholderText("Name");
  ui_chatmessage = new QLineEdit(this);
  ui_loading_background = new AOImageDisplay(this, ao_app);
  ui_loading_text = new DRTextEdit(ui_loading_background);
  ui_progress_bar = new QProgressBar(ui_loading_background);
  ui_progress_bar->setMinimum(0);
  ui_progress_bar->setMaximum(100);
  ui_progress_bar->setStyleSheet("QProgressBar{ color: white; }");
  ui_cancel = new AOButton(ui_loading_background, ao_app);

  connect(ui_public_servers, SIGNAL(clicked()), this, SLOT(on_public_servers_clicked()));
  connect(ui_favorites, SIGNAL(clicked()), this, SLOT(on_favorites_clicked()));
  connect(ui_refresh, SIGNAL(pressed()), this, SLOT(on_refresh_pressed()));
  connect(ui_refresh, SIGNAL(released()), this, SLOT(on_refresh_released()));
  connect(ui_add_to_fav, SIGNAL(pressed()), this, SLOT(on_add_to_fav_pressed()));
  connect(ui_add_to_fav, SIGNAL(released()), this, SLOT(on_add_to_fav_released()));
  connect(ui_connect, SIGNAL(pressed()), this, SLOT(on_connect_pressed()));
  connect(ui_connect, SIGNAL(released()), this, SLOT(on_connect_released()));
  connect(ui_about, SIGNAL(clicked()), this, SLOT(on_about_clicked()));
  connect(ui_server_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_server_list_clicked(QModelIndex)));
  connect(ui_chatmessage, SIGNAL(returnPressed()), this, SLOT(on_chatfield_return_pressed()));
  connect(ui_cancel, SIGNAL(clicked()), ao_app, SLOT(loading_cancelled()));

  set_widgets();
}

// sets images, position and size
void Lobby::set_widgets()
{
  QString filename = "lobby_design.ini";

  pos_size_type f_lobby = ao_app->get_element_dimensions("lobby", filename);

  if (f_lobby.width < 0 || f_lobby.height < 0)
  {
    qDebug() << "W: did not find lobby width or height in " << filename;

    // Most common symptom of bad config files, missing assets, or misnamed
    // theme folder
    call_notice("It doesn't look like your client is set up correctly. This can be "
                "due to the following reasons: \n"
                "1. Check you downloaded and extracted the resources correctly from "
                "the DRO Discord including the large 'base' folder.\n"
                "2. If you did, check that the base folder is in the same folder "
                "where you launched Danganronpa Online from: " +
                QDir::currentPath() +
                "\n"
                "3. If it is there, check that your current theme folder exists in "
                "base/themes. According to base/config.ini, your current theme is " +
                ao_app->get_theme());

    this->resize(517, 666);
  }
  else
  {
    this->resize(f_lobby.width, f_lobby.height);
  }

  set_size_and_pos(ui_background, "lobby");
  ui_background->set_image("lobbybackground.png");

  set_size_and_pos(ui_public_servers, "public_servers");
  ui_public_servers->set_image("publicservers_selected.png");

  set_size_and_pos(ui_favorites, "favorites");
  ui_favorites->set_image("favorites.png");

  set_size_and_pos(ui_refresh, "refresh");
  ui_refresh->set_image("refresh.png");

  set_size_and_pos(ui_add_to_fav, "add_to_fav");
  ui_add_to_fav->set_image("addtofav.png");

  set_size_and_pos(ui_connect, "connect");
  ui_connect->set_image("connect.png");

  set_size_and_pos(ui_version, "version");
  ui_version->setText("Version: " + ao_app->get_version_string());

  set_size_and_pos(ui_about, "about");
  ui_about->set_image("about.png");

  set_size_and_pos(ui_server_list, "server_list");
  ui_server_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "font: bold;");

  set_size_and_pos(ui_player_count, "player_count");
  ui_player_count->setStyleSheet("font: bold;"
                                 "color: white;"
                                 "qproperty-alignment: AlignCenter;");

  set_size_and_pos(ui_description, "description");
  ui_description->setReadOnly(true);
  ui_description->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "color: white;");

  set_size_and_pos(ui_chatbox, "chatbox");
  ui_chatbox->setReadOnly(true);
  ui_chatbox->setStyleSheet("QTextBrowser{background-color: rgba(0, 0, 0, 0);}");

  set_size_and_pos(ui_chatname, "chatname");
  ui_chatname->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                             "selection-background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_chatmessage, "chatmessage");
  ui_chatmessage->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "selection-background-color: rgba(0, 0, 0, 0);");

  ui_loading_background->resize(this->width(), this->height());
  ui_loading_background->set_image("loadingbackground.png");

  set_size_and_pos(ui_loading_text, "loading_label");
  ui_loading_text->setFont(QFont("Arial", 20, QFont::Bold));
  ui_loading_text->setReadOnly(true);
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->setFrameStyle(QFrame::NoFrame);
  ui_loading_text->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: rgba(255, 128, 0, 255);");
  ui_loading_text->append("Loading");

  set_size_and_pos(ui_progress_bar, "progress_bar");
  set_size_and_pos(ui_cancel, "cancel");
  ui_cancel->setText("Cancel");

  ui_loading_background->hide();

  set_fonts();
  set_stylesheets();
  set_choose_a_server();
}

void Lobby::set_size_and_pos(QWidget *p_widget, QString p_identifier)
{
  QString filename = "lobby_design.ini";

  pos_size_type design_ini_result = ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0)
  {
    qDebug() << "W: could not find " << p_identifier << " in " << filename;
    p_widget->hide();
  }
  else
  {
    p_widget->move(design_ini_result.x, design_ini_result.y);
    p_widget->resize(design_ini_result.width, design_ini_result.height);
  }
}

void Lobby::set_fonts()
{
  set_drtextedit_font(ui_player_count, "player_count");
  set_font(ui_description, "description");
  set_font(ui_chatbox, "chatbox");
  set_font(ui_chatname, "chatname");
  set_font(ui_chatmessage, "chatmessage");
  set_drtextedit_font(ui_loading_text, "loading_text");
  set_font(ui_server_list, "server_list");
}

void Lobby::set_stylesheet(QWidget *widget, QString target_tag)
{
  QString f_file = "lobby_stylesheets.css";
  QString style_sheet_string = ao_app->get_stylesheet(target_tag, f_file);
  if (style_sheet_string != "")
    widget->setStyleSheet(style_sheet_string);
}

void Lobby::set_stylesheets()
{
  set_stylesheet(ui_player_count, "[PLAYER COUNT]");
  set_stylesheet(ui_description, "[DESCRIPTION]");
  set_stylesheet(ui_chatbox, "[CHAT BOX]");
  set_stylesheet(ui_chatname, "[CHAT NAME]");
  set_stylesheet(ui_chatmessage, "[CHAT MESSAGE]");
  set_stylesheet(ui_loading_text, "[LOADING TEXT]");
  set_stylesheet(ui_server_list, "[SERVER LIST]");
}

void Lobby::set_font(QWidget *widget, QString p_identifier)
{
  QString design_file = "lobby_fonts.ini";

  if (!(bool)ao_app->get_font_property("use_custom_fonts", design_file))
    return;

  int f_weight = ao_app->get_font_property(p_identifier, design_file);

  // Font priority
  // 1. "font_" + p_identifier
  // 2. "font_default"
  // 3. System font
  QFontDatabase font_database;
  QString font_name = ao_app->get_font_name("font_" + p_identifier, design_file);
  if (!font_database.families().contains(font_name))
    font_name = ao_app->get_font_name("font_default", "lobby_fonts.ini");
  QFont font(font_name, f_weight);
  widget->setFont(font);

  QColor f_color = ao_app->get_color(p_identifier + "_color", design_file);

  bool bold = (bool)ao_app->get_font_property(p_identifier + "_bold", design_file);
  QString is_bold = "";
  if (bold)
    is_bold = "bold";

  QString class_name = widget->metaObject()->className();
  QString style_sheet_string = class_name + " { background-color: rgba(0, 0, 0, 0);\n" + "color: rgba(" +
                               QString::number(f_color.red()) + ", " + QString::number(f_color.green()) + ", " +
                               QString::number(f_color.blue()) + ", 255);\n" + "font: " + is_bold + "; }";

  widget->setStyleSheet(style_sheet_string);
}

void Lobby::set_drtextedit_font(DRTextEdit *widget, QString p_identifier)
{
  set_font(widget, p_identifier);

  QString fonts_ini = "lobby_fonts.ini";
  // Do outlines
  bool outline = (ao_app->get_font_property(p_identifier + "_outline", fonts_ini) == 1);
  widget->setOutline(outline);

  // Do horizontal alignments
  int raw_halign = ao_app->get_font_property(p_identifier + "_halign", fonts_ini);
  switch (raw_halign)
  {
  case 0:
    widget->setHorizontalAlignment(Qt::AlignLeft);
    break;
  case 1:
    widget->setHorizontalAlignment(Qt::AlignHCenter);
    break;
  case 2:
    widget->setHorizontalAlignment(Qt::AlignRight);
    break;
  default:
    qWarning() << "Unknown horizontal alignment for " + p_identifier + ". Assuming Left.";
    widget->setHorizontalAlignment(Qt::AlignLeft);
  }

  // Do vertical alignments
  int raw_valign = ao_app->get_font_property(p_identifier + "_valign", fonts_ini);
  Qt::Alignment valignment;
  switch (raw_valign)
  {
  case 0:
    valignment = Qt::AlignTop;
    break;
  case 1:
    valignment = Qt::AlignVCenter;
    break;
  case 2:
    valignment = Qt::AlignBottom;
    break;
  default:
    qWarning() << "Unknown vertical alignment for" << p_identifier << ":" << raw_valign << "Assuming Top.";
    valignment = Qt::AlignTop;
  }
  widget->setVerticalAlignment(valignment);
}

void Lobby::set_loading_text(QString p_text)
{
  ui_loading_text->clear();
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->append(p_text);
}

QString Lobby::get_chatlog()
{
  QString return_value = ui_chatbox->toPlainText();

  return return_value;
}

int Lobby::get_selected_server()
{
  return ui_server_list->currentRow();
}

void Lobby::set_loading_value(int p_value)
{
  ui_progress_bar->setValue(p_value);
}

void Lobby::on_public_servers_clicked()
{
  ui_public_servers->set_image("publicservers_selected.png");
  ui_favorites->set_image("favorites.png");

  list_servers();

  public_servers_selected = true;
}

void Lobby::on_favorites_clicked()
{
  ui_favorites->set_image("favorites_selected.png");
  ui_public_servers->set_image("publicservers.png");

  ao_app->set_favorite_list();
  // ao_app->favorite_list = read_serverlist_txt();

  list_favorites();

  public_servers_selected = false;
}

void Lobby::on_refresh_pressed()
{
  ui_refresh->set_image("refresh_pressed.png");
}

void Lobby::on_refresh_released()
{
  ui_refresh->set_image("refresh.png");

  AOPacket *f_packet = new AOPacket("ALL#%");

  ao_app->send_ms_packet(f_packet);
}

void Lobby::on_add_to_fav_pressed()
{
  ui_add_to_fav->set_image("addtofav_pressed.png");
}

void Lobby::on_add_to_fav_released()
{
  ui_add_to_fav->set_image("addtofav.png");

  // you cant add favorites from favorites m8
  if (!public_servers_selected)
    return;

  ao_app->add_favorite_server(ui_server_list->currentRow());
}

void Lobby::on_connect_pressed()
{
  ui_connect->set_image("connect_pressed.png");
}

void Lobby::on_connect_released()
{
  ui_connect->set_image("connect.png");

  AOPacket *f_packet = nullptr;

  f_packet = new AOPacket("askchaa#%");

  ao_app->send_server_packet(f_packet);
}

void Lobby::on_about_clicked()
{
  const bool hasApng = QImageReader::supportedImageFormats().contains("apng");

  QString msg = tr("<h2>Danganronpa Online</h2>"
                   "version: %1"
                   "<p><b>Source code:</b> "
                   "<a href='https://github.com/Chrezm/DRO-Client'>"
                   "https://github.com/Chrezm/DRO-Client</a>"
                   "<p><b>Development:</b><br>"
                   "Cerapter, Elf, Iuvee, Tricky Leifa, Ziella"
                   "<p>Based on Attorney Online 2:<br>"
                   "<a href='https://github.com/AttorneyOnline/AO2-Client'>"
                   "https://github.com/AttorneyOnline/AO2-Client</a>"
                   "<p>Running on Qt version %2 with the BASS audio engine.<br>"
                   "APNG plugin loaded: %3"
                   "<p>Built on %4")
                    .arg(ao_app->get_version_string())
                    .arg(QLatin1String(QT_VERSION_STR))
                    .arg(hasApng ? tr("Yes") : tr("No"))
                    .arg(QLatin1String(__DATE__));

  QMessageBox::about(this, tr("About"), msg);
}

void Lobby::on_server_list_clicked(QModelIndex p_model)
{
  int n_server = p_model.row();

  if (n_server < 0)
    return;

  if (public_servers_selected)
  {
    QVector<server_type> f_server_list = ao_app->get_server_list();

    if (n_server >= f_server_list.size())
      return;

    f_last_server = f_server_list.at(p_model.row());
  }
  else
  {
    if (n_server >= ao_app->get_favorite_list().size())
      return;

    f_last_server = ao_app->get_favorite_list().at(p_model.row());
  }

  ui_player_count->setText(nullptr);
  ui_description->moveCursor(QTextCursor::Start);
  ui_description->setText("Connecting to " + f_last_server.name + "...\n\n");
  ui_description->append(f_last_server.desc);
  ui_description->ensureCursorVisible();

  ao_app->net_manager->connect_to_server(f_last_server);
}

void Lobby::on_chatfield_return_pressed()
{
  // no you can't send empty messages
  if (ui_chatname->text() == "" || ui_chatmessage->text() == "")
    return;

  QString f_header = "CT";
  QStringList f_contents{ui_chatname->text(), ui_chatmessage->text()};

  AOPacket *f_packet = new AOPacket(f_header, f_contents);

  ao_app->send_ms_packet(f_packet);

  ui_chatmessage->clear();
}

void Lobby::list_servers()
{
  public_servers_selected = true;
  ui_favorites->set_image("favorites.png");
  ui_public_servers->set_image("publicservers_selected.png");

  ui_server_list->clear();

  for (server_type i_server : ao_app->get_server_list())
  {
    ui_server_list->addItem(i_server.name);
  }
}

void Lobby::list_favorites()
{
  ui_server_list->clear();

  for (server_type i_server : ao_app->get_favorite_list())
  {
    ui_server_list->addItem(i_server.name);
  }
}

void Lobby::append_chatmessage(QString f_name, QString f_message)
{
  ui_chatbox->append_chatmessage(f_name, f_message);
}

void Lobby::append_error(QString f_message)
{
  ui_chatbox->append_error(f_message);
}

void Lobby::set_choose_a_server()
{
  ui_player_count->setText(nullptr);
  ui_description->setText(tr("Choose a server."));
}

void Lobby::set_player_count(int players_online, int max_players)
{
  QString f_string = "Online: " + QString::number(players_online) + "/" + QString::number(max_players);
  ui_player_count->setText(f_string);
  ui_player_count->setAlignment(Qt::AlignHCenter);

  ui_description->setText("Connected to " + f_last_server.name + "\n\n");
  ui_description->append(f_last_server.desc);
  ui_description->ensureCursorVisible();
}
