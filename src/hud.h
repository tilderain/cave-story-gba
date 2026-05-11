#ifndef HUD_H
#define HUD_H

// Loads the initial HUD sprite, only stage_load() should call this
void hud_create();

void hud_force_redraw();
void hud_force_energy(); // Force redraw but only for weapon energy
// Makes the HUD visible
void hud_show();
// Hides the HUD (doesn't unload)
void hud_hide();
// Updates numbers + other info if they changed

void hud_update(void);

void hud_swap_weapon(uint8_t dir);
void hud_refresh_swap(uint8_t force);

#endif
