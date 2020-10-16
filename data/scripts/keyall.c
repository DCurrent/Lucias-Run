
void main() {

	int     player_index;		// Player index.
	void    ent;				// Player entity.

	player_index = getlocalvar("player");

	// Populate the variables.
	ent = getplayerproperty(player_index, "entity");

	// Don't error out if we aren't in a level with entities.
	if (!ent)
	{
		return;
	}	

	//dc_screen_offset_debug(player_index);
}

void dc_screen_offset_debug(int player_index)
{
	// Key press.
	int key_press = getplayerproperty(player_index, "newkeys");

	int screen_offset_y = getglobalvar("screen_offset_y");

	if (!screen_offset_y)
	{
		screen_offset_y = 0;
	}

	if (key_press & openborconstant("FLAG_MOVEUP"))
	{
		screen_offset_y++;
	}
	else if (key_press & openborconstant("FLAG_MOVEDOWN"))
	{
		screen_offset_y--;
	}

	setglobalvar("screen_offset_y", screen_offset_y);

	log("\n screen_offset_y: " + screen_offset_y);
}