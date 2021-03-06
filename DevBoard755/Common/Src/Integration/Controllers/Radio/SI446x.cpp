/*
 * SI446x.cpp
 *
 *  Created on: Feb 12, 2022
 *      Author: paul
 */

#include "SI446x.hpp"
#include <stdarg.h>

osThreadId_t Integration::SI446x::createThread()
{
	WriteLog("Creating Thread");
	osThreadAttr_t defaultTask_attributes;
	defaultTask_attributes.name = "si446Task";
	defaultTask_attributes.stack_size = 128 * 4;
    defaultTask_attributes.priority = (osPriority_t) osPriorityNormal;
	threadHandle = osThreadNew((TaskFunction_t) &Integration::SI446x::threadRunner, NULL, &defaultTask_attributes);
}

void Integration::SI446x::threadRunner(void const * argument)
{
	WriteLog("Thread starting");
	// run it in a thread since you need the HAL up and running
	RF_NIRQ = RF_NSEL = PWRDN = false;

	//this->get_chip_status_fast_clear()
	reset();
	get_chip_status(0);

	// lets get the GIPIO3 pin to flash a couple of times for good measure
	led(true);
	osDelay(500);
	led(false);
	osDelay(500);
	led(true);
	osDelay(500);
	led(false);
	osDelay(500);
	WriteLog("Thread finished");
}



void Integration::SI446x::led(bool on)
{
	 gpio_pin_cfg(    //U8 GPIO0, U8 GPIO1, U8 GPIO2, U8 GPIO3, U8 NIRQ, U8 SDO, U8 GEN_CONFIG)

	                        SI446X_CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,

	                        SI446X_CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,

	                        on? SI446X_CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DRIVE0: SI446X_CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DRIVE1,

	                        SI446X_CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,

	                        SI446X_CMD_GPIO_PIN_CFG_ARG_NIRQ_NIRQ_MODE_ENUM_DONOTHING,

	                        SI446X_CMD_GPIO_PIN_CFG_ARG_SDO_SDO_MODE_ENUM_SDO,

	                        (SI446X_CMD_GPIO_PIN_CFG_REP_GEN_CONFIG_DRV_STRENGTH_ENUM_HIGH

	                         << SI446X_CMD_GPIO_PIN_CFG_REP_GEN_CONFIG_DRV_STRENGTH_LSB)

	                        & SI446X_CMD_GPIO_PIN_CFG_REP_GEN_CONFIG_DRV_STRENGTH_MASK);

}

Integration::SI446x::~SI446x() {
}

#define SDN_DELAY 10
void Integration::SI446x::reset(void) {
	/* Put radio in shutdown, wait then release */
	radio_hal_AssertShutdown();
	osDelay(SDN_DELAY);

	radio_hal_DeassertShutdown();
	osDelay(SDN_DELAY);

	radio_comm_ClearCTS();

}

inline void Integration::SI446x::power_up(uint8_t BOOT_OPTIONS, uint8_t XTAL_OPTIONS,
		uint32_t XO_FREQ) {
	Pro2Cmd[0] = SI446X_CMD_ID_POWER_UP;
	Pro2Cmd[1] = BOOT_OPTIONS;
	Pro2Cmd[2] = XTAL_OPTIONS;
	Pro2Cmd[3] = static_cast<uint8_t>((XO_FREQ >> 24)); // @suppress("Avoid magic numbers")
	Pro2Cmd[4] = static_cast<uint8_t>((XO_FREQ >> 16)); // @suppress("Avoid magic numbers")
	Pro2Cmd[5] = static_cast<uint8_t>((XO_FREQ >> 8)); // @suppress("Avoid magic numbers")
	Pro2Cmd[6] = static_cast<uint8_t>((XO_FREQ)); // @suppress("Avoid magic numbers")

	radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_POWER_UP, Pro2Cmd);
}

inline uint8_t Integration::SI446x::configuration_init(const uint8_t *pSetPropCmd) {
	uint8_t col;
	uint8_t numOfBytes;

	/* While cycle as far as the pointer points to a command */
	while (*pSetPropCmd != 0x00) {
		/* Commands structure in the array:
		 * --------------------------------
		 * LEN | <LEN length of data>
		 */

		numOfBytes = *pSetPropCmd++;

		if (numOfBytes > 16u) {
			/* Number of command bytes exceeds maximal allowable length */
			return (SI446X_COMMAND_ERROR);
		}

		for (col = 0u; col < numOfBytes; col++) {
			Pro2Cmd[col] = *pSetPropCmd;
			pSetPropCmd++;
		}

		if (radio_comm_SendCmdGetResp(numOfBytes, Pro2Cmd, 0, 0) != 0xFF) {
			/* Timeout occured */
			return (SI446X_CTS_TIMEOUT);
		}

		if (radio_hal_NirqLevel() == 0) {
			/* Get and clear all interrupts.  An error has occured... */
			get_int_status(0, 0, 0);
			if (Si446xCmd.GET_INT_STATUS.CHIP_PEND
					& SI446X_CMD_GET_CHIP_STATUS_REP_CHIP_PEND_CMD_ERROR_PEND_MASK) {
				return SI446X_COMMAND_ERROR;
			}
		}
	}

	return SI446X_SUCCESS;

}

