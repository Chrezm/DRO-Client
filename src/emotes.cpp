#include "courtroom.h"

#include "aoemotebutton.h"

#include <QDebug>

void Courtroom::construct_emotes()
{
  ui_emotes = new QWidget(this);

  ui_emote_left = new AOButton(this, ao_app);
  ui_emote_right = new AOButton(this, ao_app);

  ui_emote_dropdown = new QComboBox(this);
  ui_pos_dropdown = new QComboBox(this);
  ui_pos_dropdown->addItem("wit", "wit");
  ui_pos_dropdown->addItem("def", "def");
  ui_pos_dropdown->addItem("pro", "pro");
  ui_pos_dropdown->addItem("jud", "jud");
  ui_pos_dropdown->addItem("hld", "hld");
  ui_pos_dropdown->addItem("hlp", "hlp");

  reconstruct_emotes();
}

void Courtroom::reconstruct_emotes()
{
  // delete previous buttons
  while (!ui_emote_list.isEmpty())
    delete ui_emote_list.takeLast();

  // resize and move
  set_size_and_pos(ui_emotes, "emotes");

  QPoint f_spacing = ao_app->get_button_spacing("emote_button_spacing", "courtroom_design.ini");

  const int button_width = 40;
  int x_spacing = f_spacing.x();
  int x_mod_count = 0;

  const int button_height = 40;
  int y_spacing = f_spacing.y();
  int y_mod_count = 0;

  emote_columns = ((ui_emotes->width() - button_width) / (x_spacing + button_width)) + 1;
  emote_rows = ((ui_emotes->height() - button_height) / (y_spacing + button_height)) + 1;

  max_emotes_on_page = emote_columns * emote_rows;

  for (int n = 0; n < max_emotes_on_page; ++n)
  {
    int x_pos = (button_width + x_spacing) * x_mod_count;
    int y_pos = (button_height + y_spacing) * y_mod_count;

    AOEmoteButton *f_emote = new AOEmoteButton(ui_emotes, ao_app, x_pos, y_pos);

    ui_emote_list.append(f_emote);

    f_emote->set_emote_number(n);

    connect(f_emote, SIGNAL(emote_clicked(int)), this, SLOT(on_emote_clicked(int)));

    ++x_mod_count;

    if (x_mod_count == emote_columns)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }

  reset_emote_page();
}

void Courtroom::reset_emote_page()
{
  m_emote_page = 0;
  m_emote_id = 0;

  if (is_spectating())
    ui_emotes->hide();
  else
    ui_emotes->show();

  set_emote_page();
  set_emote_dropdown();
}

void Courtroom::set_emote_page()
{
  ui_emote_left->hide();
  ui_emote_right->hide();

  if (is_spectating())
    return;

  const int total_emotes = m_emote_list.length();
  for (AOEmoteButton *i_button : qAsConst(ui_emote_list))
    i_button->hide();

  int total_pages = total_emotes / max_emotes_on_page;
  int emotes_on_page = 0;

  if (total_emotes % max_emotes_on_page != 0)
  {
    ++total_pages;
    // i. e. not on the last page
    if (total_pages > m_emote_page + 1)
      emotes_on_page = max_emotes_on_page;
    else
      emotes_on_page = total_emotes % max_emotes_on_page;
  }
  else
    emotes_on_page = max_emotes_on_page;

  if (total_pages > m_emote_page + 1)
    ui_emote_right->show();

  if (m_emote_page > 0)
    ui_emote_left->show();

  for (int n_emote = 0; n_emote < emotes_on_page; ++n_emote)
  {
    const int l_real_id = n_emote + m_emote_page * max_emotes_on_page;
    AOEmoteButton *l_button = ui_emote_list.at(n_emote);
    l_button->set_image(get_emote(l_real_id), l_real_id == m_emote_id);
    l_button->show();
  }
}

void Courtroom::set_emote_dropdown()
{
  QSignalBlocker l_blocker(ui_emote_dropdown);
  ui_emote_dropdown->clear();

  QStringList l_emote_list;
  for (const DREmote &i_emote : m_emote_list)
    l_emote_list.append(i_emote.comment);
  ui_emote_dropdown->addItems(l_emote_list);
}

DREmote Courtroom::get_emote(const int id)
{
  if (id < 0 || id >= m_emote_list.length())
    return DREmote();
  return m_emote_list.at(id);
}

DREmote Courtroom::get_current_emote()
{
  return get_emote(m_emote_id);
}

void Courtroom::select_emote(int p_id)
{
  const int l_min = m_emote_page * max_emotes_on_page;
  const int l_max = (max_emotes_on_page - 1) + m_emote_page * max_emotes_on_page;

  const DREmote &l_prev_emote = get_emote(m_emote_id);
  if (m_emote_id >= l_min && m_emote_id <= l_max)
    ui_emote_list.at(m_emote_id % max_emotes_on_page)->set_image(l_prev_emote, false);

  const int l_prev_emote_id = m_emote_id;
  m_emote_id = p_id;
  const DREmote &l_emote = get_emote(m_emote_id);

  if (m_emote_id >= l_min && m_emote_id <= l_max)
    ui_emote_list.at(m_emote_id % max_emotes_on_page)->set_image(l_emote, true);

  const int emote_mod = l_emote.modifier;
  if (l_prev_emote_id == m_emote_id) // toggle
    ui_pre->setChecked(!ui_pre->isChecked());
  else
    ui_pre->setChecked(emote_mod == 1 || ao_config->always_pre_enabled());

  select_default_sfx();

  ui_emote_dropdown->setCurrentIndex(m_emote_id);

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_emote_clicked(int p_id)
{
  select_emote(p_id + max_emotes_on_page * m_emote_page);
}

void Courtroom::on_emote_left_clicked()
{
  --m_emote_page;

  set_emote_page();

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_emote_right_clicked()
{
  ++m_emote_page;
  
  set_emote_page();

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_emote_dropdown_changed(int p_index)
{
  select_emote(p_index);
}
