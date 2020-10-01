#define DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH  "dc_hud_h"
#define DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL "dc_hud_s"
#define DC_HUD_VAR_KEY_SPRITE_BACKGROUND "dc_hud_b"
#define DC_HUD_VAR_KEY_SPRITE_ICON_BORDER "dc_hud_ib"

// Return a decimal fraction of
// current HP vs. max HP.
float get_status_fraction(float status, float status_max)
{
    float current = 0.0;   // Current HP.
    float max = 0.0;       // Maximum HP.
    float result = 0.0;     // Output.

    current = 0.0 + status;
    max = 0.0 + status_max;

    // Don't divide by 0!
    if (status && status_max)
    {
        result = (0.0 + status) / (0.0 + status_max);
    }
    else
    {
        result = 0.0;
    }

    return result;
}

// Caskey, Damon V.
// 2020-09-30
// 
// Accepts base key/name of a series of sprites stored
// as localvars with an indexed suffix (ex: BASE_NAME_0, 
// BASE_NAME_1, ...) and a delay time. Returns one 
// sprite from the series starting from index 0. When 
// delay time expires, next sprite in series is returned, 
// continuing in a loop.
void dc_sprite_swap_sprite(int delay, char base_name)
{
#define DC_SPRITE_SWAP_VAR_KEY_SUFFIX "dc_swap_suf"
#define DC_SPRITE_SWAP_VAR_KEY_NEXT_TIME "dc_swap_time"

    int suffix = getlocalvar(DC_SPRITE_SWAP_VAR_KEY_SUFFIX);
    int elpased_time = openborvariant("elapsed_time");
    int time_next = getlocalvar(DC_SPRITE_SWAP_VAR_KEY_NEXT_TIME);
    void sprite;

    // Initialize suffix if it is blank.
    if (!suffix)
    {
        suffix = 0;
    }

    // Check next time. If there is a valid time, we compare it
    // to elapsed time. If the time is expired, we reset the next
    // time and increment suffix. If there's no valid next time, we 
    // initialize it with our delay time.

    if (typeof(time_next) == openborconstant("VT_INTEGER"))
    {
        if (elpased_time >= time_next)
        {
            suffix++;
            time_next = elpased_time + delay;
            setlocalvar(DC_SPRITE_SWAP_VAR_KEY_NEXT_TIME, time_next);
        }
    }
    else
    {
        time_next = elpased_time + delay;
        setlocalvar(DC_SPRITE_SWAP_VAR_KEY_NEXT_TIME, time_next);
    }    

    // Use base name and suffix to get a sprite
    // from list of loaded sprites with base name.
    //
    // If there's no sprite, we've passed the end
    // of our series of sprites, so loop back
    // to suffix 0.

    sprite = getlocalvar(base_name + "_" + suffix);

    if (!sprite)
    {
        suffix = 0;
        sprite = getlocalvar(base_name + "_" + suffix);
    }
    
    setlocalvar(DC_SPRITE_SWAP_VAR_KEY_SUFFIX, suffix);

    return sprite;

#undef DC_SPRITE_SWAP_VAR_KEY_SUFFIX
#undef DC_SPRITE_SWAP_VAR_KEY_NEXT_TIME
}


// Add a frame to animation sequence 
// identified by base name.
//
// - base_name: Identifies the animaiton (Similar to Idle, Walk, etc.).
// - Frame: Which frame to add (or replace). Frames are 0 indexed.
// - sprite_path: Path to image file.
// - delay: Time before next frame.
// - loop: When this frame completes, return to {loop} frame. -1 = no loop.
void dc_kanga_manual_animation_frame_add(char base_name, int frame, void sprite, int delay, int loop)
{
    // If sprite fails to load, let's catch the
    // error now before it becomes confusing downstream.

    if (!sprite)
    {
        shutdown(1, "\n\ndc_kanga_manual_animation_frame_add(base_name {" + base_name + "}, frame {" + frame + "}, sprite_path {" + sprite + "}, delay {" + delay + "}, loop {" + loop + "}): Invalid sprite.\n");
    }

    setlocalvar(base_name + "_sprite_" + frame, sprite);
    setlocalvar(base_name + "_loop_" + frame, loop);
    setlocalvar(base_name + "_delay_" + frame, delay);
}


