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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "config.hpp"
#ifdef CL3_WITH_BCM2835

#include "bcm2835.h"
#include "io_phy_bcm2835.hpp"
#include <cl3/core/error.hpp>
#include <signal.h>
#include <sys/signalfd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

inline static bool operator==(const struct pollfd& pfd1, const struct pollfd& pfd2) { return memcmp(&pfd1, &pfd2, sizeof(struct pollfd)) == 0; }
inline static bool operator> (const struct pollfd& pfd1, const struct pollfd& pfd2) { return memcmp(&pfd1, &pfd2, sizeof(struct pollfd)) >  0; }
inline static bool operator< (const struct pollfd& pfd1, const struct pollfd& pfd2) { return memcmp(&pfd1, &pfd2, sizeof(struct pollfd)) <  0; }
inline static bool operator>=(const struct pollfd& pfd1, const struct pollfd& pfd2) { return memcmp(&pfd1, &pfd2, sizeof(struct pollfd)) >= 0; }
inline static bool operator<=(const struct pollfd& pfd1, const struct pollfd& pfd2) { return memcmp(&pfd1, &pfd2, sizeof(struct pollfd)) <= 0; }

namespace	cl3
{
	namespace	io
	{
		namespace	phy
		{
			namespace	bcm2835
			{
				using namespace error;
				using namespace system::time;
				using namespace collection::list;

				/*************************************************************************/

				static const usys_t N_GPIO_PINS = 54;

				CLASS		TPin::TPin	(TGPIO* gpio, u32_t index) : gpio(gpio), index(index), mode(gpio::MODE_TRISTATE), b_level_last(false), dt_idletimeout(-1)
				{
				}

				const gpio::TOnEdgeEvent&
							TPin::OnEdge() const
				{
					return this->on_edge;
				}

				const gpio::TOnIdleEvent&
							TPin::OnIdle() const
				{
					return this->on_idle;
				}

				void	TPin::IdleTimeout	(TTime new_idletimeout)
				{
					this->dt_idletimeout = new_idletimeout;
				}

				TTime	TPin::IdleTimeout	() const
				{
					return this->dt_idletimeout;
				}

				int		TPin::Mode	() const
				{
					return this->mode;
				}

				void		TPin::Mode	(int new_mode)
				{
					switch(new_mode)
					{
						case gpio::MODE_TRISTATE:	//	disabled/nothing/deactivated/don't care (same as INPT for bcm2835)
							break;
						case gpio::MODE_INPUT:	//	GPIO input
							bcm2835_gpio_fsel(this->index, BCM2835_GPIO_FSEL_INPT);
							break;
						case gpio::MODE_OUTPUT:	//	GPIO output
							bcm2835_gpio_fsel(this->index, BCM2835_GPIO_FSEL_OUTP);
							break;
						default:	//	other hardware specific special function
							CL3_CLASS_FAIL(TException, "cannot set special mode thru this interface, please use the bcm2835-specific Function()-property instead");
					}
					this->mode = (gpio::EMode)new_mode;
					pthread_kill(this->gpio->th_irq, SIGUSR1);
				}

				gpio::EPull TPin::Pull	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				void		TPin::Pull	(gpio::EPull new_pull)
				{
					switch(new_pull)
					{
						case gpio::PULL_DISABLED:
							bcm2835_gpio_set_pud(this->index, BCM2835_GPIO_PUD_OFF);
							break;
						case gpio::PULL_DOWN:
							bcm2835_gpio_set_pud(this->index, BCM2835_GPIO_PUD_DOWN);
							break;
						case gpio::PULL_UP:
							bcm2835_gpio_set_pud(this->index, BCM2835_GPIO_PUD_UP);
							break;
					}
				}

				bool		TPin::Level	() const
				{
					return bcm2835_gpio_lev(this->index) == HIGH;
				}

				void		TPin::Level	(bool new_level)
				{
					if(new_level)
						bcm2835_gpio_set(this->index);
					else
						bcm2835_gpio_clr(this->index);
				}

