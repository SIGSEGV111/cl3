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

#include <cl3/core/system_time.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/gpio/io_phy.hpp>
#include <cl3/gpio/config.hpp>
#include <stdio.h>

#ifdef CL3_WITH_BCM2835
	#include <cl3/gpio/io_phy_bcm2835.hpp>
#endif

#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::io::phy;
	using namespace cl3::io::phy::gpio;
	using namespace cl3::io::collection::list;

	#ifdef CL3_WITH_BCM2835
		using namespace cl3::io::phy::bcm2835;

		TEST(io_phy_bcm2835_TGPIO, Loop_05_07)
		{
			TGPIO gpio;

			TPin& pin_in  = dynamic_cast<TPin&>(*gpio.Pins()[5]);
			TPin& pin_out = dynamic_cast<TPin&>(*gpio.Pins()[7]);

			pin_in.Mode(MODE_INPUT);
			pin_out.Mode(MODE_OUTPUT);

			pin_out.Level(false);
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());

			pin_out.Level(true);
			usleep(1000);
			EXPECT_TRUE(pin_in.Level());
			usleep(1000);
			EXPECT_TRUE(pin_in.Level());
			pin_out.Level(true);
			usleep(1000);
			EXPECT_TRUE(pin_in.Level());

			pin_out.Level(false);
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());
			pin_out.Level(false);
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());

			pin_out.Level(true);
			usleep(1000);
			EXPECT_TRUE(pin_in.Level());

			pin_out.Level(false);
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());
		}

		TEST(io_phy_bcm2835_TGPIO, Broken_Loop_P1_05_24)
		{
			TGPIO gpio;

			TPin& pin_in  = dynamic_cast<TPin&>(*gpio.Pins()[5]);
			TPin& pin_out = dynamic_cast<TPin&>(*gpio.Pins()[24]);

			pin_out.Level(false);
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());

			pin_out.Level(true);
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());

			pin_out.Level(false);
			usleep(1000);
			EXPECT_FALSE(pin_in.Level());
		}

		using namespace cl3::system::time;

		struct	TEdgeDetector : gpio::TOnEdgeEvent::IReceiver
		{
			volatile TOnEdgeData data;

			void	OnRaise	(gpio::TOnEdgeEvent& event, gpio::IPin& sender, gpio::TOnEdgeData data) final override
			{
				if(!data.b_level_prev && data.b_level_now)
				{
					//	rising edge
				}
				else if(data.b_level_prev && !data.b_level_now)
				{
					//	falling edge
				}
				else
					CL3_CLASS_LOGIC_ERROR(true);	//	WTF???

				this->data.b_level_prev = data.b_level_prev;
				this->data.b_level_now = data.b_level_now;
			}

			TEdgeDetector()
			{
				data.b_level_prev = false;
				data.b_level_now = false;
			}
		};

		TEST(io_phy_bcm2835_TGPIO, Loop_05_07_OnEdgeEvent)
		{
			TGPIO gpio;

			TPin& pin_in  = dynamic_cast<TPin&>(*gpio.Pins()[5]);
			TPin& pin_out = dynamic_cast<TPin&>(*gpio.Pins()[7]);

			pin_in.Mode(MODE_INPUT);
			pin_out.Mode(MODE_OUTPUT);

			TEdgeDetector detector;

			pin_in.OnEdge().Register(&detector);
			usleep(1000);

			for(unsigned i = 0; i < 128; i++)
			{
				const bool b_state = (i % 2) == 0;
				pin_out.Level(b_state);

				usleep(1000);

				if(b_state)
				{
					EXPECT_TRUE(pin_in.Level());
					EXPECT_FALSE(detector.data.b_level_prev);
					EXPECT_TRUE(detector.data.b_level_now);
				}
				else
				{
					EXPECT_FALSE(pin_in.Level());
					EXPECT_TRUE(detector.data.b_level_prev);
					EXPECT_FALSE(detector.data.b_level_now);
				}
			}

			pin_in.OnEdge().Unregister(&detector);
		}

		TEST(io_phy_bcm2835_TSPIBus, basics)
		{
			TGPIO gpio;
			TList<IPin*> pins;
			pins.Append(gpio.Pins()[8]);
			pins.Append(gpio.Pins()[7]);
			TSPIBus spibus(0, pins);
		}

	#endif
}
