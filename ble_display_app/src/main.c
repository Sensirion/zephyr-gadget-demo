#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#include "threads/threads.h"
#include "advertisement/advertisement.h"

#if !DT_NODE_EXISTS(DT_ALIAS(co2))
#error "co2 alias not found in device tree"
#endif

#if !DT_NODE_EXISTS(DT_ALIAS(rht))
#error "rht alias not found in device tree"
#endif

LOG_MODULE_REGISTER(app);

K_THREAD_DEFINE(scd_id, STACKSIZE_DEFAULT, scd, NULL, NULL, NULL,
				PRIORITY, 0, 500);
K_THREAD_DEFINE(sht_id, STACKSIZE_DEFAULT, sht, NULL, NULL, NULL,
				PRIORITY, 0, 500);
K_THREAD_DEFINE(display_id, STACKSIZE_DISPLAY, display, NULL, NULL, NULL,
				PRIORITY, 0, 3000);

int main(void)
{
	printk("Starting application...\n");

	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0)); // Adjust based on your I2C bus
	if (!device_is_ready(i2c_dev))
	{
		printk("I2C bus not ready!\n");
		return -ENODEV;
	}
	advertising_start();

	return 0;
}
