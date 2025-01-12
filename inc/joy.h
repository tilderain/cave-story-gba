/*#define BUTTON_UP       0x0001
#define BUTTON_DOWN     0x0002
#define BUTTON_LEFT     0x0004
#define BUTTON_RIGHT    0x0008
#define BUTTON_A        0x0040
#define BUTTON_B        0x0010
#define BUTTON_C        0x0020
#define BUTTON_START    0x0080
#define BUTTON_X        0x0400
#define BUTTON_Y        0x0200
#define BUTTON_Z        0x0100
#define BUTTON_MODE     0x0800 */

#define BUTTON_UP       (1<<6)
#define BUTTON_DOWN     (1<<7)
#define BUTTON_LEFT     (1<<5)
#define BUTTON_RIGHT    (1<<4)
#define BUTTON_A        0x0040
#define BUTTON_B        (1<<1) //GBA B
#define BUTTON_C        (1<<0) //GBA A
#define BUTTON_START    (1<<3) //GBA Start
#define BUTTON_X        (1<<2) //GBA Select
#define BUTTON_Y        (1<<9) //GBA L
#define BUTTON_Z        (1<<8) //GBA R
#define BUTTON_MODE     0x0800

#define BUTTON_DIR      0x000F
#define BUTTON_BTN      0x0FF0

#define JOY_TYPE_PAD3           0x00
#define JOY_TYPE_PAD6           0x01

extern const uint16_t btn[12];
extern const char btnName[12][4];

#define joy_pressed(b) (((joystate&(b))&&((~oldstate)&(b))))
#define joy_released(b) ((((~joystate)&(b))&&(oldstate&(b))))
#define joy_down(b) ((joystate&(b)))

extern uint8_t joytype;
extern uint16_t joystate, oldstate;

void joy_init();
void joy_update();