uint8_t Integration::SI446x::apply_patch(void) {
	return 0;
}

inline void Integration::SI446x::part_info(void) {

	Pro2Cmd[0] = SI446X_CMD_ID_PART_INFO;

	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_PART_INFO, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_PART_INFO, Pro2Cmd);

	Si446xCmd.PART_INFO.CHIPREV = Pro2Cmd[0];
	Si446xCmd.PART_INFO.PART = ((uint16_t) Pro2Cmd[1] << 8) & 0xFF00;
	Si446xCmd.PART_INFO.PART |= (uint16_t) Pro2Cmd[2] & 0x00FF;
	Si446xCmd.PART_INFO.PBUILD = Pro2Cmd[3];
	Si446xCmd.PART_INFO.ID = (static_cast<uint16_t>(Pro2Cmd[4]) << 8) & 0xFF00;
	Si446xCmd.PART_INFO.ID |= (uint16_t) Pro2Cmd[5] & 0x00FF;
	Si446xCmd.PART_INFO.CUSTOMER = Pro2Cmd[6];
	Si446xCmd.PART_INFO.ROMID = Pro2Cmd[7];

}

inline void Integration::SI446x::start_tx(uint8_t CHANNEL, uint8_t CONDITION,
		uint16_t TX_LEN) {
	Pro2Cmd[0] = SI446X_CMD_ID_START_TX;
	Pro2Cmd[1] = CHANNEL;
	Pro2Cmd[2] = CONDITION;
	Pro2Cmd[3] = (uint8_t) (TX_LEN >> 8);
	Pro2Cmd[4] = (uint8_t) (TX_LEN);
	Pro2Cmd[5] = 0x00;  

	// Don't repeat the packet,
	// ie. transmit the packet only once
	Pro2Cmd[6] = 0x00; 

	radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_START_TX, Pro2Cmd);

}

inline void Integration::SI446x::start_rx(uint8_t CHANNEL, uint8_t CONDITION,
		uint16_t RX_LEN, uint8_t NEXT_STATE1, uint8_t NEXT_STATE2,
		uint8_t NEXT_STATE3) {

	Pro2Cmd[0] = SI446X_CMD_ID_START_RX;
	Pro2Cmd[1] = CHANNEL;
	Pro2Cmd[2] = CONDITION;
	Pro2Cmd[3] = static_cast<uint8_t>((RX_LEN >> 8));
	Pro2Cmd[4] = static_cast<uint8_t>((RX_LEN));
	Pro2Cmd[5] = NEXT_STATE1;
	Pro2Cmd[6] = NEXT_STATE2;
	Pro2Cmd[7] = NEXT_STATE3;

	radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_START_RX, Pro2Cmd);
}

inline void Integration::SI446x::get_int_status(uint8_t PH_CLR_PEND, uint8_t MODEM_CLR_PEND,
		uint8_t CHIP_CLR_PEND) {

	Pro2Cmd[0] = SI446X_CMD_ID_GET_INT_STATUS;
	Pro2Cmd[1] = PH_CLR_PEND;
	Pro2Cmd[2] = MODEM_CLR_PEND;
	Pro2Cmd[3] = CHIP_CLR_PEND;

	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GET_INT_STATUS, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_GET_INT_STATUS, Pro2Cmd);

	Si446xCmd.GET_INT_STATUS.INT_PEND = Pro2Cmd[0];
	Si446xCmd.GET_INT_STATUS.INT_STATUS = Pro2Cmd[1];
	Si446xCmd.GET_INT_STATUS.PH_PEND = Pro2Cmd[2];
	Si446xCmd.GET_INT_STATUS.PH_STATUS = Pro2Cmd[3];
	Si446xCmd.GET_INT_STATUS.MODEM_PEND = Pro2Cmd[4];
	Si446xCmd.GET_INT_STATUS.MODEM_STATUS = Pro2Cmd[5];
	Si446xCmd.GET_INT_STATUS.CHIP_PEND = Pro2Cmd[6];
	Si446xCmd.GET_INT_STATUS.CHIP_STATUS = Pro2Cmd[7];

}

