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

#ifndef	_include_cl3_gpio_io_phy_bcm2835_hpp_
#define	_include_cl3_gpio_io_phy_bcm2835_hpp_

#include "io_phy.hpp"
#include <cl3/core/io_collection_list.hpp>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			class	CL3PUBT	TBCM2835 : public virtual gpio::IGPIOController, public virtual bus::spi::ISPIBusController, public virtual bus::i2c::II2CBusController
			{
				private:
					CLASS TBCM2835(const TBCM2835&) = delete;
					TBCM2835& operator=(const TBCM2835&) = delete;

				protected:
					collection::list::TList<gpio::IPin*> gpio_pins;
					collection::list::TList<bus::IDevice*> devices;
					collection::list::TList<bus::spi::ISPIDevice*> spi_devices;
					collection::list::TList<bus::i2c::II2CDevice*> i2c_devices;

				public:
					//	from TBCM2835
					CL3PUBF	CLASS	TBCM2835		(const collection::IStaticCollection<u32_t>& spi_cs_pins, bool debug = false);
					CL3PUBF	CLASS	~TBCM2835		();

					//	from IGPIOController
					CL3PUBF	const collection::list::TList<gpio::IPin* const>&
									Pins			() final override CL3_GETTER;

					//	from IBusController
					CL3PUBF	u32_t	BaudrateMin		() const final override CL3_GETTER;
					CL3PUBF	u32_t	BaudrateMax		() const final override CL3_GETTER;
					CL3PUBF	const collection::list::TList<bus::IDevice* const>&
									Devices			() final override CL3_GETTER;

					//	from ISPIBusController
					CL3PUBF	const collection::list::TList<bus::spi::ISPIDevice* const>&
									SPIDevices		() final override CL3_GETTER;

					//	from II2CBusController
					CL3PUBF	const collection::list::TList<bus::i2c::II2CDevice* const>&
									I2CDevices		() final override CL3_GETTER;
					CL3PUBF	void	Scan			();
			};
		}
	}
}

#endif
#endif
