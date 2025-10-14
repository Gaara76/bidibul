#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "TCA9554PWR.h"
#include "PCF85063.h"
#include "QMI8658.h"
#include "../components/lvgl__lvgl/lvgl.h"
#include "esp_log.h"
#include "ST7701S.h"
#include "GT911.h"
#include "SD_MMC.h"
#include "LVGL_Driver.h"
#include "LVGL_UI/ui.h"
#include "ui.h"
#include "ui_theme_manager.h"
#include "Wireless.h"
#include "BAT_Driver.h"
#include "ui_events.h"


void Driver_Loop(void *parameter)
{
    while(1)
    {
        QMI8658_Loop();
        //RTC_Loop();
        //BAT_Get_Volts();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}
void Driver_Init(void)
{
    Flash_Searching();
    BAT_Init();
    I2C_Init();
    PCF85063_Init();
    QMI8658_Init();
    EXIO_Init();                    // Example Initialize EXIO 
     
    xTaskCreatePinnedToCore(
        Driver_Loop, 
        "Other Driver task",
        4096, 
        NULL, 
        3, 
        NULL, 
        0);
      
}

// Tâche démarrée depuis main.c
void start_temperature_task(void) {
    xTaskCreatePinnedToCore(
        temperature_task,
        "temperature_task",
        8192,
        NULL,
        5,
        NULL,
        1
    );
}

void app_main(void)
{   
    Wireless_Init();
    Driver_Init();
    LCD_Init();
    Touch_Init();
    SD_Init();
    nvs_flash_init();
    wifi_connection();
    LVGL_Init();
    ui_init();
    lire_donnees_sdcard();
    ESP_ERROR_CHECK(nvs_flash_init());
    init_http_client();
    // tâche de mise à jour périodique HTTP
    start_temperature_task();

    
    /////  DEMARRAGE //////
    //Set_EXIO(TCA9554_EXIO8,true);
    //vTaskDelay(pdMS_TO_TICKS(1000));
    //Set_EXIO(TCA9554_EXIO8,false);

    while (1) {
        // raise the task priority of LVGL and/or reduce the handler period can improve the performance
        vTaskDelay(pdMS_TO_TICKS(10));
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_timer_handler();
        //lv_tick_inc(100);
        }

    
}
