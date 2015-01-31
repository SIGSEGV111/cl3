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
#include <cl3/core/io_stream.hpp>
#include <cl3/core/io_collection.hpp>
#include <cl3/core/io_text_string.hpp>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			namespace	gpio
			{
				struct	IPin;
				struct	IGPIOController;

				enum	EPinMode
				{
					PINMODE_INPUT,
					PINMODE_OUTPUT
				};

				enum	EPull
				{
					PULL_DISABLED,
					PULL_LOW,
					PULL_HIGH
				};

				typedef	event::TEvent<IPin, bool>	TOnEdgeEvent;

				struct	CL3PUBT	IPin
				{
					virtual	IGPIOController*Controller	() const CL3_GETTER = 0;
					virtual	EPinMode		Mode		() const CL3_GETTER = 0;
					virtual	void			Mode		(EPinMode) CL3_SETTER = 0;
					virtual	EPull			Pull		() const CL3_GETTER = 0;
					virtual	void			Pull		(EPull) CL3_SETTER = 0;
					virtual	bool			State		() const CL3_GETTER = 0;
					virtual	void			State		(bool) CL3_SETTER = 0;
					virtual	const TOnEdgeEvent&	OnEdge	() const CL3_GETTER = 0;
				};

				struct	CL3PUBT	IGPIOController
				{
					virtual	const collection::IStaticCollection<const IPin* const>&	Pins	() const CL3_GETTER = 0;
					virtual	const collection::IStaticCollection<IPin* const>&		Pins	() CL3_GETTER = 0;
				};
			}

			namespace	pwm
			{
			};

			namespace	bus
			{
				struct	IDevice;
				struct	IBusController;

				struct	IDevice : public virtual stream::IIn<byte_t>, public virtual stream::IOut<byte_t>
				{
					virtual	IBusController*
									BusController	() const CL3_GETTER = 0;
					virtual	u32_t	Baudrate		() const CL3_GETTER = 0;
					virtual	void	Baudrate		(u32_t) CL3_SETTER = 0;
				};

				struct	IBusController
				{
					virtual	u32_t	BaudrateMin	() const CL3_GETTER = 0;
					virtual	u32_t	BaudrateMax	() const CL3_GETTER = 0;
					virtual	const collection::IStaticCollection<const IDevice* const>&
									Devices		() const CL3_GETTER = 0;
					virtual	const collection::IStaticCollection<IDevice* const>&
									Devices		() CL3_GETTER = 0;
				};

				namespace	spi
				{
					struct	ISPIDevice;
					struct	ISPIBusController;

					struct	ISPIBusController : virtual IBusController
					{
						virtual	const collection::IStaticCollection<const ISPIDevice* const>&
										SPIDevices		() const CL3_GETTER = 0;
						virtual	const collection::IStaticCollection<ISPIDevice* const>&
										SPIDevices		() CL3_GETTER = 0;
					};

					struct	ISPIDevice : IDevice
					{
						virtual	usys_t	ID				() const CL3_GETTER = 0;
						virtual	ISPIBusController*
										BusController	() const CL3_GETTER = 0;
						virtual	void	Transfer		(byte_t* buffer, usys_t sz_buffer) = 0;
					};
				}

				namespace	i2c
				{
					struct	II2CDevice;
					struct	II2CBusController;

					struct	II2CBusController : virtual IBusController
					{
						virtual	const collection::IStaticCollection<const II2CDevice* const>&
										I2CDevices		() const CL3_GETTER = 0;
						virtual	const collection::IStaticCollection<II2CDevice* const>&
										I2CDevices		() CL3_GETTER = 0;
					};

					struct	II2CDevice : IDevice
					{
						virtual	u8_t	Address			() const CL3_GETTER = 0;
						virtual	II2CBusController*
										BusController	() const CL3_GETTER = 0;
					};
				}

				namespace	_1wire
				{
					struct	I1wireDevice;
					struct	I1wireBusController;

					struct	I1wireBusController : virtual IBusController
					{
						virtual	const collection::IStaticCollection<const I1wireDevice* const>&
										_1wireDevices	() const CL3_GETTER = 0;
						virtual	const collection::IStaticCollection<I1wireDevice* const>&
										_1wireDevices	() CL3_GETTER = 0;
					};

					struct	I1wireDevice : IDevice
					{
					};

					class	CL3PUBT	TSoft1wire : public virtual I1wireBusController
					{
					};
				}
			}

			class	CL3PUBT	TBCM2835 : public virtual gpio::IGPIOController, public virtual bus::spi::ISPIBusController, public virtual bus::i2c::II2CBusController
			{
				private:
					CLASS TBCM2835(const TBCM2835&) = delete;
					TBCM2835& operator=(const TBCM2835&) = delete;

				public:
					CL3PUBF	CLASS	TBCM2835	();
					CL3PUBF	CLASS	~TBCM2835	();

					//	from IGPIOController
					CL3PUBF	const collection::IStaticCollection<const gpio::IPin* const>&
									Pins			() const final override CL3_GETTER;
					CL3PUBF	const collection::IStaticCollection<gpio::IPin* const>&
									Pins			() final override CL3_GETTER;

					//	from IBusController
					CL3PUBF	u32_t	BaudrateMin		() const final override CL3_GETTER;
					CL3PUBF	u32_t	BaudrateMax		() const final override CL3_GETTER;
					CL3PUBF	const collection::IStaticCollection<const bus::IDevice* const>&
									Devices			() const final override CL3_GETTER;
					CL3PUBF	const collection::IStaticCollection<bus::IDevice* const>&
									Devices			() final override CL3_GETTER;

					//	from ISPIBusController
					CL3PUBF	const collection::IStaticCollection<const bus::spi::ISPIDevice* const>&
									SPIDevices		() const final override CL3_GETTER;
					CL3PUBF	const collection::IStaticCollection<bus::spi::ISPIDevice* const>&
									SPIDevices		() final override CL3_GETTER;

					//	from II2CBusController
					CL3PUBF	const collection::IStaticCollection<const bus::i2c::II2CDevice* const>&
									I2CDevices		() const final override CL3_GETTER;
					CL3PUBF	const collection::IStaticCollection<bus::i2c::II2CDevice* const>&
									I2CDevices		() final override CL3_GETTER;
			};

	// 		class	CL3PUBT	TBCM2835
	// 		{
	// 			private:
	// 				CLASS TBCM2835(const TBCM2835&) = delete;
	//
	// 			protected:
	// 				class 	CL3PUBT	TPin : public gpio::IPin
	// 				{
	// 					private:
	// 						TBCM2835* controller;
	// 						CLASS TPin(TBCM2835* controller);
	//
	// 					public:
	// 						CL3PUBF	gpio::IGPIOController*	Controller	() const final override CL3_GETTER;
	// 						CL3PUBF	EPinMode			Mode		() const final override CL3_GETTER;
	// 						CL3PUBF	void				Mode		(EPinMode) final override CL3_SETTER;
	// 						CL3PUBF	bool				State		() const final override CL3_GETTER;
	// 						CL3PUBF	void				State		(bool) final override CL3_SETTER;
	// 						CL3PUBF	const TOnEdgeEvent&	OnEdge		() const final override CL3_GETTER;
	// 				};
	//
	// 				class	CL3PUBT	TSPIDevice : public bus::spi::ISPIDevice
	// 				{
	// 					private:
	// 						TBCM2835* controller;
	// 						CLASS TPin(TBCM2835* controller);
	//
	// 					public:
	// 						CL3PUBF	TBCM2835* BusController() const final override CL3_GETTER;
	// 						CL3PUBF	u32_t	Baudrate	() const final override CL3_GETTER;
	// 						CL3PUBF	void	Baudrate	(u32_t) final override CL3_SETTER;
	//
	// 						CL3PUBF	usys_t	Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;
	// 						CL3PUBF	usys_t	Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;
	// 						CL3PUBF	void	Transfer	(byte_t* buffer, usys_t sz_buffer) final override;
	// 				};
	//
	// 				class	CL3PUBT	TI2CDevice : public bus::i2c::II2CDevice
	// 				{
	// 					private:
	// 						TBCM2835* controller;
	// 						CLASS TPin(TBCM2835* controller);
	//
	// 					public:
	// 						CL3PUBF	TBCM2835* BusController() const final override CL3_GETTER;
	// 						CL3PUBF	u32_t	Baudrate	() const final override CL3_GETTER;
	// 						CL3PUBF	void	Baudrate	(u32_t) final override CL3_SETTER;
	//
	// 						CL3PUBF	usys_t	Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;
	// 						CL3PUBF	usys_t	Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;
	// 				};
	//
	// 			public:
	// 				class	CL3PUBT	TGPIOController : public gpio::IGPIOController
	// 				{
	// 					protected:
	// 						CLASS	TGPIOController	();
	// 					public:
	// 						CL3PUBF	const io::collection::IStaticCollection<IPin* const>&	Pins	() const final override CL3_GETTER;
	// 				};
	//
	// 				class	CL3PUBT	TSPIBusController : public bus::spi::ISPIBusController
	// 				{
	// 					protected:
	// 						CLASS	TSPIBusController();
	// 					public:
	// 						CL3PUBF	u32_t	BaudrateMin	() const CL3_GETTER;
	// 						CL3PUBF	u32_t	BaudrateMax	() const CL3_GETTER;
	// 						CL3PUBF	const collection::IStaticCollection<IDevice* const>&
	// 										Devices		() const CL3_GETTER;
	// 				};
	//
	// 				class	CL3PUBT	TI2CBusController : public bus::i2c::II2CBusController
	// 				{
	// 					protected:
	// 						CLASS	TI2CBusController();
	// 					public:
	// 						CL3PUBF	u32_t	BaudrateMin	() const CL3_GETTER;
	// 						CL3PUBF	u32_t	BaudrateMax	() const CL3_GETTER;
	// 						CL3PUBF	const collection::IStaticCollection<const IDevice* const>&
	// 										Devices		() const CL3_GETTER;
	// 						CL3PUBF	const collection::IStaticCollection<IDevice* const>&
	// 										Devices		() CL3_GETTER;
	// 				};
	//
	// 			protected:
	// 				TGPIOController gpio;
	// 				TSPIBusController spibus;
	// 				TI2CBusController i2cbus;
	//
	// 			public:
	// 				inline	TGPIOController&	GPIO	() { return gpio; }
	// 				inline	TSPIBusController&	SPI		() { return spibus; }
	// 				inline	TI2CBusController&	I2C		() { return i2cbus; }
	//
	// 				CL3PUBF	CLASS	TBCM2835	();
	// 				CL3PUBF	CLASS	~TBCM2835	();
	// 		};
		}
	}
}

#endif
