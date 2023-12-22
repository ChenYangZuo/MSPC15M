//
// Created by zuo-c on 2023/12/22.
//
#include "main.h"
#include "MSPC15M.h"
#include "i2c.h"
#include "stdio.h"


void MSPC15M_WriteData(uint8_t i2c_addr, uint8_t Reg, uint8_t Value) {
    HAL_StatusTypeDef status;

    uint8_t addr = i2c_addr << 1; // I2C写地址转换

    status = HAL_I2C_Mem_Write(&hi2c1, addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 0x10);

    if (status != HAL_OK) {
        printf("I2C1_Error()\n");
    }
}

uint8_t MSPC15M_ReadData(uint8_t i2c_addr, uint8_t Reg) {
    HAL_StatusTypeDef status;

    uint8_t value = 0;
    uint8_t addr = i2c_addr << 1 | 1; // I2C读地址转换

    status = HAL_I2C_Mem_Read(&hi2c1, addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 0x10);

    if (status != HAL_OK) {
        printf("I2C1_Error()\n");
    }

    return value;
}

HAL_StatusTypeDef MSPC15M_ReadBuffer(uint8_t i2c_addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t addr = 0;

    addr = i2c_addr << 1 | 1; // I2C读地址转换
    status = HAL_I2C_Mem_Read(&hi2c1, addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Length, 0x10);

    // 检测I2C通讯状态
    if (status != HAL_OK) {
        // 调用I2C通讯出错误处理函数
        printf("I2C1_Error()\n");
    }
    return status;
}

void MSPC15M_Init(uint8_t i2c_addr, MSPC15M_OTP_Typedef *MSPC15M_OTP_Structure) {
    uint8_t temp_data[18];
    volatile int32_t temp;

    /*INIT_REGCONFIG*/
    MSPC15M_WriteData(i2c_addr, 0x11, 0x60);
    MSPC15M_WriteData(i2c_addr, 0x12, 0x62);
    MSPC15M_WriteData(i2c_addr, 0x5A, 0x35);
    MSPC15M_WriteData(i2c_addr, 0x5B, 0xB3);

    /*READOTPREG*/
    MSPC15M_ReadBuffer(i2c_addr, 0x14, temp_data, 18);
    // c00
    temp = temp_data[0] << 8 | temp_data[1];
    MSPC15M_OTP_Structure->C00 = temp * (-10.0f);

    // c10
    temp = temp_data[2] << 8 | temp_data[3];
    MSPC15M_OTP_Structure->C10 = temp * (2.0f);

    // c20
    temp = temp_data[5] << 8 | temp_data[6];
    if (temp > 32767)
        MSPC15M_OTP_Structure->C20 = (temp - 65535.0) * (1.0f / 1000.0f);
    else
        MSPC15M_OTP_Structure->C20 = temp * (1.0f / 1000.0f);

    // c30
    temp = temp_data[7] << 8 | temp_data[9];
    if (temp > 32767)
        MSPC15M_OTP_Structure->C30 = (temp - 65535.0) * (1.0f / 10000.0f);
    else
        MSPC15M_OTP_Structure->C30 = temp * (1.0f / 10000.0f);

    // c01
    temp = temp_data[10] << 8 | temp_data[11];
    MSPC15M_OTP_Structure->C01 = temp * (32.0f) - (1048560.0f);

    // c11
    temp = (temp_data[8] & 0X3f) << 8 | temp_data[12];
    MSPC15M_OTP_Structure->C11 = temp * (2.0f);

    // c21
    temp = (temp_data[4] & 0X3f) << 8 | temp_data[13];
    MSPC15M_OTP_Structure->C21 = temp * (-1.0f / 1000.0f);

    // c02
    temp = (temp_data[14] & 0X0f) << 8 | temp_data[15];
    MSPC15M_OTP_Structure->C02 = temp * (-20.0f);

    // c12
    temp = temp_data[16] << 8 | temp_data[17];
    if (temp > 32767)
        MSPC15M_OTP_Structure->C12 = (temp - 65535.0) * (1.0f / 10.0f);
    else
        MSPC15M_OTP_Structure->C12 = temp * (1.0f / 10.0f);
}

void MSPC15M_Calc(int32_t Braw, int32_t Tadc, float *Press, float *Temperature, MSPC15M_OTP_Typedef *MSPC15M_OTP_Structure) {
    float Praw, Traw, T;
    float p1, p2, p3, p4, p5, p6, p7, p8, p9;

    if (Braw > 8388608) {
        Braw -= 16777216;
    }

    if (Tadc > 16384) {
        Tadc -= 32768;
    }

    T = Tadc / 128.0f;
    Traw = T / 128.0f;
    Praw = Braw / 131072.0f;
    // printf("Praw = %.3f,Traw = %.3f\n",Praw,Traw);

    *Press = MSPC15M_OTP_Structure->C00
            + MSPC15M_OTP_Structure->C10 * Praw
            + MSPC15M_OTP_Structure->C20 * Praw * Praw
            + MSPC15M_OTP_Structure->C30 * Praw * Praw * Praw
            + MSPC15M_OTP_Structure->C01 * Traw
            + MSPC15M_OTP_Structure->C11 * Traw * Praw
            + MSPC15M_OTP_Structure->C21 * Praw * Praw * Traw
            + MSPC15M_OTP_Structure->C02 * Traw * Traw
            + MSPC15M_OTP_Structure->C12 * Praw * Traw * Traw;

    *Temperature = 1.1307 * 128 * Traw + 10.428;
}