				void*		TGPIO::ThreadMain	(void* _gpio)
				{
// 					fprintf(stderr, "GPIO-IRQ-THREAD: INFO: booting ... \n");
					int fd_export = -1;
					int fd_unexport = -1;

					//	build list of all FDs to monitor
					struct pollfd pfds[N_GPIO_PINS + 1];

					try
					{
						TGPIO* gpio = reinterpret_cast<TGPIO*>(_gpio);

						//	open gpio sysfs interface
						CL3_NONCLASS_SYSERR(fd_export = open("/sys/class/gpio/export", O_CLOEXEC|O_NOCTTY|O_WRONLY));
						CL3_NONCLASS_SYSERR(fd_unexport = open("/sys/class/gpio/unexport", O_CLOEXEC|O_NOCTTY|O_WRONLY));

						//	block all signals (handle the later via signalfd)
						{
							sigset_t ss;
							sigfillset(&ss);
							pthread_sigmask(SIG_BLOCK, &ss, NULL);
						}

						// set realtime prio
						{
							sched_param p;
							memset(&p, 0, sizeof(p));
							p.sched_priority = 1;
							CL3_NONCLASS_SYSERR(sched_setscheduler(0, SCHED_FIFO, &p));
							CL3_NONCLASS_SYSERR(mlockall(MCL_CURRENT|MCL_FUTURE));
						}

						{
							//	init entries for the pins
							for(usys_t i = 0; i < N_GPIO_PINS; i++)
							{
								pfds[i].fd = -1;
								pfds[i].events = POLLPRI;
								pfds[i].revents = 0;
							}

							//	init entry for the signalfd
							sigset_t ss;
							sigemptyset(&ss);
							sigaddset(&ss, SIGTERM);
							sigaddset(&ss, SIGUSR1);
							CL3_NONCLASS_SYSERR(pfds[N_GPIO_PINS].fd = signalfd(-1, &ss, SFD_CLOEXEC|SFD_NONBLOCK));
							pfds[N_GPIO_PINS].events = POLLIN;	//	POLLIN for the signalfd
							pfds[N_GPIO_PINS].revents = 0;
						}

// 						fprintf(stderr, "GPIO-IRQ-THREAD: INFO: READY\n");
						//	set this once all configuration is done to notify the controller thread (which is waiting in TGPIO::TGPIO())
						*((volatile pthread_t*)(&gpio->th_irq)) = pthread_self();

						//	wait for signals...
						int dt_idletimeout_ms = -1;
						bool b_run = true;
						while(b_run)
						{
							int rc_poll;
							CL3_NONCLASS_SYSERR(rc_poll = poll(pfds, N_GPIO_PINS+1, dt_idletimeout_ms));

							//	reset the idle timeout
							dt_idletimeout_ms = -1;

							//	check all GPIO pins
							for(usys_t i = 0; i < N_GPIO_PINS; i++)
								if(CL3_UNLIKELY(pfds[i].fd != -1))
								{
									TPin* const pin = static_cast<TPin*>(gpio->pins[i]);	//	use static_cast to avoid virtual-function-call thru IPin and to get all members

									const bool b_level_new = pin->Level();

									//	raise edge events when we detected a level change
									if(CL3_LIKELY(pin->b_level_last != b_level_new))
									{
										const gpio::TOnEdgeData data = { pin->b_level_last, b_level_new };
										pin->b_level_last = b_level_new;
										pin->on_edge.Raise(pin, data);
									}

									if(CL3_LIKELY(pfds[i].revents))
									{
										//	clear the kernel event status
										char buffer[2] = {};
										pread(pfds[i].fd, buffer, sizeof(buffer), 0);
									}

									if(pin->on_idle.HasReceivers())
									{
										const int dt_idletimeout_ms_pin = (int)pin->dt_idletimeout.ConvertToI(TIME_UNIT_MILLISECONDS);
										if(dt_idletimeout_ms_pin > dt_idletimeout_ms || dt_idletimeout_ms == -1)
											dt_idletimeout_ms = dt_idletimeout_ms_pin;

										if(rc_poll == 0)	//	the call to poll timed-out
											pin->on_idle.Raise(pin, gpio::TOnIdleData());
									}
								}

							if(pfds[N_GPIO_PINS].revents)
							{
								//	read signals
								signalfd_siginfo si;
								while(read(pfds[N_GPIO_PINS].fd, &si, sizeof(si)) == sizeof(si))
									switch(si.ssi_signo)
									{
										case SIGTERM:
											//	shutdown requested
											b_run = false;
											break;
										case SIGUSR1:
											//	pin config changed, rebuild list
											for(unsigned i = 0; i < N_GPIO_PINS; i++)
											{
												TPin* const pin = static_cast<TPin*>(gpio->pins[i]);
												if(pin->mode == gpio::MODE_INPUT && pfds[i].fd == -1)
												{
													//	new pin to monitor

													//	tell kernel to export the pin to sysfs
													//	we wont check the error code, because either it works or the pin was already exported, so no need to act on it
													char buffer[64];
													write(fd_export, buffer, snprintf(buffer, sizeof(buffer), "%u\n", i));

													//	tell the kernel that we want to use the pin as input... hope this does not cause any conflicts with libbcm2835
													int fd;
													snprintf(buffer, sizeof(buffer), "/sys/class/gpio/gpio%u/direction", i);
													CL3_NONCLASS_SYSERR(fd = open(buffer, O_CLOEXEC|O_NOCTTY|O_WRONLY));
													CL3_NONCLASS_SYSERR(write(fd, "in\n", 3) != 3);
													close(fd);

													snprintf(buffer, sizeof(buffer), "/sys/class/gpio/gpio%u/edge", i);
													CL3_NONCLASS_SYSERR(fd = open(buffer, O_CLOEXEC|O_NOCTTY|O_WRONLY));
													CL3_NONCLASS_SYSERR(write(fd, "both\n", 5) != 5);
													close(fd);

													//	open the FD for the pin
													snprintf(buffer, sizeof(buffer), "/sys/class/gpio/gpio%u/value", i);
													CL3_NONCLASS_SYSERR(pfds[i].fd = open(buffer, O_CLOEXEC|O_NOCTTY|O_RDONLY));

													//	read the current level of the pin (NOTE: this is racey :-/)
													pin->b_level_last = pin->Level();
												}
												else if(pin->mode != gpio::MODE_INPUT && pfds[i].fd != -1)
												{
													//	monitored pin was reconfigured to non-input

													//	close the FD for the pin
													close(pfds[i].fd);
													pfds[i].fd = -1;

													//	tell kernel to unexport the pin
													//	we wont check the error code, because either it works or... I would not know what to do else...
													char buffer[16];
													write(fd_unexport, buffer, snprintf(buffer, sizeof(buffer), "%u\n", i));
												}
											}
											break;
									}
							}
						}
					}
					catch(const error::TException& ex)
					{
						fprintf(stderr, "ERROR in bcm2835-Thread:\n\tmessage: %s\n\tfile: %s:%u\n\texpression: %s\n", ex.message, ex.codefile, ex.codeline, ex.expression);
					}
					catch(...)
					{
						fprintf(stderr, "ERROR in bcm2835-Thread: unknown error\n");
					}

					for(usys_t i = 0; i < N_GPIO_PINS; i++)
						if(CL3_UNLIKELY(pfds[i].fd != -1))
						{
							close(pfds[i].fd);
							//	tell kernel to unexport the pin
							//	we wont check the error code, because either it works or... I would not know what to do else...
							char buffer[16];
							write(fd_unexport, buffer, snprintf(buffer, sizeof(buffer), "%u\n", i));
						}

// 					fprintf(stderr, "GPIO-IRQ-THREAD: INFO: terminating ... \n");

					//	close gpio sysfs interface
					close(fd_export);
					close(fd_unexport);

					//	allow swapping again
					CL3_NONCLASS_SYSERR(munlockall());

					return NULL;
				}

