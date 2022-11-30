/*
 * pc_exchange.c
 *
 *  Created on: Aug 26, 2022
 *      Author: admin
 */
#include "pc_exchange.h"
#include "string.h"

char* dev_version = "DEV_NAME:RXTX\nVersion:1.0.2\nDate:2022:10:07";

MSG_Buffer m_msg_buffer;


bool sendStandartPacket(uint8_t command,uint8_t *data,uint16_t data_length)
{
	bool result = false;
	uint8_t packet[APP_RX_DATA_SIZE] = {0};
	uint16_t crc16 = calc_crc(data, data_length);
	packet[0] = command;
	packet[1] = (data_length >> 8) & 0xFF;
	packet[2] = (data_length >> 0) & 0xFF;
	packet[3] = (crc16 >> 8) & 0xFF;
	packet[4] = (crc16 >> 0) & 0xFF;
	memcpy(&packet[5], data, data_length);
	data_length += 5;
	if(USBD_OK == CDC_Transmit_FS(packet, data_length))
	{
		result = true;
	}
	return result;
}

bool sendFullStatus(uint8_t command,uint8_t *status) {
	return sendStandartPacket(command,status,3);
}

bool sendShortStatus(uint8_t command,uint8_t status) {
	uint8_t buff[3] = {status,0x00,0x00};
	return sendStandartPacket(command,buff,3);
}

bool sendVersion()
{
	// send hardware version
	size_t len;
	len = strlen(dev_version);
	return sendStandartPacket(VCP_CMD_DEV_ID,(uint8_t*)dev_version,len);
}

void usb_cdc_parse_buffer() {
	uint8_t vcpData[APP_RX_DATA_SIZE] = {0x00};
	uint32_t vcpSize;
	if(0 != vcpRetrieveInputData(vcpData, &vcpSize) )
	{
		///================================================================
		if((m_msg_buffer.size + vcpSize) < APP_RX_DATA_SIZE) {
			memcpy(&m_msg_buffer.data[m_msg_buffer.size], &vcpData,vcpSize);
			m_msg_buffer.size += vcpSize;
			if(true == usb_cdc_check_message(m_msg_buffer.data,m_msg_buffer.size))
			{
				m_msg_buffer.size = 0;
			}
		}else {
			m_msg_buffer.size = 0;
		}
		//=================================================================
	}
}

bool usb_cdc_check_message(uint8_t *buffer,uint16_t size)
{
	bool parsed = false;
	struct VcpPackge message;
	if(VCP_MSG_HEADER_SIZE <= size)
	{
		message.command = buffer[0];
		message.lenght = buffToUint16(&buffer[1]);
		message.crc16 = buffToUint16(&buffer[3]);
		if ((message.lenght + VCP_MSG_HEADER_SIZE) <= size)
		{
			memcpy(&message.data, &buffer[VCP_MSG_HEADER_SIZE], message.lenght);
			uint16_t calc_crc16 = calc_crc(message.data, message.lenght);
			if (calc_crc16 == message.crc16)
			{
				parseCommand(&message);
				parsed = true;
			}

		}
	}
	return parsed;
}



void parseCommand(struct VcpPackge *package)
{
	uint8_t buff[APP_RX_DATA_SIZE];
	VCP_COMMAND_STATYS status = VCP_OK;
	switch(package->command)
	{
		case VCP_CMD_DEV_ID: {
			sendVersion();
			break;
		}
		default:
		{
			status = VCP_WRONG_COMMAND;
			sendShortStatus(package->command,status);
			break;
		}
	/*
		 */
	}
}