// Caskey, Damon V.
//
// Return a sprite based on animation
// sequence created with series of localvars.
//
// The localvars use a base name, and store
// the sprite, delay times, etc.
//
// Use: 
//
//  1. Load sprites, using a base name + index. Ex: Setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_" + 0).
//
//  2. Use dc_kanga_manual_animation_frame_add() to add frames, using base_name to
//  identify the animation. Ex: dc_kanga_manual_animation_frame_add("hud_hp", 0, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_0"), 60, -1); 
//
//  3. Run this function using same {base_name} as animation sequence created
//  by step 2. Use {index} to uniquely identify multiple instances of same 
//  animation. 
void dc_kanga_manual_animation(char base_name, int index)
{
#define DC_KANGA_VAR_KEY_FRAME_POS "dc_kanga_fr"
#define DC_KANGA_VAR_KEY_TIME_NEXT "dc_kanga_tn"
#define DC_KANGA_VAR_KEY_LAST_DELAY "dc_kanga_ld"

    int time_elapsed = openborvariant("elapsed_time");
    int time_next = 0;
    int frame = 0;
    int delay = 0;
    void sprite = NULL();
    int loop = 0;

    // Get current frame. If frame is empty, initialize 
    // to frame 0.

    frame = getlocalvar(DC_KANGA_VAR_KEY_FRAME_POS + base_name + index);
    
    if (typeof(frame) != openborconstant("VT_INTEGER"))
    {
        frame = 0;
    }

    // Get sprite for frame. If we don't have a sprite and are
    // not at frame 0, then we are passed the last frame. We 
    // need to set our frame index back 1 (to the last fame).
    
    sprite = getlocalvar(base_name + "_sprite_" + frame);

    if (!sprite && frame > 0)
    {
        // Go back one frame and try again.

        frame--;
        sprite = getlocalvar(base_name + "_sprite_" + frame);
    }

    // Check next time. If there is a valid time, we compare it
    // to time elapsed. If the time is expired, we check for a 
    // loop or frame increment and rest next time to NULL().
    //
    // If there's no valid next time at all, we initialize it with 
    // our current frame's delay time.

    time_next = getlocalvar(DC_KANGA_VAR_KEY_TIME_NEXT + base_name + index);

    if (typeof(time_next) == openborconstant("VT_INTEGER"))
    {
        if (time_elapsed >= time_next)
        {
            // Get loop frame. If loop is disabled, increment
            // frame. Otherwise go back to frame specified
            // by loop.
            loop = getlocalvar(base_name + "_loop_" + frame);
     
            if (loop == -1)
            {
                frame++;
            }
            else
            {
                frame = loop;
            }

            // Since we are going to be on a different next
            // time function runs, we need to reset the next
            // time value. That way the next frame will detect 
            // a missing next time and assign a next time with
            // its own delay.
            time_next = NULL();
        }
    }
    else
    {        
        delay = getlocalvar(base_name + "_delay_" + frame);
        time_next = time_elapsed + delay;
    }
    
    // Record frame and time next (that we just calculated) so we 
    // have it for the next run of function.

    setlocalvar(DC_KANGA_VAR_KEY_TIME_NEXT + base_name + index, time_next);
    setlocalvar(DC_KANGA_VAR_KEY_FRAME_POS + base_name + index, frame);

    // Now we can return the sprite and exit.
    
    return sprite;
}

