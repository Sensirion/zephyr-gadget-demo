/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/bluetooth/bluetooth.h>

#define BT_LE_AD_ONLY_GENERAL 0x06
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define MANUFACTURER_DATA 1
#define COMPANY_ID 0x06D5
#define DEVICE_ID 0xAABB
#define S_ADVT 0
#define S_TYPE 0x0A

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

static uint8_t data_buf[12] = {0xD5, 0x06,	/*company id (little endian)*/
															 0x00,				/*sample advertisement type*/
															 0x0A,				/*sample type*/
															 0xAA, 0xBB,	/*device id (big endian)*/
															 0x15, 0x5F,	/*temperature ticks (little endian)*/
															 0xFF, 0x7F,	/*humidity ticks (little endian)*/
															 0xC2, 0x01}; /*co2 vlaue (little endian)*/

static const struct bt_data ad[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_ONLY_GENERAL),
		BT_DATA(BT_DATA_MANUFACTURER_DATA, data_buf, ARRAY_SIZE(data_buf))};

/* Set Scan Response data */
static const struct bt_data sd[] = {
		BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct device *const sen_scd = DEVICE_DT_GET(DT_ALIAS(co2));
static const struct device *const sen_sht = DEVICE_DT_GET(DT_ALIAS(rht));

struct scd_data_t
{
	void *lifo_reserved; /* 1st word reserved for use by lifo */
	struct sensor_value co2;
};

struct sht_data_t
{
	void *lifo_reserved; /* 1st word reserved for use by lifo */
	struct sensor_value humi;
	struct sensor_value temp;
};

K_LIFO_DEFINE(co2_lifo);
K_LIFO_DEFINE(rht_lifo);

static void bt_ready(int ret)
{
	if (ret < 0)
	{
		printk("Bluetooth init failed (ret %d)\n", ret);
		return;
	}

	/* Start advertising with 1s interval */
	ret = bt_le_adv_start(BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_IDENTITY, BT_GAP_ADV_SLOW_INT_MIN,
																				BT_GAP_ADV_SLOW_INT_MAX, NULL),
												ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (ret < 0)
	{
		printk("Advertising failed to start (ret %d)\n", ret);
		return;
	}
}

static uint16_t get_temp_ticks(struct sensor_value *temp)
{
	uint16_t ticks = (double)(temp->val1 + (temp->val2 / 1000000.0) + 45) * 0xFFFF / 175.0;
	return ticks;
}

static uint16_t get_humi_ticks(struct sensor_value *humi)
{
	uint16_t ticks = (double)(humi->val1 + (humi->val2 / 1000000.0)) * 0xFFFF / 100.0;
	return ticks;
}

static void update_adv_data(uint16_t company_id, uint8_t s_advt, uint8_t s_type,
														uint16_t device_id, uint16_t *sample_data,
														uint8_t sample_data_len, uint8_t *data_buf)
{
	uint8_t buf_pos = 0;

	sys_put_le16(company_id, &data_buf[buf_pos]);
	buf_pos += 2;
	data_buf[buf_pos++] = s_advt;
	data_buf[buf_pos++] = s_type;
	sys_put_be16(device_id, &data_buf[buf_pos]);
	buf_pos += 2;
	for (uint8_t i = 0; i < sample_data_len; i++)
	{
		sys_put_le16(sample_data[i], &data_buf[buf_pos]);
		buf_pos += 2;
	}

	bt_le_adv_update_data(ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
}

void scd(void)
{
	struct sensor_value co2;
	int ret;

	if (!device_is_ready(sen_scd))
	{
		printf("Device %s is not ready.\n", sen_scd->name);
		return;
	}

	while (1)
	{
		/*fetch new data*/
		ret = sensor_sample_fetch(sen_scd);
		if (ret < 0)
		{
			printk("failed sample fetch from %s\n", sen_scd->name);
		}

		/*get data*/
		sensor_channel_get(sen_scd, SENSOR_CHAN_CO2, &co2);

		struct scd_data_t tx_data = {.co2 = co2};

		/*allocate new memory for tx_data*/
		size_t size = sizeof(struct scd_data_t);
		char *mem_ptr = k_malloc(size);
		__ASSERT_NO_MSG(mem_ptr != 0);

		memcpy(mem_ptr, &tx_data, size);

		/*put tx_data to a lifo*/
		k_lifo_put(&co2_lifo, mem_ptr);

		k_msleep(6000);
	}
}

void sht(void)
{
	struct sensor_value humidity, temperature;
	int ret;

	if (!device_is_ready(sen_sht))
	{
		printf("Device %s is not ready.\n", sen_sht->name);
		return;
	}

	while (1)
	{
		/*fetch new data*/
		ret = sensor_sample_fetch(sen_sht);
		if (ret < 0)
		{
			printk("failed sample fetch from %s\n", sen_sht->name);
		}

		/*get data*/
		sensor_channel_get(sen_sht, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
		sensor_channel_get(sen_sht, SENSOR_CHAN_HUMIDITY, &humidity);

		struct sht_data_t tx_data = {.temp = temperature, .humi = humidity};

		/*allocate new memory for tx_data*/
		size_t size = sizeof(struct sht_data_t);
		char *mem_ptr = k_malloc(size);
		__ASSERT_NO_MSG(mem_ptr != 0);

		memcpy(mem_ptr, &tx_data, size);

		/*put tx_data to a lifo*/
		k_lifo_put(&rht_lifo, mem_ptr);

		k_msleep(500);
	}
}

void advertisement(void)
{
	struct sensor_value humidity, temperature, co2;
	bool is_new_data;
	int ret;

	/* Initialize the Bluetooth Subsystem */
	ret = bt_enable(bt_ready);
	if (ret)
	{
		printk("Bluetooth init failed (ret %d)\n", ret);
	}

	while (1)
	{
		k_msleep(2000);
		is_new_data = false;

		/*get scd data*/
		struct scd_data_t *rx_data_scd = k_lifo_get(&co2_lifo, K_NO_WAIT);
		if (rx_data_scd != NULL)
		{
			co2 = rx_data_scd->co2;
			k_free(rx_data_scd);
			is_new_data = true;

			/*clear lifo*/
			struct scd_data_t *empty_lifo;
			empty_lifo = k_lifo_get(&co2_lifo, K_NO_WAIT);
			while (empty_lifo != NULL)
			{
				k_free(empty_lifo);
				empty_lifo = k_lifo_get(&co2_lifo, K_NO_WAIT);
			}
		}

		/*get sht data*/
		struct sht_data_t *rx_data_sht = k_lifo_get(&rht_lifo, K_NO_WAIT);
		if (rx_data_sht != NULL)
		{
			humidity = rx_data_sht->humi;
			temperature = rx_data_sht->temp;
			k_free(rx_data_sht);
			is_new_data = true;

			/*clear lifo*/
			struct sht_data_t *empty_lifo;
			empty_lifo = k_lifo_get(&rht_lifo, K_NO_WAIT);
			while (empty_lifo != NULL)
			{
				k_free(empty_lifo);
				empty_lifo = k_lifo_get(&rht_lifo, K_NO_WAIT);
			}
		}

		/*update advertisement with new data*/
		if (is_new_data)
		{
			uint16_t sample_data[3] = {get_temp_ticks(&temperature), get_humi_ticks(&humidity), co2.val1};

			update_adv_data(COMPANY_ID, S_ADVT, S_TYPE, DEVICE_ID, sample_data, sizeof(sample_data), data_buf);
		}
	}
}

K_THREAD_DEFINE(scd_id, STACKSIZE, scd, NULL, NULL, NULL,
								PRIORITY, 0, 0);
K_THREAD_DEFINE(sht_id, STACKSIZE, sht, NULL, NULL, NULL,
								PRIORITY, 0, 0);
K_THREAD_DEFINE(advertisement_id, STACKSIZE, advertisement, NULL, NULL, NULL,
								PRIORITY, 0, 0);