inline void Integration::SI446x::gpio_pin_cfg(uint8_t GPIO0, uint8_t GPIO1, uint8_t GPIO2,
		uint8_t GPIO3, uint8_t NIRQ, uint8_t SDO, uint8_t GEN_CONFIG) {

	Pro2Cmd[0] = SI446X_CMD_ID_GPIO_PIN_CFG;
	Pro2Cmd[1] = GPIO0;
	Pro2Cmd[2] = GPIO1;
	Pro2Cmd[3] = GPIO2;
	Pro2Cmd[4] = GPIO3;
	Pro2Cmd[5] = NIRQ;
	Pro2Cmd[6] = SDO;
	Pro2Cmd[7] = GEN_CONFIG;

	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GPIO_PIN_CFG, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_GPIO_PIN_CFG, Pro2Cmd);

	Si446xCmd.GPIO_PIN_CFG.GPIO[0] = Pro2Cmd[0];
	Si446xCmd.GPIO_PIN_CFG.GPIO[1] = Pro2Cmd[1];
	Si446xCmd.GPIO_PIN_CFG.GPIO[2] = Pro2Cmd[2];
	Si446xCmd.GPIO_PIN_CFG.GPIO[3] = Pro2Cmd[3];
	Si446xCmd.GPIO_PIN_CFG.NIRQ = Pro2Cmd[4];
	Si446xCmd.GPIO_PIN_CFG.SDO = Pro2Cmd[5];
	Si446xCmd.GPIO_PIN_CFG.GEN_CONFIG = Pro2Cmd[6];

}

inline void Integration::SI446x::set_property(uint8_t GROUP, uint8_t NUM_PROPS,
		uint8_t START_PROP, ...) {
	va_list argList;
	uint8_t cmdIndex;

	Pro2Cmd[0] = SI446X_CMD_ID_SET_PROPERTY;
	Pro2Cmd[1] = GROUP;
	Pro2Cmd[2] = NUM_PROPS;
	Pro2Cmd[3] = START_PROP;

	va_start(argList, START_PROP);
	cmdIndex = 4;
	while (NUM_PROPS--) {
		Pro2Cmd[cmdIndex] = va_arg(argList, uint8_t);
		cmdIndex++;
	}
	va_end(argList);

	radio_comm_SendCmd(cmdIndex, Pro2Cmd);

}

void Integration::SI446x::change_state(uint8_t NEXT_STATE1) {
	Pro2Cmd[0] = SI446X_CMD_ID_CHANGE_STATE;
	Pro2Cmd[1] = NEXT_STATE1;

	radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_CHANGE_STATE, Pro2Cmd);
}

void Integration::SI446x::nop(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_NOP;

	radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_NOP, Pro2Cmd);

}

inline void Integration::SI446x::fifo_info(uint8_t FIFO) {
	Pro2Cmd[0] = SI446X_CMD_ID_FIFO_INFO;
	Pro2Cmd[1] = FIFO;

	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_FIFO_INFO, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_FIFO_INFO, Pro2Cmd);

	Si446xCmd.FIFO_INFO.RX_FIFO_COUNT = Pro2Cmd[0];
	Si446xCmd.FIFO_INFO.TX_FIFO_SPACE = Pro2Cmd[1];
}

inline void Integration::SI446x::write_tx_fifo(uint8_t numBytes, uint8_t *pTxData) {
	radio_comm_WriteData( SI446X_CMD_ID_WRITE_TX_FIFO, 0, numBytes, pTxData);
}

inline void Integration::SI446x::read_rx_fifo(uint8_t numBytes, uint8_t *pRxData) {
	radio_comm_ReadData( SI446X_CMD_ID_READ_RX_FIFO, 0, numBytes, pRxData);
}