				CLASS		TGPIO::TGPIO	(bool debug)
				{
					CL3_CLASS_ERROR(bcm2835_init() != 1, TException, "initialization of the bcm2835 lib failed");
					bcm2835_set_debug(debug ? 1 : 0);

					//	add all 54 GPIO pins available on the BCM2835 to the list of pins
					for(u32_t i = 0; i < 54; i++)
						this->pins.Append(new TPin(this, i));

					//	set the local th_irq and the member th_irq to zero
					pthread_t th_irq_local;
					pthread_t* th_irq_member = &this->th_irq;

					memset(&th_irq_local, 0, sizeof(pthread_t));
					memset(th_irq_member, 0, sizeof(pthread_t));

					//	create the threads and safe the thread ID in the local copy of th_irq
					pthread_create(&th_irq_local, NULL, &ThreadMain, this);

					//	wait until the member version  of th_irq matches the local one
					while(memcmp(&th_irq_local, th_irq_member, sizeof(pthread_t)) != 0)
						sched_yield();

					//	the callback thread is now running properly
				}

				CLASS		TGPIO::~TGPIO	()
				{
					pthread_kill(this->th_irq, SIGTERM);
					pthread_join(this->th_irq, NULL);

					CL3_CLASS_ERROR(bcm2835_close() != 1, TException, "shutdown of the bcm2835 lib failed");
				}

