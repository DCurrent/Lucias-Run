#include "data/scripts/dc_hud/config.h"

// Run in the on create event. Preloads
// sprites that will be drawn to HUD.
void dc_preload_hud_sprites()
{
    // Health sprite.
    setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH, loadsprite("data/sprites/hud_status_health.png"));

    // Special (mp) sprite.
	setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL, loadsprite("data/sprites/hud_status_special.png"));

}

// Run in on destroy event. Removes HUD
// sprites loaded in the on create event.
void dc_unload_hud_sprites()
{
    // Health sprite.
    free(getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH));

    // Special (mp) sprite.
    free(getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL));
}
