#include "threads/threads.h"
#include "advertisement/advertisement.h"

LOG_MODULE_REGISTER(app);

K_THREAD_DEFINE(scd_id, STACKSIZE_DEFAULT, scd, NULL, NULL, NULL,
								PRIORITY, 0, 0);
K_THREAD_DEFINE(sht_id, STACKSIZE_DEFAULT, sht, NULL, NULL, NULL,
								PRIORITY, 0, 0);
K_THREAD_DEFINE(display_id, STACKSIZE_DISPLAY, display, NULL, NULL, NULL,
								PRIORITY, 0, 0);

int main(void)
{
	if (!gpio_is_ready_dt(&led))
	{
		LOG_ERR("Backlight not ready");
		return 0;
	}
	if (!gpio_is_ready_dt(&button))
	{
		LOG_ERR("Error: button device %s is not ready\n",
					 button.port->name);
		return 0;
	}
	if (!device_is_ready(display_dev))
	{
		LOG_ERR("Display not ready");
		return 0;
	}
	if (!device_is_ready(sen_scd))
	{
		LOG_ERR("Device %s is not ready.\n", sen_scd->name);
		return 0;
	}
	if (!device_is_ready(sen_sht))
	{
		LOG_ERR("Device %s is not ready.\n", sen_sht->name);
		return 0;
	}

	advertising_start();

	// Start threads after initialization
	k_thread_start(display_id);
	k_thread_start(scd_id);
	k_thread_start(sht_id);
}