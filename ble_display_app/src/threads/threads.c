#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <math.h>

#include "threads.h"
#include "../ui/ui.h"
#include "../ui/vars.h"
#include "../advertisement/advertisement.h"

static const struct device *const sen_scd = DEVICE_DT_GET(DT_ALIAS(co2));
static const struct device *const sen_sht = DEVICE_DT_GET(DT_ALIAS(rht));
static const struct device *const display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

void display(void *, void *, void *)
{
    if (!gpio_is_ready_dt(&led))
    {
        printk("Backlight not ready");
        return;
    }
    if (!gpio_is_ready_dt(&button))
    {
        printk("Error: button not ready\n");
        return;
    }
    if (!device_is_ready(display_dev))
    {
        printk("Display not ready");
        return;
    }

    display_l();
}

void display_l(void)
{
    k_thread_name_set(NULL, "Display Thread");

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&button, GPIO_INPUT);

    display_blanking_off(display_dev);
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

void scd(void *, void *, void *)
{
    if (!device_is_ready(sen_scd))
    {
        printk("Device %s is not ready.\n", sen_scd->name);
        return;
    }
    scd_l();
}

void scd_l(void)
{
    k_thread_name_set(NULL, "SCD Thread");
    struct sensor_value co2;
    int ret = 0;

    set_var_co2_val(450);
    set_var_temp_val(26);
    set_var_humi_val(50);

    while (1)
    {
        // fetch new data
        k_msleep(6000);
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
    }
}

void sht(void *, void *, void *)
{
    if (!device_is_ready(sen_sht))
    {
        printk("Device %s is not ready.\n", sen_sht->name);
        return;
    }
    sht_l();
}

void sht_l(void)
{
    k_thread_name_set(NULL, "SHT Thread");
    struct sensor_value humidity, temperature;
    int ret = 0;

    while (1)
    {
        // fetch new data
        k_msleep(500);
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

        double humi_val = humidity.val1 + (humidity.val2 / 1000000.0);

        if (fabs(humi_val - get_var_humi_val()) >= 0.1)
        {
            set_var_humi_val(humi_val);
            advertising_update();
        }
    }
}
