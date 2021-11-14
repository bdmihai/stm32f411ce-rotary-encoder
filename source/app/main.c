/*_____________________________________________________________________________
 │                                                                            |
 │ COPYRIGHT (C) 2021 Mihai Baneu                                             |
 │                                                                            |
 | Permission is hereby  granted,  free of charge,  to any person obtaining a |
 | copy of this software and associated documentation files (the "Software"), |
 | to deal in the Software without restriction,  including without limitation |
 | the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
 | and/or sell copies  of  the Software, and to permit  persons to  whom  the |
 | Software is furnished to do so, subject to the following conditions:       |
 |                                                                            |
 | The above  copyright notice  and this permission notice  shall be included |
 | in all copies or substantial portions of the Software.                     |
 |                                                                            |
 | THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
 | OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
 | MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
 | IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
 | CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
 | OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
 | THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
 |____________________________________________________________________________|
 |                                                                            |
 |  Author: Mihai Baneu                           Last modified: 21.Jan.2021  |
 |                                                                            |
 |___________________________________________________________________________*/

#include "stm32f4xx.h"
#include "stm32rtos.h"
#include "task.h"
#include "queue.h"
#include "system.h"
#include "gpio.h"
#include "i2c.h"
#include "st7066u.h"
#include "printf.h"
#include "rencoder.h"

#define EEPROM_SIZE         512
#define EEPROM_PAGE_SIZE    16
#define EEPROM_I2C_ADDRESS  0b01010000

TaskHandle_t xDisplayTaskHandle;

static void vTaskLED(void *pvParameters)
{
    (void)pvParameters;

    /* led OFF */
    gpio_set_blue_led();

    for (;;) {
        gpio_reset_blue_led();
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        gpio_set_blue_led();
        vTaskDelay(100 / portTICK_PERIOD_MS);

        gpio_reset_blue_led();
        vTaskDelay(100 / portTICK_PERIOD_MS);

        gpio_set_blue_led();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static void updateDisplay(uint16_t counter)
{
    // calculate the read address of the eeprom
    uint8_t address = EEPROM_I2C_ADDRESS + (uint8_t)(counter >> 8);
    uint16_t nread, nwrite;
    char row1_txt[17] = {0}, row2_txt[17] = {0};

    // read i2c eeprom
    nwrite = i2c_write(address, (uint8_t *)&counter, 1);
    if (nwrite != 1) {
        sprintf(row1_txt, "%s [%d]", "Write error", nread);
    } else {
        nread = i2c_read(EEPROM_I2C_ADDRESS, (uint8_t *)row1_txt, 16);
        if (nread != 16) {
            sprintf(row1_txt, "%s [%d]", "Read error", nread);
        }
    }

    // loop around the eeprom
    counter += 16;
    if (counter >= EEPROM_SIZE) {
        counter = 0;
    }

    // read i2c eeprom
    nwrite = i2c_write(address, (uint8_t *)&counter, 1);
    if (nwrite != 1) {
        sprintf(row2_txt, "%s [%d]", "Write error", nread);
    } else {
        nread = i2c_read(EEPROM_I2C_ADDRESS, (uint8_t *)row2_txt, 16);
        if (nread != 16) {
            sprintf(row2_txt, "%s [%d]", "Read error", nread);
        }
    }

    // display
    st7066u_cmd_clear_display();
    st7066u_write_str(row1_txt);
    st7066u_cmd_set_ddram(0x40);
    st7066u_write_str(row2_txt);
}

static void vTaskDisplay(void *pvParameters)
{
    (void)pvParameters;

    st7066u_hw_control_t hw = {
        .config_control_out  = gpio_config_control_out,
        .config_data_out     = gpio_config_data_out,
        .config_data_in      = gpio_config_data_in,
        .e_high              = gpio_e_high,
        .e_low               = gpio_e_low,
        .rs_high             = gpio_rs_high,
        .rs_low              = gpio_rs_low,
        .data_wr             = gpio_data_wr,
        .data_rd             = gpio_data_rd,
        .delay_us            = delay_us
    };

    st7066u_init(hw);

    st7066u_cmd_function_set(ST7066U_8_BITS_DATA, ST7066U_2_LINE_DISPLAY, ST7066U_5x8_SIZE);
    st7066u_cmd_on_off(ST7066U_DISPLAY_ON, ST7066U_CURSOR_OFF, ST7066U_CURSOR_POSITION_OFF);
    st7066u_cmd_clear_display();
    st7066u_cmd_entry_mode(ST7066U_INCREMENT_ADDRESS, ST7066U_SHIFT_DISABLED);

    st7066u_write_str("    Welcome!    ");
    st7066u_cmd_set_ddram(0x40);
    st7066u_write_str("I2C EEPROM  Demo");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    updateDisplay(0);
    for (;;) {
        rencoder_output_event_t event;
        if (xQueueReceive(rencoder_output_queue, &event, portMAX_DELAY) == pdPASS) {
            updateDisplay(event.position * 16);
        }
    }
}

int main(void)
{
    /* initialize the system */
    system_init();

    /* initialize the gpio */
    gpio_init();

    /* initialize the interupt service routines */
    isr_init();

    /* initialize the i2c interface */
    i2c_init();

    rencoder_init(0, (EEPROM_SIZE - 32) / 16);

    /* create the tasks specific to this application. */
    xTaskCreate(vTaskLED, "vTaskLED", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(vTaskDisplay, "vTaskDisplay", configMINIMAL_STACK_SIZE*2, NULL, 2, &xDisplayTaskHandle);
    xTaskCreate(rencoder_run, "vTaskEncoder", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    /* start the scheduler. */
    vTaskStartScheduler();

    /* should never get here ... */
    blink(10);
    return 0;
}
