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

#ifdef CL3_WITH_BCM2835

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "io_phy_bcm2835.hpp"
#include <cl3/core/error.hpp>
#include "bcm2835.h"

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			using namespace error;

			/*****************************************************************************/

			struct 	TGPIOPin : public gpio::IPin
			{
				TBCM2835* controller;
				u32_t id;

				CLASS	TGPIOPin	(TBCM2835* controller, u32_t id) : controller(controller), id(id) {}

				u32_t	ID			() const final override
				{
					return this->id;
				}

				TBCM2835* Controller() const final override
				{
					return this->controller;
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

				gpio::EPull Pull	() const final override
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	Pull		(gpio::EPull new_pull) final override
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
				gpio::IPin* pin_cs;
				u32_t id;

				CLASS	TSPIDevice	(TBCM2835* controller, gpio::IPin* pin_cs, u32_t id) : controller(controller), pin_cs(pin_cs), id(id) {}

				u32_t	ID			() const final override
				{
					return this->id;
				}

				TBCM2835* BusController() const final override
				{
					return this->controller;
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
				u8_t address;

				CLASS	TI2CDevice	(TBCM2835* controller, u8_t address) : controller(controller), address(address) {}

				u8_t	Address		() const final override
				{
					return this->address;
				}

				TBCM2835* BusController() const final override
				{
					return this->controller;
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

			//	from TBCM2835
			CLASS	TBCM2835::TBCM2835		(const collection::IStaticCollection<u32_t>& spi_cs_pins, bool debug)
			{
				CL3_CLASS_ERROR(bcm2835_init() != 1, TException, "failed to initialize bcm2835 library");
				bcm2835_set_debug(debug ? 1 : 0);

				// add all available GPIO pins
				for(u32_t i = 0; i < 54; i++)
					gpio_pins.Append(new TGPIOPin(this, i));

				// add one SPI device for each chip-select GPIO-pin provided by the user, and configure that pin as output
				auto it = spi_cs_pins.CreateStaticIterator();
				it->MoveHead();
				for(u32_t i = 0; it->MoveNext(); i++)
				{
					gpio_pins[i]->Pull(gpio::PULL_DISABLED);
					gpio_pins[i]->Mode(gpio::PINMODE_GPIO_OUTPUT);
					spi_devices.Add(new TSPIDevice(this, gpio_pins[i], i));
				}

				// scan i2c bus for devices
				Scan();

				// add spi- and i2c-devices to the master list of devices
				for(usys_t i = 0; i < spi_devices.Count(); i++)
					devices.Add(spi_devices[0]);

				for(usys_t i = 0; i < i2c_devices.Count(); i++)
					devices.Add(i2c_devices[0]);
			}

			CLASS	TBCM2835::~TBCM2835		()
			{
				CL3_CLASS_ERROR(bcm2835_close() != 1, TException, "failed to shutdown bcm2835 library");
			}

			//	from IGPIOController
			const collection::list::TList<gpio::IPin* const>&
					TBCM2835::Pins			()
			{
				return this->gpio_pins;
			}

			//	from IBusController
			u32_t	TBCM2835::BaudrateMin	() const
			{
				return 3815;
			}

			u32_t	TBCM2835::BaudrateMax	() const
			{
				return 125000000;
			}

			const collection::list::TList<bus::IDevice* const>&
					TBCM2835::Devices		()
			{
				return this->devices;
			}

			//	from ISPIBusController
			const collection::list::TList<bus::spi::ISPIDevice* const>&
					TBCM2835::SPIDevices	()
			{
				return this->spi_devices;
			}

			//	from II2CBusController
			const collection::list::TList<bus::i2c::II2CDevice* const>&
					TBCM2835::I2CDevices	()
			{
				return this->i2c_devices;
			}

			void	TBCM2835::Scan			()
			{
				CL3_NOT_IMPLEMENTED;
			}

			/*****************************************************************************/
		}
	}
}

#endif
