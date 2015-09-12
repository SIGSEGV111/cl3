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

#include <cl3/core/system_task_synchronization.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::task::synchronization;

	TEST(system_task_synchonization_TMutex, basics)
	{
		TMutex mutex;
		EXPECT_TRUE(mutex.HasAcquired());
	}

	TEST(system_task_synchonization_TMutex, release_acquire)
	{
		TMutex mutex;

		EXPECT_TRUE(mutex.HasAcquired());
		mutex.Release();
		EXPECT_TRUE(!mutex.HasAcquired());
		mutex.Acquire();
	}

	TEST(system_task_synchonization_TMutex, recursive)
	{
		TMutex mutex;

		EXPECT_TRUE(mutex.HasAcquired());
		mutex.Acquire();
		EXPECT_TRUE(mutex.HasAcquired());
		mutex.Release();
		EXPECT_TRUE(mutex.HasAcquired());
		mutex.Release();
		EXPECT_TRUE(!mutex.HasAcquired());
		mutex.Acquire();
		EXPECT_TRUE(mutex.HasAcquired());
	}

	TEST(system_task_synchronization_TSignal, basics)
	{
		TMutex mutex;
		TSignal signal(&mutex);

		EXPECT_TRUE(signal.HasAcquired());
		EXPECT_FALSE(signal.WaitFor(0));
		EXPECT_FALSE(signal.WaitFor(0));
		EXPECT_FALSE(signal.WaitFor(0));
		EXPECT_TRUE(signal.HasAcquired());
		signal.Raise();
		EXPECT_TRUE(signal.HasAcquired());
		EXPECT_TRUE(signal.WaitFor(0));
		EXPECT_TRUE(signal.WaitFor(0));
		EXPECT_TRUE(signal.WaitFor(0));
		EXPECT_TRUE(signal.HasAcquired());
		signal.Reset();
		EXPECT_TRUE(signal.HasAcquired());
		EXPECT_FALSE(signal.WaitFor(0));
		EXPECT_FALSE(signal.WaitFor(0));
		EXPECT_FALSE(signal.WaitFor(0));
		EXPECT_TRUE(signal.HasAcquired());
	}
}