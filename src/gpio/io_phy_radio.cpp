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

#include "io_phy_radio.hpp"
#include <cl3/core/system_types_typeinfo.hpp>
#include <cl3/core/error.hpp>

#include <stdio.h>
#include <string.h>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			namespace	radio
			{
				using namespace error;
				using namespace system::time;
				using namespace system::memory;

				void	TGPIOPulseReader::OnRaise			(event::TEvent<gpio::IPin, gpio::TOnEdgeData>&, gpio::IPin&, gpio::TOnEdgeData data)
				{
					data.b_level_prev = data.b_level_prev != this->b_inverted_line;
					data.b_level_now != data.b_level_now  != this->b_inverted_line;

					const TTime t_now = TTime::Now(TIME_CLOCK_MONOTONIC);

					if(!data.b_level_prev && data.b_level_now)
					{
						//	rising edge
						this->pt.dt_low = t_now - this->pt.dt_low;
						this->pt.dt_high = t_now;
					}
					else if(data.b_level_prev && !data.b_level_now)
					{
						//	falling edge
						this->pt.dt_high = t_now - this->pt.dt_high;

						if(sink) sink->Write(&this->pt, 1);

						this->pt.dt_low = t_now;
					}
					else
						CL3_CLASS_LOGIC_ERROR(true);	//	WTF???

					if(!this->b_idle_registered)
					{
						this->pin->IdleTimeout(dt_flush);
						this->pin->OnIdle().Register(this);
						this->b_idle_registered = true;
					}
				}

				void	TGPIOPulseReader::OnRaise			(gpio::TOnIdleEvent&, gpio::IPin&, gpio::TOnIdleData)
				{
					if(this->sink)
					{
						//	deregister OnIdle event - to prevent repeated callbacks
						pin->OnIdle().Unregister(this);
						this->b_idle_registered = false;

						//	flush buffer since the line is idle for too long
						this->sink->Flush();
					}
				}

				void	TGPIOPulseReader::Sink				(stream::IOut<TPulseTime>* os)
				{
					this->sink = os;
				}

				stream::IOut<TPulseTime>*
						TGPIOPulseReader::Sink				() const
				{
					return this->sink;
				}

				CLASS	TGPIOPulseReader::TGPIOPulseReader	(gpio::IPin* pin, bool b_inverted_line, system::time::TTime dt_flush)
					: pin(pin), sink(NULL), dt_flush(dt_flush), b_inverted_line(b_inverted_line), b_idle_registered(false)
				{
					pin->Mode(gpio::MODE_INPUT);
					pin->Pull(gpio::PULL_DISABLED);

					if(pin->Level())
						pt.dt_high = TTime::Now(TIME_CLOCK_MONOTONIC);
					else
						pt.dt_low = TTime::Now(TIME_CLOCK_MONOTONIC);

					pin->OnEdge().Register(this);
				}

				/***********************************************************************/

				TTime	TOOKDecoder::ComputeAveragePulseLength	(const TPulseTime* arr_items, usys_t n_items)
				{
					if(n_items == 0) return -1;

					//	we only consider the pulse-length of the LOW pulses (as the LOW-pulses encode the data in OOK)
					TTime dt_min = arr_items[0].dt_low;
					TTime dt_max = arr_items[0].dt_low;
					for(usys_t i = 1; i < n_items; i++)
					{
						if(arr_items[i].dt_low < dt_min) dt_min = arr_items[i].dt_low;
						if(arr_items[i].dt_low > dt_max) dt_max = arr_items[i].dt_low;
					}

					const TTime dt_avg = (dt_min + dt_max) / 2;

// 					fprintf(stderr, "ComputeAveragePulseLength:\n");
// 					fprintf(stderr, "min = %4llu µs\n", dt_min.ConvertToI(TIME_UNIT_MICROSECONDS));
// 					fprintf(stderr, "max = %4llu µs\n", dt_max.ConvertToI(TIME_UNIT_MICROSECONDS));
// 					fprintf(stderr, "avg = %4llu µs\n", dt_avg.ConvertToI(TIME_UNIT_MICROSECONDS));

					return dt_avg;
				}

				void	TOOKDecoder::Flush			()
				{
					if(this->sink && this->n_pulses_current > 0)
					{
						//	ignore the first pulse as it only contains garbage data (it contains the info how long the line was idle before the first real pulse)

						const TTime dt_avg = ComputeAveragePulseLength(this->arr_pulses+1, this->n_pulses_current-1);

						bool* arr_bits = reinterpret_cast<bool*>(this->arr_pulses);	//	EVIL! don't try this at home! (re-purpose the memory for the pulse-times as buffer for the bits)
						for(usys_t i = 1; i < this->n_pulses_current; i++)
							arr_bits[i] = this->arr_pulses[i].dt_low > dt_avg;

						this->sink->Write(arr_bits+1, this->n_pulses_current-1);
						this->sink->Flush();
					}

					this->n_pulses_current = 0;
				}

				usys_t	TOOKDecoder::Write			(const TPulseTime* arr_items_write, usys_t n_items_write_max, usys_t)
				{
					CL3_CLASS_ERROR(this->sink == NULL, TException, "need a sink to work");

					for(usys_t i = 0; i < n_items_write_max; i++)
					{
						this->arr_pulses[this->n_pulses_current++] = arr_items_write[i];
						if(this->n_pulses_current == this->n_pulses_max)
							this->Flush();
					}

					return n_items_write_max;
				}

				void	TOOKDecoder::Sink			(stream::IOut<bool>* os)
				{
					this->sink = os;
				}

				stream::IOut<bool>*
						TOOKDecoder::Sink			() const
				{
					return this->sink;
				}

				CLASS	TOOKDecoder::TOOKDecoder	(usys_t n_pulses_buffer, system::time::TTime dt_trigger) : sink(NULL), arr_pulses(NULL), n_pulses_current(0), n_pulses_max(n_pulses_buffer)
				{
					this->arr_pulses = (TPulseTime*)Alloc(n_pulses_buffer, &typeinfo::TCTTI<TPulseTime>::rtti);
				}

				CLASS	TOOKDecoder::~TOOKDecoder	()
				{
					Free(this->arr_pulses);
				}

				/***********************************************************************/

				void	TCrazy4bitDecoder::Flush	()
				{
					this->buffer = 0;
					this->index = 0;
				}

				usys_t	TCrazy4bitDecoder::Write	(const bool* arr_items_write, usys_t n_items_write_max, usys_t)
				{
					CL3_CLASS_ERROR(this->sink == NULL, TException, "need a sink to work");

					s8_t arr_bits[n_items_write_max/4+1];
					usys_t n_bits = 0;

					u8_t buffer = this->buffer;	//	copy buffer for improved performance
					u8_t index = this->index;

					for(usys_t i = 0; i < n_items_write_max; i++)
					{
						buffer <<= 1;
						buffer |= (arr_items_write[i] ? 1 : 0);
						buffer &= 0x0F;
						index++;

						if(index == 4)
						{
							if(buffer == 0x0A)
								arr_bits[n_bits++] = 1;	//	true
							else if(buffer == 0x0C)
								arr_bits[n_bits++] = 0;	//	false
							else
								arr_bits[n_bits++] = -1;	//	error
						}
					}

					this->buffer = buffer;	//	update global buffer
					this->index = index;

					this->sink->Write(arr_bits, n_bits);

					return n_items_write_max;
				}

				void	TCrazy4bitDecoder::Sink		(stream::IOut<s8_t>* os)
				{
					this->sink = os;
				}

				stream::IOut<s8_t>*
						TCrazy4bitDecoder::Sink		() const
				{
					return this->sink;
				}

				CLASS	TCrazy4bitDecoder::TCrazy4bitDecoder	() : sink(NULL), buffer(0), index(0)
				{
				}

				/***********************************************************************/

				usys_t	TSoftPT2272::Write			(const bool* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					const usys_t n_packs = n_items_write_max / 24;
// 					const usys_t n_items_accept = n_packs * 24;

// 					CL3_CLASS_ERROR(n_items_write_min > n_items_accept, TException, "this class accepts only even multiples of 24 bools as input");

					TOnDataData data_now;

					for(usys_t i = 0; i < n_packs; i++)
					{
						memset(&data_now, 0, sizeof(TOnDataData));
						const bool* arr_bits = arr_items_write + i * 24;

						for(u8_t j = 0; j < this->n_bits_address; j++)
						{
							if(!arr_bits[j*2+0]) goto gt_invalid_next;
							data_now.address <<= 1;
							data_now.address |= (arr_bits[j*2+1] ? 1 : 0);
						}

						for(u8_t j = 0; j < this->n_bits_data; j++)
						{
							if(!arr_bits[this->n_bits_address*2+j*2+0]) goto gt_invalid_next;
							data_now.arr_data[j] = arr_bits[this->n_bits_address*2+j*2+1];
						}

						if(memcmp(&this->toggle_data_unconfirmed, &data_now, sizeof(TOnDataData)) == 0)
						{
							if(this->address == (u16_t)-1 || this->address == data_now.address)
							{
								this->on_data.Raise(this, data_now);
							}
						}

						this->toggle_data_unconfirmed = data_now;

						gt_invalid_next:;
					}

					return n_items_write_max;
				}

				const TSoftPT2272::TOnDataEvent&
						TSoftPT2272::OnData			() const
				{
					return this->on_data;
				}

				CLASS	TSoftPT2272::TSoftPT2272	(u8_t n_bits_address, u8_t n_bits_data) : address((u16_t)-1), n_bits_address(n_bits_address), n_bits_data(n_bits_data)
				{
					CL3_CLASS_ERROR(n_bits_data > 6, TException, "n_bits_data must be <= 6");
					CL3_CLASS_ERROR(n_bits_address + n_bits_data != 12, TException, "n_bits_address + n_bits_data must be 12");
				}

				CLASS	TSoftPT2272::TSoftPT2272	(u8_t n_bits_address, u8_t n_bits_data, u16_t address) : address(address), n_bits_address(n_bits_address), n_bits_data(n_bits_data)
				{
					CL3_CLASS_ERROR(n_bits_data > 6, TException, "n_bits_data must be <= 6");
					CL3_CLASS_ERROR(n_bits_address + n_bits_data != 12, TException, "n_bits_address + n_bits_data must be 12");
				}
			}
		}
	}
}
