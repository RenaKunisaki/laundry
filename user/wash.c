#include "main.h"

void state_check_leds() {
    /** Pins on ports A and B are connected by photoresistor.
     *  This should produce a logical 0 when the light is
     *  strong and 1 when weak. (as the inputs have a pullup,
     *  and the photoresistor can outweigh it and pull them
     *  down when the light is strong enough.)
     *  This is kind of a hack because the ESP8266 ADC
     *  apparently can't actually be used.
     *  The order is unusual because the pins aren't numbered
     *  the same way on both sides.
     *  B0 -> A7, B1 -> A6...
     */
    //gpio16_output_set(0);
    static int whichLed = 0;
    static int prevState = -1, newState = -1;
    static int stateChangeCount = 0;
    static int isRead = 0;
    static int failCount = 0;

    if(!isRead) {
        gpio_enable(PIN_CH0, GPIO_OUTPUT);
        gpio_enable(PIN_CH1, GPIO_OUTPUT);
        gpio_enable(PIN_CH2, GPIO_OUTPUT);
        gpio_write(PIN_CH2, (whichLed >> 0) & 1);
        gpio_write(PIN_CH1, (whichLed >> 1) & 1);
        gpio_write(PIN_CH0, (whichLed >> 2) & 1);
        isRead = 1;
        //wait for ADC to stabilize
        return;
    }
    isRead = 0;

    //mcp23017_write(~(1 << (16-whichLed)));
    //mcp23017_write(g_config.ledBoardAddr, 0x0000);
    //uint16_t val = mcp23017_read(g_config.ledBoardAddr);
    //mcp23017_write(g_config.ledBoardAddr, 0xFFFF);
    //ets_intr_lock();
    //mcp23017_setPullup(0xFACE);
    //int p1 = mcp23017_readReg(MCP23017_REG_GPPUB);
    //int p2 = mcp23017_readReg(MCP23017_REG_GPPUA);
    //ets_intr_unlock();
    //os_printf("p=%02X %02X %04X\r\n", p1, p2, val);
    //val = (val & (1 << (7-whichLed))) ? 0 : 1;

    //wifi_set_opmode(NULL_MODE);
    system_soft_wdt_stop();
    ets_intr_lock( );
    //noInterrupts();
    uint32_t val = system_adc_read();
    //interrupts();
    ets_intr_unlock();
    system_soft_wdt_restart();

    if(g_config.ledBoardFlip)
        g_ledVal[(NUM_LEDS-1) - whichLed] = val;
    else g_ledVal[whichLed] = val;
    whichLed++;
    if(whichLed >= NUM_LEDS) {
        os_printf("LEDs: ");
        int i; for(i=0; i<NUM_LEDS; i++) {
            os_printf("%4d ", g_ledVal[i]);
        }
        os_printf("\r\n");
        g_state = STATE_CHECK_MOTION;
        whichLed = 0;
        newState = WASH_STATE_OFF;

        int ledState[NUM_LEDS];
        int numOn = 0;
        for(i=0; i<NUM_LEDS; i++) {
            if(g_ledVal[i] < g_config.ledOffSensitivity[i] * 4) {
                ledState[i] = -1; //off
            }
            else if(g_ledVal[i] > g_config.ledOnSensitivity[i] * 4) {
                ledState[i] =  1; //on
                numOn++;
            }
            else ledState[i] = 0; //unknown
        }


        //led 5 is the ambient light in the room.
        //led 6 is hardwired and should always read 1024.
        //led 7 is hardwired and should always read 0.
        //in practice, because of the shitty ADC, poor wiring,
        //and noise picked up by the wires, these do fluctuate a bit.
        if(g_ledVal[6] > 768 && g_ledVal[7] < 256) {
            //ignore obviously-wrong readings
            if(numOn > 1) newState = WASH_STATE_UNKNOWN;
            if     (ledState[LED_FILL ]>0) newState = WASH_STATE_FILLING;
            else if(ledState[LED_WASH ]>0) newState = WASH_STATE_WASHING;
            else if(ledState[LED_RINSE]>0) newState = WASH_STATE_RINSING;
            else if(ledState[LED_SPIN ]>0) newState = WASH_STATE_SPINNING;
            else if(ledState[LED_DONE ]>0) newState = WASH_STATE_DONE;
            else if(ledState[LED_FILL ]<0 && ledState[LED_WASH ]<0 &&
            ledState[LED_RINSE]<0 && ledState[LED_SPIN ]<0 &&
            ledState[LED_DONE ]<0) {
                newState = WASH_STATE_OFF;
            }
            failCount = 0;
        }
        else {
            failCount++;
            if(failCount == 16) {
                g_washState = WASH_STATE_SENSOR_FAIL;
                newState = g_washState;
            }
        }

        if(newState != g_washState) {
            if(newState == prevState) {
                stateChangeCount++;
                if(newState == WASH_STATE_OFF
                && stateChangeCount >= g_config.ledOffSensitivity[whichLed]) {
                    g_washState = newState;
                }
                else if(newState != WASH_STATE_OFF
                && stateChangeCount >= g_config.ledOnSensitivity[whichLed]) {
                    g_washState = newState;
                }
            }
            else stateChangeCount = 0;
            prevState = newState;
        }
    }
    //gpio16_output_set(1);
}
