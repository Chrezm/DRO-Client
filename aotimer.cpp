#include "aotimer.h"
#include <QDebug>

AOTimer::AOTimer(QWidget* p_parent, AOApplication *p_ao_app) : AOLabel(p_parent, p_ao_app)
{
  // Adapted from: https://stackoverflow.com/questions/36679708/how-to-make-a-chronometer-in-qt-c
  ao_app = p_ao_app;
  this->setStyleSheet("QLabel { color : white; }");
  connect(&firing_timer, SIGNAL(timeout()), this, SLOT(update_time()));

  set_time(start_time);
  old_manual_timer.set_time(start_time);
  set_timestep_length(manual_timer_timestep_length);
  redraw();
}

void AOTimer::update_time()
{
  time_spent_in_timestep = 0;
  manual_timer.perform_timestep();

  // Check for underflow/overflow

  // This one checks overflows, as the updated manual timer would appear to have a past time
  // compared to the manual timer a step ago (represented by old_manual_timer)
  if (manual_timer.get_timestep_length() > 0)
  {
      if (manual_timer.get_time().operator<(old_manual_timer.get_time()))
      {
        set_time(QTime(0, 0));
        firing_timer.stop();
        redraw();
        return;
      }
  }
  // This one checks underflows, as the updated manual timer would appear to have a future time
  // compared to the manual timer a step ago (represented by old_manual_timer)
  else if (manual_timer.get_timestep_length() < 0)
  {
      if (manual_timer.get_time().operator>(old_manual_timer.get_time()))
      {
        set_time(QTime(0, 0));
        firing_timer.stop();
        redraw();
        return;
      }
  }

  // If no overflow/underflow, now update the old manual timer so it matches the current timer
  // Redraw and return
  old_manual_timer.perform_timestep();
  redraw();
  firing_timer.start(firing_timer_length);
}

void AOTimer::set()
{
  set_time(start_time);
}

void AOTimer::resume()
{
  firing_timer.start(firing_timer_length);
}

void AOTimer::pause()
{
  firing_timer.stop();
}

void AOTimer::redraw()
{
  setText(manual_timer.get_time().toString("mm:ss.zzz"));
}

void AOTimer::set_time(QTime new_time)
{
  manual_timer.set_time(new_time);
  old_manual_timer.set_time(new_time);
  redraw();
}

void AOTimer::set_timestep_length(int new_timestep_length)
{
  manual_timer.set_timestep_length(new_timestep_length);
  old_manual_timer.set_timestep_length(new_timestep_length);
}

void AOTimer::set_firing_interval(int new_firing_interval)
{
  /*
   * Update the firing interval of the steptimer for all future timesteps. If a timestep
   * has started when this function is scheduled to run, the current timestep will be
   * readapted to finish in this new firing interval as follows:
   * - Assume length x of the 'current' timestep with orig has elapsed.
   * - Then, the current timestep will be adapted to have length max(0, new_interval-x)
   */

  // Update time spent so far and new future firing interval
  time_spent_in_timestep += (firing_timer_length - firing_timer.remainingTime());
  firing_timer_length = new_firing_interval;
  // For this timestep however, the firing interval will be shorter than firing_timer_length
  // to account for the fact the timer may have already been running a bit in this timestep
  if (new_firing_interval < time_spent_in_timestep)
    firing_timer.start(0);
  else
    firing_timer.start(new_firing_interval-time_spent_in_timestep);
}

void AOTimer::set_concentrate_mode()
{
  set_timestep_length(-12);
  set_firing_interval(120);
}

void AOTimer::set_normal_mode()
{
  set_timestep_length(-12);
  set_firing_interval(12);
}

void AOTimer::set_fast_forward_mode()
{
  set_timestep_length(-24);
  set_firing_interval(12);
}
