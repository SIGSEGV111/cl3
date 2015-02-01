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

#ifndef	_include_cl3_gpio_io_phy_bcm2835_hpp_
#define	_include_cl3_gpio_io_phy_bcm2835_hpp_

#include "config.hpp"
#ifdef CL3_WITH_BCM2835

#include "io_phy.hpp"
#include <cl3/core/io_collection_list.hpp>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			class	CL3PUBT	TBCM2835 : public gpio::IGPIOController
			{
				private:
					CLASS TBCM2835(const TBCM2835&) = delete;
					TBCM2835& operator=(const TBCM2835&) = delete;

				protected:
					collection::list::TList<gpio::IGPIOPin*> gpio_pins;

				public:
					class	CL3PUBT	TPin : public gpio::IGPIOPin
					{
						protected:
							TBCM2835* controller;
							u32_t id;

							CLASS	TPin	(TBCM2835* controller, u32_t id);

						public:
							CL3PUBF	u32_t		ID			() const final override;
							CL3PUBF	TBCM2835*	Controller	() const final override;
							CL3PUBF	gpio::EPinMode Mode		() const final override;
							CL3PUBF	void		Mode		(gpio::EPinMode new_pinmode) final override;
							CL3PUBF	gpio::EPull Pull		() const final override;
							CL3PUBF	void		Pull		(gpio::EPull new_pull) final override;
							CL3PUBF	bool		State		() const final override;
							CL3PUBF	void		State		(bool new_state) final override;
							CL3PUBF	const gpio::TOnEdgeEvent&
												OnEdge		() const final override;
					};

					class	CL3PUBT	TSPIBus : public bus::spi::ISPIBusController
					{
						private:
							CLASS TSPIBus(const TSPIBus&) = delete;
							TSPIBus& operator=(const TSPIBus&) = delete;

						protected:
							TBCM2835* bcm2835;
							TPin* pin_clock;
							TPin* pin_mosi;
							TPin* pin_miso;
							collection::list::TList<TPin*> pins_cs;
							collection::list::TList<bus::spi::ISPIDevice*> spi_devices;

						public:
							class	CL3PUBT	TDevice : public bus::spi::ISPIDevice
							{
								private:
									CLASS TDevice(const TDevice&) = delete;
									TDevice& operator=(const TDevice&) = delete;

								protected:
									TSPIBus* bus;
									TPin* pin_cs;
									u32_t id;

									CLASS	TDevice	(TSPIBus* bus, TPin* pin_cs, u32_t id);

								public:
									CL3PUBF	u32_t	ID			() const final override;
									CL3PUBF	TSPIBus*BusController() const final override;
									CL3PUBF	u32_t	Baudrate	() const final override;
									CL3PUBF	void	Baudrate	(u32_t new_baudrate) final override;
									CL3PUBF	usys_t	Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override;
									CL3PUBF	usys_t	Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override;
									CL3PUBF	void	Transfer	(byte_t* buffer, usys_t sz_buffer) final override;
							};

							//	from TSPIBus
							CL3PUBF	CLASS		TSPIBus			(TBCM2835* bcm2835, const collection::IStaticCollection<u32_t>& pins_chipselect, u32_t pin_clock, u32_t pin_mosi, u32_t pin_miso);
							CL3PUBF	CLASS		~TSPIBus		();

							//	from IBusController
							CL3PUBF	u32_t		BaudrateMin		() const final override CL3_GETTER;
							CL3PUBF	u32_t		BaudrateMax		() const final override CL3_GETTER;
							CL3PUBF	const collection::list::TList<bus::IDevice* const>&
												Devices			() final override CL3_GETTER;
							CL3PUBF	TDevice*	ByID			(u32_t id) CL3_GETTER;

							//	from ISPIBusController
							CL3PUBF	const collection::list::TList<bus::spi::ISPIDevice* const>&
												SPIDevices		() final override CL3_GETTER;
					};

					class	CL3PUBT	TI2CBus : public bus::i2c::II2CBusController
					{
						private:
							CLASS TI2CBus(const TI2CBus&) = delete;
							TI2CBus& operator=(const TI2CBus&) = delete;

						protected:
							TBCM2835* bcm2835;
							TPin* pin_clock;
							TPin* pin_mosi;
							TPin* pin_miso;
							collection::list::TList<TPin*> pins_cs;
							collection::list::TList<bus::i2c::II2CDevice*> i2c_devices;

						public:
							class	CL3PUBT	TDevice : public bus::i2c::II2CDevice
							{
								private:
									CLASS TDevice(const TDevice&) = delete;
									TDevice& operator=(const TDevice&) = delete;

								protected:
									TI2CBus* bus;
									u8_t address;

									CLASS	TDevice	(TI2CBus* bus, u8_t address);

								public:
									CL3PUBF	u8_t	Address		() const final override;
									CL3PUBF	TI2CBus*BusController() const final override;
									CL3PUBF	u32_t	Baudrate	() const final override;
									CL3PUBF	void	Baudrate	(u32_t new_baudrate) final override;
									CL3PUBF	usys_t	Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override;
									CL3PUBF	usys_t	Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override;
							};

							//	from TI2CBus
							CL3PUBF	CLASS		TI2CBus			(TBCM2835* bcm2835, u32_t pin_clock, u32_t pin_data);
							CL3PUBF	CLASS		~TI2CBus		();

							//	from IBusController
							CL3PUBF	u32_t		BaudrateMin		() const final override CL3_GETTER;
							CL3PUBF	u32_t		BaudrateMax		() const final override CL3_GETTER;
							CL3PUBF	const collection::list::TList<bus::IDevice* const>&
												Devices			() final override CL3_GETTER;
							CL3PUBF	TDevice*	ByAddress		(u8_t id) CL3_GETTER;

							//	from II2CBusController
							CL3PUBF	const collection::list::TList<bus::i2c::II2CDevice* const>&
												I2CDevices		() final override CL3_GETTER;
							CL3PUBF	void		Scan			();
					};

					//	from TBCM2835
					CL3PUBF	CLASS	TBCM2835		(bool debug = false);
					CL3PUBF	CLASS	~TBCM2835		();

					//	from IGPIOController
					CL3PUBF	const collection::list::TList<gpio::IGPIOPin* const>&
									Pins			() final override CL3_GETTER;
			};
		}
	}
}

#endif
#endif
