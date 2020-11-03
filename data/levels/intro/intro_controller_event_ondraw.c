#include "data/scripts/dc_hud/main.c"
#include "data/scripts/dc_math/main.c"

#ifndef DC_INSET

#define DC_INSET    1

#define KEY_SCREEN          "key_scr"

#define DC_INTRO_GROUND_INSTANCE "dcdraw_inst"
#define DC_INTRO_GROUND_SPRITE_QUE_Z_MAX "dcdraw_q_z_min"
#define DC_INTRO_GROUND_SPRITE_QUE_Z_MIN "dcdraw_q_z_max"
#define DC_INTRO_GROUND_SPRITE_QUE_NEWONLY "dcdraw_q_new"
#define DC_INTRO_GROUND_SPRITE_OFFSET_X "dcdraw_q_ox"
#define DC_INTRO_GROUND_SPRITE_OFFSET_Y "dcdraw_q_oy"

#define DC_DRAW_GROUND_SCREEN_POS_X_START "dcdraw_scr_px"
#define DC_DRAW_GROUND_SCREEN_POS_Y_START "dcdraw_scr_py"
#define DC_DRAW_GROUND_SCREEN_POS_Z_START "dcdraw_scr_pz"
#define DC_DRAW_GROUND_SCREEN_SIZE_X "dcdraw_scr_sx"
#define DC_DRAW_GROUND_SCREEN_SIZE_Y "dcdraw_scr_sy"
#define DC_DRAW_GROUND_SCREEN_COUNT "dcdraw_scr_cnt"

#define DC_DRAW_GROUND_DM_WATERMODE "dcdraw_dm_wm"
#define DC_DRAW_GROUND_DM_PERSPECTIVE "dcdraw_dm_per"
#define DC_DRAW_GROUND_DM_TINTMODE "dcdraw_dm_tm"
#define DC_DRAW_GROUND_DM_TINTCOLOR "dcdraw_dm_tc"
#define DC_DRAW_GROUND_DM_ALPHA "dcdraw_dm_am"

#define DC_DRAW_GROUND_LAYER_INDEX_START "dcdraw_lay_i"

#define DC_DRAW_GROUND_LAYER_FLOOR 3
#define DC_DRAW_GROUND_LAYER_HORIZON_POS_Y 80	// Start of "ground" draw.
#define DC_DRAW_GROUND_LAYER_FLOOR_SIZE_Y 170	// Vertical size of ground draw.

#endif // DC_INSET

// Caskey, Damon V.
// 2020-09-~~
//
// Get or allocate screen identified by {index}.
void get_screen(char index, int size_x, int size_y)
{
	void screen;
	int  x;
	int  y;

	// Get current screen.
	screen = getlocalvar(KEY_SCREEN+index);

	// If no screen is set up,
	// initialize it here.
	if (!screen)
	{
		// Allocate screen and use it to populate
		// the screen variable, then populate
		// background variable.
		screen = allocscreen(size_x, size_y);
		setlocalvar(KEY_SCREEN+index, screen);
	}

	return screen;
}

