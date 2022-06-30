/* Edge Impulse ingestion SDK
 * Copyright (c) 2021 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "src/syntiant.h"
#include <NDP.h>
#include <NDP_utils.h>
#include <Arduino.h>

#define NULL_CMD            0
#define OPEN_CMD            1
#define CLOSE_CMD           2
#define AUTO_CMD            3
#define SLEEP_CMD           4

uint32_t sys_cnt = 0;
uint8_t cmd_data = 0;           /*传递的模式*/
uint32_t now_cnt = 0;
uint32_t last_cnt = 0;
void app_loop()
{

}

void test_loop()
{
    sys_cnt++;


    if(!cmd_data)
    {
        now_cnt = sys_cnt;
        Serial.print("now:");
        Serial.println(now_cnt);

        Serial.print("last:");
        Serial.println(last_cnt);        
        if(now_cnt - last_cnt >= 20)     //3S
        {
            /*PRINTF*/
            Serial.println("---------------->");
            switch (cmd_data)
            {
            case OPEN_CMD:
                Serial.println("source_open lable");
                break;
            case CLOSE_CMD:
                Serial.println("source_close lable");
                break;
            case AUTO_CMD:
                Serial.println("source_auto lable");
                break;
            case SLEEP_CMD:
                Serial.println("source_sleep lable");
                break;
            default:
                break;
            }
            Serial.println(cmd_data);
            last_cnt = sys_cnt;
        }

        cmd_data = NULL_CMD;            /*清空模式*/
    }

    if(sys_cnt >= (0xFFFF -1))
    {
        sys_cnt = 0;
    }
    delay(500);
}

/**
 * @brief      Called when a inference matches 1 of the features
 *
 * @param[in]  event          The event
 * @param[in]  confidence     The confidence
 * @param[in]  anomaly_score  The anomaly score
 */
void on_classification_changed(const char *event, float confidence, float anomaly_score) {
    Serial.println(event);
    Serial2.println(event);

    
    // here you can write application code, e.g. to toggle LEDs based on keywords
    if (strcmp(event, "source_close") == 0) {
        cmd_data = CLOSE_CMD;
        // Serial.println("source_close lable");
    }

    if (strcmp(event, "source_open") == 0) {
        cmd_data = OPEN_CMD;
        // Serial.println("source_open lable");
    }

    if (strcmp(event, "source_auto") == 0) {
        cmd_data = AUTO_CMD;
        // Serial.println("source_auto lable");
    }

    if (strcmp(event, "source_sleep") == 0) {
        cmd_data = SLEEP_CMD;
        // Serial.println("source_sleep lable");
    }

    //test_loop();
            /*PRINTF*/
    Serial.println("---------------->");
    switch (cmd_data)
    {
    case OPEN_CMD:
        Serial.println("source_open lable");
        break;
    case CLOSE_CMD:
        Serial.println("source_close lable");
        break;
    case AUTO_CMD:
        Serial.println("source_auto lable");
        break;
    case SLEEP_CMD:
        Serial.println("source_sleep lable");
        break;
    default:
        break;
    }
    cmd_data =  NULL_CMD;
}



void setup(void)
{

    syntiant_setup();
}

void loop(void)
{
    syntiant_loop();
}


