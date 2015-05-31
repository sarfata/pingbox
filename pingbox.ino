// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_SSD1306.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_GFX.h"

Adafruit_SSD1306 oled(/* SID */ D0, /* SCK */ D1, /* DC */ D2, /* RST */ D3, /* CS */ D4);

#define PIXELS D5
#define NUM_PIXELS 4

SYSTEM_MODE(SEMI_AUTOMATIC);

uint32_t pixels[4];

void pixels_test() {
    pixels_send(pixels, sizeof(pixels));
}

void pixels_send(uint32_t *p, int num) {
    //__disable_irq();
}

void setup() {
    //pinMode(PIXELS, OUTPUT);
    //pixels_test();
    
    oled.begin(SSD1306_SWITCHCAPVCC);

    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.println("Hello, world!");
    delay(2000);
    
    loop();
    
    Spark.connect();
}


IPAddress testAddress = 0x08080808UL;


uint16_t pings[128];
uint16_t pings_index = 0;

void pings_add(int ping) {
    if (pings_index >= 127) {
        // scroll back one pixel
        memcpy(pings, &pings[1], sizeof(uint16_t) * 127);
    } else {
        pings_index++;
    }
    pings[pings_index] = ping;
}

uint16_t pings_max() {
    uint16_t max = 100;
    for (int i = 0; i < pings_index; i++) {
        if (pings[i] > max) {
            max = pings[i];
        }
    }
    return max;
}

int scale_value(int value, int max_value, int max_scale) {
    return value * max_scale / max_value;
}

void pings_draw(int x, int y, int height) {
    uint16_t max = pings_max();
    
    for (int i = 0; i < pings_index; i++) {
        if (pings[i] > 0) {
            oled.drawPixel(x + i, y + height - scale_value(pings[i], max, height), WHITE);
        }
    }
}

void loop()
{
    static int graph_column = 0;
    
    Serial.println("Looping...");
    if (WiFi.ready()) {
        oled.clearDisplay();
        oled.setCursor(0, 0);
        oled.setTextSize(2);
        
        char ssid[11];
        strncpy(ssid, WiFi.SSID(), sizeof(ssid));
        ssid[10] = 0;
        
        oled.print(ssid);
        
        oled.setCursor(0, 20);
        oled.setTextSize(1);
        oled.print("RSSI:");
        char rssi[10];
        // sadly no itoa available on Photon even with an extern
        snprintf(rssi, sizeof(rssi), "%i", WiFi.RSSI());
        oled.print(rssi);
        
        oled.setCursor(64, 20);
        //oled.setTextColor(1);
        oled.print("Ping:");
        int ping = -1;
        unsigned long time = millis();
        if (WiFi.ping(testAddress, 1) == 1) {
            ping = (int) (millis() - time);
        }
        
        char pong_str[10];
        if (ping != -1) {
            snprintf(pong_str, sizeof(pong_str), "%ims", ping);
            pings_add(ping);
            oled.print(pong_str);
        } else {
            pings_add(0);
            oled.print("KO");
        }
        
        pings_draw(0, 30, 33);
    }
    else {
        Serial.println("Connecting ...");
        oled.setCursor(2, 5);
        oled.println("Connecting...");
    }
    oled.display();
    delay(500);
}






