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

					void	TOnIRQData::Dump	() const
					{
						fprintf(stderr, "IRQ-Status:\n\tRX_FIFO_ALMOST_FULL_PEND = %hhu\n\tTX_FIFO_ALMOST_EMPTY_PEND = %hhu\n\tALT_CRC_ERROR_PEND = %hhu\n\tCRC_ERROR_PEND = %hhu\n\tPACKET_RX_PEND = %hhu\n\tPACKET_SENT_PEND = %hhu\n\tFILTER_MISS_PEND = %hhu\n\tFILTER_MATCH_PEND = %hhu\n\tRX_FIFO_ALMOST_FULL = %hhu\n\tTX_FIFO_ALMOST_EMPTY = %hhu\n\tALT_CRC_ERROR = %hhu\n\tCRC_ERROR = %hhu\n\tPACKET_RX = %hhu\n\tPACKET_SENT = %hhu\n\tFILTER_MISS = %hhu\n\tFILTER_MATCH = %hhu\n\tSYNC_DETECT_PEND = %hhu\n\tPREAMBLE_DETECT_PEND = %hhu\n\tINVALID_PREAMBLE_PEND = %hhu\n\tRSSI_PEND = %hhu\n\tRSSI_JUMP_PEND = %hhu\n\tINVALID_SYNC_PEND = %hhu\n\tPOSTAMBLE_DETECT_PEND = %hhu\n\tSYNC_DETECT = %hhu\n\tPREAMBLE_DETECT = %hhu\n\tINVALID_PREAMBLE = %hhu\n\tRSSI = %hhu\n\tRSSI_JUMP = %hhu\n\tINVALID_SYNC = %hhu\n\tPOSTAMBLE_DETECT = %hhu\n\tWUT_PEND = %hhu\n\tLOW_BATT_PEND = %hhu\n\tCHIP_READY_PEND = %hhu\n\tCMD_ERROR_PEND = %hhu\n\tSTATE_CHANGE_PEND = %hhu\n\tFIFO_UNDERFLOW_OVERFLOW_ERROR_PEND = %hhu\n\tCAL_PEND = %hhu\n\tWUT = %hhu\n\tLOW_BATT = %hhu\n\tCHIP_READY = %hhu\n\tCMD_ERROR = %hhu\n\tSTATE_CHANGE = %hhu\n\tFIFO_UNDERFLOW_OVERFLOW_ERROR = %hhu\n\tCAL = %hhu\n",
							ph_pending.rx_fifo_almost_full,
							ph_pending.tx_fifo_almost_empty,
							ph_pending.alt_crc_error,
							ph_pending.crc_error,
							ph_pending.packet_rx,
							ph_pending.packet_sent,
							ph_pending.filter_miss,
							ph_pending.filter_match,
							ph_status.rx_fifo_almost_full,
							ph_status.tx_fifo_almost_empty,
							ph_status.alt_crc_error,
							ph_status.crc_error,
							ph_status.packet_rx,
							ph_status.packet_sent,
							ph_status.filter_miss,
							ph_status.filter_match,
							modem_pending.sync_detect,
							modem_pending.preamble_detect,
							modem_pending.invalid_preamble,
							modem_pending.rssi,
							modem_pending.rssi_jump,
							modem_pending.invalid_sync,
							modem_pending.postamble_detect,
							modem_status.sync_detect,
							modem_status.preamble_detect,
							modem_status.invalid_preamble,
							modem_status.rssi,
							modem_status.rssi_jump,
							modem_status.invalid_sync,
							modem_status.postamble_detect,
							chip_pending.wut,
							chip_pending.low_batt,
							chip_pending.chip_ready,
							chip_pending.cmd_error,
							chip_pending.state_change,
							chip_pending.fifo_underflow_overflow_error,
							chip_pending.cal,
							chip_status.wut,
							chip_status.low_batt,
							chip_status.chip_ready,
							chip_status.cmd_error,
							chip_status.state_change,
							chip_status.fifo_underflow_overflow_error,
							chip_status.cal);
					}

					bool	TPartInfo::operator==	(const TPartInfo& rhs) const { return memcmp(this, &rhs, sizeof(TPartInfo)) == 0; }
					bool	TPartInfo::operator!=	(const TPartInfo& rhs) const { return memcmp(this, &rhs, sizeof(TPartInfo)) != 0; }

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

					CLASS	TPin::TPin		(TRFM* rfm, u8_t index) : rfm(rfm), index(index), mode(MODE_TRISTATE), pull(PULL_DISABLED), level(false)
					{
						switch(index)
						{
							default:
								this->mode = MODE_TRISTATE;
								break;
							case 4:	//	IRQ
								this->mode = MODE_IRQ;
								break;
							case 5:	//	MISO
								this->mode = MODE_SDO;
								break;
						}
					}

					CLASS	TPin::~TPin		()
					{
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

					int		TPin::Mode	() const
					{
						return this->mode;
					}

					void	TPin::Mode		(int new_mode)
					{
						this->mode = (EMode)new_mode;
						this->rfm->UpdateGPIO();
					}

					EPull	TPin::Pull		() const
					{
						return this->pull;
					}

					void	TPin::Pull		(EPull new_pull)
					{
						this->pull = new_pull;
						this->rfm->UpdateGPIO();
					}

					bool	TPin::Level		() const
					{
						this->rfm->UpdateGPIO();
						return this->level;
					}

					void	TPin::Level		(bool new_level)
					{
						this->level = new_level;
						this->rfm->UpdateGPIO();
					}

					/**********************************************************************/

// 						const byte_t buffer[] = { OPCODE_START_TX };
// 						this->Execute(buffer, sizeof(buffer), NULL, 0);

					void		TRFM::Sink		(IOut<byte_t>* os)
					{
						if(this->sink == NULL && os == NULL)
							return;	//	no nothing
						else if(this->sink == NULL && os != NULL)
						{
							//	enter RX state
							const byte_t buffer[] = { OPCODE_START_RX };
							this->Execute(buffer, sizeof(buffer), NULL, 0);
						}
						else if(this->sink != NULL && os != NULL)
						{
							this->sink = os;
						}
						else //if(this->sink != NULL && os == NULL)
						{
							//	enter READY state
							const byte_t buffer[] = { OPCODE_CHANGE_STATE, 3 };
							this->Execute(buffer, sizeof(buffer), NULL, 0);
							this->sink = NULL;
						}
					}

					IOut<byte_t>* TRFM::Sink	() const
					{
						return this->sink;
					}

					void		TRFM::Flush		()
					{
						CL3_NOT_IMPLEMENTED;
					}

					usys_t		TRFM::Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
					{
						CL3_NOT_IMPLEMENTED;
					}

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

					void		TRFM::OnRaise	(gpio::TOnEdgeEvent&, gpio::IPin&, gpio::TOnEdgeData data)
					{
						if(!data.b_level_now)	//	low signals an interrupt request
						{
							union
							{
								struct
								{
									byte_t raw[8];
								};

								struct
								{
									byte_t opcode;
								};

								struct
								{
									byte_t int_pend;
									byte_t int_status;
									TOnIRQData irq_data;
								};
							} buffer;

							buffer.opcode = OPCODE_GET_INT_STATUS;
							this->Execute(buffer.raw, 1, buffer.raw, 8);

							this->HandleIRQs(buffer.irq_data);
						}
					}

					void		TRFM::UpdateGPIO()
					{
						byte_t buffer[8] = { OPCODE_GPIO_PIN_CFG };
						for(unsigned i = 0; i < 6; i++)
						{
							TPin* const pin = static_cast<TPin*>(pins[i]);
							buffer[1+i] = (pin->pull == PULL_UP ? 64 : 0);
							switch(pin->mode)
							{
								case TPin::MODE_TRISTATE:
									buffer[1+i] |= 1;
									break;
								case TPin::MODE_INPUT:
									buffer[1+i] |= 4;
									break;
								case TPin::MODE_OUTPUT:
									buffer[1+i] |= (pin->level ? 3 : 2);
									break;
								case TPin::MODE_IRQ:
									buffer[1+i] |= 39;
									break;
								case TPin::MODE_SDO:
									buffer[1+i] |= 11;
									break;
								case TPin::MODE_RXSTATE:
									buffer[1+i] |= 33;
									break;
								case TPin::MODE_TXSTATE:
									buffer[1+i] |= 32;
									break;
								case TPin::MODE_RXDATA:
									buffer[1+i] |= 20;
									break;
								case TPin::MODE_RXCLOCK:
									buffer[1+i] |= 17;
									break;
								case TPin::MODE_DIVCLOCK:
									buffer[1+i] |= 7;
									break;
							}
						}
						buffer[7] = 0;

						this->Execute(buffer, 8, buffer, 7);

						for(unsigned i = 0; i < 6; i++)
						{
							TPin* const pin = static_cast<TPin*>(pins[i]);
							pin->level = (buffer[i] & 127) != 0;
						}
					}

					void	TRFM::ParseGPIOCfg	(const byte_t* p_cfg, usys_t sz_cfg)
					{
						for(unsigned i = 0; i < 6; i++)
						{
							TPin* const pin = static_cast<TPin*>(pins[i]);
							pin->pull = (p_cfg[i] & 64) != 0 ? PULL_UP : PULL_DISABLED;
							switch(p_cfg[i] & 0x3F)
							{
								case 1:
									pin->mode = TPin::MODE_TRISTATE;
									break;
								case 4:
									pin->mode = TPin::MODE_INPUT;
									break;
								case 2:
									pin->mode = TPin::MODE_OUTPUT;
									pin->level = false;
									break;
								case 3:
									pin->mode = TPin::MODE_OUTPUT;
									pin->level = true;
									break;
								case 7:
									pin->mode = TPin::MODE_DIVCLOCK;
									break;
								case 39:
									pin->mode = TPin::MODE_IRQ;
									break;
								case 11:
									pin->mode = TPin::MODE_SDO;
									break;
								case 33:
									pin->mode = TPin::MODE_RXSTATE;
									break;
								case 32:
									pin->mode = TPin::MODE_TXSTATE;
									break;
								case 20:
									pin->mode = TPin::MODE_RXDATA;
									break;
								case 17:
									pin->mode = TPin::MODE_RXCLOCK;
									break;
								default:
									CL3_CLASS_FAIL(TException, "unknown/unimplemented GPIO mode config [%d]", (p_cfg[i] & 0x1F));
							}
						}
					}

					void		TRFM::HandleIRQs(TOnIRQData irq_data)
					{
						irq_data.Dump();
						this->on_irq.Raise(this, irq_data);

						if(irq_data.ph_status.rx_fifo_almost_full && this->sink != NULL)
						{
							byte_t buffer[65] = { OPCODE_FIFO_INFO, 0x00 };
							this->Execute(buffer, 2, buffer, 2);
							const byte_t sz_rxfifo = buffer[0];
							buffer[0] = OPCODE_READ_RX_FIFO;
							this->device->Transfer(buffer, 1 + sz_rxfifo);
							this->sink->Write(buffer+1, sz_rxfifo);
						}
					}

					const TList<IPin* const>&
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

							TOnCmdData ocd;
							ocd.opcode = (EOpCode)*p_cmd;
							ocd.p_args = p_cmd+1;
							ocd.sz_args = sz_cmd-1;
							ocd.p_retval = buffer+2;
							ocd.sz_retval = sz_retval;
							this->on_cmd.Raise(this, ocd);

							if(*p_cmd == OPCODE_GPIO_PIN_CFG)
								this->ParseGPIOCfg(p_cmd+1, sz_cmd-1);

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
						if(pin_irq->OnEdge().IsRegistered(this))
							pin_irq->OnEdge().Unregister(this);
						this->pin_shutdown->Level(true);
						system::task::IThread::Sleep(0.1);
						this->pin_shutdown->Level(false);
						WaitForCTS();
						AssertChipStatus();
					}

					void		TRFM::PowerUp	()
					{
						const byte_t buffer[] = { OPCODE_POWER_UP, 0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80 };
						this->Execute(buffer, sizeof(buffer), NULL, 0);
					}

					u8_t		TRFM::Test		()
					{
						//	test if reset is working properly
						try
						{
							if(this->device->Bus()->BaudrateMin() > 10000)
								return 2;
							this->device->Baudrate(CL3_MAX(1000, this->device->Bus()->BaudrateMin()));
							this->Reset();
							this->PowerUp();
							if(this->Property(0x00, 0x00) != 0x40)
								return 3;
							this->Property(0x00, 0x00, 127);
							if(this->Property(0x00, 0x00) != 127)
								return 4;
							this->Reset();
							this->PowerUp();
							if(this->Property(0x00, 0x00) != 0x40)
								return 5;

						}
						catch(const TChipException&)
						{
							return 1;
						}

						//	test SPI-bus connection stability
						try
						{
							this->device->Baudrate(CL3_MIN(10000000, this->device->Bus()->BaudrateMax()));

							//	get reference values
							const TPartInfo pi_ref = this->Identify();
							byte_t arr_ref[10];
							for(unsigned j = 0; j < sizeof(arr_ref); j++)
								arr_ref[j] = this->Property(0x00, j);

							for(unsigned i = 0; i < 0x1000; i++)
							{
								const TPartInfo pi_test = this->Identify();
								if(pi_test != pi_ref)	//	unless somebody switched the chip in-flight, this should not happen
									return 7;

								for(unsigned j = 0; j < sizeof(arr_ref); j++)
								{
									if(this->Property(0x00, j) != arr_ref[j])
										return 8;
								}
							}
						}
						catch(const TChipException&)
						{
							return 6;
						}

						//	test IRQ/GPIO connection
						try
						{
							pins[4]->Mode(MODE_OUTPUT);
							pins[4]->Pull(PULL_DISABLED);

							for(unsigned i = 0; i < 0x100; i++)
							{
								const bool target_level = (i % 2) == 0;
								pins[4]->Level(target_level);
								system::task::IThread::Sleep(0.001);
								if(pin_irq->Level() != target_level)
									return 10;
							}
						}
						catch(const TChipException&)
						{
							return 9;
						}

						this->Reset();
						return 0;
					}

					void		TRFM::Patch		(const byte_t* p_patch, usys_t sz_patch)
					{
						CL3_CLASS_ERROR((sz_patch % 8) != 0, TException, "invalid patch: patch-size must be a multiple of 8");
						sz_patch /= 8;
						this->Reset();
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

						Property(0x12, 0x0b, 64);	//	IRQ when TX-FIFO is completly empty
						Property(0x12, 0x0c,  1);	//	IRQ as soon as one byte is in RX-FIFO

						static_cast<TPin*>(pins[4])->Mode(TPin::MODE_IRQ);	//	we need the IRQ-line, so enable it in any case
						pin_irq->OnEdge().Register(this);	//	register event handler on IRQ-line
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

					TIRQStatus	TRFM::IRQStatus	()
					{
						byte_t buffer[8] = { OPCODE_GET_INT_STATUS, 0xff, 0xff, 0xff };
						this->Execute(buffer, 4, buffer, 8);
						TIRQStatus irq_status;
						irq_status.ph_status = *reinterpret_cast<const TPaketHandlerIRQ*>(buffer+3);
						irq_status.modem_status = *reinterpret_cast<const TModemIRQ*>(buffer+5);
						irq_status.chip_status = *reinterpret_cast<const TChipIRQ*>(buffer+7);
						return irq_status;
					}

					CLASS		TRFM::TRFM		(bus::spi::IDevice* device, IPin* pin_shutdown, IPin* pin_irq)
						: device(device), pin_shutdown(pin_shutdown), pin_irq(pin_irq), sink(NULL)
					{
						this->device->Baudrate(CL3_MIN(10000000, this->device->Bus()->BaudrateMax()));

						pin_shutdown->Mode(MODE_OUTPUT);
						pin_shutdown->Pull(PULL_DISABLED);
						pin_shutdown->Level(false);

						pin_irq->Mode(MODE_INPUT);
						pin_irq->Pull(PULL_DISABLED);

						for(u8_t i = 0; i < 6; i++)
							this->pins.Append(new TPin(this, i));
					}

					CLASS		TRFM::~TRFM		()
					{
						if(pin_irq->OnEdge().IsRegistered(this))
							pin_irq->OnEdge().Unregister(this);
						this->pin_shutdown->Level(true);
						system::task::IThread::Sleep(0.010);

						for(u8_t i = 0; i < 6; i++)
							delete this->pins[i];
					}
				}
			}
		}
	}
}
