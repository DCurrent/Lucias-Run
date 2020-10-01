#include "data/scripts/dc_hud/config.h"

#import "data/scripts/dc_hud/block-sprites.c"
#import "data/scripts/dc_hud/health-fraction.c"

// Draw player HUD, with icons, magic jars, and
// life blocks for the target entity.
void dc_player_hud()
{
    #define HEALTH_BLOCK_MAX        5   // Maximum number of health blocks that can be displayed for a single HUD entry.
    #define MAGIC_BLOCK_MAX         5   // Maximum number of magic blocks.

    #define PLAYER_HUD_WIDTH        80  // Total width of each player HUD (with padding).
    //#define MP_AREA_MARGIN_LEFT     33  // Left of player HUD to first status icon.
    #define MP_BLOCK_MARGIN_LEFT    0
    #define MP_BLOCK_MARGIN_RIGHT   1

    #define PLAYER_HUD_HEIGHT       32
    #define PLAYER_HUD_POS_Y        208

    #define PLAYER_HUD_HEALTH_OFFSET_X  33 
    #define PLAYER_HUD_HEALTH_OFFSET_Y  7

    #define PLAYER_HUD_MP_OFFSET_X  33 
    #define PLAYER_HUD_MP_OFFSET_Y  14
    

    void    ent;                    // Target entity
    int     max_players;            // Number of available players.
    int     i;                      // Loop cursor.
    int     exists;                 // Entity exists flag.
    int     dead;                   // Entity dead flag.
    int     resolution_y;           // Screens vertical size in pixels.
    int     player_index;
    int     magic_count;            // How many symbols (jars, blocks, etc.) of magic to display.
    int     sprite;           // Placeholder for sprite reference.
    float   health_fraction;        // A sub-division of current health percentage.
    float   block_fraction;         //
    int     block_size_h;           // Size of mp block 9after OpenBOR auto trims).
    int     block_space;            // Size of mp block with margins included.
    int     block_position_left;    // Starting position of mp blocks in each player's HUD.
    int     block_position_x;       // X position of an individual mp block.
    int     block_position_Y;       // Y position of an individual mp block.

    // Get and loop through player collection.
    max_players = openborvariant("maxplayers");

    for(player_index=0; player_index < max_players; player_index++)
    {
        // Get target entity for this loop increment.
        ent = getplayerproperty(player_index, "entity");

        // Make sure we got a valid target pointer.
        if(!target)
        {
            continue;
        }

        // Make sure the entity exists in play. We perform this
        // check because it's possible for an entity to be
        // removed but its pointer is still valid.
        exists  = getentityproperty(ent, "exists");

        if(!exists)
        {
            continue;
        }

        // We're leaving dead entities on the screen but
        // don't want to draw their HUD any more. For
        // this purpose the dead flag will work well
        // as a filter.
        dead = getentityproperty(ent, "dead");

        if(dead)
        {
            continue;
        }

        resolution_y    = openborvariant("vresolution");
        magic_count     = getentityproperty(ent, "mp") / MAGIC_BLOCK_MAX;
        sprite    = getlocalvar(VAR_KEY_SPRITE_MAGIC_JAR);

        // Magic meter.

        // We're drawing MP blocks in a row, so Y position
        // is always the same.

        block_position_Y = PLAYER_HUD_POS_Y + PLAYER_HUD_MP_OFFSET_Y;

        // X position will depend on several factors. Some
        // we can do here, and the rest will need to be
        // in a loop.
        //
        // Let's get the width of our meter block.
        // Remember that OpenBOR auto trims all sprites
        // as it loads them, so the size will reflect that.

        block_size_h = getgfxproperty(sprite, "width");

        // Now we add the block's margins, and that will get
        // total spacing for one block.

        block_space = block_size_h + MP_BLOCK_MARGIN_LEFT + MP_BLOCK_MARGIN_RIGHT;

        // Our starting position will be the leftmost of
        // current player's (in loop) HUD area. To get this,
        // we multiply current player index by total X size of
        // the player HUD.

        block_position_left = player_index * PLAYER_HUD_WIDTH;

        // Add the meter area's left margin to space it
        // out from the start of player HUD area.

        block_position_left += PLAYER_HUD_MP_OFFSET_X;

        for(i=0; i<magic_count; i++)
        {
            // Multiply the total X space of a block
            // by the current cursor position. This places
            // each block in a row, left to right.
            block_position_x = i * block_space;

            // Now add the margin from player HUD to start
            // of block area to align blocks properly with
            // player HUD design.
            block_position_x += block_position_left;

            // Draw the MP sprite.
            drawsprite(sprite, block_position_x, block_position_Y, openborconstant("FRONTPANEL_Z")+18001);
        }

        
    }

    #undef HEALTH_BLOCK_MAX
    #undef MAGIC_BLOCK_MAX

    #undef PLAYER_HUD_WIDTH
    #undef MP_AREA_MARGIN_LEFT
    #undef MP_BLOCK_MARGIN_LEFT
    #undef MP_BLOCK_MARGIN_RIGHT
}

