/*
 * LCD_I2C.h
 *
 *  Created on: Oct 14, 2024
 *      Author: jacoubi
 */

#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_

#include "stm32c0xx_hal.h"
#include <string.h>

extern LCD_ADDR_CONFIG; // address configuration


#define LCD_ADDR (LCD_ADDR_CONFIG << 1)

#define PIN_RS    (1 << 0)
#define PIN_EN    (1 << 2)
#define BACKLIGHT (1 << 3)

#define LCD_DELAY_MS 5


void I2C_Scan();
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Init();
void LCD_SendString(char *str);
void init();
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Clear();
void LCD_ShiftText(const char *text, char direction, int delayMs, int durationMs); // direction R,r for right and L,l for left
#endif /* INC_LCD_I2C_H_ */
