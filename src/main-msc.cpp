
/**
 * This example demonstrates how to use the USB MSC (Mass Storage Class) library to emulate a USB flash drive
 * and write to the SD card interface of the FLASH NAND chip.
 */
#include "USB.h"
#include "SD.h"
#include "SPI.h"
#include "USBMSC.h"
#include "SDCardMultiSector.h"

/*
GIOO33: SD2/ NC: NOT CONNECTED
GPIO34: SD3/ CS: CHIP SELECT
GPIO35: CMD/ DI: DATA IN/ MOSI
GPIO36: CLK/ CLK: CLOCK
GPIO37: SD0/ DO: DATA OUT/ MISO
GPIO38: SD1/ NC: NOT CONNECTED
*/
#define SPI_SCK       36
#define SPI_MISO      37
#define SPI_MOSI      35
#define SD_CS         34

#define SDSLOT_CLK GPIO_NUM_36
#define SDSLOT_CMD GPIO_NUM_35
#define SDSLOT_DAT0 GPIO_NUM_37
#define SDSLOT_DAT1 GPIO_NUM_38
#define SDSLOT_DAT2 GPIO_NUM_33
#define SDSLOT_DAT3 GPIO_NUM_34

SDCard *card;
USBMSC msc;
volatile uint32_t writeCounter = 0;
volatile uint32_t readCounter = 0;
volatile uint64_t writeByteCounter = 0;
volatile uint64_t readByteCounter = 0;


static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
    return SD.writeRAW((uint8_t*)buffer, lba) ? 512 : -1;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
    return SD.readRAW((uint8_t*)buffer, lba)  ? 512 : -1;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  Serial.printf("StartStop: %d %d %d\n", power_condition, start, load_eject);
  return true;

}

void setup()
{

    Serial.begin(115200);
    Serial.setDebugOutput(true);

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS); 
    

    pinMode(SD_CS, OUTPUT ); 
    digitalWrite(SD_CS, HIGH); // Turn power off to NAND (reset)

    delay(300);

    digitalWrite(SD_CS, LOW); // Turn power on to NAND

    delay(5000);

    if ( !SD.begin(SD_CS /*, SPI, 4000000U, "/sd", 5, true*/) )
    {
        Serial.println("Card Mount Failed");
        return;
    }


    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    // keyboard.begin();
    msc.vendorID("ESP32");
    msc.productID("USB_MSC");
    msc.productRevision("1.0");
    msc.onRead(onRead);
    msc.onWrite(onWrite);
    msc.onStartStop(onStartStop);
    msc.mediaPresent(true);
    msc.begin(SD.cardSize() / 512, 512); //?
    //msc.begin(card->getSectorCount(), card->getSectorSize());
    USB.begin();
}

void loop()
{
  if (readCounter > 0 || writeCounter > 0) {
    log_e("Read: %d, Write: %d, readBytes: %d, writeBytes: %d\n", readCounter, writeCounter, readByteCounter, writeByteCounter);
    readCounter = 0;
    writeCounter = 0;
    readByteCounter = 0;
    writeByteCounter = 0;
  }
  delay(1000);
  log_e("Time: %d", millis());
}