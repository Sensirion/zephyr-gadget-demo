#ifndef THREADS_H
#define THREADS_H

#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <math.h>


#include "../ui/vars.h"
#include "../advertisement/advertisement.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>

/* size of stack area used by each thread */
#define STACKSIZE_DEFAULT 1024
#define STACKSIZE_DISPLAY 4096

/* scheduling priority used by each thread */
#define PRIORITY 7

static const struct device *const sen_scd = DEVICE_DT_GET(DT_ALIAS(co2));
static const struct device *const sen_sht = DEVICE_DT_GET(DT_ALIAS(rht));
static const struct device *const display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

void display(void);
void scd(void);
void sht(void);

#endif /* THREADS_H */
