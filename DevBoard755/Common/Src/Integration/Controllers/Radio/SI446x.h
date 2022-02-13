/*
 * SI446x.h
 *
 *  Created on: Feb 12, 2022
 *      Author: paul
 */

#ifndef SRC_INTEGRATION_SI446X_H_
#define SRC_INTEGRATION_SI446X_H_
#ifdef __cplusplus
#include "si446x_cmd.h"

namespace Integration {

#define SI466X_FIFO_SIZE 64

class SI446x {
public:
	enum {
		SI446X_SUCCESS,
		SI446X_NO_PATCH,
		SI446X_CTS_TIMEOUT,
		SI446X_PATCH_FAIL,
		SI446X_COMMAND_ERROR
	};

	SI446x();
	virtual ~SI446x();
	void si446x_reset(void);
	void si446x_power_up(uint8_t BOOT_OPTIONS, uint8_t XTAL_OPTIONS,
			uint32_t XO_FREQ);
	uint8_t si446x_configuration_init(const uint8_t *pSetPropCmd);
	uint8_t si446x_apply_patch(void);
	void si446x_part_info(void);
	void si446x_start_tx(uint8_t CHANNEL, uint8_t CONDITION, U16 TX_LEN);
	void si446x_start_rx(uint8_t CHANNEL, uint8_t CONDITION, U16 RX_LEN,
			uint8_t NEXT_STATE1, uint8_t NEXT_STATE2, uint8_t NEXT_STATE3);

	void si446x_get_int_status(uint8_t PH_CLR_PEND, uint8_t MODEM_CLR_PEND,
			uint8_t CHIP_CLR_PEND);
	void si446x_gpio_pin_cfg(uint8_t GPIO0, uint8_t GPIO1, uint8_t GPIO2,
			uint8_t GPIO3, uint8_t NIRQ, uint8_t SDO, uint8_t GEN_CONFIG);

	void si446x_set_property(uint8_t GROUP, uint8_t NUM_PROPS,
			uint8_t START_PROP, ...);
	void si446x_change_state(uint8_t NEXT_STATE1);
	void si446x_nop(void);

	void si446x_fifo_info(uint8_t FIFO);
	void si446x_write_tx_fifo(uint8_t numBytes, uint8_t *pData);
	void si446x_read_rx_fifo(uint8_t numBytes, uint8_t *pRxData);

	void si446x_get_property(uint8_t GROUP, uint8_t NUM_PROPS,
			uint8_t START_PROP);
	void si446x_func_info(void);

	void si446x_frr_a_read(uint8_t respByteCount);
	void si446x_frr_b_read(uint8_t respByteCount);
	void si446x_frr_c_read(uint8_t respByteCount);
	void si446x_frr_d_read(uint8_t respByteCount);

	void si446x_get_adc_reading(uint8_t ADC_EN);
	void si446x_get_packet_info(uint8_t FIELD_NUMBER_MASK, U16 LEN,
			S16 DIFF_LEN);
	void si446x_get_ph_status(uint8_t PH_CLR_PEND);
	void si446x_get_modem_status(uint8_t MODEM_CLR_PEND);
	void si446x_get_chip_status(uint8_t CHIP_CLR_PEND);

	void si446x_ircal_manual(uint8_t IRCAL_AMP, uint8_t IRCAL_PH);
	void si446x_protocol_cfg(uint8_t PROTOCOL);

	void si446x_request_device_state(void);

	void si446x_tx_hop(uint8_t INTE, uint8_t FRAC2, uint8_t FRAC1,
			uint8_t FRAC0, uint8_t VCO_CNT1, uint8_t VCO_CNT0,
			uint8_t PLL_SETTLE_TIME1, uint8_t PLL_SETTLE_TIME0);
	void si446x_rx_hop(uint8_t INTE, uint8_t FRAC2, uint8_t FRAC1,
			uint8_t FRAC0, uint8_t VCO_CNT1, uint8_t VCO_CNT0);

	void si446x_start_tx_fast(void);
	void si446x_start_rx_fast(void);

	void si446x_get_int_status_fast_clear(void);
	void si446x_get_int_status_fast_clear_read(void);

	void si446x_gpio_pin_cfg_fast(void);

	void si446x_get_ph_status_fast_clear(void);
	void si446x_get_ph_status_fast_clear_read(void);

	void si446x_get_modem_status_fast_clear(void);
	void si446x_get_modem_status_fast_clear_read(void);

	void si446x_get_chip_status_fast_clear(void);
	void si446x_get_chip_status_fast_clear_read(void);

	void si446x_fifo_info_fast_reset(uint8_t FIFO);
	void si446x_fifo_info_fast_read(void);

private:

	uint8_t radio_comm_GetResp(uint8_t byteCount, U8 *pData);
	void radio_comm_SendCmd(uint8_t byteCount, uint8_t *pData);
	void radio_comm_ReadData(uint8_t cmd, BIT pollCts, uint8_t byteCount,
			uint8_t *pData);
	void radio_comm_WriteData(uint8_t cmd, BIT pollCts, uint8_t byteCount,
			uint8_t *pData);

	uint8_t radio_comm_PollCTS(void);
	uint8_t radio_comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t *pCmdData,
			uint8_t respByteCount, uint8_t *pRespData);
	void radio_comm_ClearCTS(void);
	BIT ctsWentHigh = 0;
};

}

inline uint8_t Integration::SI446x::radio_comm_GetResp(uint8_t byteCount,
		U8 *pData) {

	SEGMENT_VARIABLE(ctsVal = 0u, U8, SEG_DATA);
	SEGMENT_VARIABLE(errCnt = RADIO_CTS_TIMEOUT, U16, SEG_DATA);

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
		ctsWentHigh = 1;
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
	ctsWentHigh = 0;
}

inline void Integration::SI446x::radio_comm_ReadData(uint8_t cmd, BIT pollCts,
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
	ctsWentHigh = 0;
}

inline void Integration::SI446x::radio_comm_WriteData(uint8_t cmd, BIT pollCts,
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
	ctsWentHigh = 0;
}

inline uint8_t Integration::SI446x::radio_comm_PollCTS(void) {
#ifdef RADIO_USER_CFG_USE_GPIO1_FOR_CTS
    while(!radio_hal_Gpio1Level())
    {
        /* Wait...*/
    }
    ctsWentHigh = 1;
    return 0xFF;
#else
	return radio_comm_GetResp(0, 0);
#endif
}

inline uint8_t Integration::SI446x::radio_comm_SendCmdGetResp(
		uint8_t cmdByteCount, uint8_t *pCmdData, uint8_t respByteCount,
		uint8_t *pRespData) {

	radio_comm_SendCmd(cmdByteCount, pCmdData);
	return radio_comm_GetResp(respByteCount, pRespData);
}

inline void Integration::SI446x::radio_comm_ClearCTS(void) {
	ctsWentHigh = 0;
}

#endif
#endif /* SRC_INTEGRATION_SI446X_H_ */
