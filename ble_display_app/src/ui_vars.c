#include "ui/vars.h"

static int32_t co2_val = 0;
static double temp_val = 0;
static double humi_val = 0;

int32_t get_var_co2_val()
{
  return co2_val;
}

void set_var_co2_val(int32_t value)
{
  co2_val = value;
}

double get_var_temp_val()
{
  return temp_val;
}

void set_var_temp_val(double value)
{
  temp_val = value;
}

double get_var_humi_val()
{
  return humi_val;
}

void set_var_humi_val(double value)
{
  humi_val = value;
}