				const TList<gpio::IPin* const>&
							TGPIO::Pins		()
				{
					return this->pins;
				}

				u64_t		TGPIO::Tick		()
				{
					return bcm2835_st_read();
				}

				/*************************************************************************/

				CLASS	TSPIDevice::TBusLock::TBusLock	(TSPIDevice* device) : device(device)
				{
					//	TODO:
					//	device->bus->Mutex().Acquire();

					//	set baudrate and other SPI parameters
					bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
					bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);	//	TODO: make this a property of TSPIDevice
					bcm2835_spi_setClockDivider(this->device->divider);

					//	activate chip-select
					if(this->device->pin_cs)
						this->device->pin_cs->Level(false);	//	logical "0" means "ON", logical "1" means "OFF"; so this switches the device *ON*
				}

				CLASS	TSPIDevice::TBusLock::~TBusLock	()
				{
					//	deactivate chip-select
					if(this->device->pin_cs)
						this->device->pin_cs->Level(true);	//	logical "0" means "ON", logical "1" means "OFF"; so this switches the device *OFF*

					//	no need to reset the baudrate - the next device will set it to its prefered value

					//	TODO:
					//	bus->Mutex().Release();
				}




				CLASS		TSPIDevice::TSPIDevice	(TSPIBus* bus, gpio::IPin* pin_cs) : bus(bus), pin_cs(pin_cs), divider(65536)
				{
				}

				CLASS		TSPIDevice::~TSPIDevice	()
				{
					//	nothing to do yet
				}

				bus::spi::IBus*	TSPIDevice::Bus		() const
				{
					return this->bus;
				}

				u32_t		TSPIDevice::Baudrate	() const
				{
					return 250000000 / this->divider;
				}

				void		TSPIDevice::Baudrate	(u32_t new_baudrate)
				{
					CL3_CLASS_ERROR(new_baudrate < this->bus->BaudrateMin() || new_baudrate > this->bus->BaudrateMax(), TException, "unsupported baudrate requested");
					for(this->divider = 2; (250000000 / divider) > new_baudrate; this->divider *= 2);
				}

				usys_t		TSPIDevice::Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t)
				{
					memset(arr_items_read, 0, n_items_read_max);
					this->Transfer(arr_items_read, n_items_read_max);
					return n_items_read_max;
				}

				usys_t		TSPIDevice::Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t)
				{
					TBusLock lock(this);
					bcm2835_spi_writenb(const_cast<char*>(reinterpret_cast<const char*>(arr_items_write)), n_items_write_max);
					return n_items_write_max;
				}

				void		TSPIDevice::Transfer	(byte_t* buffer, usys_t sz_buffer)
				{
					TBusLock lock(this);
					bcm2835_spi_transfern(reinterpret_cast<char*>(buffer), sz_buffer);
				}

				/*************************************************************************/

				CLASS		TSPIBus::TSPIBus		(unsigned idx_bus, const collection::IStaticCollection<gpio::IPin*>& pins_chipselect) : idx_bus(idx_bus)
				{
					CL3_CLASS_ERROR_NOARGS(idx_bus != 0, TNotImplementedException);

					bcm2835_spi_begin();
					bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);	//	handle chip-select in cl3-/user-code to support more than two devices on the bus and to support crazy devices

					auto it = pins_chipselect.CreateStaticIterator();
					it->MoveHead();
					while(it->MoveNext())
					{
						gpio::IPin* const pin = it->Item();

						if(pin == NULL)
						{
							//	handle chip-select in user code
							this->devices.Append(new TSPIDevice(this, NULL));
						}
						else
						{
							//	handle chip-select in cl3 code
							pin->Mode(gpio::MODE_OUTPUT);
							pin->Pull(gpio::PULL_DISABLED);
							pin->Level(true);	//	logical "0" means "ON", logical "1" means "OFF"; so this switches the device *OFF*
							this->devices.Append(new TSPIDevice(this, pin));
						}
					}
				}

				CLASS		TSPIBus::~TSPIBus		()
				{
					for(usys_t i = 0; i < this->devices.Count(); i++)
						delete this->devices[i];

					bcm2835_spi_end();
				}

				u32_t		TSPIBus::BaudrateMin	() const
				{
					return 3814;
				}

				u32_t		TSPIBus::BaudrateMax	() const
				{
					return 125000000;
				}

				const TList<bus::spi::IDevice* const>&
							TSPIBus::Devices		()
				{
					return this->devices;
				}
			}
		}
	}
}

#endif
