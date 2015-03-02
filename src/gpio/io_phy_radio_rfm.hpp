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
						OPCODE_TX_FIFO_WRITE = 0x66,
						OPCODE_RX_FIFO_READ = 0x77
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
					});

					CL3_PACK(struct TRawIRQInfo
					{
						u8_t	chip_int_pend : 1,
								modem_int_pend : 1,
								ph_int_pend : 1,
								__unused0 : 5;

						u8_t	chip_int_status : 1,
								modem_int_status : 1,
								ph_int_status : 1,
								__unused1 : 5;

						u8_t	rx_fifo_almost_full_pend : 1,
								tx_fifo_almost_empty_pend : 1,
								alt_crc_error_pend : 1,
								crc_error_pend : 1,
								packet_rx_pend : 1,
								packet_sent_pend : 1,
								filter_miss_pend : 1,
								filter_match_pend : 1;

						u8_t	rx_fifo_almost_full : 1,
								tx_fifo_almost_empty : 1,
								alt_crc_error : 1,
								crc_error : 1,
								packet_rx : 1,
								packet_sent : 1,
								filter_miss : 1,
								filter_match : 1;

						u8_t	sync_detect_pend : 1,
								preamble_detect_pend : 1,
								invalid_preamble_pend : 1,
								rssi_pend : 1,
								rssi_jump_pend : 1,
								invalid_sync_pend : 1,
								postamble_detect_pend : 1,
								__unused2 : 1;

						u8_t	sync_detect : 1,
								preamble_detect : 1,
								invalid_preamble : 1,
								rssi : 1,
								rssi_jump : 1,
								invalid_sync : 1,
								postamble_detect : 1,
								__unused3 : 1;

						u8_t	wut_pend : 1,
								low_batt_pend : 1,
								chip_ready_pend : 1,
								cmd_error_pend : 1,
								state_change_pend : 1,
								fifo_underflow_overflow_error_pend : 1,
								cal_pend : 1,
								__unused4 : 1;

						u8_t	wut : 1,
								low_batt : 1,
								chip_ready : 1,
								cmd_error : 1,
								state_change : 1,
								fifo_underflow_overflow_error : 1,
								cal : 1,
								__unused5 : 1;

						CL3PUBF	bool	operator==	(const TRawIRQInfo& other) const;
						CL3PUBF	bool	operator!=	(const TRawIRQInfo& other) const;
						CL3PUBF	bool	Pending		() const;
						CL3PUBF	void	Print		() const;
					});

					CL3_PACK(struct	TIRQInfo
					{
					});

					struct	TOnIRQData
					{
						TIRQInfo state;
						TIRQInfo pending;
					};

					typedef event::TEvent<TRFM, TIRQInfo> TOnIRQEvent;

					class	TPin : public gpio::IPin
					{
						friend class TRFM;
						private:
							CLASS TPin(const TPin&) = delete;
							TPin& operator=(const TPin&) = delete;

						protected:
							TRFM* rfm;
							u8_t index;

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

							CL3PUBF	gpio::EMode
											Mode		() const final override CL3_GETTER;
							CL3PUBF	void	Mode		(gpio::EMode) final override CL3_SETTER;
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

					class	TRFM : public gpio::IGPIO
					{
						private:
							CLASS TRFM(const TRFM&) = delete;
							TRFM& operator=(const TRFM&) = delete;

						protected:
							bus::spi::IDevice* device;
							gpio::IPin* pin_shutdown;
							gpio::IPin* pin_irq;
							TOnIRQEvent on_irq;
							collection::list::TList<gpio::IPin* const> pins;

							CL3PUBF	const char*	FetchAndClearError	();
							CL3PUBF	void		AssertChipStatus	();

						public:
							//	from IGPIO
							CL3PUBF const collection::list::TList<gpio::IPin* const>&
												Pins		() final override CL3_GETTER;

							//	from TRFM
							CL3PUBF	const TOnIRQEvent&
												OnIRQ		() const CL3_GETTER;

							CL3PUBF	void		WaitForCTS	();
							CL3PUBF	void		Execute		(const byte_t* p_cmd, usys_t sz_cmd, byte_t* p_retval, usys_t sz_retval);
							CL3PUBF	void		Property	(u8_t group, u8_t index, u8_t value) CL3_SETTER;
							CL3PUBF	u8_t		Property	(u8_t group, u8_t index) CL3_GETTER;

							CL3PUBF	void		Reset		();
							CL3PUBF	void		Test		();
							CL3PUBF	TPartInfo	Identify	();
							CL3PUBF	void		Patch		(const byte_t* p_patch, usys_t sz_patch);
							CL3PUBF	void		Configure	(const byte_t* p_config, usys_t sz_config);
							CL3PUBF	void		StartRX		();
							CL3PUBF	void		StartTX		();
							CL3PUBF	void		Stop		();

							CL3PUBF	text::string::TString
												ChipName	();

							CL3PUBF	TIRQInfo	IRQState	();

							CL3PUBF	CLASS		TRFM		(bus::spi::IDevice* device, gpio::IPin* pin_shutdown, gpio::IPin* pin_irq = NULL);
							CL3PUBF	CLASS		~TRFM		();
					};
				}
			}
		}
	}
}

#endif
