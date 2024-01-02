//
// Created by zuo-c on 2023/12/22.
//

#ifndef MSPC15M_MSPC15M_H
#define MSPC15M_MSPC15M_H


#define MSPC15M_IIC_ADDR 0X64

typedef struct {
    float C00;
    float C10;
    float C20;
    float C30;
    float C01;
    float C11;
    float C21;
    float C02;
    float C12;
} MSPC15M_OTP_Typedef;


void MSPC15M_WriteData(uint8_t i2c_addr, uint8_t Reg, uint8_t Value);
void MSPC15M_WriteData_SPI(uint8_t spi_addr, uint8_t data);
uint8_t MSPC15M_ReadData(uint8_t i2c_addr, uint8_t Reg);
HAL_StatusTypeDef MSPC15M_ReadBuffer(uint8_t i2c_addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length);
void MSPC15M_ReadBuffer_SPI(uint8_t spi_addr, uint8_t *data, uint8_t length);

void MSPC15M_Init(uint8_t i2c_addr, MSPC15M_OTP_Typedef *MSPC15M_OTP_Structure);
void MSPC15M_Init_SPI(MSPC15M_OTP_Typedef *MSPC15M_OTP_Structure)
void MSPC15M_Calc(int32_t Braw, int32_t Tadc, float *Press, float *Temperature, MSPC15M_OTP_Typedef *MSPC15M_OTP_Structure);

#endif //MSPC15M_MSPC15M_H
