

#ifndef bmp280_i2c_I2C_DRIVER_H
#define bmp280_i2c_I2C_DRIVER_H

#include "stm32f1xx_hal.h" //i2c funtionality
//#include <stdint.h>

//defines

#define device_i2c_adress (0x76 <<1) //  SDO to GND results in slave address 1110110 (0x76);


#define REG_DEVICE_ID 0xD0 //adress to device id register
#define REG_CTRL_MEAS 0xF4
#define REG_STATUS 0xF3
#define REG_CONFIG 0xF5
#define REG_TEMP_DATA 0xFA
#define REG_PRESS_DATA 0xF7
//other adress have to be included

// Global Variables Caliberation

// END

//default values of register for debugging and testing
#define REG_DEVICE_ID_VALUE_DEFAULT 0x58 // default value



typedef struct {

	I2C_HandleTypeDef *i2c_handler;
	float temp;
	float pressure;

}bmp280;



float BMP280_CompensateTemperature(int32_t adc_T);

uint32_t BMP280_ReadRawTemperature(bmp280 * dev) ;

uint32_t BMP280_ReadRawPressure(bmp280 * dev) ;

float BMP280_CompensatePressure(int32_t adc_P ,bmp280 * dev) ;


void BMP280_ReadCalibrationData(bmp280 * dev);
HAL_StatusTypeDef bmp280_initilize(bmp280 * dev ,I2C_HandleTypeDef * i2c_handler );


HAL_StatusTypeDef bmp280_readtemp(bmp280 * dev , uint8_t reg_adress );
HAL_StatusTypeDef bmp280_readpressure(bmp280 * dev , uint8_t reg_adress );


HAL_StatusTypeDef bmp280_read_register(bmp280 * dev , uint8_t reg_adress , uint8_t *data );
HAL_StatusTypeDef bmp280_read_registers(bmp280 * dev , uint8_t reg_adress , uint8_t *data  , uint8_t length);


HAL_StatusTypeDef bmp280_write_register(bmp280 * dev , uint8_t reg_adress , uint8_t *data  );





#endif
