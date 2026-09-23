VPATH += keyboards/ploopyco/common
SRC += tmag5273wheel.c
I2C_DRIVER_REQUIRED = yes
POINTING_DEVICE_DRIVER = pmw3360
RGBLIGHT_ENABLE = yes

## Uncomment to get debug output (also see keyboard_post_init_user() 
## in keymap.c for more things to uncomment.)
#CONSOLE_ENABLE = yes