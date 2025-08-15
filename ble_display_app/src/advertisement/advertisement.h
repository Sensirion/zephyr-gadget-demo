#ifndef ADVERTISEMENT_H
#define ADVERTISEMENT_H

uint16_t get_temp_ticks(double temp);
uint16_t get_humi_ticks(double humi);
void bt_ready(int ret);
void update_adv_data(uint16_t company_id, uint8_t s_advt, uint8_t s_type,
                     uint16_t device_id, uint16_t *sample_data,
                     uint8_t sample_data_len, uint8_t *data_buf);
void advertising_start(void);
void advertising_update(void);

#endif /*ADVERTISEMENT_H*/
