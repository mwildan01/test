/*
 * ***************************  Define Include  ***************************
 */
#include "can.h"
#include "main.h"

//Tambah header rabu siang
//Tambah header rabu sore

/*
 * ***************************  Define Handler  ***************************
 */
extern CAN_HandleTypeDef hcan;


CAN_FilterTypeDef sFilterConfig; 	//declare CAN filter structure
CAN_TxHeaderTypeDef txHeader; 		//declare a specific header for message transmittions
CAN_RxHeaderTypeDef rxHeader; 		//declare header for message reception


//Tambah handler rabu 30-12-2020

/*
 * ***************************  Define Variable  ***************************
 */

extern uint8_t rxMessage[8], txData[8];

uint32_t txMailbox;
uint8_t canFlag;

//Tambah variable

/**
  *****************************************************************************************************************************
  *													CAN TASK
  *
  *****************************************************************************************************************************
  */

void can_Routine()
{
	can_MessageProcess();

	if(canFlag)
	{

		canFlag = 0;
	}
}

// Interrupt CAN setiap ada data yang masuk
void CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rxHeader, rxMessage);  //Terima data CAN, disimpan di rxMessage

//	can_Parsing();

	HAL_CAN_IRQHandler(&hcan);
}

/**
  *****************************************************************************************************************************
  *													Inisialisasi CAN1
  *
  *****************************************************************************************************************************
  *
  *
  * parameter	:	Settingan untuk Baudrate 500 kbps  -> ada di main.c -> MX_CAN1_Init()
  *
  * 				System Clock	= 200 Mhz
  * 				APB1 Prescaler	= 4
  * 				APB1 Clock		= 50 MHz  	//APB1 adalah peripheral yg dipake untuk CAN1
  *
  *					CAN1 Prescaler	= 10
  *					SJW				= 1 tq		//tq = time quanta
  *					TimeSeg1		= 8 tq		//tq total = 10
  *					TimeSeg2		= 1 tq
  *
  *					Rumus Bitrate CAN -> Baudrate = (Clock / CAN prescaler) / (SJW + TimeSeg1 + TimeSeg2)
  *
  *					Perhitungannya bisa buka link ini -> http://www.bittiming.can-wiki.info
  *
  */

//Fungsi untuk mem-filter atawa menyaring data yang masuk
void can_FilterConfig(void)
{
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	//  sFilterConfig.FilterIdHigh = ((ARDUINO_ID << 5)  | (0x12345678 >> 18)) & 0xFFFF; // STID[10:0] & EXTID[17:13]
	//  sFilterConfig.FilterIdHigh = ARDUINO_ID<<5;	//ID yang diterima adalah
	sFilterConfig.FilterIdHigh = 0x00000;			//Semua ID diterima
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000 << 5;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.FilterBank = 0;

	if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)	//configure CAN filter
	{
		/* Filter configuration Error */
		Error_Handler();
	}

	HAL_CAN_Start(&hcan); //start CAN
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING); //enable interrupts
}

/**
  *****************************************************************************************************************************
  *													Additional Function
  *
  *****************************************************************************************************************************
  */

//Fungsi untuk mengirim data CAN dengan ID Extended
void can_TransmitExtID(uint32_t id, uint8_t *data, uint8_t len) {

	txHeader.IDE = CAN_ID_EXT;
	txHeader.ExtId = id;
	txHeader.RTR = CAN_RTR_DATA;
	txHeader.DLC = len;

	HAL_CAN_AddTxMessage(&hcan, &txHeader, data, &txMailbox);
}

//Data CAN yang masuk diproses sesuai ID dan Command-nya
void can_MessageProcess(void)
{
	if(rxHeader.IDE == CAN_ID_EXT)
	{
		uint8_t senderID = can_GetSenderID(rxHeader);
//		CAN_PACKET_ID cmd = can_GetPacketID(rxHeader);

		if(senderID == CONTROLLER_1_ID)
		{

		}
	}
}

void can_Parsing(void)
{
//	controller_Id	= rxHeader.ExtId;
//	level			= rxMessage[0];
//	ph				= rxMessage[1];
//	temperature		= rxMessage[2];
//	turbidity 		=	((uint16_t) rxMessage[3]) << 8 |
//						((uint16_t) rxMessage[4]);
//	flowIn			= rxMessage[5];
//	flowOut			= rxMessage[6];
//
//	canFlag = 1;
}

//Mendapatkan ID pengirim + Command
uint32_t can_GetCANID(uint32_t destinationID, CAN_PACKET_ID packetID)
{
	uint32_t CANID;

	CANID = ((uint32_t) destinationID) | ((uint32_t)packetID << 8);

	return CANID;
}

//Mendapatkan Command CAN
CAN_PACKET_ID can_GetPacketID(CAN_RxHeaderTypeDef canMsg)
{
	CAN_PACKET_ID packetID;

	packetID = (CAN_PACKET_ID)((canMsg.ExtId >> 8) & 0xFF);

	return packetID;
}

//Mendapatkan ID pengirim
uint32_t can_GetSenderID(CAN_RxHeaderTypeDef canMsg)
{
	uint32_t senderID;

	senderID = canMsg.ExtId & 0xFF;

	return senderID;
}
