#include "aonotepicker.h"

#include "courtroom.h"
#include "debug_functions.h"

#include <QDebug>
#include <QFileDialog>

AONotePicker::AONotePicker(QWidget *p_parent, AOApplication *p_ao_app)
    : QLabel(p_parent)
{
  ao_app = p_ao_app;
}

void Courtroom::on_file_selected()
{
  for (int i = 0; i < ui_note_area->m_layout->count() - 1; ++i)
  {
    AONotePicker *f_notepicker = static_cast<AONotePicker *>(
        ui_note_area->m_layout->itemAt(i)->widget());
    f_notepicker->m_hover->set_image("note_select.png");
  }

  AOButton *f_button = static_cast<AOButton *>(sender());
  AONotePicker *f_notepicker = static_cast<AONotePicker *>(f_button->parent());

  if (f_notepicker->real_file.isEmpty())
  {
    call_notice("You must give a filepath to load a note from!");
    return;
  }

  if (current_file != f_notepicker->real_file)
  {
    current_file = f_notepicker->real_file;
    load_note();
    f_notepicker->m_hover->set_image("note_select_selected.png");
  }
  else
  {
    current_file = "";
  }
}

void Courtroom::on_set_file_button_clicked()
{
  AOButton *f_button = static_cast<AOButton *>(sender());
  AONotePicker *f_notepicker = static_cast<AONotePicker *>(f_button->parent());
  QString f_filename = QFileDialog::getOpenFileName(
      this, "Open File", QDir::currentPath(), "Text files (*.txt)");

  if (f_filename != "")
  {
    f_notepicker->m_line->setText(f_filename);

    // If this notepicker is the currently selected slot, update the notepad as
    // the file given changes.
    if (f_notepicker->real_file == current_file)
    {
      current_file = f_filename;
      load_note();
    }

    f_notepicker->real_file = f_filename;

    set_note_files();
  }
}

void Courtroom::on_delete_button_clicked()
{
  AOButton *f_button = static_cast<AOButton *>(sender());
  AONotePicker *f_notepicker = static_cast<AONotePicker *>(f_button->parent());

  if (current_file == f_notepicker->real_file)
  {
    current_file = "";
  }

  ui_note_area->m_layout->removeWidget(f_notepicker);
  delete f_notepicker;
  set_note_files();
}
