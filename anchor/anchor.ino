/*
   ESP32 UWB Anchor
*/

#include <SPI.h>
#include "DW1000Ranging.h"

#define ANCHOR_ADD "82:17:5B:D5:A9:9A:E2:9C"

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;
const uint8_t PIN_SS  = 4;

#define DRONE_OFFSET 0.0148
#define WARMUP_TIME 5000

unsigned long startTime;
unsigned long windowStart = 0;

float sumRange = 0;
float sumRx = 0;
int sampleCount = 0;

void inactiveDevice(DW1000Device *device)
{
  Serial.print("#inactive:");
  Serial.println(device->getShortAddress(), HEX);
}

void newRange()
{
  if (millis() - startTime < WARMUP_TIME)
    return;

  DW1000Device* dev = DW1000Ranging.getDistantDevice();

  float r  = dev->getRange();
  float rx = dev->getRXPower();

  if (r <= 0 || r > 100)
    return;

  float corrected = r - DRONE_OFFSET;

  // เริ่ม window ใหม่
  if (windowStart == 0)
    windowStart = millis();

  // สะสมค่า
  sumRange += corrected;
  sumRx += rx;
  sampleCount++;

  // ครบ 1 วินาที
  if (millis() - windowStart >= 1000)
  {
    if (sampleCount > 0)
    {
      float avgRange = (sumRange / sampleCount) - 0.75;
      float avgRx = sumRx / sampleCount;

      Serial.print(windowStart / 1000);   // esp time (sec)
      Serial.print(",");
      Serial.print(dev->getShortAddress(), HEX);
      Serial.print(",");
      Serial.print(avgRange, 3);
      Serial.print(",");
      Serial.println(avgRx, 2);
    }

    // reset window
    sumRange = 0;
    sumRx = 0;
    sampleCount = 0;
    windowStart = millis();
  }
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);

  // DW1000.setAntennaDelay(16446);

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  DW1000Ranging.useRangeFilter(true);

  DW1000Ranging.startAsAnchor(
    ANCHOR_ADD,
    DW1000.MODE_LONGDATA_RANGE_LOWPOWER
  );

  startTime = millis();

  // Serial.println("esp_time,tag_id,range_m,rx_power_dbm");
}

void loop()
{
  DW1000Ranging.loop();
}