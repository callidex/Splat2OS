/*
 * SI446x.h
 *
 *  Created on: Feb 12, 2022
 *      Author: paul
 */

#ifndef SRC_INTEGRATION_SI446X_H_
#define SRC_INTEGRATION_SI446X_H_

#ifdef __cplusplus
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <cmsis_os.h>
#include "si446x_cmd.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_spi.h"


#define SI466X_FIFO_SIZE 64
#define RADIO_CTS_TIMEOUT

class SI446x {
public:
	osThreadId_t createThread();
	enum {
		SI446X_SUCCESS,
		SI446X_NO_PATCH,
		SI446X_CTS_TIMEOUT,
		SI446X_PATCH_FAIL,
		SI446X_COMMAND_ERROR
	};
	uint8_t Pro2Cmd[7];
	union si446x_cmd_reply_union Si446xCmd;
	SI446x();
	void led(bool on);
//    SI446x(SPI_HandleTypeDef *  hspi, PortMap *portmap, Logger* logger );
	virtual ~SI446x();
	void reset(void);
	void shutdown(bool b);
	void nsel(bool b);

	void power_up(uint8_t BOOT_OPTIONS, uint8_t XTAL_OPTIONS,
			uint32_t XO_FREQ);
	uint8_t configuration_init(const uint8_t *pSetPropCmd);
	uint8_t apply_patch(void);
	void part_info(void);
	void start_tx(uint8_t CHANNEL, uint8_t CONDITION, uint16_t TX_LEN);
	void start_rx(uint8_t CHANNEL, uint8_t CONDITION, uint16_t RX_LEN,
			uint8_t NEXT_STATE1, uint8_t NEXT_STATE2, uint8_t NEXT_STATE3);

	void get_int_status(uint8_t PH_CLR_PEND, uint8_t MODEM_CLR_PEND,
			uint8_t CHIP_CLR_PEND);
	void gpio_pin_cfg(uint8_t GPIO0, uint8_t GPIO1, uint8_t GPIO2,
			uint8_t GPIO3, uint8_t NIRQ, uint8_t SDO, uint8_t GEN_CONFIG);

	void set_property(uint8_t GROUP, uint8_t NUM_PROPS,
			uint8_t START_PROP, ...);
	void change_state(uint8_t NEXT_STATE1);
	void nop(void);

	void fifo_info(uint8_t FIFO);
	void write_tx_fifo(uint8_t numBytes, uint8_t *pTxData);
	void read_rx_fifo(uint8_t numBytes, uint8_t *pRxData);

	void get_property(uint8_t GROUP, uint8_t NUM_PROPS,
			uint8_t START_PROP);
	void func_info(void);

	void frr_a_read(uint8_t respByteCount);
	void frr_b_read(uint8_t respByteCount);
	void frr_c_read(uint8_t respByteCount);
	void frr_d_read(uint8_t respByteCount);

	void get_adc_reading(uint8_t ADC_EN);
	void get_packet_info(uint8_t FIELD_NUMBER_MASK, uint16_t LEN,
			int16_t DIFF_LEN);
	void get_ph_status(uint8_t PH_CLR_PEND);
	void get_modem_status(uint8_t MODEM_CLR_PEND);
	void get_chip_status(uint8_t CHIP_CLR_PEND);

	void ircal_manual(uint8_t IRCAL_AMP, uint8_t IRCAL_PH);
	void protocol_cfg(uint8_t PROTOCOL);

	void request_device_state(void);

	void tx_hop(uint8_t INTE, uint8_t FRAC2, uint8_t FRAC1,
			uint8_t FRAC0, uint8_t VCO_CNT1, uint8_t VCO_CNT0,
			uint8_t PLL_SETTLE_TIME1, uint8_t PLL_SETTLE_TIME0);
	void rx_hop(uint8_t INTE, uint8_t FRAC2, uint8_t FRAC1,
			uint8_t FRAC0, uint8_t VCO_CNT1, uint8_t VCO_CNT0);

	void start_tx_fast(void);
	void start_rx_fast(void);

	void get_int_status_fast_clear(void);
	void get_int_status_fast_clear_read(void);

	void gpio_pin_cfg_fast(void);

	void get_ph_status_fast_clear(void);
	void get_ph_status_fast_clear_read(void);

	void get_modem_status_fast_clear(void);
	void get_modem_status_fast_clear_read(void);

	void get_chip_status_fast_clear(void);
	void get_chip_status_fast_clear_read(void);

	void fifo_info_fast_reset(uint8_t FIFO);
	void fifo_info_fast_read(void);

	osThreadId_t getThreadHandle() { return threadHandle; };
private:
//	PortMap * _portmap;
//	Logger * _logger;
	uint16_t _shutdown_pin, _nsel_pin, _cts_pin;
	GPIO_TypeDef * _shutdown_port, *_nsel_port, *_cts_port;
	void (*nsel_func)(bool);
	void (*shutdown_func)(bool);

	osThreadId_t threadHandle;
	void threadRunner(void const * argument);
	SPI_HandleTypeDef *  _hspi;
	uint8_t radio_comm_GetResp(uint8_t byteCount, uint8_t *pData);
	void radio_comm_SendCmd(uint8_t byteCount, uint8_t *pData);
	void radio_comm_ReadData(uint8_t cmd, bool pollCts, uint8_t byteCount,
			uint8_t *pData);
	void radio_comm_WriteData(uint8_t cmd, bool pollCts, uint8_t byteCount,
			uint8_t *pData);

	uint8_t radio_comm_PollCTS(void);
	uint8_t radio_comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t *pCmdData,
			uint8_t respByteCount, uint8_t *pRespData);
	void radio_comm_ClearCTS(void);
	bool ctsWentHigh = false;
	bool ctsVal= false;
	uint8_t errCnt= 0;
    bool PWRDN;
    bool RF_NSEL;
    bool RF_NIRQ;
	bool radio_hal_Gpio3Level(void);
	bool radio_hal_Gpio2Level(void);
	bool radio_hal_Gpio1Level(void);
	bool radio_hal_Gpio0Level(void);
	void radio_hal_AssertShutdown(void);
	void radio_hal_DeassertShutdown(void);
	void radio_hal_ClearNsel(void);
	void radio_hal_SetNsel(void);
	bool radio_hal_NirqLevel(void);
	void radio_hal_SpiWriteByte(uint8_t byteToWrite);

	uint8_t radio_hal_SpiReadByte(void);
	void radio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData);

	void radio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData);

};
 #endif




#endif /* SRC_INTEGRATION_SI446X_H_ */