inline void Integration::SI446x::get_property(uint8_t GROUP, uint8_t NUM_PROPS,
		uint8_t START_PROP) {
	Pro2Cmd[0] = SI446X_CMD_ID_GET_PROPERTY;
	Pro2Cmd[1] = GROUP;
	Pro2Cmd[2] = NUM_PROPS;
	Pro2Cmd[3] = START_PROP;

	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GET_PROPERTY, Pro2Cmd,
			Pro2Cmd[2], Pro2Cmd);

	Si446xCmd.GET_PROPERTY.DATA[0] = Pro2Cmd[0];
	Si446xCmd.GET_PROPERTY.DATA[1] = Pro2Cmd[1];
	Si446xCmd.GET_PROPERTY.DATA[2] = Pro2Cmd[2];
	Si446xCmd.GET_PROPERTY.DATA[3] = Pro2Cmd[3];
	Si446xCmd.GET_PROPERTY.DATA[4] = Pro2Cmd[4];
	Si446xCmd.GET_PROPERTY.DATA[5] = Pro2Cmd[5];
	Si446xCmd.GET_PROPERTY.DATA[6] = Pro2Cmd[6];
	Si446xCmd.GET_PROPERTY.DATA[7] = Pro2Cmd[7];
	Si446xCmd.GET_PROPERTY.DATA[8] = Pro2Cmd[8];
	Si446xCmd.GET_PROPERTY.DATA[9] = Pro2Cmd[9];
	Si446xCmd.GET_PROPERTY.DATA[10] = Pro2Cmd[10];
	Si446xCmd.GET_PROPERTY.DATA[11] = Pro2Cmd[11];
	Si446xCmd.GET_PROPERTY.DATA[12] = Pro2Cmd[12];
	Si446xCmd.GET_PROPERTY.DATA[13] = Pro2Cmd[13];
	Si446xCmd.GET_PROPERTY.DATA[14] = Pro2Cmd[14];
	Si446xCmd.GET_PROPERTY.DATA[15] = Pro2Cmd[15];

}

inline void Integration::SI446x::func_info(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_FUNC_INFO;

	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_FUNC_INFO, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_FUNC_INFO, Pro2Cmd);

	Si446xCmd.FUNC_INFO.REVEXT = Pro2Cmd[0];
	Si446xCmd.FUNC_INFO.REVBRANCH = Pro2Cmd[1];
	Si446xCmd.FUNC_INFO.REVINT = Pro2Cmd[2];
	Si446xCmd.FUNC_INFO.FUNC = Pro2Cmd[5];
}

inline void Integration::SI446x::frr_a_read(uint8_t respByteCount) {
	radio_comm_ReadData(SI446X_CMD_ID_FRR_A_READ, 0, respByteCount, Pro2Cmd);

	Si446xCmd.FRR_A_READ.FRR_A_VALUE = Pro2Cmd[0];
	Si446xCmd.FRR_A_READ.FRR_B_VALUE = Pro2Cmd[1];
	Si446xCmd.FRR_A_READ.FRR_C_VALUE = Pro2Cmd[2];
	Si446xCmd.FRR_A_READ.FRR_D_VALUE = Pro2Cmd[3];
}

inline void Integration::SI446x::frr_b_read(uint8_t respByteCount) {
	radio_comm_ReadData(SI446X_CMD_ID_FRR_B_READ, 0, respByteCount, Pro2Cmd);

	Si446xCmd.FRR_B_READ.FRR_B_VALUE = Pro2Cmd[0];
	Si446xCmd.FRR_B_READ.FRR_C_VALUE = Pro2Cmd[1];
	Si446xCmd.FRR_B_READ.FRR_D_VALUE = Pro2Cmd[2];
	Si446xCmd.FRR_B_READ.FRR_A_VALUE = Pro2Cmd[3];
}

inline void Integration::SI446x::frr_c_read(uint8_t respByteCount) {
	radio_comm_ReadData(SI446X_CMD_ID_FRR_C_READ, 0, respByteCount, Pro2Cmd);

	Si446xCmd.FRR_C_READ.FRR_C_VALUE = Pro2Cmd[0];
	Si446xCmd.FRR_C_READ.FRR_D_VALUE = Pro2Cmd[1];
	Si446xCmd.FRR_C_READ.FRR_A_VALUE = Pro2Cmd[2];
	Si446xCmd.FRR_C_READ.FRR_B_VALUE = Pro2Cmd[3];

}

inline void Integration::SI446x::frr_d_read(uint8_t respByteCount) {
	radio_comm_ReadData(SI446X_CMD_ID_FRR_D_READ, 0, respByteCount, Pro2Cmd);

	Si446xCmd.FRR_D_READ.FRR_D_VALUE = Pro2Cmd[0];
	Si446xCmd.FRR_D_READ.FRR_A_VALUE = Pro2Cmd[1];
	Si446xCmd.FRR_D_READ.FRR_B_VALUE = Pro2Cmd[2];
	Si446xCmd.FRR_D_READ.FRR_C_VALUE = Pro2Cmd[3];
}

