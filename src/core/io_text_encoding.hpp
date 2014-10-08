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
				CL3PUBF extern const ICodec* CODEC_CXX_CHAR;
				CL3PUBF extern const ICodec* CODEC_CXX_WCHAR;
				CL3PUBF extern const ICodec* const CODEC_UTF32;	//	fake codec (just passes thru all data)

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

				struct	CL3PUBT	TTranscodeException : virtual error::TException
				{
					const ICodec* codec;
					EDirection direction;
					EReason reason;
					usys_t n_input_items_processed;
					usys_t n_output_items_written;
					EErrorAction action;

					CL3PUBF	CLASS	TTranscodeException	(const ICodec* codec, EDirection direction, EReason reason, usys_t n_input_items_processed, usys_t n_output_items_written);
					CL3PUBF	CLASS	TTranscodeException	(TTranscodeException&&);
					virtual	CLASS	~TTranscodeException();
				};

				struct	CL3PUBT	ICodec
				{
					virtual	string::TString	Name	() const CL3_GETTER = 0;
					virtual	system::memory::TUniquePtr<IEncoder>	CreateEncoder	() const CL3_WARN_UNUSED_RESULT = 0;
					virtual	system::memory::TUniquePtr<IDecoder>	CreateDecoder	() const CL3_WARN_UNUSED_RESULT = 0;
				};

				struct	CL3PUBT	IXCoder
				{
					virtual	const ICodec*	Codec	() const CL3_GETTER = 0;
					virtual	void			Reset	() = 0;
					virtual	bool			IsDirty	() const CL3_GETTER = 0;
					virtual	CLASS			~IXCoder() {}
				};

				class	CL3PUBT	IEncoder : public stream::ISource<byte_t>, public virtual stream::IOut<TUTF32>, public virtual IXCoder
				{
					protected:
						event::TEvent<IEncoder,TTranscodeException&> on_error;

					public:
						inline	const event::TEvent<IEncoder,TTranscodeException&>&	OnError	() const { return on_error; }
						virtual	CLASS	~IEncoder	() {}
				};

				class	CL3PUBT	IDecoder : public stream::ISource<TUTF32>, public virtual stream::IOut<byte_t>, public virtual IXCoder
				{
					protected:
						event::TEvent<IDecoder,TTranscodeException&> on_error;

					public:
						inline	const event::TEvent<IDecoder,TTranscodeException&>&	OnError	() const { return on_error; }
						virtual	CLASS	~IDecoder	() {}
				};
















				class	CL3PUBT	ADecoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<byte_t>
				{
					protected:
						system::memory::TUniquePtr<IDecoder> decoder;

					public:
						using stream::IOut<TUTF32>::Write;

						CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override;

						CL3PUBF	CLASS	ADecoder	(const ICodec* codec);
						CL3PUBF	virtual	~ADecoder	();
				};

				class	CL3PUBT	AEncoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<byte_t>
				{
					protected:
						system::memory::TUniquePtr<IEncoder> encoder;

					public:
						using stream::IOut<byte_t>::Write;

						CL3PUBF	usys_t	Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override;

						CL3PUBF	CLASS	AEncoder	(const ICodec* codec);
						CL3PUBF	virtual	~AEncoder	();
				};









				class	CL3PUBT	ACharDecoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<char>
				{
					protected:
						system::memory::TUniquePtr<IDecoder> decoder;

					public:
						using stream::IOut<TUTF32>::Write;

						CL3PUBF	usys_t	Write	(const char* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override;
				};

				class	CL3PUBT	AWCharDecoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<wchar_t>
				{
					protected:
						system::memory::TUniquePtr<IDecoder> decoder;

					public:
						using stream::IOut<TUTF32>::Write;

						CL3PUBF	usys_t	Write	(const wchar_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override;
				};

				class	CL3PUBT	ACharEncoder : public virtual stream::IIn<TUTF32>, public virtual stream::IIn<char>
				{
					public:
						using stream::IIn<TUTF32>::Read;

						CL3PUBF	usys_t	Read	(char* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override;
				};

				class	CL3PUBT	AWCharEncoder : public virtual stream::IIn<TUTF32>, public virtual stream::IIn<wchar_t>
				{
					public:
						using stream::IIn<TUTF32>::Read;

						CL3PUBF	usys_t	Read	(wchar_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override;
				};
			}
		}
	}
}

#endif
