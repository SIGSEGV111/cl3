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

					class	TRFM : public stream::ISource<byte_t>, public stream::IOut<byte_t>, public gpio::IGPIO
					{
						private:
							CLASS TRFM(const TRFM&) = delete;
							TRFM& operator=(const TRFM&) = delete;

						protected:
							bus::spi::IDevice* device;
							gpio::IPin* pin_shutdown;
							gpio::IPin* pin_irq;
							gpio::IPin* pin_rxdata;
							gpio::IPin* pin_txdata;
							IOut<byte_t>* sink;
							TOnIRQEvent on_irq;
							collection::list::TList<gpio::IPin* const> pins;

						public:
							//	from ISource
							CL3PUBF	void		Sink		(IOut<byte_t>* os) final override CL3_SETTER;
							CL3PUBF	IOut<byte_t>* Sink		() const final override CL3_GETTER;

							//	from IOut
							using IOut<byte_t>::Write;
							CL3PUBF	void		Flush		() final override;
							CL3PUBF	usys_t		Space		() const final override CL3_GETTER;
							CL3PUBF	usys_t		Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

							//	from IGPIO
							CL3PUBF const collection::list::TList<gpio::IPin* const>&
												Pins		() final override CL3_GETTER;

							//	from TRFM
							CL3PUBF	const TOnIRQEvent&
												OnIRQ		() const CL3_GETTER;

							CL3PUBF	void		ExecuteRaw	(const void* p_cmd, usys_t sz_cmd, void* p_retval, usys_t sz_retval);
							CL3PUBF	void		Configure	(const void* p_config, usys_t sz_config);	//	Reset() + Identify() + upload correct Patch() + upload config
							CL3PUBF	TPartInfo	Identify	();
							CL3PUBF	text::string::TString
												ChipName	();
							CL3PUBF	void		Reset		();
							CL3PUBF	void		Patch		(const void* p_patch, usys_t sz_patch);
							CL3PUBF	TIRQInfo	IRQState	();

							CL3PUBF	CLASS		TRFM		(bus::spi::IDevice* device, gpio::IPin* pin_shutdown, gpio::IPin* pin_irq = NULL, gpio::IPin* pin_rxdata = NULL, gpio::IPin* pin_txdata = NULL, bool b_autoflush = true);
							CL3PUBF	CLASS		~TRFM		();
					};
				}
			}
		}
	}
}

#endif