void dc_draw_ground_multi_layer()
{	
	int instance = getlocalvar(DC_INTRO_GROUND_INSTANCE);

	int sprite_que_z_min = getlocalvar(DC_INTRO_GROUND_SPRITE_QUE_Z_MAX);
	int sprite_que_z_max = getlocalvar(DC_INTRO_GROUND_SPRITE_QUE_Z_MIN);
	int sprite_que_new = getlocalvar(DC_INTRO_GROUND_SPRITE_QUE_NEWONLY);
	int sprite_offset_x = getlocalvar(DC_INTRO_GROUND_SPRITE_OFFSET_X);
	int sprite_offset_y = getlocalvar(DC_INTRO_GROUND_SPRITE_OFFSET_Y);

	int screen_pos_x = getlocalvar(DC_DRAW_GROUND_SCREEN_POS_X_START);
	int screen_pos_y = getlocalvar(DC_DRAW_GROUND_SCREEN_POS_Y_START);
	int screen_pos_z = getlocalvar(DC_DRAW_GROUND_SCREEN_POS_Z_START);
	int screen_size_x = getlocalvar(DC_DRAW_GROUND_SCREEN_SIZE_X);
	int screen_size_y = getlocalvar(DC_DRAW_GROUND_SCREEN_SIZE_Y);
	int screen_count = getlocalvar(DC_DRAW_GROUND_SCREEN_COUNT);

	int dm_watermode = getlocalvar(DC_DRAW_GROUND_DM_WATERMODE);
	int dm_perspective = getlocalvar(DC_DRAW_GROUND_DM_PERSPECTIVE);
	int dm_tintmode = getlocalvar(DC_DRAW_GROUND_DM_TINTMODE);
	int dm_tintcolor = getlocalvar(DC_DRAW_GROUND_DM_TINTCOLOR);
	int dm_alpha = getlocalvar(DC_DRAW_GROUND_DM_ALPHA);

	int layer_index_start = getlocalvar(DC_DRAW_GROUND_LAYER_INDEX_START);

	float affine_size_near = dc_perspective_get_size_near();
	float affine_size_far = dc_perspective_get_size_far();

	changelayerproperty("fglayer", layer_index_start, "z", sprite_que_z_min);

	int i = 0; // cursor
	void screen;

	// Get the drawmethods we're going to use so we can
	// restore them when we're done.

	int drawmethod_enabled = getdrawmethod(NULL(), "enabled");
	int drawmethod_tintcolor = getdrawmethod(NULL(), "tintcolor");
	int drawmethod_alpha = getdrawmethod(NULL(), "alpha");
	int drawmethod_watermode = getdrawmethod(NULL(), "watermode");
	int drawmethod_perspective = getdrawmethod(NULL(), "perspective");
	int drawmethod_tintmode = getdrawmethod(NULL(), "tintmode");
	int drawmethod_beginsize = getdrawmethod(NULL(), "beginsize");
	int drawmethod_endsize = getdrawmethod(NULL(), "endsize");
	int drawmethod_shift_x = getdrawmethod(NULL(), "shiftx");

	changedrawmethod(NULL(), "reset", 1);
	changedrawmethod(NULL(), "enabled", 1);
	changedrawmethod(NULL(), "tintcolor", dm_tintcolor);
	changedrawmethod(NULL(), "alpha", dm_alpha);
	changedrawmethod(NULL(), "watermode", dm_watermode);
	changedrawmethod(NULL(), "perspective", dm_perspective);
	changedrawmethod(NULL(), "tintmode", dm_tintmode);
	//changedrawmethod(NULL(), "shiftx", -256);
	
	float affine_size_increment = (affine_size_near - affine_size_far) / screen_count;
	float affine_size_current = affine_size_far;
	
	int screen_position_y = screen_pos_y;
	int screen_offset_y = sprite_offset_y;

	for (i = 0; i < screen_count; i++)
	{
		//changedrawmethod(NULL(), "tintmode", 1);

		//if (i % 2 == 0)
		//{
		//	changedrawmethod(NULL(), "tintcolor", rgbcolor(255, 0, 0));
		//}
		//else
		//{
		//	changedrawmethod(NULL(), "tintcolor", rgbcolor(0, 0, 255));
		//}

		screen = get_screen("multi_" + instance + i, screen_size_x, screen_size_y);
		
		clearscreen(screen);
		drawspriteq(screen, sprite_que_new, sprite_que_z_min, sprite_que_z_max, sprite_offset_x, screen_offset_y);
				
		// Use previous (or starting) size for beginsize, and 
		// incremented size for endside.

		changedrawmethod(NULL(), "beginsize", affine_size_current);
		
		affine_size_current += affine_size_increment;

		changedrawmethod(NULL(), "endsize", affine_size_current);

		// Draw the screen.
		drawscreen(screen, screen_pos_x, screen_position_y, screen_pos_z);

		// Increment sizes and position for next cycle.
		screen_position_y += screen_size_y;
	}

	// Restore the previous drawmethods.
	changedrawmethod(NULL(), "enabled", drawmethod_enabled);
	changedrawmethod(NULL(), "tintcolor", drawmethod_tintcolor);
	changedrawmethod(NULL(), "alpha", drawmethod_alpha);
	changedrawmethod(NULL(), "watermode", drawmethod_watermode);
	changedrawmethod(NULL(), "perspective", drawmethod_perspective);
	changedrawmethod(NULL(), "tintmode", drawmethod_tintmode);
	//changedrawmethod(NULL(), "beginsize", drawmethod_beginsize);
	//changedrawmethod(NULL(), "endsize", drawmethod_endsize);
	changedrawmethod(NULL(), "shiftx", drawmethod_shift_x);
}