inline void Integration::SI446x::get_adc_reading(uint8_t ADC_EN) {
	Pro2Cmd[0] = SI446X_CMD_ID_GET_ADC_READING;
	Pro2Cmd[1] = ADC_EN;

	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GET_ADC_READING, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_GET_ADC_READING, Pro2Cmd);

	Si446xCmd.GET_ADC_READING.GPIO_ADC = ((uint16_t) Pro2Cmd[0] << 8) & 0xFF00;
	Si446xCmd.GET_ADC_READING.GPIO_ADC |= (uint16_t) Pro2Cmd[1] & 0x00FF;
	Si446xCmd.GET_ADC_READING.BATTERY_ADC = ((uint16_t) Pro2Cmd[2] << 8) & 0xFF00;
	Si446xCmd.GET_ADC_READING.BATTERY_ADC |= (uint16_t) Pro2Cmd[3] & 0x00FF;
	Si446xCmd.GET_ADC_READING.TEMP_ADC = ((uint16_t) Pro2Cmd[4] << 8) & 0xFF00;
	Si446xCmd.GET_ADC_READING.TEMP_ADC |= (uint16_t) Pro2Cmd[5] & 0x00FF;

}

inline void Integration::SI446x::get_packet_info(uint8_t FIELD_NUMBER_MASK, uint16_t LEN,
		int16_t DIFF_LEN) {
	Pro2Cmd[0] = SI446X_CMD_ID_PACKET_INFO;
	Pro2Cmd[1] = FIELD_NUMBER_MASK;
	Pro2Cmd[2] = (uint8_t)(LEN >> 8);
	Pro2Cmd[3] = (uint8_t)(LEN);
	// the different of the byte, althrough it is signed, but to command hander
	// it can treat it as unsigned
	Pro2Cmd[4] = static_cast<uint8_t>(((uint16_t) (DIFF_LEN) >> 8));
	Pro2Cmd[5] = (uint8_t)(DIFF_LEN);

	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_PACKET_INFO, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_PACKET_INFO, Pro2Cmd);

	Si446xCmd.PACKET_INFO.LENGTH = (static_cast<uint16_t>(Pro2Cmd[0]) << 8)
			& 0xFF00;
	Si446xCmd.PACKET_INFO.LENGTH |= (uint16_t) Pro2Cmd[1] & 0x00FF;
}


inline void Integration::SI446x::get_ph_status(uint8_t PH_CLR_PEND) {
Pro2Cmd[0] = SI446X_CMD_ID_GET_PH_STATUS;
Pro2Cmd[1] = PH_CLR_PEND;

radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GET_PH_STATUS, Pro2Cmd,
SI446X_CMD_REPLY_COUNT_GET_PH_STATUS, Pro2Cmd);

Si446xCmd.GET_PH_STATUS.PH_PEND = Pro2Cmd[0];
Si446xCmd.GET_PH_STATUS.PH_STATUS = Pro2Cmd[1];
}

inline void Integration::SI446x::get_modem_status(uint8_t MODEM_CLR_PEND) {
Pro2Cmd[0] = SI446X_CMD_ID_GET_MODEM_STATUS;
Pro2Cmd[1] = MODEM_CLR_PEND;

radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GET_MODEM_STATUS, Pro2Cmd,
SI446X_CMD_REPLY_COUNT_GET_MODEM_STATUS, Pro2Cmd);

Si446xCmd.GET_MODEM_STATUS.MODEM_PEND = Pro2Cmd[0];
Si446xCmd.GET_MODEM_STATUS.MODEM_STATUS = Pro2Cmd[1];
Si446xCmd.GET_MODEM_STATUS.CURR_RSSI = Pro2Cmd[2];
Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI = Pro2Cmd[3];
Si446xCmd.GET_MODEM_STATUS.ANT1_RSSI = Pro2Cmd[4];
Si446xCmd.GET_MODEM_STATUS.ANT2_RSSI = Pro2Cmd[5];
	Si446xCmd.GET_MODEM_STATUS.AFC_FREQ_OFFSET =
			(static_cast<uint16_t>(Pro2Cmd[6]) << 8) & 0xFF00;
	Si446xCmd.GET_MODEM_STATUS.AFC_FREQ_OFFSET |=
			static_cast<uint16_t>(Pro2Cmd[7]) & 0x00FF;

}

void Integration::SI446x::get_chip_status(uint8_t CHIP_CLR_PEND) {
	Pro2Cmd[0] = SI446X_CMD_ID_GET_CHIP_STATUS;
	Pro2Cmd[1] = CHIP_CLR_PEND;

	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GET_CHIP_STATUS, Pro2Cmd,
			SI446X_CMD_REPLY_COUNT_GET_CHIP_STATUS, Pro2Cmd);

	Si446xCmd.GET_CHIP_STATUS.CHIP_PEND = Pro2Cmd[0];
	Si446xCmd.GET_CHIP_STATUS.CHIP_STATUS = Pro2Cmd[1];
	Si446xCmd.GET_CHIP_STATUS.CMD_ERR_STATUS = Pro2Cmd[2];
}

