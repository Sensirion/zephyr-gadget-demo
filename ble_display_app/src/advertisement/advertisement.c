#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/bluetooth/bluetooth.h>
#include "../ui/vars.h"

#include "advertisement.h"

#define BT_LE_AD_ONLY_GENERAL 0x06
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define MANUFACTURER_DATA 1
#define COMPANY_ID 0x06D5
#define DEVICE_ID 0xAABB
#define S_ADVT 0
#define S_TYPE 0x0A

static uint8_t data_buf[12] = {0xD5, 0x06,  /*company id (little endian)*/
                               0x00,        /*sample advertisement type*/
                               0x0A,        /*sample type*/
                               0xAA, 0xBB,  /*device id (big endian)*/
                               0x15, 0x5F,  /*temperature ticks (little endian)*/
                               0xFF, 0x7F,  /*humidity ticks (little endian)*/
                               0xC2, 0x01}; /*co2 vlaue (little endian)*/

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_ONLY_GENERAL),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, data_buf, ARRAY_SIZE(data_buf))};

/* Set Scan Response data */
static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

uint16_t get_temp_ticks(double temp)
{
  uint16_t ticks = (double)(temp + 45) * 0xFFFF / 175.0;
  return ticks;
}

uint16_t get_humi_ticks(double humi)
{
  uint16_t ticks = (double)(humi) * 0xFFFF / 100.0;
  return ticks;
}

void bt_ready(int ret)
{
  if (ret)
  {
    printk("Bluetooth init failed (ret %d)\n", ret);
    return;
  }

  printk("Bluetooth initialized\n");

  // Start advertising with 1s interval
  ret = bt_le_adv_start(BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_IDENTITY, BT_GAP_ADV_SLOW_INT_MIN,
                                        BT_GAP_ADV_SLOW_INT_MAX, NULL),
                        ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));

  if (ret < 0)
  {
    printk("Advertising failed to start (ret %d)\n", ret);
    return;
  }

  printk("Advertising started...\n");
}

void update_adv_data(uint16_t company_id, uint8_t s_advt, uint8_t s_type,
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

void advertising_start(void)
{
  int ret;

  // Start advertising
  ret = bt_enable(bt_ready);
  if (ret)
  {
    printk("Bluetooth init failed (err %d)\n", ret);
    return;
  }
}

void advertising_update(void)
{
  uint16_t sample_data[3] = {
      get_temp_ticks(get_var_temp_val()),
      get_humi_ticks(get_var_humi_val()),
      get_var_co2_val()};

  update_adv_data(COMPANY_ID, S_ADVT, S_TYPE, DEVICE_ID, sample_data, ARRAY_SIZE(sample_data), data_buf);
}
