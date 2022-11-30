/*
 * pc_exchange.h
 *
 *  Created on: Aug 26, 2022
 *      Author: admin
 */

#ifndef INC_PC_EXCHANGE_H_
#define INC_PC_EXCHANGE_H_

#include "main.h"
#include "usbd_cdc_if.h"


#define VCP_CDC_TIMEOUT 500
#define VCP_MSG_HEADER_SIZE (1 + 2 + 2)	//1-id;2-len;2-crc.

typedef enum  {
    VCP_NO_TYPE,
    VCP_COMMAND_SEND,

    VCP_TIMEOUT,

    VCP_OK					= 0x55,
    VCP_LONG_RW_DONE,

	SPI_WRITE_ERROR 	= 0x81,
	REG_NOT_SET_EROR,
	DAC_SET_ERROR,
	VCP_FLASH_ERROR,

    VCP_CRC_ERROR           = 0x93,
    VCP_WRONG_COMMAND 		= 0x94,
    VCP_WRONG_COMMAND_DATA  = 0x95,
    VCP_DATA_TIMEOUT 		= 0xF0,

    VCP_UNKNOW_ERROR,
	VCP_FULL_BUFFER,
    VCP_LAST_STATUS
}VCP_COMMAND_STATYS;

typedef enum {
	VCP_CMD_NONE = 0x00,
	VCP_CMD_DEV_ID = 0x01,	//RESET_MCU

	VCP_CMD_LAST,
}VCP_COMMANSD;

struct VcpPackge
{
	uint8_t command;
	uint16_t crc16;
	uint16_t lenght;
	uint8_t data[APP_RX_DATA_SIZE];
	bool long_msg;
	uint16_t position;
};

typedef struct {
	uint8_t data[APP_RX_DATA_SIZE];
	uint16_t size;
	uint32_t timeout_counter;
}MSG_Buffer;

extern MSG_Buffer m_msg_buffer;

void usb_cdc_parse_buffer();
bool usb_cdc_check_message(uint8_t *buffer,uint16_t size);

void parseCommand(struct VcpPackge *package);

#endif /* INC_PC_EXCHANGE_H_ */