void dc_debug_layers()
{
#define DC_LAYER_DEBUG_TEXT_POS_START_X 10
#define DC_LAYER_DEBUG_TEXT_POS_START_Y 10
#define DC_LAYER_DEBUG_FONT_SIZE_Y 10

	int i;
	int layer_count = get_level_property(NULL(), openborconstant("LEVEL_PROP_LAYER_FOREGROUND_COUNT"));
	int text_pos_x = DC_LAYER_DEBUG_TEXT_POS_START_X;
	int text_pos_y = DC_LAYER_DEBUG_TEXT_POS_START_Y;
	int text_font = 1;
	char text_string = "";

	//text_string = "layer_count: " + layer_count;
	//settextobj(text_index, DC_LAYER_DEBUG_TEXT_POS_START_X, text_pos_y, text_font, openborconstant("FRONTPANEL_Z"), text_string);

	for (i = 0; i < layer_count; i++)
	{
		text_pos_x = DC_LAYER_DEBUG_TEXT_POS_START_X;
		text_pos_y = DC_LAYER_DEBUG_TEXT_POS_START_Y + DC_LAYER_DEBUG_FONT_SIZE_Y * i;
		
		//text_font = 0;
		//text_string = "Layer " + i + ": ";

		//settextobj(0 + i, text_pos_x, text_pos_y, text_font, openborconstant("FRONTPANEL_Z"), text_string);

		// Value goes after label on X plane, so add width of label to pos_x.
		//text_pos_x += strwidth(text_string, text_font);
		text_font = 1;
		text_string = "Z: " + getlayerproperty("fglayer", i, "z");

		settextobj(i, text_pos_x, text_pos_y, text_font, openborconstant("FRONTPANEL_Z"), text_string);
	}

#undef DC_LAYER_DEBUG_TEXT_POS_START_X
#undef DC_LAYER_DEBUG_TEXT_POS_START_Y
#undef DC_LAYER_DEBUG_FONT_SIZE_Y
}

void clear()
{
	void scr = getindexedvar(0);
	if (scr) {
		free(scr);
		free(getscriptvar(1));
	}
}

float dc_math_bezier_curve(float tangent, float point_a, float point_b, float point_c)
{
	float result = 0.0;

	result = pow(1 - tangent, 2) * point_a + 2 * (1 - tangent) * tangent * point_b + pow(tangent, 2) * point_c;

	return result;
}

void dc_math_bezier_curve_test(float point_a, float point_b, float point_c)
{

	float increment = 0.01;
	float pos_y_start = 70;
	float vertical_range = 160;

	int pos_x = 0;
	int pos_y = pos_y_start;
	int pos_z = 1000;	

	float i = 0.0;	// Loop cursor (also used as tangent);
	float point;	// Point along bezier curve.

	// Get size of Y increment.
	float y_increment = vertical_range / ((1 - increment) * 100);

	//log("\n -- ");

	for (i = 0.0; i < 1.0, i += 0.01)
	{
		point = dc_math_bezier_curve(i, point_a, point_b, point_c);

		//log("\n Point: " + point);
		//log("\n pos_y: " + pos_y);

		pos_x = trunc(point);

		drawdot(pos_x, pos_y, pos_z, rgbcolor(255, 0, 0), 0);

		pos_y += y_increment;
	}
}

