#include "data/scripts/dc_hud/config.h"

// Return a decimal fraction of
// current HP vs. max HP.
float get_status_fraction(int status, int status_max)
{
	float current;   // Current HP.
	float max;       // Maximum HP.
	float result;           // Output.

	current = 0.0 + status;
	max      = 0.0 + status_max;

	// Don't divide by 0!
	if(status && status_max)
    {
        result = status / status_max;
    }
    else
    {
        result = 0.0;
    }

	return result;
}
