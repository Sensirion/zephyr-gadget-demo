#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *co2;
    lv_obj_t *temp;
    lv_obj_t *humi;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_CO2 = 2,
    SCREEN_ID_TEMP = 3,
    SCREEN_ID_HUMI = 4,
};

void create_screen_main();
void tick_screen_main();

void create_screen_co2();
void tick_screen_co2();

void create_screen_temp();
void tick_screen_temp();

void create_screen_humi();
void tick_screen_humi();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/