//inline void Integration::SI446x::ircal_manual(uint8_t IRCAL_AMP,
//	Pro2Cmd[0] = SI446X_CMD_ID_IRCAL_MANUAL;
//	Pro2Cmd[1] = IRCAL_AMP;
//	Pro2Cmd[2] = IRCAL_PH;
//{
//	radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_IRCAL_MANUAL,
//			Pro2Cmd,
//			SI446X_CMD_REPLY_COUNT_IRCAL_MANUAL,
//			Pro2Cmd );
//
//	Si446xCmd.IRCAL_MANUAL.IRCAL_AMP_REPLY = Pro2Cmd[0];
//	Si446xCmd.IRCAL_MANUAL.IRCAL_PH_REPLY = Pro2Cmd[1];
//}

void Integration::SI446x::protocol_cfg(uint8_t PROTOCOL) {
}

void Integration::SI446x::request_device_state(void) {
}

void Integration::SI446x::tx_hop(uint8_t INTE, uint8_t FRAC2, uint8_t FRAC1,
		uint8_t FRAC0, uint8_t VCO_CNT1, uint8_t VCO_CNT0,
		uint8_t PLL_SETTLE_TIME1, uint8_t PLL_SETTLE_TIME0) {
	Pro2Cmd[0] = SI446X_CMD_ID_TX_HOP;
	Pro2Cmd[1] = INTE;
	Pro2Cmd[2] = FRAC2;
	Pro2Cmd[3] = FRAC1;
	Pro2Cmd[4] = FRAC0;
	Pro2Cmd[5] = VCO_CNT1;
	Pro2Cmd[6] = VCO_CNT0;
	Pro2Cmd[7] = PLL_SETTLE_TIME1;
	Pro2Cmd[8] = PLL_SETTLE_TIME0;

	radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_TX_HOP, Pro2Cmd);
}

void Integration::SI446x::rx_hop(uint8_t INTE, uint8_t FRAC2, uint8_t FRAC1,
		uint8_t FRAC0, uint8_t VCO_CNT1, uint8_t VCO_CNT0) {
	Pro2Cmd[0] = SI446X_CMD_ID_RX_HOP;
	Pro2Cmd[1] = INTE;
	Pro2Cmd[2] = FRAC2;
	Pro2Cmd[3] = FRAC1;
	Pro2Cmd[4] = FRAC0;
	Pro2Cmd[5] = VCO_CNT1;
	Pro2Cmd[6] = VCO_CNT0;

	radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_RX_HOP, Pro2Cmd);
}

inline void Integration::SI446x::start_tx_fast(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_START_TX;

	radio_comm_SendCmd(1, Pro2Cmd);
}

inline void Integration::SI446x::start_rx_fast(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_START_RX;

	radio_comm_SendCmd(1, Pro2Cmd);
}

void Integration::SI446x::get_int_status_fast_clear(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_GET_INT_STATUS;

	radio_comm_SendCmd(1, Pro2Cmd);
}

inline void Integration::SI446x::get_int_status_fast_clear_read(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_GET_INT_STATUS;

	radio_comm_SendCmdGetResp(1, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_GET_INT_STATUS, Pro2Cmd);

	Si446xCmd.GET_INT_STATUS.INT_PEND = Pro2Cmd[0];
	Si446xCmd.GET_INT_STATUS.INT_STATUS = Pro2Cmd[1];
	Si446xCmd.GET_INT_STATUS.PH_PEND = Pro2Cmd[2];
	Si446xCmd.GET_INT_STATUS.PH_STATUS = Pro2Cmd[3];
	Si446xCmd.GET_INT_STATUS.MODEM_PEND = Pro2Cmd[4];
	Si446xCmd.GET_INT_STATUS.MODEM_STATUS = Pro2Cmd[5];
	Si446xCmd.GET_INT_STATUS.CHIP_PEND = Pro2Cmd[6];
	Si446xCmd.GET_INT_STATUS.CHIP_STATUS = Pro2Cmd[7];
}

inline void Integration::SI446x::gpio_pin_cfg_fast(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_GPIO_PIN_CFG;

	radio_comm_SendCmdGetResp(1, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_GPIO_PIN_CFG, Pro2Cmd);

	Si446xCmd.GPIO_PIN_CFG.GPIO[0] = Pro2Cmd[0];
	Si446xCmd.GPIO_PIN_CFG.GPIO[1] = Pro2Cmd[1];
	Si446xCmd.GPIO_PIN_CFG.GPIO[2] = Pro2Cmd[2];
	Si446xCmd.GPIO_PIN_CFG.GPIO[3] = Pro2Cmd[3];
	Si446xCmd.GPIO_PIN_CFG.NIRQ = Pro2Cmd[4];
	Si446xCmd.GPIO_PIN_CFG.SDO = Pro2Cmd[5];
	Si446xCmd.GPIO_PIN_CFG.GEN_CONFIG = Pro2Cmd[6];
}

