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

#ifndef	_include_cl3_gpio_io_phy_radio_hpp_
#define	_include_cl3_gpio_io_phy_radio_hpp_

#include "io_phy.hpp"
#include <cl3/core/system_time.hpp>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			namespace	radio
			{
				struct	TPulseTime
				{
					system::time::TTime dt_low;
					system::time::TTime dt_high;
				};

				class CL3PUBT	TGPIOPulseReader : public stream::ISource<TPulseTime>, protected gpio::TOnEdgeEvent::IReceiver, protected gpio::TOnIdleEvent::IReceiver
				{
					protected:
						gpio::IPin* pin;
						stream::IOut<TPulseTime>* sink;

						TPulseTime pt;
						system::time::TTime dt_flush;

						bool b_inverted_line;

						void	OnRaise	(gpio::TOnEdgeEvent&, gpio::IPin&, gpio::TOnEdgeData data) final override;
						void	OnRaise	(gpio::TOnIdleEvent&, gpio::IPin&, gpio::TOnIdleData) final override;

					public:
						//	from ISource
						CL3PUBF	void	Sink				(stream::IOut<TPulseTime>* os) final override CL3_SETTER;
						CL3PUBF	stream::IOut<TPulseTime>*
										Sink				() const final override CL3_GETTER;

						CL3PUBF	CLASS	TGPIOPulseReader	(gpio::IPin* pin, bool b_inverted_line = false, system::time::TTime dt_flush = 0.001);
				};

				class CL3PUBT	TOOKDecoder : public stream::IOut<TPulseTime>, public stream::ISource<bool>
				{
					private:
						CLASS TOOKDecoder(const TOOKDecoder&) = delete;
						TOOKDecoder& operator=(const TOOKDecoder&) = delete;

					protected:
						stream::IOut<bool>* sink;
						TPulseTime* arr_pulses;
						usys_t n_pulses_max;
						usys_t n_pulses_current;

					public:
						CL3PUBF	static	system::time::TTime	ComputeAveragePulseLength	(const TPulseTime* arr_items, usys_t n_items);

						//	from IOut
						using IOut<TPulseTime>::Write;
						CL3PUBF	void	Flush				() final override;
						CL3PUBF	usys_t	Write				(const TPulseTime* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						//	from ISource
						CL3PUBF	void	Sink				(stream::IOut<bool>* os) final override CL3_SETTER;
						CL3PUBF	stream::IOut<bool>*
										Sink				() const final override CL3_GETTER;

						CL3PUBF	CLASS	TOOKDecoder			(usys_t n_pulses_buffer = 64, system::time::TTime dt_latch = -1);
						CL3PUBF	CLASS	~TOOKDecoder		();
				};

				//	This is a decoder for the crazy 4bit pattern which is received by my RFM26W module... duno if this is useful to others....
				class CL3PUBT	TCrazy4bitDecoder : public stream::IOut<bool>, public stream::ISource<s8_t>
				{
					protected:
						stream::IOut<s8_t>* sink;
						u8_t buffer;
						u8_t index;

					public:
						//	from IOut
						using IOut<bool>::Write;
						CL3PUBF	void	Flush				() final override;
						CL3PUBF	usys_t	Write				(const bool* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						//	from ISource
						CL3PUBF	void	Sink				(stream::IOut<s8_t>* os) final override CL3_SETTER;
						CL3PUBF	stream::IOut<s8_t>*
										Sink				() const final override CL3_GETTER;

						CL3PUBF	CLASS	TCrazy4bitDecoder	();
				};

				//	this emulates the PT2272 chip, which decodes the 12bit signals sent by the PT2262 chip
				//	in contrast to the actual physical PT2272 chip, this one can be configured in software via its constructor
				//	(you do not need to order a new one, if you want a different configuration, just create a new instance ;-))
				class CL3PUBT	TSoftPT2272 : public stream::IOut<bool>
				{
					public:
						struct TOnSignalData
						{
							u16_t address;
							bool arr_toggle[6];	//	mutable! change what you want
						};

						typedef event::TEvent<TSoftPT2272, TOnSignalData&> TOnSignalEvent;

					protected:
						TOnSignalEvent on_signal;
						u8_t n_bits_address;
						u8_t n_bits_data;
						u16_t address;
						bool arr_state[6];	//	pin state

					public:
						//	from IOut
						CL3PUBF	usys_t	Write			(const bool* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						CL3PUBF	const TOnSignalEvent&
										OnSignal		() const;

						CL3PUBF	CLASS	TSoftPT2272		(u16_t address, u8_t n_bits_address = 5, u8_t n_bits_data = 2);
				};
			}
		}
	}
}

#endif
