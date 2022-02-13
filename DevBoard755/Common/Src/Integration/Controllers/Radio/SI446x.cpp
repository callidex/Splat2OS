/*
 * SI446x.cpp
 *
 *  Created on: Feb 12, 2022
 *      Author: paul
 */

#include <SI446x.h>

namespace Integration {

SI446x::SI446x() {
	// TODO Auto-generated constructor stub

}

SI446x::~SI446x() {
	// TODO Auto-generated destructor stub
}


inline void SI446x::reset(void) {
    uint8_t loopCount;

    /* Put radio in shutdown, wait then release */
    radio_hal_AssertShutdown();
    osDelay(10);
    radio_hal_DeassertShutdown();
    osDelay(10);
    radio_comm_ClearCTS();
}

inline void SI446x::power_up(uint8_t BOOT_OPTIONS,
		uint8_t XTAL_OPTIONS, uint32_t XO_FREQ) {
	 Pro2Cmd[0] = SI446X_CMD_ID_POWER_UP;
	    Pro2Cmd[1] = BOOT_OPTIONS;
	    Pro2Cmd[2] = XTAL_OPTIONS;
	    Pro2Cmd[3] = (U8)(XO_FREQ >> 24);
	    Pro2Cmd[4] = (U8)(XO_FREQ >> 16);
	    Pro2Cmd[5] = (U8)(XO_FREQ >> 8);
	    Pro2Cmd[6] = (U8)(XO_FREQ);

	    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_POWER_UP, Pro2Cmd );
}

inline uint8_t SI446x::configuration_init(
		const uint8_t *pSetPropCmd) {
	SEGMENT_VARIABLE(col, U8, SEG_DATA);
	  SEGMENT_VARIABLE(numOfBytes, U8, SEG_DATA);

	  /* While cycle as far as the pointer points to a command */
	  while (*pSetPropCmd != 0x00)
	  {
	    /* Commands structure in the array:
	     * --------------------------------
	     * LEN | <LEN length of data>
	     */

	    numOfBytes = *pSetPropCmd++;

	    if (numOfBytes > 16u)
	    {
	      /* Number of command bytes exceeds maximal allowable length */
	      return SI446X_COMMAND_ERROR;
	    }

	    for (col = 0u; col < numOfBytes; col++)
	    {
	      Pro2Cmd[col] = *pSetPropCmd;
	      pSetPropCmd++;
	    }

	    if (radio_comm_SendCmdGetResp(numOfBytes, Pro2Cmd, 0, 0) != 0xFF)
	    {
	      /* Timeout occured */
	      return SI446X_CTS_TIMEOUT;
	    }

	    if (radio_hal_NirqLevel() == 0)
	    {
	      /* Get and clear all interrupts.  An error has occured... */
	      si446x_get_int_status(0, 0, 0);
	      if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CHIP_PEND_CMD_ERROR_PEND_MASK)
	      {
	        return SI446X_COMMAND_ERROR;
	      }
	    }
	  }

	  return SI446X_SUCCESS;

}

inline uint8_t SI446x::apply_patch(void) {
}

inline void SI446x::part_info(void) {

	  Pro2Cmd[0] = SI446X_CMD_ID_PART_INFO;

	    radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_PART_INFO,
	                              Pro2Cmd,
	                              SI446X_CMD_REPLY_COUNT_PART_INFO,
	                              Pro2Cmd );

	    Si446xCmd.PART_INFO.CHIPREV         = Pro2Cmd[0];
	    Si446xCmd.PART_INFO.PART            = ((U16)Pro2Cmd[1] << 8) & 0xFF00;
	    Si446xCmd.PART_INFO.PART           |= (U16)Pro2Cmd[2] & 0x00FF;
	    Si446xCmd.PART_INFO.PBUILD          = Pro2Cmd[3];
	    Si446xCmd.PART_INFO.ID              = ((U16)Pro2Cmd[4] << 8) & 0xFF00;
	    Si446xCmd.PART_INFO.ID             |= (U16)Pro2Cmd[5] & 0x00FF;
	    Si446xCmd.PART_INFO.CUSTOMER        = Pro2Cmd[6];
	    Si446xCmd.PART_INFO.ROMID           = Pro2Cmd[7];

}

