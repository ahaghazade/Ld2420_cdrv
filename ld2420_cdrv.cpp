/**
 ******************************************************************************
 * @file           : ld2420_cdrv.c
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 DiodeGroup.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component
 *
 *
 ******************************************************************************
 * @verbatim
 * @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "ld2420_cdrv.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static ld2420_res_t fLd2420_CompareArrays(uint8_t *a, uint8_t *b, size_t len, size_t *ignoreIndices = nullptr, size_t ignoreCount = 0);
static ld2420_res_t fLd2420_ReadResponse(uint8_t *expected, size_t len, size_t *ignoreIndices = nullptr, size_t ignoreCount = 0, unsigned long timeout = LD2420_RESPONOSE_TIMOUT_MS);
static void fLd2420_SendCommand(uint8_t *cmd, size_t len);
static ld2420_res_t fLd2420_SendAndReadCommand(uint8_t *cmd, size_t cmdLen, uint8_t *expected, size_t respLen, size_t *ignoreIndices = nullptr, size_t ignoreCount = 0, int request_try = LD2420_REQUEST_RETRY);
static ld2420_res_t fLd2420_EnterCMDMode();
static ld2420_res_t fLd2420_ExitCMDMode();
static ld2420_res_t fLd2420_SetThresh(uint16_t add, uint16_t value, bool up);
static int fLd2420_ReadThresh(uint16_t add , bool up, int request_retry = LD2420_REQUEST_RETRY , int request_timeout = LD2420_RESPONOSE_TIMOUT_MS);
static ld2420_res_t fLd2420_SetMaxDistance(uint16_t distance);
static ld2420_res_t fLd2420_SetReportDelay(uint16_t delay);
static int fLd2420_ReadMaxDistance(int request_retry = LD2420_REQUEST_RETRY , int request_timeout = LD2420_RESPONOSE_TIMOUT_MS);
static int fLd2420_ReadReportDelay(int request_retry = LD2420_REQUEST_RETRY , int request_timeout = LD2420_RESPONOSE_TIMOUT_MS);
static ld2420_res_t fLd2420_InitialABDThresh();

/* Variables -----------------------------------------------------------------*/
static uint8_t enterCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t enterResp[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x08, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t exitCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xFE, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t exitResp[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFE, 0x01, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
 
static uint8_t setMaxDisCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x08, 0x00, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t setABDResp[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0x07, 0x01, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t readMaxDisCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0x08, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t readMaxDisResp[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x08, 0x00, 0x08, 0x01, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t setDelayCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x14, 0x00, 0x07, 0x00, 0x02, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x00, 0x64, 0x00, 0x00, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t setDelayResp[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0x07, 0x01, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t readDelayCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x08, 0x00, 0x08, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t readDelayResp[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x10, 0x00, 0x08, 0x01, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t setThreshCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x08, 0x00, 0x07, 0x00, 0x10, 0x00, 0x60, 0xEA, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t readThreshCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint8_t readThreshResp[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x08, 0x00, 0x08, 0x01, 0x00, 0x00, 0x60, 0xEA, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
static uint16_t upperThreshValues[16] = {60000, 30000, 20000, 10000, 500, 400, 400, 300, 300, 300, 300, 250, 250, 200, 200, 200};
static uint16_t lowerThreshValues[16] = {40000, 20000, 10000, 500, 300, 200, 200, 150, 150, 100, 100, 100, 100, 100, 100, 100};

/*
╔═════════════════════════════════════════════════════════════════════════════════╗
║                          ##### Exported Functions #####                         ║
╚═════════════════════════════════════════════════════════════════════════════════╝*/

ld2420_res_t fLd2420_Init() {

  return fLd2420_InitialABDThresh();
}


ld2420_res_t fLd2420_ConfigABDParams(uint8_t config, int value) {

  bool ConfigSuccess = false;
  //Enter Command Mode
  Serial.println("\n>>> Sending Enter Command Mode...");
  if(fLd2420_EnterCMDMode() == LD2420_RES_OK) {

    Serial.println(":::: Enter Command Mode successful");
    switch (config)
    {
      //
      case 0: //set min dis
        Serial.println(":::: Set min distance successful");
        break;
      
      case 1: //set max dis
        if (fLd2420_SetMaxDistance(value)) 
        {
          Serial.println(":::: Set max distance successful");
          //Read Max Distance
          Serial.println("\n>>> Sending Read Max Distance...");
          int returnedValue = fLd2420_ReadMaxDistance();
          if(returnedValue == value)
            ConfigSuccess = true;
        } 
        else 
          Serial.println("Set Max Distance failed");

        break;
      
      case 2: //set time
      if(fLd2420_SetReportDelay(value))
      {
        Serial.println(":::: Set report delay successful");
        Serial.println("\n>>> Sending Read report delay...");
        int returnedValue = fLd2420_ReadReportDelay();
        if(returnedValue == value)
          ConfigSuccess = true;
      }
      else
        Serial.println("Set report delay failed");
        
        break;

    }
    Serial.println("\nSending Exit Command Mode...");
    if(fLd2420_ExitCMDMode() == LD2420_RES_OK) 
      Serial.println(":::: Exit Command Mode successful");
    else 
      Serial.println("Exit Command Mode failed after retries");
  }

  if(!ConfigSuccess) {
    return LD2420_RES_CONFIG_ABB_PARAM_FAIL;
  }
  return LD2420_RES_OK;
}

/*
╔═════════════════════════════════════════════════════════════════════════════════╗
║                            ##### Private Functions #####                        ║
╚═════════════════════════════════════════════════════════════════════════════════╝*/


static ld2420_res_t fLd2420_CompareArrays(uint8_t *a, uint8_t *b, size_t len, size_t *ignoreIndices, size_t ignoreCount) {

  for(size_t i = 0; i < len; i++) {

    bool ignore = false;
    for(size_t j = 0; j < ignoreCount; j++) {
    
      if(i == ignoreIndices[j]) {

        ignore = true;
        break;
      }
    }
    if(!ignore && a[i] != b[i]) {
      return LD2420_RES_COMPARE_ARRAYS_NOT_EQUAL;
    }
  }
  return LD2420_RES_OK;
}

static ld2420_res_t fLd2420_ReadResponse(uint8_t *expected, size_t len, size_t *ignoreIndices, size_t ignoreCount, unsigned long timeout) {
  
  uint8_t buffer[32];
  size_t index = 0;
  unsigned long start = millis();
  bool headerFound = false;
  uint8_t header[2] = {0xFD, 0xFC};

  // Wait for FD FC header
  while(millis() - start < timeout) {

    if(Serial1.available()) {
    
      uint8_t byte = Serial1.read();
      if(!headerFound) {
    
        if(byte == header[index]) {
    
          index++;
          if(index == 2) { // Full header FD FC received
    
            headerFound = true;
            index = 0;
            buffer[0] = header[0];
            buffer[1] = header[1];
            index = 2;
          }
        } else {
          index = 0; // Reset if header sequence breaks
        }
      } else if(index < len) {
        buffer[index++] = byte;
      }
      if (index == len) {
        break; // Full response received
      }
    }
  }

  if(index != len) {

    Serial.print("Response length mismatch: expected ");
    Serial.print(len);
    Serial.print(", got ");
    Serial.println(index);
    return LD2420_RES_READ_RESPONSE_FAIL;
  }

  return fLd2420_CompareArrays(buffer, expected, len, ignoreIndices, ignoreCount);
}

static void fLd2420_SendCommand(uint8_t *cmd, size_t len) {
  Serial1.write(cmd, len);
}

static ld2420_res_t fLd2420_SendAndReadCommand(uint8_t *cmd, size_t cmdLen, uint8_t *expected, size_t respLen, size_t *ignoreIndices, size_t ignoreCount, int request_try) {

  for (int retry = 0; retry < request_try; retry++) {

    fLd2420_SendCommand(cmd, cmdLen);
    if (fLd2420_ReadResponse(expected, respLen, ignoreIndices, ignoreCount) == LD2420_RES_OK) {
      return LD2420_RES_OK;
    }
    Serial.print("Retry ");
    Serial.print(retry + 1);
    Serial.println(" for command...");
    delay(100); // Short delay between retries
  }
  return LD2420_RES_SEND_AND_READ_FAIL;
}

static ld2420_res_t fLd2420_EnterCMDMode() {

  size_t ignoreIndices[] = {12, 13}; // Ignore bytes 12-13 in enterResp
  if (fLd2420_SendAndReadCommand(enterCmd, sizeof(enterCmd), enterResp, sizeof(enterResp), ignoreIndices, sizeof(ignoreIndices)) != LD2420_RES_OK) {
    return LD2420_RES_ENTER_CMD_MODE_FAIL;
  }else { 
    return LD2420_RES_OK;
  }
}

static ld2420_res_t fLd2420_ExitCMDMode() {

  if(fLd2420_SendAndReadCommand(exitCmd, sizeof(exitCmd), exitResp, sizeof(exitResp)) != LD2420_RES_OK) {
  return LD2420_RES_EXIT_CMD_MODE_FAIL;
  } else { 
    return false;
  }
}

static ld2420_res_t fLd2420_SetThresh(uint16_t add, uint16_t value, bool up) {

  if(up) add += 16; // 0x10 first add of upper thresh add
  else add += 32;   // 0x20 first add of lower thresh add

  setThreshCmd[8] = add & 0xFF;        // Low byte
  setThreshCmd[9] = (add >> 8) & 0xFF; // High byte

  setThreshCmd[10] = value & 0xFF;        // Low byte
  setThreshCmd[11] = (value >> 8) & 0xFF; // High byte

  if(fLd2420_SendAndReadCommand(setThreshCmd, sizeof(setThreshCmd), setABDResp, sizeof(setABDResp), nullptr, 0) != LD2420_RES_OK) {
    return LD2420_RES_SET_TRESH_VALUE_FAIL;
  }else { 
    return LD2420_RES_OK;
  }
}

static int fLd2420_ReadThresh(uint16_t add, bool up, int request_retry , int request_timeout) {

  uint8_t response[32]; //{0xFD, 0xFC, 0xFB, 0xFA, 0x08, 0x00, 0x08, 0x01, 0x00, 0x00, 0x60, 0xEA, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};

  if(up) add += 16; // 0x10 first add of upper thresh add
  else add += 32;   // 0x20 first add of lower thresh add

  readThreshCmd[8] = add & 0xFF;        // Low byte
  readThreshCmd[9] = (add >> 8) & 0xFF; // High byte

  size_t index = 0;
  for (int retry = 0; retry < request_retry; retry++) 
  {
    fLd2420_SendCommand(readThreshCmd, sizeof(readThreshCmd));
    index = 0;
    unsigned long start = millis();
    bool headerFound = false;
    uint8_t header[2] = {0xFD, 0xFC};

    while (millis() - start < request_timeout && index < sizeof(readThreshResp)) {
      if (Serial1.available()) {
        uint8_t byte = Serial1.read();
        if (!headerFound) {
          if (byte == header[index]) {
            index++;
            if (index == 2) {
              headerFound = true;
              index = 0;
              response[0] = header[0];
              response[1] = header[1];
              index = 2;
            }
          } else {
            index = 0;
          }
        } else if (index < sizeof(readThreshResp)) {
          response[index++] = byte;
        }
      }
    }

    size_t ignoreIndices[] = {10, 11, 12, 13}; //distance values 4 byte
    if (index == sizeof(readThreshResp) && fLd2420_CompareArrays(response, readThreshResp, sizeof(readThreshResp), ignoreIndices, sizeof(ignoreIndices))) {
      // Check success/failure (bytes 8-9: 00 00 for success)
      if (response[8] == 0x00 && response[9] == 0x00) {
        Serial.println("Operation successful");
        // Parse max distance value (bytes 10-13, little-endian)
        uint32_t ThreshValue = (response[13] << 24) | (response[12] << 16) | (response[11] << 8) | response[10];
        Serial.print("Read Thresh successful, Value: ");
        Serial.println(ThreshValue);
        return ThreshValue;
        
      } else {
        Serial.println("Read Operation failed");
        continue;
      }
    } 
    else 
    {
      Serial.print("Retry ");
      Serial.print(retry + 1);
      Serial.println(" for Read Thresh Value...");
      delay(100);
    }
  }

  if (index != sizeof(readThreshResp)) {
    Serial.println("Read Thresh Value failed after retries");
    return -1;
  }
} 

static ld2420_res_t fLd2420_SetMaxDistance(uint16_t distance) {

  // Set add in little-endian (bytes 10-11) 
  setMaxDisCmd[10] = distance & 0xFF;        // Low byte
  setMaxDisCmd[11] = (distance >> 8) & 0xFF; // High byte

  // Send command with retries
  Serial.print("\n>>> Sending Set Max Distance (");
  Serial.print(distance);
  Serial.println(" meters)...");

  if(fLd2420_SendAndReadCommand(setMaxDisCmd, sizeof(setMaxDisCmd), setABDResp, sizeof(setABDResp), nullptr, 0) != LD2420_RES_OK) {
    return LD2420_RES_SET_MAX_DISTANCE_FAIL;
  }else { 
    return LD2420_RES_OK;
  }
}

static ld2420_res_t fLd2420_SetReportDelay(uint16_t delay) {  

  // Set distance in little-endian (bytes 10-11)
  setDelayCmd[22] = delay & 0xFF;        // Low byte
  setDelayCmd[23] = (delay >> 8) & 0xFF; // High byte

  // Send command with retries
  Serial.print(">>> Sending Set Report Delay (");
  Serial.print(delay);
  Serial.println(" seconds)...");

  if(fLd2420_SendAndReadCommand(setDelayCmd, sizeof(setDelayCmd), setDelayResp, sizeof(setDelayResp), nullptr, 0) != LD2420_RES_OK) {
    return LD2420_RES_SET_REPORT_DELAY_FAIL;
  }else { 
    return LD2420_RES_OK;
  }
}

static int fLd2420_ReadMaxDistance(int request_retry , int request_timeout) {

  uint8_t response[32];
  size_t index = 0;
  for (int retry = 0; retry < request_retry; retry++) 
  {
    fLd2420_SendCommand(readMaxDisCmd, sizeof(readMaxDisCmd));
    index = 0;
    unsigned long start = millis();
    bool headerFound = false;
    uint8_t header[2] = {0xFD, 0xFC};

    while (millis() - start < request_timeout && index < sizeof(readMaxDisResp)) {
      if (Serial1.available()) {
        uint8_t byte = Serial1.read();
        if (!headerFound) {
          if (byte == header[index]) {
            index++;
            if (index == 2) {
              headerFound = true;
              index = 0;
              response[0] = header[0];
              response[1] = header[1];
              index = 2;
            }
          } else {
            index = 0;
          }
        } else if (index < sizeof(readMaxDisResp)) {
          response[index++] = byte;
        }
      }
    }

    size_t ignoreIndices[] = {10, 11, 12, 13}; //distance values 4 byte
    if (index == sizeof(readMaxDisResp) && fLd2420_CompareArrays(response, readMaxDisResp, sizeof(readMaxDisResp), ignoreIndices, sizeof(ignoreIndices))) {
      // Check success/failure (bytes 8-9: 00 00 for success)
      if (response[8] == 0x00 && response[9] == 0x00) {
        Serial.println("Operation successful");
        // Parse max distance value (bytes 10-13, little-endian)
        uint32_t maxDis = (response[13] << 24) | (response[12] << 16) | (response[11] << 8) | response[10];
        Serial.print("Read Max Distance successful: ");
        Serial.print(maxDis);
        Serial.println(" meters");
        return maxDis;
        
      } else {
        Serial.println("Read Operation failed");
        continue;
      }
    } 
    else 
    {
      Serial.print("Retry ");
      Serial.print(retry + 1);
      Serial.println(" for Read Max Distance...");
      delay(100);
    }
  }

  if (index != sizeof(readMaxDisResp)) {
    Serial.println("Read Max Distance failed after retries");
    return -1;
  }
} 

static int fLd2420_ReadReportDelay(int request_retry , int request_timeout) {

  uint8_t response[32];
  size_t index = 0;
  for (int retry = 0; retry < request_retry; retry++) 
  {
    fLd2420_SendCommand(readDelayCmd, sizeof(readDelayCmd));
    index = 0;
    unsigned long start = millis();
    bool headerFound = false;
    uint8_t header[2] = {0xFD, 0xFC};

    while (millis() - start < request_timeout && index < sizeof(readDelayResp)) {
      if (Serial1.available()) {
        uint8_t byte = Serial1.read();
        if (!headerFound) {
          if (byte == header[index]) {
            index++;
            if (index == 2) {
              headerFound = true;
              index = 0;
              response[0] = header[0];
              response[1] = header[1];
              index = 2;
            }
          } else {
            index = 0;
          }
        } else if (index < sizeof(readDelayResp)) {
          response[index++] = byte;
        }
      }
    }

    size_t ignoreIndices[] = {18, 19, 20, 21}; //report delay 4 byte
    if (index == sizeof(readDelayResp) && fLd2420_CompareArrays(response, readDelayResp, sizeof(readDelayResp), ignoreIndices, sizeof(ignoreIndices))) {
      // Check success/failure (bytes 8-9: 00 00 for success)
      if (response[8] == 0x00 && response[9] == 0x00) {
        Serial.println("Operation successful");
        // Parse report delay value (bytes 18-21, little-endian)
        uint32_t delay = (response[21] << 24) | (response[20] << 16) | (response[19] << 8) | response[18];
        Serial.print("Read report delay successful: ");
        Serial.print(delay);
        Serial.println(" seconds");
        return delay;
        
      } else {
        Serial.println("Read Operation failed");
        continue;
      }
    } 
    else 
    {
      Serial.print("Retry ");
      Serial.print(retry + 1);
      Serial.println(" for Read report delay...");
      delay(100);
    }
  }

  if (index != sizeof(readDelayResp)) {
    Serial.println("Read report delay failed after retries");
    return -1;
  }
}

static ld2420_res_t fLd2420_InitialABDThresh() {

  bool ConfigSuccess = true;
  Serial.println("\n>>> Sending Enter Command Mode...");
  if(fLd2420_EnterCMDMode()) 
  {
    Serial.println(":::: Enter Command Mode successful");
  }
  Serial.println("\n>>> Config ABDThresh...");
  for(int i = 0 ; i < 16 ; i++)
  {
    //Setting upper thresh
    if (fLd2420_SetThresh(i, upperThreshValues[i] , 1)) 
    {
      Serial.printf("\n:::: Set upper thresh[%d] value(%u) successful\n", i, upperThreshValues[i]);
      //Read Max Distance
      Serial.println("\n>>> Sending Read Thresh...");
      uint16_t returnedValue = fLd2420_ReadThresh(i, 1, 3, 1000);
      Serial.printf("Readed Value = %d\n", returnedValue);
      if(returnedValue =! upperThreshValues[i])
        ConfigSuccess = false;
      // else
      //   printf("Setting upper Thresh[%d] with value = %u success\n", i, returnedValue);
    } 
    else 
      Serial.printf("Setting upper Thresh[%d] failed\n", i);
    //Setting lower thresh
    if (fLd2420_SetThresh(i, lowerThreshValues[i] , 0)) 
    {
      Serial.printf("\n:::: Set upper thresh[%d] value(%u) successful\n", i, lowerThreshValues[i]);
      //Read Max Distance
      Serial.println("\n>>> Sending Read Thresh...");
      uint16_t returnedValue = fLd2420_ReadThresh(i, 1, 3, 1000);
      Serial.printf("Readed Value = %d\n", returnedValue);
      if(returnedValue =! lowerThreshValues[i])
        ConfigSuccess = false;
      // else
      //   printf("Setting lower Thresh[%d] with value = %u success\n", i, returnedValue);
    } 
    else 
      Serial.printf("Setting lower Thresh[%d] failed\n", i);
  }
  
  Serial.println("\nSending Exit Command Mode...");
  if(fLd2420_ExitCMDMode()) {
    Serial.println(":::: Exit Command Mode successful");
    return LD2420_RES_OK;
  } else {
    Serial.println("Exit Command Mode failed after retries");
    return LD2420_RES_CONFIG_ABB_PARAM_FAIL;
  }
}

/**End of Group_Name
  * @}
  */
/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
