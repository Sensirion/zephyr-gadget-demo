#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_CO2_VAL = 0,
    FLOW_GLOBAL_VARIABLE_HUMI_VAL = 1,
    FLOW_GLOBAL_VARIABLE_TEMP_VAL = 2
};

// Native global variables

extern int32_t get_var_co2_val();
extern void set_var_co2_val(int32_t value);
extern double get_var_humi_val();
extern void set_var_humi_val(double value);
extern double get_var_temp_val();
extern void set_var_temp_val(double value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/