/*
 * LCD_I2C.c
 *
 *  Created on: Oct 14, 2024
 *      Author: jacoubi
 */

#include "LCD_I2C.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

void I2C_Scan() {
    char info[] = "Scanning I2C bus...\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);

    HAL_StatusTypeDef res;
    for(uint16_t i = 0; i < 128; i++) {
        res = HAL_I2C_IsDeviceReady(&hi2c1, i << 1, 1, 10);
        if(res == HAL_OK) {
            char msg[64];
            snprintf(msg, sizeof(msg), "0x%02X", i);
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        } else {
            HAL_UART_Transmit(&huart2, (uint8_t*)".", 1, HAL_MAX_DELAY);
        }
    }

    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef LCD_SendInternal(uint8_t lcd_addr, uint8_t data, uint8_t flags) {
    HAL_StatusTypeDef res;
    for(;;) {
        res = HAL_I2C_IsDeviceReady(&hi2c1, lcd_addr, 1, HAL_MAX_DELAY);
        if(res == HAL_OK)
            break;
    }

    uint8_t up = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;

    uint8_t data_arr[4];
    data_arr[0] = up|flags|BACKLIGHT|PIN_EN;
    data_arr[1] = up|flags|BACKLIGHT;
    data_arr[2] = lo|flags|BACKLIGHT|PIN_EN;
    data_arr[3] = lo|flags|BACKLIGHT;

    res = HAL_I2C_Master_Transmit(&hi2c1, lcd_addr, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
    HAL_Delay(LCD_DELAY_MS);
    return res;
}

void LCD_SendCommand(uint8_t cmd) {
    LCD_SendInternal(LCD_ADDR, cmd, 0);
}

void LCD_SendData(uint8_t data) {
    LCD_SendInternal(LCD_ADDR, data, PIN_RS);
}

void LCD_Init() {
    // 4-bit mode, 2 lines, 5x7 format
    LCD_SendCommand(0b00110000);
    // display & cursor home (keep this!)
    LCD_SendCommand(0b00000010);
    // display on, right shift, underline off, blink off
    LCD_SendCommand(0b00001100);
    // clear display (optional here)
    LCD_SendCommand(0b00000001);
}

void LCD_SendString(char *str) {
	
	while(*str) {
        LCD_SendData((uint8_t)(*str));
        str++;
    }
}

void init() {
    I2C_Scan();
    LCD_Init(LCD_ADDR);
}

void LCD_SetCursor(uint8_t row, uint8_t column) {
    uint8_t address;

    // Calculate the address depending on the row (row 0 or row 1 for a 16x2 display)
    switch (row) {
        case 0:
            address = 0x00 + column;  // First row address
            break;
        case 1:
            address = 0x40 + column;  // Second row address
            break;
        // Add more cases if you are using a 20x4 or similar display with more rows
        default:
            address = 0x00 + column;  // Default to the first row if the row is invalid
            break;
    }

    // Send the DDRAM address command (Set DDRAM address is 0x80 + address)
    LCD_SendCommand(0x80 | address);
}

void LCD_Clear() {
    // Send the clear display command (0x01)
    LCD_SendCommand(0x01);
    HAL_Delay(2);  // Wait for 2 milliseconds
}

void LCD_ShiftText(const char *text, char direction, int delayMs, int durationMs) {

    char displayBuffer[20];  //16x2 LCD display + extra space for shift
    snprintf(displayBuffer, sizeof(displayBuffer), "  %s  ", text);  // Append some space to avoid sticking text 
    int len = strlen(text)+1;
    
    // Add blank spaces at the end of the string
    

    uint32_t startTime = HAL_GetTick();  // Get the starting time in milliseconds

    for (int shift = 0; ; shift++) {
        // Check if the elapsed time has exceeded the specified duration
        uint32_t elapsedTime = HAL_GetTick() - startTime;
        if (elapsedTime >= durationMs) {
            break;  // Stop shifting when the duration is exceeded
        }

        shift = shift % len;  // Normalize shift to avoid overflow

        // Clear the display before shifting
        LCD_Clear();

        // Perform the shifting based on direction
        if (direction == 'R' || direction == 'r') {  // Right shift
            for (int i = 0; i < 16; i++) {
                displayBuffer[i] = text[(len - shift + i) % len];  // Adjust index based on shift
            }
        } else if (direction == 'L' || direction == 'l') {  // Left shift
            for (int i = 0; i < 16; i++) {
                displayBuffer[i] = text[(shift + i) % len];
            }
        }

        displayBuffer[16] = '\0';  // Null=terminate the display buffer

        // Set the cursor to the beginning of the first line
        LCD_SetCursor(0, 0);

        // Write the shifted text to the LCD
        LCD_SendString(displayBuffer);

        // Add a delay to control the speed of the shift
        HAL_Delay(delayMs);
    }
}