// Draw the player hud (life, MP).
void dc_player_hud()
{
#define HEALTH_BLOCK_MAX        5   // Maximum number of health blocks that can be displayed for a single HUD entry.
#define MAGIC_BLOCK_MAX         5   // Maximum number of magic blocks.

#define PLAYER_HUD_WIDTH        120  // Total width of each player HUD (with padding).

#define HEALTH_BLOCK_MARGIN_RIGHT   1
#define MP_BLOCK_MARGIN_RIGHT       1

#define PLAYER_HUD_HEIGHT       42
#define PLAYER_HUD_POS_X        0
#define PLAYER_HUD_POS_Y        230

#define PLAYER_HUD_HEALTH_OFFSET_X  31 
#define PLAYER_HUD_HEALTH_OFFSET_Y  8

#define PLAYER_HUD_MP_OFFSET_X  31 
#define PLAYER_HUD_MP_OFFSET_Y  18

#define PLAYER_HUD_ICON_BORDER_OFFSET_X  3 
#define PLAYER_HUD_ICON_BORDER_OFFSET_Y  5

#define PLAYER_HUD_HEALTH_ANIMATION_DELAY 20

    void    ent;                    // Target entity
    int     max_players;            // Number of available players.
    int     i;                      // Loop cursor.
    int     exists;                 // Entity exists flag.
    int     dead;                   // Entity dead flag.
    int     player_index;
    int     mp_icon_pos_x;
    int     mp_icon_pos_y;
    int     hp_icon_pos_x;
    int     hp_icon_pos_y;
    int     icon_border_pos_x;
    int     icon_border_pos_y;
    int     status_current;
    int     status_max;

    int mp_icon_pos_x_loop;
    int hp_icon_pos_x_loop;

    void hp_sprite = dc_kanga_manual_animation("hud_hp", 0);
    void hp_sprite_empty = getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_0");

    void mp_sprite = dc_kanga_manual_animation("hud_mp", 0);
    void hud_back_sprite = getlocalvar(DC_HUD_VAR_KEY_SPRITE_BACKGROUND);
    void icon_border_sprite = getlocalvar(DC_HUD_VAR_KEY_SPRITE_ICON_BORDER);

    // Draw HUD background.
    //drawsprite(hud_back_sprite, PLAYER_HUD_POS_X, PLAYER_HUD_POS_Y, openborconstant("FRONTPANEL_Z") + 1000);

    // Get and loop through player collection.
    max_players = openborvariant("maxplayers");

    // Get base X and Y position of icon. Start at the
    // top left of HUD, and add offsets.

    mp_icon_pos_x = PLAYER_HUD_POS_X + PLAYER_HUD_MP_OFFSET_X;
    mp_icon_pos_y = PLAYER_HUD_POS_Y + PLAYER_HUD_MP_OFFSET_Y;

    hp_icon_pos_x = PLAYER_HUD_POS_X + PLAYER_HUD_HEALTH_OFFSET_X;
    hp_icon_pos_y = PLAYER_HUD_POS_Y + PLAYER_HUD_HEALTH_OFFSET_Y;

    icon_border_pos_x = PLAYER_HUD_POS_X + PLAYER_HUD_ICON_BORDER_OFFSET_X;
    icon_border_pos_y = PLAYER_HUD_POS_Y + PLAYER_HUD_ICON_BORDER_OFFSET_Y;

    for (player_index = 0; player_index < max_players; player_index++)
    {
        // Get target entity for this loop increment, then
        // make sure it is still valis and exists.

        ent = getplayerproperty(player_index, "entity");

        if (!ent)
        {
            continue;
        }

        exists = getentityproperty(ent, "exists");

        if (!exists)
        {
            continue;
        }

        // We're leaving dead entities on the screen but
        // don't want to draw their HUD any more. For
        // this purpose the dead flag will work well
        // as a filter.
        dead = getentityproperty(ent, "dead");

        if (dead)
        {
            continue;
        }

        // Mutiply cursor by width of HUD to
        // move our base X position to each
        // respective player's HUD.

        mp_icon_pos_x_loop = mp_icon_pos_x + player_index * PLAYER_HUD_WIDTH;
        hp_icon_pos_x_loop = hp_icon_pos_x + player_index * PLAYER_HUD_WIDTH;        
        
        // Draw HP status.

        status_current = get_entity_property(ent, "hp_old");
        status_max = getentityproperty(ent, "maxhealth");

        dc_draw_status_icons(status_current, status_max, hp_sprite, hp_sprite_empty, HEALTH_BLOCK_MAX, hp_icon_pos_x_loop, hp_icon_pos_y, HEALTH_BLOCK_MARGIN_RIGHT);

        // Draw MP status.

        status_current = get_entity_property(ent, "mp_old");
        status_max = getentityproperty(ent, "maxmp");

        dc_draw_status_icons(status_current, status_max, mp_sprite, mp_sprite, MAGIC_BLOCK_MAX, mp_icon_pos_x_loop, mp_icon_pos_y, MP_BLOCK_MARGIN_RIGHT);
    }

#undef HEALTH_BLOCK_MAX
#undef MAGIC_BLOCK_MAX

#undef PLAYER_HUD_WIDTH
#undef MP_AREA_MARGIN_LEFT
#undef MP_BLOCK_MARGIN_LEFT
#undef MP_BLOCK_MARGIN_RIGHT
}

