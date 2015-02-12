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

/*
    PARTS OF THE CODE WERE COPIED FROM THE BCM2835-LIB PROJECT
    Copyright (C) 2011-2013 Mike McCauley
    FOR DETAILS SEE <bcm2835.c> AND/OR <bcm2835.h>
*/

#include "config.hpp"
#ifdef CL3_WITH_BCM2835

#include "bcm2835.h"
#include "io_phy_bcm2835.hpp"
#include <cl3/core/error.hpp>

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			using namespace error;

			/*************************************************************************/

			/*
			PINMODE_GPIO_INPUT,
			PINMODE_GPIO_OUTPUT,
			PINMODE_GPIO_CLOCK,
			PINMODE_SPI_MASTER_CLOCK,
			PINMODE_SPI_MASTER_MOSI,
			PINMODE_SPI_MASTER_MISO,
			PINMODE_SPI_SLAVE_CLOCK,
			PINMODE_SPI_SLAVE_MOSI,
			PINMODE_SPI_SLAVE_MISO,
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
			*/

			namespace	gpio
			{
				static const EPinMode PIN2FUNC[54][6] =
					{
						{PINMODE_I2C_DATA,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  0
						{PINMODE_I2C_CLOCK,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  1
						{PINMODE_I2C_DATA,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  2
						{PINMODE_I2C_CLOCK,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  3
						{PINMODE_GPIO_CLOCK,		PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  4
						{PINMODE_GPIO_CLOCK,		PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  5
						{PINMODE_GPIO_CLOCK,		PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  6
						{PINMODE_GPIO_OUTPUT,		PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  7
						{PINMODE_GPIO_OUTPUT,		PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  8
						{PINMODE_SPI_MASTER_MISO,	PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, //  9
						{PINMODE_SPI_MASTER_MOSI,	PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, // 10
						{PINMODE_SPI_MASTER_CLOCK,	PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, // 11
						{PINMODE_PWM,				PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, // 12
						{PINMODE_PWM,				PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, // 13
						{PINMODE_UART_TX,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, // 14
						{PINMODE_UART_RX,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED,		PINMODE_UNDEFINED,			PINMODE_UNDEFINED }, // 15
						{PINMODE_UNDEFINED,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UART_CTS,			PINMODE_GPIO_OUTPUT,		PINMODE_UART_CTS  }, // 16
						{PINMODE_UNDEFINED,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UART_RTS,			PINMODE_GPIO_OUTPUT,		PINMODE_UART_RTS  }, // 17
						{PINMODE_PCM_CLOCK,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_SPI_SLAVE_MOSI,	PINMODE_GPIO_OUTPUT,		PINMODE_PWM       }, // 18
						{PINMODE_PCM_FS,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_SPI_SLAVE_CLOCK,	PINMODE_SPI_MASTER_MISO,	PINMODE_PWM       }, // 19
						{PINMODE_PCM_DIN,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_SPI_SLAVE_MISO,	PINMODE_SPI_MASTER_MOSI,	PINMODE_GPIO_CLOCK}, // 20
						{PINMODE_PCM_DOUT,			PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_SPI_SLAVE_SELECT,	PINMODE_SPI_MASTER_CLOCK,	PINMODE_GPIO_CLOCK}, // 21

						{PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_JTAG_RESET, PINMODE_UNDEFINED}, // 22
						{PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_JTAG_RTCK,  PINMODE_UNDEFINED}, // 23
						{PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_JTAG_TDO,   PINMODE_UNDEFINED}, // 24
						{PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_JTAG_TCK,   PINMODE_UNDEFINED}, // 25
						{PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_JTAG_TDI,   PINMODE_UNDEFINED}, // 26
						{PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_JTAG_TMS,   PINMODE_UNDEFINED}, // 27

						{PINMODE_I2C_DATA,  PINMODE_UNDEFINED, PINMODE_PCM_CLOCK, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED}, // 28
						{PINMODE_I2C_CLOCK, PINMODE_UNDEFINED, PINMODE_PCM_FS,    PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UNDEFINED}, // 29
						{PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_PCM_DIN,   PINMODE_UART_CTS,  PINMODE_UNDEFINED, PINMODE_UART_CTS }, // 30
						{PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_PCM_DOUT,  PINMODE_UART_RTS,  PINMODE_UNDEFINED, PINMODE_UART_RTS }, // 31

						{PINMODE_GPIO_CLOCK, PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UART_TX, PINMODE_UNDEFINED, PINMODE_UART_TX }, // 32
						{PINMODE_UNDEFINED,  PINMODE_UNDEFINED, PINMODE_UNDEFINED, PINMODE_UART_RX, PINMODE_UNDEFINED, PINMODE_UART_RX }, // 33
					};
			}

			CLASS		TBCM2835::TPin::TPin	(TBCM2835* bcm2835, u32_t id) : bcm2835(bcm2835), id(id)
			{
			}

			u32_t		TBCM2835::TPin::ID		() const
			{
				return this->id;
			}

			TBCM2835*	TBCM2835::TPin::Controller() const
			{
				return this->bcm2835;
			}

			gpio::EPinMode TBCM2835::TPin::Mode	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			void		TBCM2835::TPin::Mode	(gpio::EPinMode new_pinmode)
			{
				CL3_NOT_IMPLEMENTED;
				//bcm2835_gpio_fsel
				/*
					BCM2835_GPIO_FSEL_INPT
					BCM2835_GPIO_FSEL_OUTP
					BCM2835_GPIO_FSEL_ALT0
					BCM2835_GPIO_FSEL_ALT1
					BCM2835_GPIO_FSEL_ALT2
					BCM2835_GPIO_FSEL_ALT3
					BCM2835_GPIO_FSEL_ALT4
					BCM2835_GPIO_FSEL_ALT5
				*/
			}

			gpio::EPull TBCM2835::TPin::Pull	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			void		TBCM2835::TPin::Pull	(gpio::EPull new_pull)
			{
				switch(new_pull)
				{
					case gpio::PULL_DISABLED:
						bcm2835_gpio_set_pud(this->id, BCM2835_GPIO_PUD_OFF);
						break;
					case gpio::PULL_DOWN:
						bcm2835_gpio_set_pud(this->id, BCM2835_GPIO_PUD_DOWN);
						break;
					case gpio::PULL_UP:
						bcm2835_gpio_set_pud(this->id, BCM2835_GPIO_PUD_UP);
						break;
				}
			}

			bool		TBCM2835::TPin::State	() const
			{
				return bcm2835_gpio_lev(this->id) == HIGH;
			}

			void		TBCM2835::TPin::State	(bool new_state)
			{
				if(new_state)
					bcm2835_gpio_set(this->id);
				else
					bcm2835_gpio_clr(this->id);
			}

			const gpio::TOnEdgeEvent&
						TBCM2835::TPin::OnEdge	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			/*************************************************************************/

			CLASS		TBCM2835::TSPIBus::TDevice::TDevice		(TSPIBus* bus, TPin* pin_cs, u32_t id) : bus(bus), pin_cs(pin_cs), id(id), baudrate(bus->BaudrateMin())
			{
			}

			u32_t		TBCM2835::TSPIBus::TDevice::ID			() const
			{
				return this->id;
			}

			TBCM2835::TSPIBus* TBCM2835::TSPIBus::TDevice::BusController() const
			{
				return this->bus;
			}

			u32_t		TBCM2835::TSPIBus::TDevice::Baudrate	() const
			{
				return this->baudrate;
			}

			void		TBCM2835::TSPIBus::TDevice::Baudrate	(u32_t new_baudrate)
			{
				CL3_CLASS_ERROR(new_baudrate < this->bus->BaudrateMin() || new_baudrate < this->bus->BaudrateMax(), TException, "unsupported baudrate requested");
				u32_t divider = 1;
				for(; (125000000 / divider) > new_baudrate; divider *= 2);
				this->baudrate = 125000000 / divider;
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

			CLASS		TBCM2835::TSPIBus::TSPIBus		(TBCM2835* bcm2835, const collection::IStaticCollection<u32_t>& pins_chipselect, u32_t pin_clock, u32_t pin_mosi, u32_t pin_miso) : bcm2835(bcm2835), pin_clock(bcm2835->ByID(pin_clock)), pin_mosi(bcm2835->ByID(pin_mosi)), pin_miso(bcm2835->ByID(pin_miso))
			{
				CL3_CLASS_ERROR_NOARGS(pin_clock != 11 || pin_mosi != 10 || pin_miso != 9, TNotImplementedException);	//	FIXME: can only use SPI0 for now...
				bcm2835_spi_begin();
				bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);	//	handle chip-select in cl3-/user-code to support more than two devices on the bus and to support crazy devices

				this->pin_clock->Mode(gpio::PINMODE_SPI_MASTER_CLOCK);
				this->pin_mosi->Mode(gpio::PINMODE_SPI_MASTER_MOSI);
				this->pin_miso->Mode(gpio::PINMODE_SPI_MASTER_MISO);

				auto it = pins_chipselect.CreateStaticIterator();
				it->MoveHead();
				for(u32_t i = 0; it->MoveNext(); i++)
				{
					if(it->Item() == (u32_t)-1)
					{
						//	handle chip-select in user code
						this->spi_devices.Append(new TBCM2835::TSPIBus::TDevice(this, NULL, i));
					}
					else
					{
						//	handle chip-select in cl3 code
						TPin* const pin = bcm2835->ByID(it->Item());
						pin->Mode(gpio::PINMODE_GPIO_OUTPUT);
						pin->Pull(gpio::PULL_DOWN);
						pin->State(false);
						this->spi_devices.Append(new TBCM2835::TSPIBus::TDevice(this, pin, i));
					}
				}
			}

			CLASS		TBCM2835::TSPIBus::~TSPIBus		()
			{
				bcm2835_spi_end();

				for(usys_t i = 0; i < this->spi_devices.Count(); i++)
					delete this->spi_devices[i];
			}

			u32_t		TBCM2835::TSPIBus::BaudrateMin	() const
			{
				return 3815;
			}

			u32_t		TBCM2835::TSPIBus::BaudrateMax	() const
			{
				return 125000000;
			}

			const collection::list::TList<bus::IDevice* const>&
						TBCM2835::TSPIBus::Devices		()
			{
				CL3_NOT_IMPLEMENTED;	//	TODO: implement a type-converting wrapper collection
			}

			TBCM2835::TSPIBus::TDevice* TBCM2835::TSPIBus::ByID(u32_t id)
			{
				return &dynamic_cast<TBCM2835::TSPIBus::TDevice&>(*this->spi_devices[id]);
			}

			const collection::list::TList<bus::spi::ISPIDevice* const>&
						TBCM2835::TSPIBus::SPIDevices	()
			{
				return this->spi_devices;
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
				CL3_CLASS_ERROR(bcm2835_init() != 1, TException, "initialization of the bcm2835 lib failed");
				bcm2835_set_debug(debug ? 1 : 0);

				//	add all 54 GPIO pins available on the BCM2835 to the list of pins
				for(u32_t i = 0; i < 54; i++)
					this->gpio_pins.Append(new TPin(this, i));
			}

			CLASS		TBCM2835::~TBCM2835	()
			{
				CL3_CLASS_ERROR(bcm2835_close() != 1, TException, "shutdown of the bcm2835 lib failed");
			}

			const collection::list::TList<gpio::IGPIOPin* const>&
						TBCM2835::Pins		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			TBCM2835::TPin*	TBCM2835::ByID	(u32_t id)
			{
				return this->gpio_pins[id];
			}

			/*************************************************************************/
		}
	}
}

#endif
