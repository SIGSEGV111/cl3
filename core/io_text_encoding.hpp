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

#ifndef	_include_cl3_core_io_text_encoding_hpp_
#define	_include_cl3_core_io_text_encoding_hpp_

#include "io_text.hpp"
#include "event.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace	encoding
			{
				CL3PUBF extern const ICodec* const CODEC_CXX_CHAR;
				CL3PUBF extern const ICodec* const CODEC_CXX_WCHAR;

				enum	EDirection
				{
					DIRECTION_ENCODE,
					DIRECTION_DECODE
				};

				enum	EReason
				{
					REASON_INCOMPLETE,
					REASON_NOT_REPRESENTABLE,
					REASON_INVALID
				};

				enum	EErrorAction
				{
					ERRORACTION_CONTINUE,	//	ignore current input item and continue with next
					ERRORACTION_ABORT		//	throw TTranscodeException
				};

				struct	CL3PUBT	TTranscodeException : error::TException
				{
					const ICodec* codec;
					EDirection direction;
					EReason reason;
					usys_t n_input_items_processed;
					usys_t n_output_items_written;
					EErrorAction action;

					CL3PUBF	CLASS	TTranscodeException	(const ICodec* codec, EDirection direction, EReason reason, usys_t n_input_items_processed, usys_t n_output_items_written);
				};

				struct	CL3PUBT	ICodec
				{
					virtual	string::TString	Name	() const CL3_GETTER = 0;
					virtual	system::memory::TUniquePtr<IEncoder>	CreateEncoder	() const CL3_WARN_UNUSED_RESULT = 0;
					virtual	system::memory::TUniquePtr<IDecoder>	CreateDecoder	() const CL3_WARN_UNUSED_RESULT = 0;
				};

				class	CL3PUBT	IEncoder : public stream::ISource<byte_t>, public virtual stream::AOutPassive<TUTF32>
				{
					protected:
						event::TEvent<IEncoder,TTranscodeException> on_error;

					public:
						inline	const event::TEvent<IEncoder,TTranscodeException>&	OnError	() const { return on_error; }

						//	resets the internal state of the encoder
						virtual	void	Reset	() = 0;

						//	encodes the UTF32 characters into bytes, returns the number of characters left in the internal state buffer after processing
						//	virtual	usys_t	Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1) = 0;

						virtual	CLASS	~IEncoder	() {}
				};

				class	CL3PUBT	IDecoder : public stream::ISource<TUTF32>, public virtual stream::AOutPassive<byte_t>
				{
					protected:
						event::TEvent<IDecoder,TTranscodeException> on_error;

					public:
						inline	const event::TEvent<IDecoder,TTranscodeException>&	OnError	() const { return on_error; }

						//	resets the internal state of the decoder
						virtual	void	Reset	() = 0;

						//	decodes the bytes into UTF32 characters, returns the number of bytes left in the internal state buffer after processing
						//	virtual	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1) = 0;

						virtual	CLASS	~IDecoder	() {}
				};















				class	CL3PUBT	ADecoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<byte_t>
				{
					public:
						using stream::IOut<TUTF32>::Write;
						using stream::IOut<TUTF32>::ReadIn;

						CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(io::stream::IIn<byte_t>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						CL3PUBF	CLASS	ADecoder	(const ICodec* codec);
						CL3PUBF	virtual	~ADecoder	();
				};

				class	CL3PUBT	AEncoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<byte_t>
				{
					public:
						using stream::IOut<byte_t>::Write;
						using stream::IOut<byte_t>::ReadIn;

						CL3PUBF	usys_t	Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(io::stream::IIn<TUTF32>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						CL3PUBF	CLASS	AEncoder	(const ICodec* codec);
						CL3PUBF	virtual	~AEncoder	();
				};









				class	CL3PUBT	ACharDecoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<char>
				{
					public:
						using stream::IOut<TUTF32>::Write;
						using stream::IOut<TUTF32>::ReadIn;

						CL3PUBF	usys_t	Write	(const char* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(io::stream::IIn<char>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);
				};

				class	CL3PUBT	AWCharDecoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<wchar_t>
				{
					public:
						using stream::IOut<TUTF32>::Write;
						using stream::IOut<TUTF32>::ReadIn;

						CL3PUBF	usys_t	Write	(const wchar_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(io::stream::IIn<wchar_t>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);
				};

				class	CL3PUBT	ACharEncoder : public virtual stream::IIn<TUTF32>, public virtual stream::IIn<char>
				{
					public:
						using stream::IIn<TUTF32>::Read;
						using stream::IIn<TUTF32>::WriteOut;

						CL3PUBF	usys_t	Read	(char* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min = (usys_t)-1);
						CL3PUBF	uoff_t	WriteOut(io::stream::IOut<char>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min = (uoff_t)-1);
				};

				class	CL3PUBT	AWCharEncoder : public virtual stream::IIn<TUTF32>, public virtual stream::IIn<wchar_t>
				{
					public:
						using stream::IIn<TUTF32>::Read;
						using stream::IIn<TUTF32>::WriteOut;

						CL3PUBF	usys_t	Read	(wchar_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min = (usys_t)-1);
						CL3PUBF	uoff_t	WriteOut(io::stream::IOut<wchar_t>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min = (uoff_t)-1);
				};
			}
		}
	}
}

#endif
