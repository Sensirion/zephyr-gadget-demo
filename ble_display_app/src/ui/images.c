#include "images.h"

const ext_img_desc_t images[4] = {
    { "sensi_humi", &img_sensi_humi },
    { "sensi_co2", &img_sensi_co2 },
    { "sensi_temp", &img_sensi_temp },
    { "sensi_logo", &img_sensi_logo },
};