void dc_scroll_control(void ent, float speed, int z_min, int z_max)
{
	if (getentityproperty(ent, "type") != openborconstant("TYPE_OBSTACLE"))
	{
		return;
	}

	float autoscroll_y = get_level_property(NULL(), openborconstant("LEVEL_PROP_AUTO_SCROLL_Y"));
	
	float pos_z = get_entity_property(ent, "position_z");
	float pos_x = get_entity_property(ent, "position_x");

	if (pos_z > (z_max + 200) || pos_z < (z_min - 200))
	{
		killentity(ent);
		return;
	}

	int layer_index = DC_DRAW_GROUND_LAYER_FLOOR;

	float autoscroll_y = get_level_property(NULL(), openborconstant("LEVEL_PROP_AUTO_SCROLL_Y"));

	
	float layer_autoscroll_ratio = getlayerproperty("fglayer", layer_index, "bgspeedratio");
	float layer_z_scroll_ratio = getlayerproperty("fglayer", layer_index, "zratio");
	                    
			
	//
	settextobj(1, 10, 30, 0, openborconstant("FRONTPANEL_Z"), "autoscroll_y: " + autoscroll_y);
	settextobj(2, 10, 40, 0, openborconstant("FRONTPANEL_Z"), "layer_autoscroll_ratio: " + layer_autoscroll_ratio);

	// Based on engine formula of (time_elapsed - traveltime) * autoscroll_y / 30 * 4; and
	// then mutiplying auto scroll ratio of layer.
	float base_adjustment = (-autoscroll_y / 22 * 4) * layer_autoscroll_ratio;

	settextobj(2, 10, 40, 0, openborconstant("FRONTPANEL_Z"), "base_adjustment: " + base_adjustment);

	float OldRange = (260.0 - DC_DRAW_GROUND_LAYER_HORIZON_POS_Y);
	float NewRange = (5.0 - 0.08);
	float NewValue = (((pos_z - DC_DRAW_GROUND_LAYER_HORIZON_POS_Y) * NewRange) / OldRange) + 0.08;

	settextobj(3, 10, 50, 0, openborconstant("FRONTPANEL_Z"), "NewValue: " + NewValue);

	pos_z += base_adjustment * NewValue;

	settextobj(4, 10, 60, 0, openborconstant("FRONTPANEL_Z"), "pos_x: " + pos_x);
	settextobj(5, 10, 70, 0, openborconstant("FRONTPANEL_Z"), "pos_z: " + pos_z);


	// Get center of level.

	int level_center_x = get_level_property(NULL(), openborconstant("LEVEL_PROP_SIZE_X")) * 0.5;

	float percentage = (pos_z - z_min) / (z_max - z_min);

	speed = speed * percentage;

	if (speed < 0)
	{
		speed = 0.001;
	}

	if (pos_x > level_center_x)
	{
		//pos_x = pos_x += (3.0 * speed);
		-(openborvariant("xpos") * 0.08);
	}
	else if (pos_x < level_center_x)
	{
		//pos_x = pos_x -= (3.0 * speed);// 
		- (openborvariant("xpos") * 0.08);
	}

	// need to find some way to nullify default scrolling.
	pos_x = 60 + openborvariant("xpos");

	//set_entity_property(ent, "position_x", pos_x);
	set_entity_property(ent, "position_z", pos_z);
}

// Caskey, Damon V.
// 2020-10-15
//
// Spawn wall obstacles.
void dc_repeat_spawn(int instance, char model_name, float pos_x, float pos_y, float pos_z, float distance_z, int layer_scroll_sync)
{
	float last_spawn_pos_z;

	void spawn_new = NULL();
	void spawn_last = getlocalvar("dc_repeat_spawner" + instance);

	// If we have spawned an entity, get distance between
	// it and the provided Z position. The see if the
	// distance between them is enough.

	//log("\n\n spawn_last: " + spawn_last);

	if (spawn_last)
	{
		last_spawn_pos_z = get_entity_property(spawn_last, "position_z");
	}

	if (!spawn_last || dc_math_difference_float(pos_z, last_spawn_pos_z) >= distance_z)
	{
		clearspawnentry();
		
		pos_x -= (openborvariant("xpos") * 0.08);
		
		setspawnentry("coords", pos_x, pos_z, pos_y);
		setspawnentry("name", model_name);
		spawn_new = spawn();
	
		log("\n spawn_new: " + spawn_new);
		settextobj(0, 10, 20, 0, openborconstant("FRONTPANEL_Z"), "Ents: " + openborvariant("count_entities") + "/" + openborvariant("ent_max"));

		// Set up to scroll with the selected layer instead
		// of at default speed.
		if (layer_scroll_sync != -1)
		{
			changeentityproperty(spawn_new, "scroll", 0);// getlayerproperty("fglayer", layer_scroll_sync, "xratio"));
		}

		setlocalvar("dc_repeat_spawner" + instance, spawn_new);
	}
}