// Accept properties from HUD control code 
// icons to build a status meter.
//
// status_current: Current HP/MP/etc..
// status_max: Maximum HP/MP/etc..
// icon_sprite: Icon used as a unit of measure.
// icon_sprite_empty: Icon used when unit of measure is empty.
// icon_count: Number of units maximum status divides into and shown to screen. 
// icon_pos_x: Starting horizontal position of meter.
// icon_pos_y: Starting vertical position of meter.
// margin: Blank area around each icon.

void dc_draw_status_icons(int status_current, int status_max, void icon_sprite, void icon_sprite_empty, int icon_count, int icon_pos_x, int icon_pos_y, int margin)
{
#define DC_DRAW_STATUS_ICON_SIZE_X 9 // Hard coded because we don't get a reliable graphic size when switching sprites.

    int i;
    float status_fraction = 0.0;
    float status_fraction_last = 0.0;
    int icon_space_x = 0;
    int icon_size_x = 0;
    int icon_size_y = 0;
    int icon_pos_x_loop = 0;
    int sprite_to_draw = 0;

    int drawmethod_enabled = 0;
    int drawmethod_scale_x = 0;
    int drawmethod_scale_y = 0;
    int drawmethod_tint_mode = 0;
    int drawmethod_tint_color = 0;
    int drawmethod_fill_color = 0;

    int rgb_base;   // Temp RGB value after truncating decimal.

    // Get status %, multiplied by number of displayable icons.

    status_fraction = 0.0 + icon_count * get_status_fraction(status_current, status_max);
        
    // We need to know how wide the sprite is, and
    // add in margins for total width.

    icon_size_x = DC_DRAW_STATUS_ICON_SIZE_X; // getgfxproperty(icon_sprite, "width");
    icon_size_y = getgfxproperty(icon_sprite, "srcheight");    
    icon_space_x = icon_size_x + margin;

    // Loop each fraction of status.
    for (i = 0; i < icon_count; i++)
    {
        status_fraction_last = status_fraction - i;

        // Multiply the total X space of icon by
        // current cursor positon. This places
        // each block in a row, left to right.

        icon_pos_x_loop = icon_pos_x + i * icon_space_x;

        // Get current drawmethods so we can restore them.
                        
        drawmethod_enabled = getdrawmethod(NULL(), "enabled");
        drawmethod_scale_x = getdrawmethod(NULL(), "scalex");
        drawmethod_scale_y = getdrawmethod(NULL(), "scaley");
        drawmethod_tint_mode = getdrawmethod(NULL(), "tintmode");
        drawmethod_tint_color = getdrawmethod(NULL(), "tintcolor");
        drawmethod_fill_color = getdrawmethod(NULL(), "fillcolor");  

        // Reset the drawmethod.

        changedrawmethod(NULL(), "reset", 1);

        // Draw icon according to the fraction remaining.
        // This will always be 100% until we are at last 
        // division of status_fraction.
                
        if (status_fraction_last >= 1)
        {
            // This icon represents a "full" section
            // of status_fraction. We'll draw it as is.

            changedrawmethod(NULL(), "enabled", 0);

            sprite_to_draw = icon_sprite;

        }
        else if (status_fraction_last < 1 && status_fraction_last >= 0)
        {
            changedrawmethod(NULL(), "enabled", 1);

            rgb_base = trunc(status_fraction_last * 255);

            changedrawmethod(NULL(), "tintmode", 2);
            changedrawmethod(NULL(), "tintcolor", rgbcolor(rgb_base, rgb_base, rgb_base));    

            sprite_to_draw = icon_sprite;
        }
        else
        {
            // This section and the icon it reprsents is
            // completly empty. Draw it with a fillcolor.

            changedrawmethod(NULL(), "enabled", 1);
            changedrawmethod(NULL(), "fillcolor", rgbcolor(1,1,1));

            sprite_to_draw = icon_sprite_empty;
        }

        drawsprite(sprite_to_draw, icon_pos_x_loop, icon_pos_y, openborconstant("FRONTPANEL_Z") + 18001);

        // Restore drawmethods to previous state.       
        changedrawmethod(NULL(), "enabled", drawmethod_enabled);
        changedrawmethod(NULL(), "scalex", drawmethod_scale_x);
        changedrawmethod(NULL(), "scaley", drawmethod_scale_y);
        changedrawmethod(NULL(), "tintmode", drawmethod_tint_mode);
        changedrawmethod(NULL(), "tintcolor", drawmethod_tint_color);
        changedrawmethod(NULL(), "fillcolor", drawmethod_fill_color);
    }

#undef DC_DRAW_STATUS_ICON_SIZE_X
}

