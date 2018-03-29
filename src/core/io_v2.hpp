#pragma once

namespace cl3
{
	namespace io_v2
	{
		template<typename T>
		struct IOutPassive
		{
			virtual bool EndOfOutputReached() const CL3_GETTER = 0;
			virtual IWaitable& OnOutputReady() const CL3_GETTER = 0;
			virtual usys_t Write(const T*, usys_t) = 0;
		};

		template<typename T>
		struct IInPassive
		{
			virtual bool EndOfInputReached() const CL3_GETTER = 0;
			virtual IWaitable& OnInputReady() const CL3_GETTER = 0;
			virtual usys_t Read(T*, usys_t) = 0;
		};

		template<typename T>
		struct IOutActive
		{
			virtual IWaitable& OnOutputReady() const CL3_GETTER = 0;
			virtual usys_t ReadIn(IInPassive<T>&, usys_t) CL3_WARN_UNUSED_RESULT = 0;
		};

		template<typename T>
		struct IInActive
		{
			virtual IWaitable& OnInputReady() const CL3_GETTER = 0;
			virtual usys_t WriteOut(IOutPassive<T>&, usys_t) CL3_WARN_UNUSED_RESULT = 0;
		};

		class TKernelStream : public IInPassive<byte_t>, public IOutPassive<byte_t>
		{
		};

		template<typename T>
		struct IBuffer
		{
		};

		template<typename T>
		class TFifoBuffer : public IInActive<T>, public IOutActive<T>
		{
		};

		template<typename T>
		class TIOP : private IFiber, public IOutPassive<T>
		{
			public:
				using item_t = typeinfo::features::pass_by<T>::type;

			private:
				TFifoBuffer<T> fifo;

				struct TEOF {};

				void Main() final override
				{
					try { this->IOPMain(); } catch(const TEOF&) {}
				}

			protected:
				inline item_t NextItem()
				{
					item_t item;

					while(CL3_UNLIKLY(fifo.Read(&item, 1) != 1))
						if(CL3_UNLIKLY(fifo.EndOfInputReached()))
							throw TEOF();
						else
							fifo.OnInputReady().WaitFor();

					return item;
				}

				virtual void IOPMain() = 0;
		};

		void ExampleParser()
		{
			// NextItem() will throw, if there are no more items

			for(;;)
			{
				while(NextItem() != 't');

				if(NextItem() != 'e')
					continue;
				if(NextItem() != 's')
					continue;
				if(NextItem() != 't')
					continue;

				TestFound();
			}
		}

		void ExampleUser1()
		{
			TKernelStream ks;
			TFifoBuffer fifo;
			while(!ks.EndOfInputReached())
			{
				ks.OnInputReady().WaitFor();
				fifo.ReadIn(ks);
			}
		}

		struct TUTF8Decoder : public TIOP
		{
			void IOPMain()
			{
				for(;;)
				{
					union
					{
						byte_t buffer[4];
						struct
						{
							byte_t lead;
							byte_t arr_follow[3];
						};
					};

					CL3_ERROR(!IsLeadByte(lead = NextItem()));

					if(CL3_LIKLY(IsASCII(lead)))
					{
						PutItem(lead);
						continue;
					}
					else
					{
						const unsigned n_follow = NumFollowBytes(lead);
						for(unsigned i = 0; i < n_follow; i++)
						{
							arr_follow[i] = NextItem();
							CL3_ERROR(!IsFollowByte(arr_follow[i]));
						}

						//...

						PutItem(...);
					}
				}
			}
		};

		template<typename T>
		struct ISource
		{
			ISink<T>* sink;
		};

		template<typename T>
		struct ISink
		{
			ISource<T>* source;
		};

		template<typename TIn, typename TOut>
		struct IProcessor : public IFiber, public ISink<TIn>, public ISource<TOut>
		{
		};

		void ExampleUser2()
		{
			TString s;

			TFile("test.txt") >> TUTF8Decoder() >> s;

			TString::ReadIn(decoder);
			decoder.Read() blocks because
			TUTF8Decoder
		}
	}
}