// Caskey, Damon V.
// 2020-10-14
//
// Accept an entity, a Z position of minimal scale, and
// Z position of maximum scale. Sets drawmethod scale
// based on the entitiy's current Z position.
void dc_kanga_auto_scale(void ent, int z_min, int z_max)
{
	
	//disable
	//return;

	float pos_z = get_entity_property(ent, "position_z");
	float percentage = (pos_z - z_min) / (z_max - z_min);
	void draw = get_entity_property(ent, "drawmethod");

	// Drawmethod accepts size values from 0 to 256.
	int size = round(percentage * 256);

	// Cap minimum size to 1.
	if (size < 1)
	{
		size = 1;
	}

	//log("\n\n entity: " + ent);
	//log("\n\t pos_z: " + pos_z);
	//log("\n\t percentage: " + percentage);
	//log("\n\t size: " + size);
	
	set_drawmethod_property(draw, "enable", 1);

	set_drawmethod_property(draw, "scale_x", size);
	set_drawmethod_property(draw, "scale_y", size);

	//log("\n\t scale_x: " + get_drawmethod_property(draw, "scale_x"));
	//log("\n\t scale_y: " + get_drawmethod_property(draw, "scale_y"));

	float OldRange = (260.0 - DC_DRAW_GROUND_LAYER_HORIZON_POS_Y);
	float NewRange = (5.0 - 0.08);
	float NewValue = (((pos_z - DC_DRAW_GROUND_LAYER_HORIZON_POS_Y) * NewRange) / OldRange) + 0.08;

	changeentityproperty(ent, "scroll", NewValue);
}

// Caskey, Damon V.
// Iterate through entities 
// and run functions. This is
// just so we don't
// have dozens of entity
// iterattors spread out
// over mutiple functions.
void dc_entity_iterator()
{
	void ent = NULL();
	int entity_count = openborvariant("count_entities");
	int i = 0;

	// Loop through entity collection.
	for (i = 0; i < entity_count; i++)
	{
		// Get target entity for this loop increment.
		ent = getentity(i);
	
		dc_scroll_control(ent, 0.22, DC_DRAW_GROUND_LAYER_HORIZON_POS_Y, 220);

		dc_kanga_auto_scale(ent, DC_DRAW_GROUND_LAYER_HORIZON_POS_Y, DC_DRAW_GROUND_LAYER_FLOOR_SIZE_Y);
	}
}

float dc_perspective_get_size_far()
{
	return getglobalvar("DC3DSIZF");
}

float dc_perspective_get_size_near()
{
	return getglobalvar("DC3DSIZN");
}

void dc_perspective_set_size_far(float size)
{
	setglobalvar("DC3DSIZF", size);
}

void dc_perspective_set_size_near(float size)
{
	setglobalvar("DC3DSIZN", size);
}

void oncreate()
{
	
	// log("\n ondraw oncreate");

	

	dc_preload_hud_sprites();
}

void ondestroy()
{
	// log("\n ondraw ondestroy");

	//dc_perspective_set_size_far(NULL());
	//dc_perspective_set_size_near(NULL());

	dc_unload_hud_sprites();
}