// Caskey, Damon V.
// 2020-09-30
//
// Delete loaded sprites and localvar references 
// of {base_group}.
void dc_kanga_free_sprite_group(char base_name)
{
    int i = 0;
    void sprite;
    char full_name;

    do
    {
        full_name = base_name + "_" + i;

        sprite = getlocalvar(full_name);

        if (sprite)
        {
            free(sprite);
            setlocalvar(full_name, NULL());
        }        

        i++;

    } while(sprite);
}

// Run in the on create event. Preloads
// sprites that will be drawn to HUD.
void dc_preload_hud_sprites()
{
    // Health sprites.
    setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_" + 0, loadsprite("data/sprites/heart_0.png"));
    setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_" + 1, loadsprite("data/sprites/heart_1.png"));
    setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_" + 2, loadsprite("data/sprites/heart_2.png"));

    // Special (mp) sprite.
    setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_" + 0, loadsprite("data/sprites/star_0.png"));
    setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_" + 1, loadsprite("data/sprites/star_1.png"));
    setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_" + 2, loadsprite("data/sprites/star_2.png"));
    setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_" + 3, loadsprite("data/sprites/star_3.png"));
    setlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_" + 4, loadsprite("data/sprites/star_4.png"));

    // Add health sprites to animation sequence.
    dc_kanga_manual_animation_frame_add("hud_hp", 0, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_0"), 60, -1);
    dc_kanga_manual_animation_frame_add("hud_hp", 1, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_1"), 30, -1);
    dc_kanga_manual_animation_frame_add("hud_hp", 2, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_2"), 30, -1);
    dc_kanga_manual_animation_frame_add("hud_hp", 3, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH + "_1"), 30, 0);

    // Add special sprites to animation sequence.
    dc_kanga_manual_animation_frame_add("hud_mp", 0, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_4"), 20, -1);
    dc_kanga_manual_animation_frame_add("hud_mp", 1, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_3"), 20, -1);
    dc_kanga_manual_animation_frame_add("hud_mp", 2, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_2"), 20, -1);
    dc_kanga_manual_animation_frame_add("hud_mp", 3, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_1"), 20, -1);
    dc_kanga_manual_animation_frame_add("hud_mp", 4, getlocalvar(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL + "_0"), 20, 0);
}

// Run in on destroy event. Removes HUD
// sprites loaded in the on create event.
void dc_unload_hud_sprites()
{   
    dc_kanga_free_sprite_group(DC_HUD_VAR_KEY_SPRITE_STATUS_HEALTH);
    dc_kanga_free_sprite_group(DC_HUD_VAR_KEY_SPRITE_STATUS_SPECIAL);
}
