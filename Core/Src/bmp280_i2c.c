/*
 * bmp280_i2c.c
 *
 *  Created on: Jun 29, 2024
 *      Author: Uzair
 */

#include "bmp280_i2c.h"

uint16_t dig_T1;
int16_t dig_T2, dig_T3;

uint16_t dig_P1;
int16_t dig_P2, dig_P3 , dig_P4 , dig_P5,dig_P6,dig_P7 ,dig_P8,dig_P9;





uint8_t temp_raw[3];





HAL_StatusTypeDef bmp280_initilize(bmp280 * dev ,I2C_HandleTypeDef * i2c_handler ){
	dev->i2c_handler = i2c_handler;

	dev->pressure = 0.0f;
	dev->temp = 0.0f;
	//testing and read register to know if sensor is initilized correctly

	uint8_t err_num = 0;
	HAL_StatusTypeDef status ;
	uint8_t reg_data;
	uint8_t data_not_match_cout = 0;
	uint8_t reg_data2;
//    uint8_t temp_raw[3];
//    int32_t temp;


	status = bmp280_read_register(dev, REG_DEVICE_ID, &reg_data);
	if(status != HAL_OK){
		err_num++;
	}


	if(reg_data != REG_DEVICE_ID_VALUE_DEFAULT ){
		data_not_match_cout++;
		return 255;
	}

	uint8_t write_data = 0x27;
	status = bmp280_write_register(dev, REG_CTRL_MEAS, &write_data);

	if(status != HAL_OK){
		err_num++;
	}


	 write_data = 0x00;

	status = bmp280_write_register(dev, REG_CONFIG, &write_data);




	status = bmp280_read_register(dev, REG_STATUS, &reg_data2);
	if(status != HAL_OK){
		err_num++;
	}

	BMP280_ReadCalibrationData(dev);



//	status = bmp280_read_registers(dev, 0xFA, &temp_raw , 3);
//	if(status != HAL_OK){
//		err_num++;
//	}
//    temp = (int32_t)(((uint32_t)(temp_raw[0]) << 12) | ((uint32_t)(temp_raw[1]) << 4) | ((uint32_t)(temp_raw[2]) >> 4));
//    float temperature = temp / 5120.0;


	return err_num;
}
HAL_StatusTypeDef bmp280_read_register(bmp280 * dev , uint8_t reg_adress , uint8_t *data ){

return HAL_I2C_Mem_Read(dev->i2c_handler, device_i2c_adress, reg_adress, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);

}


HAL_StatusTypeDef bmp280_read_registers(bmp280 * dev , uint8_t reg_adress , uint8_t *data , uint8_t length  ){

return HAL_I2C_Mem_Read(dev->i2c_handler, device_i2c_adress, reg_adress, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);

}

HAL_StatusTypeDef bmp280_write_register(bmp280 * dev , uint8_t reg_adress , uint8_t *data  ){

	return HAL_I2C_Mem_Write(dev->i2c_handler, device_i2c_adress, reg_adress, I2C_MEMADD_SIZE_8BIT, data,  1, HAL_MAX_DELAY);


}



void BMP280_ReadCalibrationData(bmp280 * dev){
	  uint8_t calib[24];

	  bmp280_read_registers(dev, 0x88,calib, 24);
	  dig_T1 = (uint16_t)((calib[1] << 8) | calib[0]);
	  dig_T2 = (int16_t)((calib[3] << 8) | calib[2]);
	  dig_T3 = (int16_t)((calib[5] << 8) | calib[4]);


//	  updated code for pressure
	  	 dig_P1 = (uint16_t)((calib[7] << 8) | calib[6]);
	     dig_P2 = (int16_t)((calib[9] << 8) | calib[8]);
	     dig_P3 = (int16_t)((calib[11] << 8) | calib[10]);
	     dig_P4 = (int16_t)((calib[13] << 8) | calib[12]);
	     dig_P5 = (int16_t)((calib[15] << 8) | calib[14]);
	     dig_P6 = (int16_t)((calib[17] << 8) | calib[16]);
	     dig_P7 = (int16_t)((calib[19] << 8) | calib[18]);
	     dig_P8 = (int16_t)((calib[21] << 8) | calib[20]);
	     dig_P9 = (int16_t)((calib[23] << 8) | calib[22]);


}




uint32_t BMP280_ReadRawTemperature(bmp280 * dev) {

    // Read temperature registers

	bmp280_read_registers(dev,REG_TEMP_DATA ,temp_raw, 3);


    return (uint32_t)(((uint32_t)temp_raw[0] << 12) | ((uint32_t)temp_raw[1] << 4) | ((uint32_t)temp_raw[2] >> 4));
}





uint32_t BMP280_ReadRawPressure(bmp280 * dev) {
    uint8_t press_raw[3];


	bmp280_read_registers(dev,REG_PRESS_DATA ,press_raw, 3);


    return (uint32_t)(((uint32_t)press_raw[0] << 12) | ((uint32_t)press_raw[1] << 4) | ((uint32_t)press_raw[2] >> 4));
}



float BMP280_CompensateTemperature(int32_t adc_T) {
    int32_t var1, var2;
    float T;
    int32_t t_fine;

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;

    return T / 100.0;
}



float BMP280_CompensatePressure(int32_t adc_P , bmp280 * dev) {
	 int32_t var1, var2;
	    int64_t p;

	    // Calculate t_fine locally
	    int32_t adc_T = BMP280_ReadRawTemperature(dev);
	    int32_t t_fine_local;
	    int32_t var1_temp, var2_temp;

	    var1_temp = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
	    var2_temp = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

	    t_fine_local = var1_temp + var2_temp;

	    // Use local t_fine for pressure calculation
	    var1 = ((int64_t)t_fine_local) - 128000;
	    var2 = var1 * var1 * (int64_t)dig_P6;
	    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
	    var2 = var2 + (((int64_t)dig_P4) << 35);
	    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
	    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;

	    if (var1 == 0) {
	        return 0; // Avoid division by zero
	    }

	    p = 1048576 - adc_P;
	    p = (((p << 31) - var2) * 3125) / var1;
	    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	    var2 = (((int64_t)dig_P8) * p) >> 19;

	    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);

	    return (float)p / 256.0 / 1000.0; // Convert Pa to kPa
}
