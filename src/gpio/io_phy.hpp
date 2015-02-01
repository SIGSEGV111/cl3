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
#include <cl3/core/io_stream.hpp>
#include <cl3/core/io_collection.hpp>
#include <cl3/core/io_text_string.hpp>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			using namespace system::types;

			namespace	gpio
			{
				struct	IGPIOPin;
				struct	IGPIOController;

				//	NOTE: not all controller support all modes on all pins, but they should tell you when you try set the mode on the pin (with a throw())
				enum	EPinMode
				{
					PINMODE_GPIO_INPUT,
					PINMODE_GPIO_OUTPUT,
					PINMODE_SPI_MASTER_CLOCK,	//	act as SPI master on this pin, and use it to output the clock signal
					PINMODE_SPI_MASTER_MOSI,	//	act as SPI master on this pin, and use it to send data to the slave
					PINMODE_SPI_MASTER_MISO,	//	act as SPI master on this pin, and use it to receive data from the slave
					PINMODE_SPI_SLAVE_CLOCK,	//	act as SPI slave on this pin, and use it to receive the clock signal
					PINMODE_SPI_SLAVE_MOSI,		//	act as SPI slave on this pin, and use it to receive data from the master
					PINMODE_SPI_SLAVE_MISO,		//	act as SPI slave on this pin, and use it to send data to the master
					PINMODE_I2C_CLOCK,
					PINMODE_I2C_DATA,
					PINMODE_PWM,
					PINMODE_UART_TX,
					PINMODE_UART_RX,
					PINMODE_UART_CTS,
					PINMODE_UART_RTS,
					PINMODE_PCM_CLOCK,
					PINMODE_PCM_FS,
					PINMODE_PCM_DIN,
					PINMODE_PCM_DOUT,
					PINMODE_JTAG_RESET,
					PINMODE_JTAG_RTCK,
					PINMODE_JTAG_TDO,
					PINMODE_JTAG_TCK,
					PINMODE_JTAG_TDI,
					PINMODE_JTAG_TMS
				};

				enum	EPull
				{
					PULL_DISABLED,
					PULL_LOW,
					PULL_HIGH
				};

				typedef	event::TEvent<IGPIOPin, bool>	TOnEdgeEvent;

				struct	CL3PUBT	IGPIOPin
				{
					virtual	u32_t			ID			() const CL3_GETTER = 0;
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
					virtual	const collection::IStaticCollection<const IGPIOPin* const>&	Pins	() const CL3_GETTER = 0;
					virtual	const collection::IStaticCollection<IGPIOPin* const>&		Pins	() CL3_GETTER = 0;
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
						virtual	const collection::IStaticCollection<ISPIDevice* const>&
										SPIDevices		() CL3_GETTER = 0;
						virtual	ISPIDevice*	ByID		(u32_t id) CL3_GETTER = 0;
					};

					struct	ISPIDevice : IDevice
					{
						virtual	u32_t	ID				() const CL3_GETTER = 0;
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
						virtual	const collection::IStaticCollection<II2CDevice* const>&
										I2CDevices		() CL3_GETTER = 0;
						virtual	II2CDevice* ByAddress	(u8_t address) CL3_GETTER = 0;
						virtual	void	Scan			() = 0;
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
		}
	}
}

#endif