inline void Integration::SI446x::get_ph_status_fast_clear(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_GET_PH_STATUS;
	Pro2Cmd[1] = 0;

	radio_comm_SendCmd(2, Pro2Cmd);
}

inline void Integration::SI446x::get_ph_status_fast_clear_read(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_GET_PH_STATUS;

	radio_comm_SendCmdGetResp(1, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_GET_PH_STATUS, Pro2Cmd);

	Si446xCmd.GET_PH_STATUS.PH_PEND = Pro2Cmd[0];
	Si446xCmd.GET_PH_STATUS.PH_STATUS = Pro2Cmd[1];
}

inline void Integration::SI446x::get_modem_status_fast_clear(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_GET_MODEM_STATUS;
	Pro2Cmd[1] = 0;

	radio_comm_SendCmd(2, Pro2Cmd);
}

inline void Integration::SI446x::get_modem_status_fast_clear_read(void) {

	Pro2Cmd[0] = SI446X_CMD_ID_GET_MODEM_STATUS;

	radio_comm_SendCmdGetResp(1, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_GET_MODEM_STATUS, Pro2Cmd);

	Si446xCmd.GET_MODEM_STATUS.MODEM_PEND = Pro2Cmd[0];
	Si446xCmd.GET_MODEM_STATUS.MODEM_STATUS = Pro2Cmd[1];
	Si446xCmd.GET_MODEM_STATUS.CURR_RSSI = Pro2Cmd[2];
	Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI = Pro2Cmd[3];
	Si446xCmd.GET_MODEM_STATUS.ANT1_RSSI = Pro2Cmd[4];
	Si446xCmd.GET_MODEM_STATUS.ANT2_RSSI = Pro2Cmd[5];
	Si446xCmd.GET_MODEM_STATUS.AFC_FREQ_OFFSET = ((uint16_t) Pro2Cmd[6] << 8)
			& 0xFF00;
	Si446xCmd.GET_MODEM_STATUS.AFC_FREQ_OFFSET |= (uint16_t) Pro2Cmd[7] & 0x00FF;
}

void Integration::SI446x::get_chip_status_fast_clear(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_GET_CHIP_STATUS;
	Pro2Cmd[1] = 0;

	radio_comm_SendCmd(2, Pro2Cmd);
}

inline void Integration::SI446x::get_chip_status_fast_clear_read(void) {
	this->Pro2Cmd[0] = SI446X_CMD_ID_GET_CHIP_STATUS;

	radio_comm_SendCmdGetResp(1, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_GET_CHIP_STATUS, Pro2Cmd);

	Si446xCmd.GET_CHIP_STATUS.CHIP_PEND = Pro2Cmd[0];
	Si446xCmd.GET_CHIP_STATUS.CHIP_STATUS = Pro2Cmd[1];
	Si446xCmd.GET_CHIP_STATUS.CMD_ERR_STATUS = Pro2Cmd[2];
}

void Integration::SI446x::fifo_info_fast_reset(uint8_t FIFO) {
	Pro2Cmd[0] = SI446X_CMD_ID_FIFO_INFO;
	Pro2Cmd[1] = FIFO;

	radio_comm_SendCmd(2, Pro2Cmd);
}

void Integration::SI446x::fifo_info_fast_read(void) {
	Pro2Cmd[0] = SI446X_CMD_ID_FIFO_INFO;

	radio_comm_SendCmdGetResp(1, Pro2Cmd,
	SI446X_CMD_REPLY_COUNT_FIFO_INFO, Pro2Cmd);

	Si446xCmd.FIFO_INFO.RX_FIFO_COUNT = Pro2Cmd[0];
	Si446xCmd.FIFO_INFO.TX_FIFO_SPACE = Pro2Cmd[1];
}

inline void Integration::SI446x::radio_comm_WriteData(uint8_t cmd, bool pollCts,
		uint8_t byteCount, uint8_t *pData) {
	if (pollCts) {
		while (!ctsWentHigh) {
			radio_comm_PollCTS();
		}
	}
	radio_hal_ClearNsel();
	radio_hal_SpiWriteByte(cmd);
	radio_hal_SpiWriteData(byteCount, pData);
	radio_hal_SetNsel();
	ctsWentHigh = false;
}

