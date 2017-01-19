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
#include "hal_adc.h"

#define SEND_DATA_EVENT 0x01

void readTemp(char* temp);
void setio(char i);
uint8 isLOn=0;
// This list should be filled with Application specific Cluster IDs.
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
  
  
  0,          //  byte  AppNumInClusters;
  (cId_t *)NULL,  //  byte *pAppInClusterList;
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList   //  byte *pAppInClusterList;
};

endPointDesc_t GenericApp_epDesc;
byte GenericApp_TaskID;
byte GenericApp_TransID;
devStates_t GenericApp_NwkState;
//void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void GenericApp_SendTheMessage(unsigned char theMessageData[]);
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );

void GenericApp_Init( byte task_id )
{
  halUARTCfg_t uartConfig;//´®¿Ú
    
  GenericApp_TaskID = task_id;
  GenericApp_NwkState=DEV_INIT;
  GenericApp_TransID = 0;
  
  GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;
  GenericApp_epDesc.task_id = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  
  GenericApp_epDesc.latencyReq = noLatencyReqs;
  afRegister( &GenericApp_epDesc ); 
  
    RegisterForKeys( GenericApp_TaskID );
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
        
        case ZDO_STATE_CHANGE:
        GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
        if(GenericApp_NwkState==DEV_END_DEVICE)
        {
          osal_set_event(GenericApp_TaskID,SEND_DATA_EVENT);
          setio(0);
          HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
          isLOn=0;
        }
        break;
        
        case AF_INCOMING_MSG_CMD:
        GenericApp_MessageMSGCB(MSGpkt);
        break;
        
     /*
        case KEY_CHANGE:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;
          */
        default:
        break;
      }  
      osal_msg_deallocate( (uint8 *)MSGpkt );
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    }
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
  
  if ( events & SEND_DATA_EVENT )
  {
    //GenericApp_SendTheMessage("EndDevice");
    //osal_start_timerEx(GenericApp_TaskID,SEND_DATA_EVENT,1000);
    return(events^SEND_DATA_EVENT);
  }
  return 0;
}

void GenericApp_SendTheMessage(unsigned char theMessageData[])
{ 
  afAddrType_t my_DstAddr;
  my_DstAddr.addrMode=(afAddrMode_t)Addr16Bit;
  my_DstAddr.endPoint=GENERICAPP_ENDPOINT;
  my_DstAddr.addr.shortAddr=0xFFFF; 

  AF_DataRequest(&my_DstAddr
                 ,&GenericApp_epDesc
                 ,GENERICAPP_CLUSTERID
                 ,osal_strlen(theMessageData)
                 ,theMessageData
                 ,&GenericApp_TransID
                 ,AF_DISCV_ROUTE
                 ,AF_DEFAULT_RADIUS);
}


void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pkt)
{
  unsigned char buffer=0; 
   switch ( pkt->clusterId )
  {
  case GENERICAPP_CLUSTERID:
     osal_memcpy(&buffer,pkt->cmd.Data,1);
     if(buffer==1)
     {
        setio(1);
        HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
        isLOn=1;
        GenericApp_SendTheMessage("(LONOK)\n\0");  
     }
     if(buffer==2)
     {
       setio(0);
       HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
       isLOn=0;
       GenericApp_SendTheMessage("(LOFFOK)\n\0"); 
       
     }
     if(buffer==3)
     {
        
      char str[9]="(XXXX)\n\0";
      readTemp(str);
      GenericApp_SendTheMessage(str);     
     } 
     if(buffer==4)
     {
        GenericApp_SendTheMessage("(ZEOK)\n\0");
     }
    break;
  }
}




void readTemp(char* temp)
{
  uint16 value;
  ATEST = 0x01;
  TR0  |= 0x01; 
   //  Clear ADC interrupt flag 
  ADCIF = 0;
  ADCCON3 = (HAL_ADC_REF_125V | 0x30 | HAL_ADC_CHN_TEMP);
// Wait for the conversion to finish 
  while ( !ADCIF );
 // Get the result 
  value = ADCL;
  value |= ((uint16) ADCH) << 8;
  
  float t=value;
  t=((t/56.0)-(1480.0/3.5))*100.0;
  value=t;
  if(t>9999)t=9999;
  if(t<0)t=0;
  temp[1]=(uint8)(value/1000);
  value=value-(temp[1]*1000);
  temp[2]=(uint8)(value/100);
  value=value-(temp[2]*100);
  temp[3]=(uint8)(value/10);
  value=value-(temp[3]*10);
  temp[4]=(uint8)value;
  temp[1]+=48;
  temp[2]+=48;
  temp[3]+=48;
  temp[4]+=48;
}

void setio(char i)
{
  P0DIR |= 0x40;  
  P0_6=i;
}