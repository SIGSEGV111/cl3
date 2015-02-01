/*
    libcl3 - common library version 3
    Copyright (C) 2013	Simon Brennecke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY
			{
				CL3_NOT_IMPLEMENTED;
			}
 without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.hpp"
#ifdef CL3_WITH_BCM2835

#include "bcm2835.h"
#include "io_phy_bcm2835.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			/*************************************************************************/

			CLASS		TBCM2835::TPin::TPin	(TBCM2835* controller, u32_t id)
			{
				CL3_NOT_IMPLEMENTED;
			}

			u32_t		TBCM2835::TPin::ID		() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			TBCM2835*	TBCM2835::TPin::Controller() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			gpio::EPinMode TBCM2835::TPin::Mode	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			void		TBCM2835::TPin::Mode	(gpio::EPinMode new_pinmode)
			{
				CL3_NOT_IMPLEMENTED;
			}

			gpio::EPull TBCM2835::TPin::Pull	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			void		TBCM2835::TPin::Pull	(gpio::EPull new_pull)
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool		TBCM2835::TPin::State	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			void		TBCM2835::TPin::State	(bool new_state)
			{
				CL3_NOT_IMPLEMENTED;
			}

			const gpio::TOnEdgeEvent&
						TBCM2835::TPin::OnEdge	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			/*************************************************************************/

			CLASS		TBCM2835::TSPIBus::TDevice::TDevice		(TSPIBus* bus, TPin* pin_cs, u32_t id)
			{
				CL3_NOT_IMPLEMENTED;
			}

			u32_t		TBCM2835::TSPIBus::TDevice::ID			() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			TBCM2835::TSPIBus* TBCM2835::TSPIBus::TDevice::BusController() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			u32_t		TBCM2835::TSPIBus::TDevice::Baudrate	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			void		TBCM2835::TSPIBus::TDevice::Baudrate	(u32_t new_baudrate)
			{
				CL3_NOT_IMPLEMENTED;
			}

			usys_t		TBCM2835::TSPIBus::TDevice::Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			usys_t		TBCM2835::TSPIBus::TDevice::Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			void		TBCM2835::TSPIBus::TDevice::Transfer	(byte_t* buffer, usys_t sz_buffer)
			{
				CL3_NOT_IMPLEMENTED;
			}

			/*************************************************************************/

			CLASS		TBCM2835::TSPIBus::TSPIBus		(TBCM2835* bcm2835, const collection::IStaticCollection<u32_t>& pins_chipselect, u32_t pin_clock, u32_t pin_mosi, u32_t pin_miso)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TBCM2835::TSPIBus::~TSPIBus		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			u32_t		TBCM2835::TSPIBus::BaudrateMin	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			u32_t		TBCM2835::TSPIBus::BaudrateMax	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::list::TList<bus::IDevice* const>&
						TBCM2835::TSPIBus::Devices		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			TBCM2835::TSPIBus::TDevice* TBCM2835::TSPIBus::ByID(u32_t id)
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::list::TList<bus::spi::ISPIDevice* const>&
						TBCM2835::TSPIBus::SPIDevices	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			/*************************************************************************/

			CLASS		TBCM2835::TI2CBus::TDevice::TDevice		(TI2CBus* bus, u8_t address)
			{
				CL3_NOT_IMPLEMENTED;
			}

			u8_t		TBCM2835::TI2CBus::TDevice::Address		() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			TBCM2835::TI2CBus*	TBCM2835::TI2CBus::TDevice::BusController	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			u32_t		TBCM2835::TI2CBus::TDevice::Baudrate	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			void		TBCM2835::TI2CBus::TDevice::Baudrate	(u32_t new_baudrate)
			{
				CL3_NOT_IMPLEMENTED;
			}

			usys_t		TBCM2835::TI2CBus::TDevice::Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			usys_t		TBCM2835::TI2CBus::TDevice::Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			/*************************************************************************/

			CLASS		TBCM2835::TI2CBus::TI2CBus		(TBCM2835* bcm2835, u32_t pin_clock, u32_t pin_data)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TBCM2835::TI2CBus::~TI2CBus		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			u32_t		TBCM2835::TI2CBus::BaudrateMin	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			u32_t		TBCM2835::TI2CBus::BaudrateMax	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::list::TList<bus::IDevice* const>&
						TBCM2835::TI2CBus::Devices		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			TBCM2835::TI2CBus::TDevice* TBCM2835::TI2CBus::ByAddress(u8_t id)
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::list::TList<bus::i2c::II2CDevice* const>&
						TBCM2835::TI2CBus::I2CDevices	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			void		TBCM2835::TI2CBus::Scan			()
			{
				CL3_NOT_IMPLEMENTED;
			}

			/*************************************************************************/

			CLASS		TBCM2835::TBCM2835	(bool debug)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TBCM2835::~TBCM2835	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			const collection::list::TList<gpio::IGPIOPin* const>&
						TBCM2835::Pins		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			/*************************************************************************/
		}
	}
}

#endif
