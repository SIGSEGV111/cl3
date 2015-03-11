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
				enum	EModulation
				{
					MODULATION_NONE,
					MODULATION_CW,
					MODULATION_OOK,
					MODULATION_2FSK,
					MODULATION_2GFSK,
					MODULATION_4FSK,
					MODULATION_4GFSK
				};

				struct	TPulseTime
				{
					system::time::TTime dt_low;
					system::time::TTime dt_high;
				};

				class CL3PUBT	TGPIOPulseReader : public stream::ISource<TPulseTime>, protected gpio::TOnEdgeEvent::IReceiver, protected gpio::TOnIdleEvent::IReceiver
				{
					private:
						CLASS TGPIOPulseReader(const TGPIOPulseReader&) = delete;
						TGPIOPulseReader& operator=(const TGPIOPulseReader&) = delete;

					protected:
						gpio::IPin* pin;
						stream::IOut<TPulseTime>* sink;

						TPulseTime pt;
						system::time::TTime dt_flush;

						bool b_inverted_lineidle;
						bool b_idle_registered;

						void	OnRaise	(gpio::TOnEdgeEvent&, gpio::IPin&, gpio::TOnEdgeData data) final override;
						void	OnRaise	(gpio::TOnIdleEvent&, gpio::IPin&, gpio::TOnIdleData) final override;

					public:
						//	from ISource
						CL3PUBF	void	Sink				(stream::IOut<TPulseTime>* os) final override CL3_SETTER;
						CL3PUBF	stream::IOut<TPulseTime>*
										Sink				() const final override CL3_GETTER;

						CL3PUBF	CLASS	TGPIOPulseReader	(gpio::IPin* pin, bool b_inverted_lineidle, system::time::TTime dt_flush);
						CL3PUBF	CLASS	~TGPIOPulseReader	();
				};

				class CL3PUBT	TGPIOBusReader : public stream::ISource<bool>, protected gpio::TOnEdgeEvent::IReceiver, protected gpio::TOnIdleEvent::IReceiver
				{
					private:
						CLASS TGPIOBusReader(const TGPIOBusReader&) = delete;
						TGPIOBusReader& operator=(const TGPIOBusReader&) = delete;

					protected:
						stream::IOut<bool>* sink;
						gpio::IPin* pin_data;
						gpio::IPin* pin_clock;
						bool b_idle_registered;

						void	OnRaise	(gpio::TOnEdgeEvent&, gpio::IPin&, gpio::TOnEdgeData data) final override;
						void	OnRaise	(gpio::TOnIdleEvent&, gpio::IPin&, gpio::TOnIdleData) final override;

					public:
						//	from ISource
						CL3PUBF	void	Sink				(stream::IOut<bool>* os) final override CL3_SETTER;
						CL3PUBF	stream::IOut<bool>*
										Sink				() const final override CL3_GETTER;

						CL3PUBF	CLASS	TGPIOBusReader		(gpio::IPin* pin_data, gpio::IPin* pin_clock, system::time::TTime dt_flush);
						CL3PUBF	CLASS	~TGPIOBusReader		();
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

						CL3PUBF	CLASS	TOOKDecoder			(usys_t n_pulses_buffer = 64);
						CL3PUBF	CLASS	~TOOKDecoder		();
				};

				//	this emulates the PT2272 chip, which decodes the 12bit signals sent by the PT2262 chip
				//	in contrast to the actual physical PT2272 chip, this one can be configured in software via its constructor
				//	(you do not need to order a new one, if you want a different configuration, just create a new instance ;-))
				class CL3PUBT	TSoftPT2272 : public stream::IOut<bool>
				{
					public:
						struct TOnDataData
						{
							u16_t address;
							bool arr_data[6];

							CL3PUBF	CLASS	TOnDataData	();
						};

						typedef event::TEvent<TSoftPT2272, TOnDataData> TOnDataEvent;

					protected:
						TOnDataEvent on_data;

						u16_t address;
						u8_t n_bits_address;
						u8_t n_bits_data;
						bool arr_state[6];	//	pin state

						TOnDataData toggle_data_unconfirmed;

					public:
						inline	u8_t	DataBits	() const CL3_GETTER { return this->n_bits_data; }
						inline	u8_t	AddressBits	() const CL3_GETTER { return this->n_bits_address; }

						//	returns an array of 6 booleans which hold the current state of the output pins
						inline	const bool*	State		() const { return this->arr_state; }

						//	from IOut
						using IOut<bool>::Write;
						CL3PUBF	usys_t	Write			(const bool* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						CL3PUBF	const TOnDataEvent&
										OnData			() const;

						CL3PUBF	CLASS	TSoftPT2272		(u8_t n_bits_address, u8_t n_bits_data);	//	promiscuous mode
						CL3PUBF	CLASS	TSoftPT2272		(u8_t n_bits_address, u8_t n_bits_data, u16_t address);
				};
			}
		}
	}
}

#endif
