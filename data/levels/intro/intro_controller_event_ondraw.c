#include "data/scripts/dc_hud/main.c"

#ifndef DC_INSET

#define DC_INSET    1

#define KEY_SCREEN          "key_scr"

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

void dc_draw_cloud_layer()
{	
#define DC_INTRO_CLOUD_SCREEN_POSITION_X 0
#define DC_INTRO_CLOUD_SCREEN_POSITION_Y 0
#define DC_INTRO_CLOUD_SCREEN_POSITION_Z -149

#define DC_INTRO_CLOUD_SPRITE_QUE_Z_MAX -150
#define DC_INTRO_CLOUD_SPRITE_QUE_Z_MIN -150 //openborconstant("MIN_INT")
#define DC_INTRO_CLOUD_SPRITE_QUE_NEWONLY 0
#define DC_INTRO_CLOUD_SPRITE_OFFSET_X 0
#define DC_INTRO_CLOUD_SPRITE_OFFSET_Y 0


	void scr = get_screen("cloud", openborvariant("hresolution"), 70);

	// Clear the screen, then add our target sprites.

	clearscreen(scr);
	drawspriteq(scr, DC_INTRO_CLOUD_SPRITE_QUE_NEWONLY, DC_INTRO_CLOUD_SPRITE_QUE_Z_MIN, DC_INTRO_CLOUD_SPRITE_QUE_Z_MAX, DC_INTRO_CLOUD_SPRITE_OFFSET_X, DC_INTRO_CLOUD_SPRITE_OFFSET_Y);

	// Set global drawMethods.

	changedrawmethod(NULL(), "reset", 1);
	changedrawmethod(NULL(), "enabled", 1);
	changedrawmethod(NULL(), "watermode", 3);
	changedrawmethod(NULL(), "beginsize", 1);
	changedrawmethod(NULL(), "endsize", 0.01);
	changedrawmethod(NULL(), "perspective", 1);
	changedrawmethod(NULL(), "alpha", 0);
	// Draw the screen, then disable the drawmethod.

	drawscreen(scr, DC_INTRO_CLOUD_SCREEN_POSITION_X, DC_INTRO_CLOUD_SCREEN_POSITION_Y, DC_INTRO_CLOUD_SCREEN_POSITION_Z);
	
	changedrawmethod(NULL(), "enabled", 0);
}

void dc_draw_ground_layer()
{
#define DC_INTRO_GROUND_SCREEN_POSITION_X 0
#define DC_INTRO_GROUND_SCREEN_POSITION_Y 70
#define DC_INTRO_GROUND_SCREEN_POSITION_Z -149

#define DC_INTRO_GROUND_SPRITE_QUE_Z_MAX -151
#define DC_INTRO_GROUND_SPRITE_QUE_Z_MIN -151
#define DC_INTRO_GROUND_SPRITE_QUE_NEWONLY 0
#define DC_INTRO_GROUND_SPRITE_OFFSET_X 0
#define DC_INTRO_GROUND_SPRITE_OFFSET_Y 0


	void scr = get_screen("ground", openborvariant("hresolution"), 160);

	// Clear the screen, then add our target sprites.

	clearscreen(scr);
	drawspriteq(scr, DC_INTRO_GROUND_SPRITE_QUE_NEWONLY, DC_INTRO_GROUND_SPRITE_QUE_Z_MIN, DC_INTRO_GROUND_SPRITE_QUE_Z_MAX, DC_INTRO_GROUND_SPRITE_OFFSET_X, DC_INTRO_GROUND_SPRITE_OFFSET_Y);

	// Set global drawMethods.

	changedrawmethod(NULL(), "reset", 1);
	changedrawmethod(NULL(), "enabled", 1);
	changedrawmethod(NULL(), "watermode", 3);
	changedrawmethod(NULL(), "beginsize", 0.08);
	changedrawmethod(NULL(), "endsize", 3);
	changedrawmethod(NULL(), "perspective", 1);
	changedrawmethod(NULL(), "tintmode", openborconstant("BLEND_MODE_ALPHA_NEGATIVE"));
	changedrawmethod(NULL(), "tintcolor", rgbcolor(0, 200, 0));
	changedrawmethod(NULL(), "alpha", 0);
	// Draw the screen, then disable the drawmethod.

	drawscreen(scr, DC_INTRO_GROUND_SCREEN_POSITION_X, DC_INTRO_GROUND_SCREEN_POSITION_Y, DC_INTRO_GROUND_SCREEN_POSITION_Z);

	changedrawmethod(NULL(), "enabled", 0);
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

void oncreate()
{
	// log("\n ondraw oncreate");

	dc_preload_hud_sprites();
}

void ondestroy()
{
	// log("\n ondraw ondestroy");

	dc_unload_hud_sprites();
}

void main() 
{
	//dc_debug_layers();

	// Perspective cloud screen draw.
	dc_draw_cloud_layer();
	dc_draw_ground_layer();

	dc_player_hud();

	dc_math_bezier_curve_test(20, 60, 20);

	dc_math_bezier_curve_test(80, 160, 80);

	dc_math_bezier_curve_test(120, 280, 120);

	dc_math_bezier_curve_test(200, 420, 200);
}

