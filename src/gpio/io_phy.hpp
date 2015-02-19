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

#ifndef	_include_cl3_gpio_io_phy_hpp_
#define	_include_cl3_gpio_io_phy_hpp_

#include <cl3/core/event.hpp>
#include <cl3/core/system_types.hpp>
#include <cl3/core/system_time.hpp>
#include <cl3/core/io_stream.hpp>
#include <cl3/core/io_collection.hpp>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			using namespace system::types;

			namespace	gpio
			{
				struct	CL3PUBT	IPin;
				struct	CL3PUBT	IGPIO;

				enum	EPull
				{
					PULL_DISABLED,
					PULL_DOWN,
					PULL_UP
				};

				enum	EMode
				{
					MODE_INPUT,		//	GPIO input
					MODE_OUTPUT,	//	GPIO output
					MODE_TRISTATE,	//	disabled/nothing/deactivated/don't care
					MODE_OTHER		//	other hardware specific special function
				};

				struct	TOnEdgeData
				{
					bool b_level_prev;
					bool b_level_now;
				};

				struct	TOnIdleData
				{
				};

				typedef event::TEvent<IPin, TOnEdgeData>	TOnEdgeEvent;
				typedef event::TEvent<IPin, TOnIdleData>	TOnIdleEvent;

				struct IPin
				{
					//	Event callbacks *MAY* be processed by a dedicated thread!
					virtual	CL3PUBF	const TOnEdgeEvent&	OnEdge	() const CL3_GETTER = 0;
					virtual	CL3PUBF	const TOnIdleEvent&	OnIdle	() const CL3_GETTER = 0;

					virtual	CL3PUBF	void				IdleTimeout	(system::time::TTime) CL3_SETTER = 0;
					virtual	CL3PUBF	system::time::TTime	IdleTimeout	() const CL3_GETTER = 0;

					virtual	CL3PUBF	EMode	Mode	() const CL3_GETTER = 0;
					virtual	CL3PUBF	void	Mode	(EMode) CL3_SETTER = 0;
					virtual	CL3PUBF	EPull	Pull	() const CL3_GETTER = 0;
					virtual	CL3PUBF	void	Pull	(EPull) CL3_SETTER = 0;
					virtual	CL3PUBF	bool	Level	() const CL3_GETTER = 0;
					virtual	CL3PUBF	void	Level	(bool) CL3_SETTER = 0;
					virtual	CL3PUBF	CLASS	~IPin	();
				};

				struct IGPIO
				{
					virtual	CL3PUBF const collection::IStaticCollection<IPin* const>&
											Pins	() CL3_GETTER = 0;
				};
			}

			namespace	bus
			{
				namespace	spi
				{
					struct	CL3PUBT	IDevice;
					struct	CL3PUBT	IBus;

					struct	IBus
					{
						virtual	u32_t	BaudrateMin	() const CL3_GETTER = 0;
						virtual	u32_t	BaudrateMax	() const CL3_GETTER = 0;
						virtual	CL3PUBF const collection::IStaticCollection<IDevice* const>&
										Devices		() CL3_GETTER = 0;
					};

					struct	IDevice : stream::IIn<byte_t>, stream::IOut<byte_t>
					{
						virtual	CL3PUBF u32_t	Baudrate	() const CL3_GETTER = 0;
						virtual	CL3PUBF void	Baudrate	(u32_t new_baudrate) CL3_SETTER = 0;
						virtual	CL3PUBF void	Transfer	(byte_t* buffer, usys_t sz_buffer) = 0;
						virtual	CL3PUBF CLASS	~IDevice	();
					};
				}

				namespace	i2c
				{
					struct	CL3PUBT	IDevice;
					struct	CL3PUBT	IBus;

					struct	IBus
					{
						virtual	CL3PUBF u32_t		Baudrate	() const CL3_GETTER = 0;
						virtual	CL3PUBF void		Baudrate	(u32_t new_baudrate) CL3_SETTER = 0;
						virtual	CL3PUBF const collection::IStaticCollection<IDevice* const>&
													Devices		() CL3_GETTER = 0;
						virtual	CL3PUBF IDevice*	ByAddress	(u8_t address) CL3_GETTER = 0;
						virtual	CL3PUBF void		Scan		() = 0;
					};

					struct	IDevice : stream::IIn<byte_t>, stream::IOut<byte_t>
					{
						virtual	CL3PUBF u8_t		Address		() const CL3_GETTER = 0;
						virtual	CL3PUBF CLASS		~IDevice	();
					};
				}
			}
		}
	}
}

#endif
