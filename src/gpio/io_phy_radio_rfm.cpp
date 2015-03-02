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

// #include <cl3/core/util.hpp>
// #include <cl3/core/io_text_terminal.hpp>

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
					using namespace system::time;

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

					CLASS	TChipException::TChipException	(TRFM*, const char* msg) : TException("radio chip reported error: %s", msg)
					{
					}

					CLASS	TChipException::TChipException	(TChipException&& other) : TException((TException&&)other)
					{
					}

					CLASS	TChipException::~TChipException	()
					{
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

					const char*	TRFM::FetchAndClearError()
					{
						WaitForCTS();
						byte_t buffer[] = { OPCODE_GET_CHIP_STATUS, 0x77, 0, 0, 0, 0 };
						this->device->Transfer(buffer, 2);
						WaitForCTS();
						buffer[0] = OPCODE_READ_CMD_BUFF;
						memset(buffer+1, 0, 5);
						this->device->Transfer(buffer, 5);

						if(buffer[2] & 0x08)
						{
							switch(buffer[4])
							{
								case 0:
									//	no error
									return "No error ???? => logic error in control program or misbehaving radio chip.";
								case 16:
									return "Bad command issued.";
								case 17:
									return "Argment(s) in issued command were invalid.";
								case 18:
									return "Command was issued before previous command was completed.";
								case 19:
									return "Command issued was not allowed while in the current device state.";
								case 49:
									return "Invalid bootmode supplied.";
								case 64:
									return "Bad Property ID (aka. \"index\") was provided.";
								default:
									return "undocumented error";
							}
						}

						return NULL;
					}

					void		TRFM::AssertChipStatus	()
					{
						const char* const err = FetchAndClearError();
						CL3_CLASS_ERROR(err != NULL, TChipException, this, err);
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

					void		TRFM::WaitForCTS()
					{
						byte_t buffer[2];

						buffer[0] = (byte_t)OPCODE_READ_CMD_BUFF;
						buffer[1] = 0;
						this->device->Transfer(buffer, 2);
						if(buffer[1] == 0xff)
							return;

						const TTime timeout = TTime::Now(TIME_CLOCK_MONOTONIC) + 1.000;
						do
						{
							buffer[0] = (byte_t)OPCODE_READ_CMD_BUFF;
							buffer[1] = 0;
							this->device->Transfer(buffer, 2);
							if(buffer[1] == 0xff)
								return;
						}
						while(TTime::Now(TIME_CLOCK_MONOTONIC) < timeout);
						CL3_CLASS_FAIL(TException, "radio chip hung-up");
					}

					void		TRFM::Execute	(const byte_t* p_cmd, usys_t sz_cmd, byte_t* p_retval, usys_t sz_retval)
					{
						AssertChipStatus();	//	ensure the chip is ready and has no errors flagged
						this->device->Write(p_cmd, sz_cmd);	//	send the command to the chip

						if(sz_retval > 0 && p_retval != NULL)
						{
							WaitForCTS();	//	wait until chip has finished processing the command
							byte_t buffer[2+sz_retval];
							buffer[0] = (byte_t)OPCODE_READ_CMD_BUFF;
							buffer[1] = 0;	//	placeholder for CTS
							this->device->Transfer(buffer, 2+sz_retval);

							CL3_CLASS_LOGIC_ERROR(buffer[1] != 0xff);	//	check CTS again (should always be 0xff, since we already waited for this)
							memcpy(p_retval, buffer+2, sz_retval);	//	copy only the return values to the user-provided buffer (skip opcode and CTS)
						}

						AssertChipStatus();	//	ensure the chip is ready and has no errors flagged (this would now detect errors with the just executed command)
					}

					void		TRFM::Property	(u8_t group, u8_t index, u8_t value)
					{
						const byte_t buffer[] = { OPCODE_SET_PROPERTY, group, 1, index, value };
						this->Execute(buffer, sizeof(buffer), NULL, 0);
					}

					u8_t		TRFM::Property	(u8_t group, u8_t index)
					{
						u8_t value;
						const byte_t buffer[] = { OPCODE_GET_PROPERTY, group, 1, index };
						this->Execute(buffer, sizeof(buffer), &value, 1);
						return value;
					}

					void		TRFM::Reset		()
					{
						this->pin_shutdown->Level(true);
						system::task::IThread::Sleep(0.1);
						this->pin_shutdown->Level(false);
						system::task::IThread::Sleep(0.1);
						byte_t buffer[] = { OPCODE_NOP, 0 };
						this->device->Transfer(buffer, 2);
						CL3_CLASS_ERROR(buffer[1] != 0xff, TException, "radio chip unoperational or connection problem(buffer[1] == CTS == 0x%02hhx)", buffer[1]);
						AssertChipStatus();
					}

					void		TRFM::Test		()
					{
						CL3_NOT_IMPLEMENTED;
					}

					void		TRFM::Patch		(const byte_t* p_patch, usys_t sz_patch)
					{
						WaitForCTS();
						CL3_CLASS_ERROR((sz_patch % 8) != 0, TException, "invalid patch: patch-size must be a multiple of 8");
						sz_patch /= 8;
						for(usys_t i = 0; i < sz_patch; i++)
						{
							const byte_t* const line = p_patch + (i * 8);
							this->device->Write(line, 8);
							WaitForCTS();
						}
						AssertChipStatus();
					}

					void		TRFM::Configure	(const byte_t* p_config, usys_t sz_config)
					{
						for(usys_t p = 0; p < sz_config && p_config[p] != 0;)
						{
							const byte_t len = p_config[p++];
							CL3_CLASS_ERROR(len > 16, TException, "invalid config: command length bigger than 16 bytes");
							this->Execute(p_config + p, len, NULL, 0);
							p += len;
						}
					}

					void		TRFM::StartRX	()
					{
						const byte_t buffer[] = { OPCODE_START_RX };
						this->Execute(buffer, sizeof(buffer), NULL, 0);
					}

					void		TRFM::StartTX	()
					{
						const byte_t buffer[] = { OPCODE_START_TX };
						this->Execute(buffer, sizeof(buffer), NULL, 0);
					}

					void		TRFM::Stop		()
					{
						CL3_NOT_IMPLEMENTED;
					}

					TPartInfo	TRFM::Identify	()
					{
						TPartInfo part_info;
						const byte_t buffer[] = { OPCODE_PART_INFO };
						this->Execute(buffer, sizeof(buffer), (byte_t*)&part_info, sizeof(part_info));
						part_info.part = be16toh(part_info.part);
						part_info.id = be16toh(part_info.id);
						return part_info;
					}

					TString		TRFM::ChipName	()
					{
						const TPartInfo part_info = this->Identify();
						char chip_name[16];
						snprintf(chip_name, sizeof(chip_name), "si%hx", part_info.part);
						return chip_name;
					}

					TIRQInfo	TRFM::IRQState	()
					{
						CL3_NOT_IMPLEMENTED;
					}

					CLASS		TRFM::TRFM		(bus::spi::IDevice* device, IPin* pin_shutdown, IPin* pin_irq)
						: device(device), pin_shutdown(pin_shutdown), pin_irq(pin_irq)
					{
						device->Baudrate(2000000);
					}

					CLASS		TRFM::~TRFM		()
					{
						this->pin_shutdown->Level(true);
						system::task::IThread::Sleep(0.010);
					}
				}
			}
		}
	}
}
