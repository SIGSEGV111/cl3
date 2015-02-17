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

#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/system_time.hpp>
#include <cl3/gpio/io_phy_radio.hpp>

#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::io::collection::list;
	using namespace cl3::io::phy::radio;
	using namespace cl3::system::types;
	using namespace cl3::system::time;

	static void GenerateOOKPulseLengthPattern (TPulseTime* arr_pulses, const bool* arr_bits, usys_t n, TTime dt_low_false, TTime dt_low_true, TTime dt_high, float f_noise)
	{
		for(usys_t i = 0; i < n; i++)
		{
			//	encode data
			arr_pulses[i].dt_low = arr_bits[i] ? dt_low_true : dt_low_false;
			arr_pulses[i].dt_high = dt_high;

			//	add noise
			if(i % 3)
				arr_pulses[i].dt_low *= (1.00 + f_noise);
			else
				arr_pulses[i].dt_low *= (1.00 - f_noise);

			if(i % 5)
				arr_pulses[i].dt_high *= (1.00 + f_noise);
			else
				arr_pulses[i].dt_high *= (1.00 - f_noise);
		}
	}

	TEST(io_phy_radio_TOOKDecoder, decode_24bit_f350us_t950us_h150us_n20pc)
	{
		//	this is the 24bit pattern that is sent by a PT2262 chip when you press a button
		//	we will use 350 us for a "false" signal, and 950 us for a "true" signal, 150 us whitespace (high level), and add 25% noise
		//	this should still decode flawlessly

		const bool arr_bits[] = { 1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0 };
		TPulseTime arr_pulses[25];	//	1+24 (the first one is ignored, but should usually have a long dt_low time, as the line would have been idle for quite a while)
		TList<bool> ls_bits_out;

		GenerateOOKPulseLengthPattern(arr_pulses+1, arr_bits, 24, 0.000350, 0.000950, 0.000150, 0.20);

		TOOKDecoder ook_decoder;
		ook_decoder.Sink(&ls_bits_out);
		ook_decoder.Write(arr_pulses, 25);
		ook_decoder.Flush();

		EXPECT_EQ(24, ls_bits_out.Count());

		for(usys_t i = 0; i < CL3_MIN(24, ls_bits_out.Count()); i++)
			EXPECT_EQ(arr_bits[i], ls_bits_out[i]);
	}
}