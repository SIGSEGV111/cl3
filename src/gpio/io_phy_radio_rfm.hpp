/*
    libcl3 - common library version 3
    Copyright (C) 2013	Simon Brennecke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef	_include_cl3_gpio_io_phy_radio_rfm_hpp_
#define	_include_cl3_gpio_io_phy_radio_rfm_hpp_

#include "io_phy.hpp"
#include <cl3/core/io_stream.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/error.hpp>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			namespace	radio
			{
				namespace	rfm
				{
					class CL3PUBT TRFM;
					class CL3PUBT TPin;

					enum	EOpCode
					{
						OPCODE_NOP = 0x00,
						OPCODE_PART_INFO = 0x01,
						OPCODE_POWER_UP = 0x02,
						OPCODE_PATCH_IMAGE = 0x04,
						OPCODE_FUNC_INFO = 0x10,
						OPCODE_SET_PROPERTY = 0x11,
						OPCODE_GET_PROPERTY = 0x12,
						OPCODE_GPIO_PIN_CFG = 0x13,
						OPCODE_GET_SENSOR_READING = 0x14,
						OPCODE_FIFO_INFO = 0x15,
						OPCODE_GET_INT_STATUS = 0x20,
						OPCODE_GET_PH_STATUS = 0x21,
						OPCODE_GET_MODEM_STATUS = 0x22,
						OPCODE_GET_CHIP_STATUS = 0x23,
						OPCODE_START_TX = 0x31,
						OPCODE_START_RX = 0x32,
						OPCODE_REQUEST_DEVICE_STATE = 0x33,
						OPCODE_CHANGE_STATE = 0x34,
						OPCODE_READ_CMD_BUFF = 0x44,
						OPCODE_FAST_RESPONSE_A = 0x50,
						OPCODE_FAST_RESPONSE_B = 0x51,
						OPCODE_FAST_RESPONSE_C = 0x53,
						OPCODE_FAST_RESPONSE_D = 0x57,
						OPCODE_WRITE_TX_FIFO = 0x66,
						OPCODE_READ_RX_FIFO = 0x77
					};

					CL3_PACK(struct TOpCode
					{
						u8_t opcode;
						union
						{
							struct
							{
								byte_t fifo[64];
							};
						};
					});

					CL3_PACK(struct	TPartInfo
					{
						u8_t chiprev;
						u16_t part;
						u8_t pbuild;
						u16_t id;
						u8_t customer;
						u8_t romid;

						CL3PUBF	bool	operator==	(const TPartInfo& rhs) const;
						CL3PUBF	bool	operator!=	(const TPartInfo& rhs) const;
					});

					CL3_PACK(struct	TPaketHandlerIRQ
					{
						byte_t	rx_fifo_almost_full : 1,
								tx_fifo_almost_empty : 1,
								alt_crc_error : 1,
								crc_error : 1,
								packet_rx : 1,
								packet_sent : 1,
								filter_miss : 1,
								filter_match : 1;
					});

					CL3_PACK(struct	TModemIRQ
					{
						byte_t	sync_detect : 1,
								preamble_detect : 1,
								invalid_preamble : 1,
								rssi : 1,
								rssi_jump : 1,
								invalid_sync : 1,
								postamble_detect : 1,
								__unused0 : 1;
					});

					CL3_PACK(struct	TChipIRQ
					{
						byte_t	wut : 1,
								low_batt : 1,
								chip_ready : 1,
								cmd_error : 1,
								state_change : 1,
								fifo_underflow_overflow_error : 1,
								cal : 1,
								__unused0 : 1;
					});

					CL3_PACK(struct	TOnIRQData
					{
						TPaketHandlerIRQ ph_pending;
						TPaketHandlerIRQ ph_status;
						TModemIRQ modem_pending;
						TModemIRQ modem_status;
						TChipIRQ chip_pending;
						TChipIRQ chip_status;

						CL3PUBF	void	Dump	() const;
					});

					struct	TIRQStatus
					{
						TPaketHandlerIRQ ph_status;
						TModemIRQ modem_status;
						TChipIRQ chip_status;
					};

					struct	TOnCmdData
					{
						EOpCode opcode;
						const byte_t* p_args;
						usys_t sz_args;
						const byte_t* p_retval;
						usys_t sz_retval;
					};

					typedef event::TEvent<TRFM, TOnIRQData&> TOnIRQEvent;
					typedef event::TEvent<TRFM, const TOnCmdData&> TOnCmdEvent;

					class	TPin : public gpio::IPin
					{
						public:
							enum	EMode
							{
								MODE_TRISTATE = 0,
								MODE_INPUT = 1,
								MODE_OUTPUT = 2,
								MODE_IRQ,
								MODE_SDO,
								MODE_RXSTATE,
								MODE_TXSTATE,
								MODE_RXDATA,
								MODE_RXCLOCK,
								MODE_DIVCLOCK
							};

						friend class TRFM;
						private:
							CLASS TPin(const TPin&) = delete;
							TPin& operator=(const TPin&) = delete;

						protected:
							TRFM* rfm;
							u8_t index;
							EMode mode;
							gpio::EPull pull;
							bool level;

							CL3PUBF	CLASS	TPin	(TRFM* rfm, u8_t index);

						public:
							CL3PUBF	CLASS	~TPin	();

							//	from IPin
							CL3PUBF	const gpio::TOnEdgeEvent&
											OnEdge		() const final override CL3_GETTER;
							CL3PUBF	const gpio::TOnIdleEvent&
											OnIdle		() const final override CL3_GETTER;

							CL3PUBF	void	IdleTimeout	(system::time::TTime) final override CL3_SETTER;
							CL3PUBF	system::time::TTime
											IdleTimeout	() const final override CL3_GETTER;

							CL3PUBF	int		Mode		() const final override CL3_GETTER;
							CL3PUBF	void	Mode		(int) final override CL3_SETTER;
							CL3PUBF	gpio::EPull
											Pull		() const final override CL3_GETTER;
							CL3PUBF void	Pull		(gpio::EPull) final override CL3_SETTER;
							CL3PUBF	bool	Level		() const final override CL3_GETTER;
							CL3PUBF	void	Level		(bool) final override CL3_SETTER;
					};

					class	TChipException : public error::TException
					{
						public:
							CL3PUBF	CLASS	TChipException	(TRFM* rfm, const char* msg);
							CL3PUBF	CLASS	TChipException	(TChipException&&);
							CL3PUBF	virtual	~TChipException	();
					};

					class	TRFM : public gpio::IGPIO, public stream::ISource<byte_t>, public stream::IOut<byte_t>, protected gpio::TOnEdgeEvent::IReceiver
					{
						friend class TPin;
						private:
							CLASS TRFM(const TRFM&) = delete;
							TRFM& operator=(const TRFM&) = delete;

						protected:
							bus::spi::IDevice* device;
							gpio::IPin* pin_shutdown;
							gpio::IPin* pin_irq;
							IOut<byte_t>* sink;
							TOnIRQEvent on_irq;
							TOnCmdEvent on_cmd;
							collection::list::TList<gpio::IPin* const> pins;

							CL3PUBF	const char*	FetchAndClearError	();
							CL3PUBF	void		AssertChipStatus	();

							void	OnRaise		(gpio::TOnEdgeEvent&, gpio::IPin&, gpio::TOnEdgeData data) final override;
							void	UpdateGPIO	();
							void	ParseGPIOCfg(const byte_t* p_cfg, usys_t sz_cfg);
							void	HandleIRQs	(TOnIRQData);

						public:
							//	from ISource
							void				Sink		(stream::IOut<byte_t>* os) final override CL3_SETTER;
							stream::IOut<byte_t>* Sink		() const final override CL3_GETTER;

							//	from IOut
							void				Flush		() final override;	//	flush send-fifo, blocks until all bytes are sent
							usys_t				Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

							//	from IGPIO
							CL3PUBF const collection::list::TList<gpio::IPin* const>&
												Pins		() final override CL3_GETTER;

							//	from TRFM
							CL3PUBF	const TOnIRQEvent&
												OnIRQ		() const CL3_GETTER;

							CL3PUBF	const TOnCmdEvent&
												OnCmd		() const CL3_GETTER;

							CL3PUBF	void		WaitForCTS	();
							CL3PUBF	void		Execute		(const byte_t* p_cmd, usys_t sz_cmd, byte_t* p_retval, usys_t sz_retval);
							CL3PUBF	void		Property	(u8_t group, u8_t index, u8_t value) CL3_SETTER;
							CL3PUBF	u8_t		Property	(u8_t group, u8_t index) CL3_GETTER;

							CL3PUBF	void		Reset		();
							CL3PUBF	void		PowerUp		();
							CL3PUBF	u8_t		Test		();
							CL3PUBF	TPartInfo	Identify	();
							CL3PUBF	void		Patch		(const byte_t* p_patch, usys_t sz_patch);
							CL3PUBF	void		Configure	(const byte_t* p_config, usys_t sz_config);

							CL3PUBF	text::string::TString
												ChipName	();

							CL3PUBF	TIRQStatus	IRQStatus	();

							CL3PUBF	CLASS		TRFM		(bus::spi::IDevice* device, gpio::IPin* pin_shutdown, gpio::IPin* pin_irq = NULL);
							CL3PUBF	CLASS		~TRFM		();
					};
				}
			}
		}
	}
}

#endif
