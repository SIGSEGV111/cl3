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

#include "io_phy.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			/*****************************************************************************/

			struct 	TPin : public gpio::IPin
			{
				TBCM2835* controller;

				TBCM2835* Controller() const final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				gpio::EPinMode
						Mode		() const final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	Mode		(gpio::EPinMode new_pinmode) final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				bool	State		() const final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	State		(bool new_state) final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				const gpio::TOnEdgeEvent&
						OnEdge		() const final override
				{
					CL3_NOT_IMPLEMENTED;
				}
			};

			struct	TSPIDevice : public bus::spi::ISPIDevice
			{
				TBCM2835* controller;

				TBCM2835* BusController() const final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				u32_t	Baudrate	() const final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	Baudrate	(u32_t new_baudrate) final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	Transfer	(byte_t* buffer, usys_t sz_buffer) final override
				{
					CL3_NOT_IMPLEMENTED;
				}
			};

			struct	TI2CDevice : public bus::i2c::II2CDevice
			{
				TBCM2835* controller;

				TBCM2835* BusController() const final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				u32_t	Baudrate	() const final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	Baudrate	(u32_t new_baudrate) final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override
				{
					CL3_NOT_IMPLEMENTED;
				}
			};

			/*****************************************************************************/

			CLASS	TBCM2835::TBCM2835		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TBCM2835::~TBCM2835		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			//	from IGPIOController
			const collection::IStaticCollection<const gpio::IPin* const>&
					TBCM2835::Pins			() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::IStaticCollection<gpio::IPin* const>&
					TBCM2835::Pins			()
			{
				CL3_NOT_IMPLEMENTED;
			}

			//	from IBusController
			u32_t	TBCM2835::BaudrateMin	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			u32_t	TBCM2835::BaudrateMax	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::IStaticCollection<const bus::IDevice* const>&
					TBCM2835::Devices		() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::IStaticCollection<bus::IDevice* const>&
					TBCM2835::Devices		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			//	from ISPIBusController
			const collection::IStaticCollection<const bus::spi::ISPIDevice* const>&
					TBCM2835::SPIDevices	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::IStaticCollection<bus::spi::ISPIDevice* const>&
					TBCM2835::SPIDevices	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			//	from II2CBusController
			const collection::IStaticCollection<const bus::i2c::II2CDevice* const>&
					TBCM2835::I2CDevices	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::IStaticCollection<bus::i2c::II2CDevice* const>&
					TBCM2835::I2CDevices	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			/*****************************************************************************/
		}
	}
}
