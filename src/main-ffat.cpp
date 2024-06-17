/**
 * This example shows how to use the FLASH NAND chip together with LittleFS 
 * library to read/write files from/to the SD card interface.
 */

#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>
#include "SPI.h"
#include "SD.h"

#include "ff.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "vfs_fat_internal.h"
#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "diskio_impl.h"

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



boolean listDir(fs::FS &fs, const char * dirname, uint8_t levels, bool monitor){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.print( F( "Failed to open directory w: " ) );
        Serial.println( dirname );
        return false;
    }
    if(!root.isDirectory()){
        Serial.println( F( "Not a directory" ) );
        return false;
    }

    Serial.print( "root = " );
    Serial.println( root.path() );

    File file = root.openNextFile();

    Serial.print( "file = " );
    Serial.println( file );
    Serial.print( "file.path = " );
    Serial.println( file.path() );

    while ( file )
    {
        if ( file.isDirectory() )
        {
          if ( monitor )
          {
            Serial.print( F( "  DIR : " ) );
            Serial.println(file.name());
          }
          if(levels)
          {
            String fname2 = file.name();
            String fname3 = "/" + fname2;
            listDir(fs, fname3.c_str(), levels -1, monitor);
          }
        } 
        else
        {
          if ( monitor )
          {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
          }
        }
        file = root.openNextFile();
    }

    root.close();
    return true;
}

esp_err_t formatSD()
{
    FRESULT res = FR_OK;
    esp_err_t err = ESP_OK;
    const size_t workbuf_size = 4096;
    void *workbuf = NULL;
    log_e( "partitioning card");

    workbuf = ff_memalloc(workbuf_size);
    if (workbuf == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    DWORD plist[] = {100, 0, 0, 0};
    res = f_fdisk(0, plist, workbuf);
    if (res != FR_OK)
    {
        err = ESP_FAIL;
        log_e("f_fdisk failed (%d)", res);
    }
    else
    {
        size_t alloc_unit_size = 512;
        //ESP_LOGW(TAG, "formatting card, allocation unit size=%d", alloc_unit_size);
        res = f_mkfs("", FM_FAT, alloc_unit_size, workbuf, workbuf_size);
        if (res != FR_OK)
        {
            err = ESP_FAIL;
            log_e("f_mkfs failed (%d)", res);
        }
    }

    log_e("partitioning card finished");
    free(workbuf);
    return err;
}

void setup(){
  Serial.begin(115200);
  
  delay(3000);
  Serial.println("Starting");

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, -1);  
  pinMode(SD_CS, OUTPUT ); 
  digitalWrite(SD_CS, HIGH); // Turn power off to NAND (reset)

  delay(3000);

  digitalWrite(SD_CS, LOW); // Turn power on to NAND


  delay(1000);

  if ( !SD.begin( SD_CS ) )
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

  listDir(SD, "/", 100, true);
  Serial.println("-end-");
}

void loop()
{
}