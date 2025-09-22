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
#define LD2420_RESPONOSE_TIMOUT_MS 1000
#define LD2420_REQUEST_RETRY       3

/**
 * @brief Return codes for WS2812 operations
 * 
 */
typedef uint8_t ld2420_res_t;

#define LD2420_RES_OK                           ((uint8_t)0)
#define LD2420_RES_ERROR_INIT_FAIL              ((uint8_t)1)
#define LD2420_RES_COMPARE_ARRAYS_NOT_EQUAL     ((uint8_t)2)
#define LD2420_RES_READ_RESPONSE_FAIL           ((uint8_t)3)
#define LD2420_RES_SEND_AND_READ_FAIL           ((uint8_t)4)
#define LD2420_RES_ENTER_CMD_MODE_FAIL          ((uint8_t)5)
#define LD2420_RES_EXIT_CMD_MODE_FAIL           ((uint8_t)6)
#define LD2420_RES_SET_TRESH_VALUE_FAIL         ((uint8_t)7)
#define LD2420_RES_SET_MAX_DISTANCE_FAIL        ((uint8_t)8)
#define LD2420_RES_SET_REPORT_DELAY_FAIL        ((uint8_t)9)
#define LD2420_RES_CONFIG_ABB_PARAM_FAIL        ((uint8_t)10)

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
ld2420_res_t fLd2420_Init();
ld2420_res_t fLd2420_ConfigABDParams(uint8_t config, int value);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* LD2420_CDRV_H */

/************************ © COPYRIGHT DiodeGroup *****END OF FILE****/