#include "courtroom.h"

bool Courtroom::is_audio_muted()
{
    return m_audio_mute;
}

void Courtroom::set_audio_mute_enabled(bool p_enabled)
{
    if (m_audio_mute == p_enabled)
        return;
    m_audio_mute = p_enabled;

    // restore volume
    set_effects_volume(ao_config->effects_volume());
    set_music_volume(ao_config->music_volume());
    set_blips_volume(ao_config->blips_volume());
}

void Courtroom::set_effects_volume(int p_volume)
{
    m_effects_player->set_volume(is_audio_muted() ? 0 : p_volume);
    m_shouts_player->set_volume(is_audio_muted() ? 0 : p_volume);
}

void Courtroom::set_system_volume(int p_volume)
{
    m_system_player->set_volume(p_volume);
}

void Courtroom::set_music_volume(int p_volume)
{
    m_music_player->set_volume(is_audio_muted() ? 0 : p_volume);
}

void Courtroom::set_blips_volume(int p_volume)
{
    m_blips_player->set_volume(is_audio_muted() ? 0 : p_volume);
}

void Courtroom::on_config_effects_volume_changed(int p_volume)
{
    set_effects_volume(p_volume);
}

void Courtroom::on_config_system_volume_changed(int p_volume)
{
    set_system_volume(p_volume);
}

void Courtroom::on_config_music_volume_changed(int p_volume)
{
    set_music_volume(p_volume);
}

void Courtroom::on_config_blips_volume_changed(int p_volume)
{
    set_blips_volume(p_volume);
}
