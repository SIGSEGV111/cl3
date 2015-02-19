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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "io_phy_radio_rfm.hpp"
#include <cl3/core/system_task.hpp>
#include <cl3/core/io_file.hpp>
#include <string.h>
#include <stdio.h>

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
					using namespace stream;
					using namespace error;
					using namespace collection::list;
					using namespace text::string;
					using namespace gpio;
					using namespace file;

					bool	TRawIRQInfo::operator==	(const TRawIRQInfo& other) const
					{
						return memcmp(this, &other, sizeof(TRawIRQInfo)) == 0;
					}

					bool	TRawIRQInfo::operator!=	(const TRawIRQInfo& other) const
					{
						return memcmp(this, &other, sizeof(TRawIRQInfo)) != 0;
					}

					bool	TRawIRQInfo::Pending	() const
					{
						return this->chip_int_pend || this->modem_int_pend || this->ph_int_pend;
					}

					void	TRawIRQInfo::Print		() const
					{
						fprintf(stderr, "IRQ-Status:\n\tCHIP_INT_PEND = %hhu\n\tMODEM_INT_PEND = %hhu\n\tPH_INT_PEND = %hhu\n\tCHIP_INT_STATUS = %hhu\n\tMODEM_INT_STATUS = %hhu\n\tPH_INT_STATUS = %hhu\n\tRX_FIFO_ALMOST_FULL_PEND = %hhu\n\tTX_FIFO_ALMOST_EMPTY_PEND = %hhu\n\tALT_CRC_ERROR_PEND = %hhu\n\tCRC_ERROR_PEND = %hhu\n\tPACKET_RX_PEND = %hhu\n\tPACKET_SENT_PEND = %hhu\n\tFILTER_MISS_PEND = %hhu\n\tFILTER_MATCH_PEND = %hhu\n\tRX_FIFO_ALMOST_FULL = %hhu\n\tTX_FIFO_ALMOST_EMPTY = %hhu\n\tALT_CRC_ERROR = %hhu\n\tCRC_ERROR = %hhu\n\tPACKET_RX = %hhu\n\tPACKET_SENT = %hhu\n\tFILTER_MISS = %hhu\n\tFILTER_MATCH = %hhu\n\tSYNC_DETECT_PEND = %hhu\n\tPREAMBLE_DETECT_PEND = %hhu\n\tINVALID_PREAMBLE_PEND = %hhu\n\tRSSI_PEND = %hhu\n\tRSSI_JUMP_PEND = %hhu\n\tINVALID_SYNC_PEND = %hhu\n\tPOSTAMBLE_DETECT_PEND = %hhu\n\tSYNC_DETECT = %hhu\n\tPREAMBLE_DETECT = %hhu\n\tINVALID_PREAMBLE = %hhu\n\tRSSI = %hhu\n\tRSSI_JUMP = %hhu\n\tINVALID_SYNC = %hhu\n\tPOSTAMBLE_DETECT = %hhu\n\tWUT_PEND = %hhu\n\tLOW_BATT_PEND = %hhu\n\tCHIP_READY_PEND = %hhu\n\tCMD_ERROR_PEND = %hhu\n\tSTATE_CHANGE_PEND = %hhu\n\tFIFO_UNDERFLOW_OVERFLOW_ERROR_PEND = %hhu\n\tCAL_PEND = %hhu\n\tWUT = %hhu\n\tLOW_BATT = %hhu\n\tCHIP_READY = %hhu\n\tCMD_ERROR = %hhu\n\tSTATE_CHANGE = %hhu\n\tFIFO_UNDERFLOW_OVERFLOW_ERROR = %hhu\n\tCAL = %hhu\n",
							chip_int_pend,
							modem_int_pend,
							ph_int_pend,
							chip_int_status,
							modem_int_status,
							ph_int_status,
							rx_fifo_almost_full_pend,
							tx_fifo_almost_empty_pend,
							alt_crc_error_pend,
							crc_error_pend,
							packet_rx_pend,
							packet_sent_pend,
							filter_miss_pend,
							filter_match_pend,
							rx_fifo_almost_full,
							tx_fifo_almost_empty,
							alt_crc_error,
							crc_error,
							packet_rx,
							packet_sent,
							filter_miss,
							filter_match,
							sync_detect_pend,
							preamble_detect_pend,
							invalid_preamble_pend,
							rssi_pend,
							rssi_jump_pend,
							invalid_sync_pend,
							postamble_detect_pend,
							sync_detect,
							preamble_detect,
							invalid_preamble,
							rssi,
							rssi_jump,
							invalid_sync,
							postamble_detect,
							wut_pend,
							low_batt_pend,
							chip_ready_pend,
							cmd_error_pend,
							state_change_pend,
							fifo_underflow_overflow_error_pend,
							cal_pend,
							wut,
							low_batt,
							chip_ready,
							cmd_error,
							state_change,
							fifo_underflow_overflow_error,
							cal);
					}

					/**********************************************************************/

					CLASS	TPin::TPin		(TRFM* rfm, u8_t index)
					{
						CL3_NOT_IMPLEMENTED;
					}

					CLASS	TPin::~TPin		()
					{
						CL3_NOT_IMPLEMENTED;
					}

					const TOnEdgeEvent&
							TPin::OnEdge	() const
					{
						CL3_NOT_IMPLEMENTED;
					}

					const TOnIdleEvent&
							TPin::OnIdle	() const
					{
						CL3_NOT_IMPLEMENTED;
					}

					void	TPin::IdleTimeout	(system::time::TTime)
					{
						CL3_NOT_IMPLEMENTED;
					}

					system::time::TTime
							TPin::IdleTimeout	() const
					{
						CL3_NOT_IMPLEMENTED;
					}

					EMode	TPin::Mode		() const
					{
						CL3_NOT_IMPLEMENTED;
					}

					void	TPin::Mode		(EMode)
					{
						CL3_NOT_IMPLEMENTED;
					}

					EPull	TPin::Pull		() const
					{
						CL3_NOT_IMPLEMENTED;
					}

					void	TPin::Pull		(EPull)
					{
						CL3_NOT_IMPLEMENTED;
					}

					bool	TPin::Level		() const
					{
						CL3_NOT_IMPLEMENTED;
					}

					void	TPin::Level		(bool)
					{
						CL3_NOT_IMPLEMENTED;
					}

					/**********************************************************************/

					void		TRFM::Sink		(IOut<byte_t>* os)
					{
						this->sink = os;
					}

					IOut<byte_t>* TRFM::Sink	() const
					{
						return this->sink;
					}

					void		TRFM::Flush		()
					{
						CL3_NOT_IMPLEMENTED;	//	send TX-FIFO
					}

					usys_t		TRFM::Space		() const
					{
						CL3_NOT_IMPLEMENTED;	//	query TX-FIFO
					}

					usys_t		TRFM::Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
					{
						CL3_NOT_IMPLEMENTED;
					}

					CL3PUBF const TList<IPin* const>&
								TRFM::Pins		()
					{
						return this->pins;
					}

					const TOnIRQEvent&
								TRFM::OnIRQ		() const
					{
						return this->on_irq;
					}

					void		TRFM::ExecuteRaw(const void* p_cmd, usys_t sz_cmd, void* p_retval, usys_t sz_retval)
					{
						CL3_NOT_IMPLEMENTED;
					}

					void		TRFM::Configure	(const void* p_config, usys_t sz_config)
					{
						this->Reset();
// 						const TPartInfo part_info = this->Identify();
// 						TFile file_patch(TString("patch/") +
						CL3_NOT_IMPLEMENTED;
					}

					TPartInfo	TRFM::Identify	()
					{
						CL3_NOT_IMPLEMENTED;
					}

					TString		TRFM::ChipName	()
					{
						const TPartInfo part_info = this->Identify();
						char chip_name[16];
						snprintf(chip_name, sizeof(chip_name), "si%hx", part_info.part);
						return chip_name;
					}

					void		TRFM::Reset		()
					{
						this->pin_shutdown->Level(true);
						system::task::IThread::Sleep(0.010);
						this->pin_shutdown->Level(false);
						system::task::IThread::Sleep(0.010);
					}

					void		TRFM::Patch		(const void* p_patch, usys_t sz_patch)
					{
						CL3_NOT_IMPLEMENTED;
					}

					TIRQInfo	TRFM::IRQState	()
					{
						CL3_NOT_IMPLEMENTED;
					}

					CLASS		TRFM::TRFM		(bus::spi::IDevice* device, IPin* pin_shutdown, IPin* pin_irq, IPin* pin_rxdata, IPin* pin_txdata, bool b_autoflush)
					{
						CL3_NOT_IMPLEMENTED;
					}

					CLASS		TRFM::~TRFM		()
					{
						CL3_NOT_IMPLEMENTED;
					}
				}
			}
		}
	}
}