inline void SI446x::start_tx(uint8_t CHANNEL,
		uint8_t CONDITION, U16 TX_LEN) {
    Pro2Cmd[0] = SI446X_CMD_ID_START_TX;
    Pro2Cmd[1] = CHANNEL;
    Pro2Cmd[2] = CONDITION;
    Pro2Cmd[3] = (U8)(TX_LEN >> 8);
    Pro2Cmd[4] = (U8)(TX_LEN);
    Pro2Cmd[5] = 0x00;

    // Don't repeat the packet,
    // ie. transmit the packet only once
    Pro2Cmd[6] = 0x00;

    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_START_TX, Pro2Cmd );


}

inline void SI446x::start_rx(uint8_t CHANNEL,
		uint8_t CONDITION, U16 RX_LEN, uint8_t NEXT_STATE1, uint8_t NEXT_STATE2,
		uint8_t NEXT_STATE3) {


    Pro2Cmd[0] = SI446X_CMD_ID_START_RX;
    Pro2Cmd[1] = CHANNEL;
    Pro2Cmd[2] = CONDITION;
    Pro2Cmd[3] = (U8)(RX_LEN >> 8);
    Pro2Cmd[4] = (U8)(RX_LEN);
    Pro2Cmd[5] = NEXT_STATE1;
    Pro2Cmd[6] = NEXT_STATE2;
    Pro2Cmd[7] = NEXT_STATE3;

    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_START_RX, Pro2Cmd );
}

inline void SI446x::get_int_status(uint8_t PH_CLR_PEND,
		uint8_t MODEM_CLR_PEND, uint8_t CHIP_CLR_PEND) {

    Pro2Cmd[0] = SI446X_CMD_ID_GET_INT_STATUS;
    Pro2Cmd[1] = PH_CLR_PEND;
    Pro2Cmd[2] = MODEM_CLR_PEND;
    Pro2Cmd[3] = CHIP_CLR_PEND;

    radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GET_INT_STATUS,
                              Pro2Cmd,
                              SI446X_CMD_REPLY_COUNT_GET_INT_STATUS,
                              Pro2Cmd );

    Si446xCmd.GET_INT_STATUS.INT_PEND       = Pro2Cmd[0];
    Si446xCmd.GET_INT_STATUS.INT_STATUS     = Pro2Cmd[1];
    Si446xCmd.GET_INT_STATUS.PH_PEND        = Pro2Cmd[2];
    Si446xCmd.GET_INT_STATUS.PH_STATUS      = Pro2Cmd[3];
    Si446xCmd.GET_INT_STATUS.MODEM_PEND     = Pro2Cmd[4];
    Si446xCmd.GET_INT_STATUS.MODEM_STATUS   = Pro2Cmd[5];
    Si446xCmd.GET_INT_STATUS.CHIP_PEND      = Pro2Cmd[6];
    Si446xCmd.GET_INT_STATUS.CHIP_STATUS    = Pro2Cmd[7];

}

inline void SI446x::gpio_pin_cfg(uint8_t GPIO0,
		uint8_t GPIO1, uint8_t GPIO2, uint8_t GPIO3, uint8_t NIRQ, uint8_t SDO,
		uint8_t GEN_CONFIG) {

    Pro2Cmd[0] = SI446X_CMD_ID_GPIO_PIN_CFG;
    Pro2Cmd[1] = GPIO0;
    Pro2Cmd[2] = GPIO1;
    Pro2Cmd[3] = GPIO2;
    Pro2Cmd[4] = GPIO3;
    Pro2Cmd[5] = NIRQ;
    Pro2Cmd[6] = SDO;
    Pro2Cmd[7] = GEN_CONFIG;

    radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GPIO_PIN_CFG,
                              Pro2Cmd,
                              SI446X_CMD_REPLY_COUNT_GPIO_PIN_CFG,
                              Pro2Cmd );

    Si446xCmd.GPIO_PIN_CFG.GPIO[0]        = Pro2Cmd[0];
    Si446xCmd.GPIO_PIN_CFG.GPIO[1]        = Pro2Cmd[1];
    Si446xCmd.GPIO_PIN_CFG.GPIO[2]        = Pro2Cmd[2];
    Si446xCmd.GPIO_PIN_CFG.GPIO[3]        = Pro2Cmd[3];
    Si446xCmd.GPIO_PIN_CFG.NIRQ         = Pro2Cmd[4];
    Si446xCmd.GPIO_PIN_CFG.SDO          = Pro2Cmd[5];
    Si446xCmd.GPIO_PIN_CFG.GEN_CONFIG   = Pro2Cmd[6];

}