void main() 
{
	// dc_debug_layers();

	// Perspective ground draw.
	setlocalvar(DC_INTRO_GROUND_INSTANCE, 0);
	setlocalvar(DC_INTRO_GROUND_SPRITE_QUE_Z_MAX, -151);
	setlocalvar(DC_INTRO_GROUND_SPRITE_QUE_Z_MIN, -151);
	setlocalvar(DC_INTRO_GROUND_SPRITE_QUE_NEWONLY, 0);
	setlocalvar(DC_INTRO_GROUND_SPRITE_OFFSET_X, 0);
	setlocalvar(DC_INTRO_GROUND_SPRITE_OFFSET_Y, 0);

	setlocalvar(DC_DRAW_GROUND_SCREEN_POS_X_START, 0);
	setlocalvar(DC_DRAW_GROUND_SCREEN_POS_Y_START, DC_DRAW_GROUND_LAYER_HORIZON_POS_Y);
	setlocalvar(DC_DRAW_GROUND_SCREEN_POS_Z_START, -127);
	setlocalvar(DC_DRAW_GROUND_SCREEN_SIZE_X, openborvariant("hresolution"));
	setlocalvar(DC_DRAW_GROUND_SCREEN_SIZE_Y, DC_DRAW_GROUND_LAYER_FLOOR_SIZE_Y);
	setlocalvar(DC_DRAW_GROUND_SCREEN_COUNT, 1);

	setlocalvar(DC_DRAW_GROUND_DM_WATERMODE, openborconstant("WATER_MODE_SHEAR"));
	setlocalvar(DC_DRAW_GROUND_DM_PERSPECTIVE, openborconstant("WATER_PERSPECTIVE_TILE"));
	setlocalvar(DC_DRAW_GROUND_DM_TINTMODE, openborconstant("BLEND_MODE_ALPHA_NEGATIVE"));
	setlocalvar(DC_DRAW_GROUND_DM_TINTCOLOR, rgbcolor(0, 200, 0));
	setlocalvar(DC_DRAW_GROUND_DM_ALPHA, 0);

	setlocalvar(DC_DRAW_GROUND_LAYER_INDEX_START, DC_DRAW_GROUND_LAYER_FLOOR);

	dc_perspective_set_size_far(0.08);
	dc_perspective_set_size_near(5.0);

	dc_draw_ground_multi_layer();

	

	// Perspective cloud screen draw.
	setlocalvar(DC_INTRO_GROUND_INSTANCE, 1);
	setlocalvar(DC_INTRO_GROUND_SPRITE_QUE_Z_MAX, -150);
	setlocalvar(DC_INTRO_GROUND_SPRITE_QUE_Z_MIN, -150);
	setlocalvar(DC_INTRO_GROUND_SPRITE_QUE_NEWONLY, 0);
	setlocalvar(DC_INTRO_GROUND_SPRITE_OFFSET_X, 0);
	setlocalvar(DC_INTRO_GROUND_SPRITE_OFFSET_Y, 0);

	setlocalvar(DC_DRAW_GROUND_SCREEN_POS_X_START, 0);
	setlocalvar(DC_DRAW_GROUND_SCREEN_POS_Y_START, 0);
	setlocalvar(DC_DRAW_GROUND_SCREEN_POS_Z_START, -130);
	setlocalvar(DC_DRAW_GROUND_SCREEN_SIZE_X, openborvariant("hresolution"));
	setlocalvar(DC_DRAW_GROUND_SCREEN_SIZE_Y, DC_DRAW_GROUND_LAYER_HORIZON_POS_Y);
	setlocalvar(DC_DRAW_GROUND_SCREEN_COUNT, 1);

	setlocalvar(DC_DRAW_GROUND_DM_WATERMODE, openborconstant("WATER_MODE_SHEAR"));
	setlocalvar(DC_DRAW_GROUND_DM_PERSPECTIVE, openborconstant("WATER_PERSPECTIVE_TILE"));
	setlocalvar(DC_DRAW_GROUND_DM_TINTMODE, openborconstant("BLEND_MODE_NONE"));
	setlocalvar(DC_DRAW_GROUND_DM_TINTCOLOR, 0);
	setlocalvar(DC_DRAW_GROUND_DM_ALPHA, 0);

	setlocalvar(DC_DRAW_GROUND_LAYER_INDEX_START, 0);

	dc_perspective_set_size_far(1.0);
	dc_perspective_set_size_near(0.01);

	dc_draw_ground_multi_layer();


	dc_player_hud();

	// Spawn rows of trees.
	//dc_repeat_spawn(1, "tree", 200, 0, 65, 100);

	//dc_repeat_spawn(0, "tree", 100, 0, 65, 0.5);
	dc_repeat_spawn(1, "tree", 200, 0, 65, 10, DC_DRAW_GROUND_LAYER_FLOOR);
	dc_repeat_spawn(2, "tree", 250, 0, 65, 10, DC_DRAW_GROUND_LAYER_FLOOR);
	dc_repeat_spawn(3, "tree", 350, 0, 65, 0.5, DC_DRAW_GROUND_LAYER_FLOOR);
	dc_repeat_spawn(4, "tree", 400, 0, 65, 10, DC_DRAW_GROUND_LAYER_FLOOR);
	//dc_repeat_spawn(5, "tree", 500, 0, 65, 0.5);

	dc_entity_iterator();

	//dc_math_bezier_curve_test(20, 60, 20);
	//dc_math_bezier_curve_test(80, 160, 80);
	//dc_math_bezier_curve_test(120, 280, 120);
	//dc_math_bezier_curve_test(200, 420, 200);
}