inline uint8_t Integration::SI446x::radio_comm_GetResp(uint8_t byteCount,
		uint8_t *pData) {

	uint8_t ctsVal = 0u;
	uint16_t errCnt = 10000;

	while (errCnt != 0)      //wait until radio IC is ready with the data
	{
		radio_hal_ClearNsel();
		radio_hal_SpiWriteByte(0x44);    //read CMD buffer
		ctsVal = radio_hal_SpiReadByte();
		if (ctsVal == 0xFF) {
			if (byteCount) {
				radio_hal_SpiReadData(byteCount, pData);
			}
			radio_hal_SetNsel();
			break;
		}
		radio_hal_SetNsel();
		errCnt--;
	}

	if (errCnt == 0) {
		while (1) {
			/* ERROR!!!!  CTS should never take this long. */
#ifdef RADIO_COMM_ERROR_CALLBACK
	        RADIO_COMM_ERROR_CALLBACK();
	      #endif
		}
	}

	if (ctsVal == 0xFF) {
		ctsWentHigh = true;
	}

	return ctsVal;
}

inline void Integration::SI446x::radio_comm_SendCmd(uint8_t byteCount,
		uint8_t *pData) {
	while (!ctsWentHigh) {
		radio_comm_PollCTS();
	}
	radio_hal_ClearNsel();
	radio_hal_SpiWriteData(byteCount, pData);
	radio_hal_SetNsel();
	ctsWentHigh = false;
}

inline void Integration::SI446x::radio_comm_ReadData(uint8_t cmd, bool pollCts,
		uint8_t byteCount, uint8_t *pData) {
	if (pollCts) {
		while (!ctsWentHigh) {
			radio_comm_PollCTS();
		}
	}
	radio_hal_ClearNsel();
	radio_hal_SpiWriteByte(cmd);
	radio_hal_SpiReadData(byteCount, pData);
	radio_hal_SetNsel();
	ctsWentHigh = false;
}
uint8_t Integration::SI446x::radio_comm_PollCTS(void) {

    while( HAL_GPIO_ReadPin(_cts_port, _cts_pin) == GPIO_PIN_RESET)
    {
    	osDelay(10);
    }
    ctsWentHigh = true;
    return 0xFF;
//	return radio_comm_GetResp(0, 0);
//#endif
}

uint8_t Integration::SI446x::radio_comm_SendCmdGetResp(
		uint8_t cmdByteCount, uint8_t *pCmdData, uint8_t respByteCount,
		uint8_t *pRespData) {

	radio_comm_SendCmd(cmdByteCount, pCmdData);
	return radio_comm_GetResp(respByteCount, pRespData);
}

void Integration::SI446x::radio_comm_ClearCTS(void) {
	ctsWentHigh = false;
}

void Integration::SI446x::radio_hal_AssertShutdown(void) {
	PWRDN = true;
	shutdown(PWRDN);
}

void Integration::SI446x::radio_hal_DeassertShutdown(void) {
	PWRDN = false;
	shutdown(PWRDN);
}

void Integration::SI446x::radio_hal_ClearNsel(void) {
	RF_NSEL = false;
	nsel(RF_NSEL);
}
void Integration::SI446x::shutdown(bool b)
{
	HAL_GPIO_WritePin(_shutdown_port, _shutdown_pin, b?GPIO_PIN_SET:GPIO_PIN_RESET);
}
void Integration::SI446x::nsel(bool b)
{
	HAL_GPIO_WritePin(_nsel_port, _nsel_pin, b?GPIO_PIN_SET:GPIO_PIN_RESET);
}

void Integration::SI446x::radio_hal_SetNsel(void) {
	RF_NSEL = true;
	nsel(RF_NSEL);
}

bool Integration::SI446x::radio_hal_NirqLevel(void) {
	return RF_NIRQ;
}

void Integration::SI446x::radio_hal_SpiWriteByte(uint8_t pData) {

	HAL_SPI_Transmit(_hspi, &pData, 1, 1000);
}

uint8_t Integration::SI446x::radio_hal_SpiReadByte(void) {

	uint8_t  data;
	HAL_SPI_Receive(_hspi, &data, 1, 1000);
	return data;
}

void Integration::SI446x::radio_hal_SpiWriteData(uint8_t byteCount, uint8_t *pData) {

	HAL_SPI_Transmit(_hspi, pData, byteCount, 1000);
}

void Integration::SI446x::radio_hal_SpiReadData(uint8_t byteCount, uint8_t *pData) {
	HAL_SPI_Receive(_hspi, pData, byteCount, 1000);
}
