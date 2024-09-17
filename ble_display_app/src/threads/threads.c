#include "threads.h"
#include "../ui/ui.h"

void display(void)
{
  //------------------ turn backlight on ------------------
  gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

  gpio_pin_configure_dt(&button, GPIO_INPUT);

  display_blanking_off(display_dev);

  lv_init();
  ui_init();

  uint8_t act_screen = 1;

  while (1)
  {
    lv_task_handler();
    ui_tick();
    k_msleep(100);
    if (gpio_pin_get_dt(&button) > 0)
    {
      if (++act_screen > 4)
      {
        act_screen = 1;
      }
      loadScreen(act_screen);
      while (gpio_pin_get_dt(&button) > 0)
      {
      }
    }
  }
}

void scd(void)
{
  struct sensor_value co2;
  int ret;

  while (1)
  {
    // fetch new data
    ret = sensor_sample_fetch(sen_scd);
    if (ret < 0)
    {
      printk("failed sample fetch from %s\n", sen_scd->name);
    }

    // get data
    sensor_channel_get(sen_scd, SENSOR_CHAN_CO2, &co2);

    if (co2.val1 != get_var_co2_val())
    {
      set_var_co2_val(co2.val1);
      advertising_update();
    }

    k_msleep(6000);
  }
}

void sht(void)
{
  struct sensor_value humidity, temperature;
  int ret;

  while (1)
  {
    // fetch new data
    ret = sensor_sample_fetch(sen_sht);
    if (ret < 0)
    {
      printk("failed sample fetch from %s\n", sen_sht->name);
    }

    // get data
    sensor_channel_get(sen_sht, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
    sensor_channel_get(sen_sht, SENSOR_CHAN_HUMIDITY, &humidity);

    double temp_val = temperature.val1 + (temperature.val2 / 1000000.0);

    if (fabs(temp_val - get_var_temp_val()) >= 0.1)
    {
      set_var_temp_val(temp_val);
      advertising_update();
    }
    else
    {
      printk("no change in temperature\n");
    }
    
    double humi_val = humidity.val1 + (humidity.val2 / 1000000.0);

    if (fabs(humi_val - get_var_humi_val()) >= 0.1)
    {
      set_var_humi_val(humi_val);
      advertising_update();
    }

    k_msleep(500);
  }
}
