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

#include <pthread.h>
#include "io_phy.hpp"
#include <cl3/core/io_collection_list.hpp>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			namespace	bcm2835
			{
				class	TPin;
				class	TGPIO;
				class	TSPIDevice;
				class	TSPIBus;

				class	CL3PUBT	TPin : public gpio::IPin
				{
					friend class TGPIO;
					private:
						CLASS TPin(const TPin&) = delete;
						TPin& operator=(const TPin&) = delete;

					protected:
						TGPIO* gpio;
						u32_t index;
						gpio::EMode mode;
						bool b_level_last;
						gpio::TOnEdgeEvent on_edge;
						gpio::TOnIdleEvent on_idle;
						system::time::TTime dt_idletimeout;

						CLASS	TPin	(TGPIO*, u32_t index);

					public:
						CL3PUBF	int			Function	() const CL3_GETTER;
						CL3PUBF	void		Function	(int) CL3_SETTER;

						//	from IPin
						CL3PUBF	const gpio::TOnEdgeEvent&	OnEdge	() const final override CL3_GETTER;
						CL3PUBF	const gpio::TOnIdleEvent&	OnIdle	() const final override CL3_GETTER;

						CL3PUBF	void				IdleTimeout	(system::time::TTime) final override CL3_SETTER;
						CL3PUBF	system::time::TTime	IdleTimeout	() const final override CL3_GETTER;

						CL3PUBF	int			Mode		() const final override CL3_GETTER;
						CL3PUBF	void		Mode		(int) final override CL3_SETTER;
						CL3PUBF	gpio::EPull Pull		() const final override CL3_GETTER;
						CL3PUBF	void		Pull		(gpio::EPull new_pull) final override CL3_SETTER;
						CL3PUBF	bool		Level		() const final override;
						CL3PUBF	void		Level		(bool new_level) final override;
				};

				class	CL3PUBT	TGPIO : public gpio::IGPIO
				{
					friend class TPin;
					private:
						CLASS TGPIO(const TGPIO&) = delete;
						TGPIO& operator=(const TGPIO&) = delete;

					protected:
						pthread_t th_irq;
						collection::list::TList<gpio::IPin* const> pins;

						static void* ThreadMain(void*);

					public:
						CL3PUBF	CLASS	TGPIO	(bool debug = false);
						CL3PUBF	CLASS	~TGPIO	();
						CL3PUBF const collection::list::TList<gpio::IPin* const>&
										Pins	() final override CL3_GETTER;

						CL3PUBF	static	u64_t	Tick	();	//	incremented every microsecond
				};

				class	CL3PUBT	TSPIDevice : public bus::spi::IDevice
				{
					friend class TSPIBus;
					private:
						CLASS TSPIDevice(const TSPIDevice&) = delete;
						TSPIDevice& operator=(const TSPIDevice&) = delete;

					protected:
						struct	TBusLock
						{
							TSPIDevice* device;

							CLASS	TBusLock	(const TBusLock&) = delete;
							CLASS	TBusLock	(TSPIDevice* device);
							CLASS	~TBusLock	();
						};

						TSPIBus* bus;
						gpio::IPin* pin_cs;
						unsigned divider;

						CL3PUBF CLASS	TSPIDevice	(TSPIBus*, gpio::IPin*);

					public:
						CL3PUBF CLASS	~TSPIDevice	();

						//	from IDevice
						CL3PUBF	bus::spi::IBus*	Bus	() const CL3_GETTER;
						CL3PUBF u32_t	Baudrate	() const final override CL3_GETTER;
						CL3PUBF void	Baudrate	(u32_t new_baudrate) final override CL3_SETTER;
						CL3PUBF void	Transfer	(byte_t* buffer, usys_t sz_buffer) final override;

						//	from IIn
						CL3PUBF usys_t	Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;

						//	from IOut
						CL3PUBF usys_t	Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;
				};

				class	CL3PUBT	TSPIBus : public bus::spi::IBus
				{
					private:
						CLASS TSPIBus(const TSPIBus&) = delete;
						TSPIBus& operator=(const TSPIBus&) = delete;

					protected:
						collection::list::TList<bus::spi::IDevice* const> devices;
						unsigned idx_bus;

					public:
						CL3PUBF	CLASS	TSPIBus		(unsigned idx_bus, const collection::IStaticCollection<gpio::IPin*>& pins_chipselect);
						CL3PUBF	CLASS	~TSPIBus	();

						//	from IBus
						CL3PUBF u32_t	BaudrateMin	() const final override CL3_GETTER;
						CL3PUBF u32_t	BaudrateMax	() const final override CL3_GETTER;
						CL3PUBF const collection::list::TList<bus::spi::IDevice* const>&
										Devices		() CL3_GETTER;
				};
			}
		}
	}
}

#endif
#endif
