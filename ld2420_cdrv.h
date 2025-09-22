/**
******************************************************************************
* @file           : ld2420_cdrv.h
* @brief          : Header file for WS2812 LED driver
* @note           :
* @copyright      : COPYRIGHT© 2025 DiodeGroup
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright© 2025 DiodeGroup.
* All rights reserved.</center></h2>
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
* @verbatim
* @endverbatim
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LD2420_CDRV_H
#define LD2420_CDRV_H

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h> // For uint8_t and size_t

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/
#define ResponseTimeout 1000

/**
 * @brief Return codes for WS2812 operations
 * 
 */
typedef uint8_t ld2420_res_t;

#define LD2420_RES_OK                           ((uint8_t)0)
#define LD2420_RES_ERROR_INIT_FAIL              ((uint8_t)1)

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief WS2812 effect types
 * 
 */
typedef enum {

    SetMinDistance = 0,
    SetMaxDistance,
    SetDelayTime,  

}eLd2420Config;

/**
 * @brief WS2812 configuration structure
 * 
 */
typedef struct {

    Stream* ComPort;

    bool Init;

}sLd2420;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
static ld2420_res_t fLd2420_Init();
static ld2420_res_t fLd2420_CompareArrays(uint8_t *a, uint8_t *b, size_t len, size_t *ignoreIndices = nullptr, size_t ignoreCount = 0);
static ld2420_res_t fLd2420_ReadResponse(uint8_t *expected, size_t len, size_t *ignoreIndices = nullptr, size_t ignoreCount = 0, unsigned long timeout = 1000);
static void fLd2420_SendCommand(uint8_t *cmd, size_t len);
static ld2420_res_t fLd2420_SendAndReadCommand(uint8_t *cmd, size_t cmdLen, uint8_t *expected, size_t respLen, size_t *ignoreIndices = nullptr, size_t ignoreCount = 0, int request_try = 3);
static ld2420_res_t fLd2420_EnterCMDMode();
static ld2420_res_t fLd2420_ExitCMDMode();
static ld2420_res_t fLd2420_SetMaxDistance(uint16_t distance);
static ld2420_res_t fLd2420_SetReportDelay(uint16_t delay);
static int fLd2420_ReadMaxDistance(int request_retry = 3 , int request_timeout = 1000);
static int fLd2420_ReadReportDelay(int request_retry = 3 , int request_timeout = 1000);
static ld2420_res_t fLd2420_ConfigABDParams(uint8_t config, int value);
static ld2420_res_t fLd2420_InitialABDThresh();
static ld2420_res_t fLd2420_SetThresh(uint16_t add, uint16_t value, bool up);
static int fLd2420_ReadThresh(uint16_t add , bool up, int request_retry = 3 , int request_timeout = 1000);

/* Exported variables --------------------------------------------------------*/
extern uint8_t enterCmd[];
extern uint8_t enterResp[];
extern uint8_t setMaxDisCmd[];
extern uint8_t setABDResp[];
extern uint8_t readMaxDisCmd[];
extern uint8_t readMaxDisResp[];
extern uint8_t exitCmd[];
extern uint8_t exitResp[];
extern uint8_t setDelayCmd[];
extern uint8_t setDelayResp[];
extern uint8_t readDelayCmd[];
extern uint8_t readDelayResp[];
extern uint8_t setThreshCmd[];
extern uint8_t readThreshCmd[];
extern uint8_t readThreshResp[];
extern uint16_t upperThreshValues[];
extern uint16_t lowerThreshValues[];

#ifdef __cplusplus
}
#endif

#endif /* LD2420_CDRV_H */

/************************ © COPYRIGHT DiodeGroup *****END OF FILE****/