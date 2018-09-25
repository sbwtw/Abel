/**********************************************************************************************
 * Filename:       wisnuc.c
 *
 * Description:    This file contains the implementation of the service.
 *
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <icall.h>

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "wisnuc.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
* GLOBAL VARIABLES
*/

// wisnuc Service UUID
CONST uint8_t wisnucUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(WISNUC_SERV_UUID), HI_UINT16(WISNUC_SERV_UUID)
};

// stationSession UUID
CONST uint8_t wisnuc_StationSessionUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(WISNUC_STATIONSESSION_UUID)
};
// stationId UUID
CONST uint8_t wisnuc_StationIdUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(WISNUC_STATIONID_UUID)
};
// stationStatus UUID
CONST uint8_t wisnuc_StationStatusUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(WISNUC_STATIONSTATUS_UUID)
};
// bindingProgress UUID
CONST uint8_t wisnuc_BindingProgressUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(WISNUC_BINDINGPROGRESS_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static wisnucCBs_t *pAppCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t wisnucDecl = { ATT_BT_UUID_SIZE, wisnucUUID };

// Characteristic "StationSession" Properties (for declaration)
static uint8_t wisnuc_StationSessionProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic "StationSession" Value variable
static uint8_t wisnuc_StationSessionVal[WISNUC_STATIONSESSION_LEN] = {0};

// Characteristic "StationSession" CCCD
static gattCharCfg_t *wisnuc_StationSessionConfig;
// Characteristic "StationId" Properties (for declaration)
static uint8_t wisnuc_StationIdProps = GATT_PROP_READ;

// Characteristic "StationId" Value variable
static uint8_t wisnuc_StationIdVal[WISNUC_STATIONID_LEN] = {0};
// Characteristic "StationStatus" Properties (for declaration)
static uint8_t wisnuc_StationStatusProps = GATT_PROP_READ;

// Characteristic "StationStatus" Value variable
static uint8_t wisnuc_StationStatusVal[WISNUC_STATIONSTATUS_LEN] = {0};
// Characteristic "BindingProgress" Properties (for declaration)
static uint8_t wisnuc_BindingProgressProps = GATT_PROP_READ;

// Characteristic "BindingProgress" Value variable
static uint8_t wisnuc_BindingProgressVal[WISNUC_BINDINGPROGRESS_LEN] = {0};

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t wisnucAttrTbl[] =
{
  // wisnuc Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&wisnucDecl
  },
    // StationSession Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &wisnuc_StationSessionProps
    },
      // StationSession Characteristic Value
      {
        { ATT_UUID_SIZE, wisnuc_StationSessionUUID },
        GATT_PERMIT_READ,
        0,
        wisnuc_StationSessionVal
      },
      // StationSession CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&wisnuc_StationSessionConfig
      },
    // StationId Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &wisnuc_StationIdProps
    },
      // StationId Characteristic Value
      {
        { ATT_UUID_SIZE, wisnuc_StationIdUUID },
        GATT_PERMIT_READ,
        0,
        wisnuc_StationIdVal
      },
    // StationStatus Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &wisnuc_StationStatusProps
    },
      // StationStatus Characteristic Value
      {
        { ATT_UUID_SIZE, wisnuc_StationStatusUUID },
        GATT_PERMIT_READ,
        0,
        wisnuc_StationStatusVal
      },
    // BindingProgress Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &wisnuc_BindingProgressProps
    },
      // BindingProgress Characteristic Value
      {
        { ATT_UUID_SIZE, wisnuc_BindingProgressUUID },
        GATT_PERMIT_READ,
        0,
        wisnuc_BindingProgressVal
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t wisnuc_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t wisnuc_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t wisnucCBs =
{
  wisnuc_ReadAttrCB,  // Read callback function pointer
  wisnuc_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * Wisnuc_AddService- Initializes the Wisnuc service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t Wisnuc_AddService( uint8_t rspTaskId )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  wisnuc_StationSessionConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( wisnuc_StationSessionConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, wisnuc_StationSessionConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( wisnucAttrTbl,
                                        GATT_NUM_ATTRS( wisnucAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &wisnucCBs );

  return ( status );
}

/*
 * Wisnuc_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t Wisnuc_RegisterAppCBs( wisnucCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*
 * Wisnuc_SetParameter - Set a Wisnuc parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t Wisnuc_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case WISNUC_STATIONSESSION_ID:
      if ( len == WISNUC_STATIONSESSION_LEN )
      {
        memcpy(wisnuc_StationSessionVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( wisnuc_StationSessionConfig, (uint8_t *)&wisnuc_StationSessionVal, FALSE,
                                    wisnucAttrTbl, GATT_NUM_ATTRS( wisnucAttrTbl ),
                                    INVALID_TASK_ID,  wisnuc_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case WISNUC_STATIONID_ID:
      if ( len == WISNUC_STATIONID_LEN )
      {
        memcpy(wisnuc_StationIdVal, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case WISNUC_STATIONSTATUS_ID:
      if ( len == WISNUC_STATIONSTATUS_LEN )
      {
        memcpy(wisnuc_StationStatusVal, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case WISNUC_BINDINGPROGRESS_ID:
      if ( len == WISNUC_BINDINGPROGRESS_LEN )
      {
        memcpy(wisnuc_BindingProgressVal, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*
 * Wisnuc_GetParameter - Get a Wisnuc parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t Wisnuc_GetParameter( uint8_t param, uint16_t *len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*********************************************************************
 * @fn          wisnuc_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t wisnuc_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the StationSession Characteristic Value
if ( ! memcmp(pAttr->type.uuid, wisnuc_StationSessionUUID, pAttr->type.len) )
  {
    if ( offset > WISNUC_STATIONSESSION_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, WISNUC_STATIONSESSION_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the StationId Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, wisnuc_StationIdUUID, pAttr->type.len) )
  {
    if ( offset > WISNUC_STATIONID_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, WISNUC_STATIONID_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the StationStatus Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, wisnuc_StationStatusUUID, pAttr->type.len) )
  {
    if ( offset > WISNUC_STATIONSTATUS_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, WISNUC_STATIONSTATUS_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the BindingProgress Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, wisnuc_BindingProgressUUID, pAttr->type.len) )
  {
    if ( offset > WISNUC_BINDINGPROGRESS_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, WISNUC_BINDINGPROGRESS_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has READ permissions.
    *pLen = 0;
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return status;
}


/*********************************************************************
 * @fn      wisnuc_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t wisnuc_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;

  // See if request is regarding a Client Characterisic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
    // Allow only notifications.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY);
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has WRITE permissions.
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (paramID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnChangeCb )
      pAppCBs->pfnChangeCb(connHandle, paramID, len, pValue); // Call app function from stack task context.

  return status;
}
