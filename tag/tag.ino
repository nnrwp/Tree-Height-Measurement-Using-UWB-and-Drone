/*
   ESP32 UWB Tag (Drone Side)
*/

#include <SPI.h>
#include "DW1000Ranging.h"

#define TAG_ADD "7D:00:22:EA:82:60:3B:9C"

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;
const uint8_t PIN_SS  = 4;

void dummyRange() {
  // ต้องมี callback เพื่อให้ state machine ทำงานครบ
}

void newDevice(DW1000Device *device)
{
  Serial.print("Connected: ");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
  Serial.print("Inactive: ");
  Serial.println(device->getShortAddress(), HEX);
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);

  // ใส่ค่าที่ calibrate แล้ว
  // DW1000.setAntennaDelay(16446);

  DW1000Ranging.attachNewRange(dummyRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  DW1000Ranging.useRangeFilter(true);

  DW1000Ranging.startAsTag(
    TAG_ADD,
    DW1000.MODE_LONGDATA_RANGE_LOWPOWER
  );
}

void loop()
{
  DW1000Ranging.loop();
}