inline void SI446x::set_property(uint8_t GROUP,
		uint8_t NUM_PROPS, uint8_t START_PROP, ...) {
	  va_list argList;
	    U8 cmdIndex;

	    Pro2Cmd[0] = SI446X_CMD_ID_SET_PROPERTY;
	    Pro2Cmd[1] = GROUP;
	    Pro2Cmd[2] = NUM_PROPS;
	    Pro2Cmd[3] = START_PROP;

	    va_start (argList, START_PROP);
	    cmdIndex = 4;
	    while(NUM_PROPS--)
	    {
	        Pro2Cmd[cmdIndex] = va_arg (argList, U8);
	        cmdIndex++;
	    }
	    va_end(argList);

	    radio_comm_SendCmd( cmdIndex, Pro2Cmd );

}

inline void SI446x::change_state(uint8_t NEXT_STATE1) {
	   Pro2Cmd[0] = SI446X_CMD_ID_CHANGE_STATE;
	    Pro2Cmd[1] = NEXT_STATE1;

	    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_CHANGE_STATE, Pro2Cmd );
}

inline void SI446x::nop(void) {
    Pro2Cmd[0] = SI446X_CMD_ID_NOP;

    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_NOP, Pro2Cmd );

}

inline void SI446x::fifo_info(uint8_t FIFO) {
}

inline void SI446x::write_tx_fifo(uint8_t numBytes,
		uint8_t *pData) {
}

inline void SI446x::read_rx_fifo(uint8_t numBytes,
		uint8_t *pRxData) {
}

inline void SI446x::get_property(uint8_t GROUP,
		uint8_t NUM_PROPS, uint8_t START_PROP) {
}

inline void SI446x::func_info(void) {
}

inline void SI446x::frr_a_read(uint8_t respByteCount) {
}

inline void SI446x::frr_b_read(uint8_t respByteCount) {
}

inline void SI446x::frr_c_read(uint8_t respByteCount) {
}

inline void SI446x::frr_d_read(uint8_t respByteCount) {
}

inline void SI446x::get_adc_reading(uint8_t ADC_EN) {
}

inline void SI446x::get_packet_info(
		uint8_t FIELD_NUMBER_MASK, U16 LEN, S16 DIFF_LEN) {
}

inline void SI446x::get_ph_status(uint8_t PH_CLR_PEND) {
}

inline void SI446x::get_modem_status(
		uint8_t MODEM_CLR_PEND) {
}

inline void SI446x::get_chip_status(uint8_t CHIP_CLR_PEND) {
}

inline void SI446x::ircal_manual(uint8_t IRCAL_AMP,
		uint8_t IRCAL_PH) {
}

inline void SI446x::protocol_cfg(uint8_t PROTOCOL) {
}

inline void SI446x::request_device_state(void) {
}

inline void SI446x::tx_hop(uint8_t INTE, uint8_t FRAC2,
		uint8_t FRAC1, uint8_t FRAC0, uint8_t VCO_CNT1, uint8_t VCO_CNT0,
		uint8_t PLL_SETTLE_TIME1, uint8_t PLL_SETTLE_TIME0) {
}

inline void SI446x::rx_hop(uint8_t INTE, uint8_t FRAC2,
		uint8_t FRAC1, uint8_t FRAC0, uint8_t VCO_CNT1, uint8_t VCO_CNT0) {
}

inline void SI446x::start_tx_fast(void) {
}

inline void SI446x::start_rx_fast(void) {
}

inline void SI446x::get_int_status_fast_clear(void) {
}

inline void SI446x::get_int_status_fast_clear_read(void) {
}

inline void SI446x::gpio_pin_cfg_fast(void) {
}

inline void SI446x::get_ph_status_fast_clear(void) {
}

inline void SI446x::get_ph_status_fast_clear_read(void) {
}

inline void SI446x::get_modem_status_fast_clear(void) {
}

inline void SI446x::get_modem_status_fast_clear_read(void) {
}

inline void SI446x::get_chip_status_fast_clear(void) {
}

inline void SI446x::get_chip_status_fast_clear_read(void) {
}

inline void SI446x::fifo_info_fast_reset(uint8_t FIFO) {
}

inline void SI446x::fifo_info_fast_read(void) {
}

} /* namespace Integration */
