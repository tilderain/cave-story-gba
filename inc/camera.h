// Pre calculated camera bounds values to speed up entity_on_screen()
static int32_t camera_xmin, camera_ymin;
static uint32_t camera_xsize, camera_ysize;

static struct {
	int32_t x, y; // Fixed point units
	Entity *target; // Player, boss, NPC, or NULL
	// Offset is the point relative to the target's position
	// that the camera tries to center on
	int16_t x_offset, y_offset;
	// Apply some math ahead of time to speed up sprite positioning
	uint16_t x_shifted, y_shifted;
	// Marks a position which, after moving 16 pixels away will remark
	// to the new position, and reactivate any entities that came on screen
	int32_t x_mark, y_mark;
} camera;

// Initialize the camera with default values (upper left, no target)
void camera_init();
// Center camera directly on a specific point
// This does not redraw the tilemap, call stage_draw_screen() manually after
void camera_set_position(int32_t x, int32_t y);
// Shake camera for a specified number of frames
void camera_shake(uint16_t time);
// Per frame update for camera, moves toward the target entity and scrolls the tilemap
void camera_update();
