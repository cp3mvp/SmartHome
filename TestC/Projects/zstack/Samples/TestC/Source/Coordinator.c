#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "Common.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "OSAL_Nv.h"

const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
  GENERICAPP_CLUSTERID
};
                        
const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
  GENERICAPP_ENDPOINT,              //  int Endpoint;
  GENERICAPP_PROFID,                //  uint16 AppProfId[2];
  GENERICAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  GENERICAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  GENERICAPP_FLAGS,                 //  int   AppFlags:4;
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList,  //  byte *pAppInClusterList;
  0,          //  byte  AppNumInClusters;
  (cId_t *)NULL   //  byte *pAppInClusterList;
};

static uint8 SerialApp_TxLen;
endPointDesc_t GenericApp_epDesc;
byte GenericApp_TaskID;
byte GenericApp_TransID;
static void rxCB(uint8 port,uint8 event);

void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void GenericApp_SendTheMessage(unsigned char theMessageData);

void GenericApp_Init( byte task_id )
{
  GenericApp_TaskID = task_id;
  GenericApp_TransID = 0;
  

  
  GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;
  GenericApp_epDesc.task_id = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;
  afRegister( &GenericApp_epDesc );
  
  halUARTCfg_t uartConfig;
  uartConfig.configured           = TRUE;              // 2x30 don't care - see uart driver.
  uartConfig.baudRate             = HAL_UART_BR_9600;
  uartConfig.flowControl          = FALSE;
  uartConfig.callBackFunc         = rxCB;
  HalUARTOpen (0, &uartConfig);
 
}

UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case AF_INCOMING_MSG_CMD:
          GenericApp_MessageMSGCB(MSGpkt);
            break;
          
        default:
          break;
      }
      osal_msg_deallocate( (uint8 *)MSGpkt );
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    }
    return (events ^ SYS_EVENT_MSG);
  }
  return 0;
}

void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pkt)
{
  unsigned char buffer[16]; 
  
  switch ( pkt->clusterId )
  {
  case GENERICAPP_CLUSTERID:
     osal_memcpy(buffer,pkt->cmd.Data,12);
     HalUARTWrite(0,buffer,12);
     
   break;
  }
}

static void rxCB(uint8 port,uint8 event)
{
  unsigned char Uartbuf[80];
  if ((event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT)) &&
  #if SERIAL_APP_LOOPBACK
    (SerialApp_TxLen < SERIAL_APP_TX_MAX))
  #else
    !SerialApp_TxLen)
  #endif
  {
    SerialApp_TxLen = HalUARTRead(0, Uartbuf, 80);
    if (SerialApp_TxLen)
    {
      if(osal_memcmp(Uartbuf, "LON",3)) GenericApp_SendTheMessage(1);
      if(osal_memcmp(Uartbuf, "LOFF",4)) GenericApp_SendTheMessage(2);
      if(osal_memcmp(Uartbuf, "TEMP",4)) GenericApp_SendTheMessage(3);
      if(osal_memcmp(Uartbuf, "TEST",4)) 
      {
        GenericApp_SendTheMessage(4);
        HalUARTWrite(0,"\n(ZSOK)\n",8);        
      }
      SerialApp_TxLen=0;
    }
  }
}

void GenericApp_SendTheMessage(unsigned char theMessageData)
{ 
  afAddrType_t my_DstAddr;
  my_DstAddr.addrMode=(afAddrMode_t)Addr16Bit;
  my_DstAddr.endPoint=GENERICAPP_ENDPOINT;
  my_DstAddr.addr.shortAddr=0xFFFF; 

  AF_DataRequest(&my_DstAddr
                 ,&GenericApp_epDesc
                 ,GENERICAPP_CLUSTERID
                 ,1
                 ,&theMessageData
                 ,&GenericApp_TransID
                 ,AF_DISCV_ROUTE
                 ,AF_DEFAULT_RADIUS);
}

