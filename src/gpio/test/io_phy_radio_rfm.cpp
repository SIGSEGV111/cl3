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

// #define	TEST_RFM

#ifdef	TEST_RFM

#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_text_encoding.hpp>
#include <cl3/core/system_time.hpp>
#include <cl3/gpio/io_phy_bcm2835.hpp>
#include <cl3/gpio/io_phy_radio.hpp>
#include <cl3/gpio/io_phy_radio_rfm.hpp>
#include <stdio.h>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::io::collection::list;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::encoding;
	using namespace cl3::io::phy::bcm2835;
	using namespace cl3::io::phy::gpio;
	using namespace cl3::io::phy::radio;
	using namespace cl3::io::phy::radio::rfm;
	using namespace cl3::system::types;
	using namespace cl3::system::time;

	TEST(io_phy_radio_rfm_TRFM, Identify)
	{
		try
		{
			TGPIO gpio;
			TList<IPin*> pins;
			pins.Append(gpio.Pins()[8]);
			pins.Append(gpio.Pins()[7]);
			TSPIBus spibus(0, pins);

			TRFM rfm(spibus.Devices()[0], gpio.Pins()[24], gpio.Pins()[25]);

			rfm.Reset();

			TPartInfo part_info = rfm.Identify();

			fprintf(stderr, "INFO: chip reported model info:\n\tCHIPREV: %hhx\n\tPART: Si%hx\n\tPBUILD: %hhu\n\tID: %hu\n\tCUSTOMER: %hhu\n\tROMID: %hhu\n", part_info.chiprev, part_info.part, part_info.pbuild, part_info.id, part_info.customer, part_info.romid);
		}
		catch(const cl3::error::TException& ex)
		{
			fprintf(stderr, "ERROR:\n\tmessage: %s\n\tfile: %s:%u\n\texpression: %s\n", ex.message, ex.codefile, ex.codeline, ex.expression);
			throw "failed";
		}
	}
}

#endif