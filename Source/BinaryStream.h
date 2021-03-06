/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
**
** 0CC-FamiTracker is (C) 2014-2018 HertzDevil
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.  To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/


#pragma once

#include "ft0cc/cpputil/array_view.hpp"
#include "ft0cc/cpputil/enum_traits.hpp"
#include <string>
#include <string_view>
#include <utility>

namespace details {

template <typename T, std::size_t N, std::size_t... Is>
static constexpr T
MakeIntegerLE(const std::byte (&buf)[N], std::index_sequence<Is...>) noexcept {
	return static_cast<T>((... | (std::to_integer<std::uintmax_t>(buf[Is]) << (Is * 8))));
}

} // namespace details

class CBinaryIOException : public std::runtime_error {
	using std::runtime_error::runtime_error;
};

class CBinaryReader {
public:
	virtual ~CBinaryReader() noexcept = default;

private:
	virtual void BeforeRead() { }
	virtual void AfterRead() { }

public:
	[[nodiscard]] virtual std::size_t ReadBytes(array_view<std::byte> buf) = 0;
	virtual void SeekReader(std::size_t pos) = 0;
	virtual std::size_t GetReaderPos() = 0;

	void ReadBuffer(array_view<std::byte> buf) {
		if (ReadBytes(buf) != buf.size())
			throw CBinaryIOException {"Unexpected EOF reached"};
	}

	template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
	T ReadInt() {
		return DoRead([&] { return ReadIntImpl<T>(); });
	}

	template <typename CharT>
	std::basic_string<CharT> ReadStringNull() {
		return DoRead([&] {
			std::basic_string<CharT> str;
			while (true)
				if (CharT ch = ReadIntImpl<CharT>())
					str += ch;
				else
					break;
			return str;
		});
	}

	template <typename CharT>
	std::basic_string<CharT> ReadString() {
		return DoRead([&] {
			auto sz = ReadIntImpl<std::uint32_t>();
			return ReadStringN<CharT>(sz);
		});
	}

	template <typename CharT>
	std::basic_string<CharT> ReadStringN(std::size_t count) {
		return DoRead([&] {
			std::basic_string<CharT> str;
			bool finished = false;
			for (std::size_t i = 0; i < count; ++i)
				if (CharT ch = ReadIntImpl<CharT>(); !finished && ch)
					str += ch;
				else
					finished = true;
			return str;
		});
	}

	/*
	template <typename T, std::enable_if_t<std::is_base_of<CBinarySerializableInterface, T>, int> = 0>
	std::unique_ptr<CBinarySerializableInterface> ReadObject() {
		auto x = std::make_unique<T>();

		if (
			return x;
		return nullptr;
	}
	*/
private:
	template <typename T>
	T ReadIntImpl() {
		std::byte buf[sizeof(T)] = { };
		std::size_t readcount = ReadBytes(buf);
		if (readcount != sizeof(T))
			throw CBinaryIOException {"Unexpected EOF reached"};
		return details::MakeIntegerLE<T>(buf, std::make_index_sequence<sizeof(T)> { });
	}

	template <typename F>
	auto DoRead(F f) {
		BeforeRead();
		auto&& x = f();
		AfterRead();
		return std::forward<decltype(x)>(x);
	}
};

class CBinaryWriter {
public:
	virtual ~CBinaryWriter() noexcept = default;

private:
	virtual void BeforeWrite() { }
	virtual void AfterWrite() { }

public:
	virtual std::size_t WriteBytes(array_view<const std::byte> buf) = 0;
	virtual void SeekWriter(std::size_t pos) = 0;
	virtual std::size_t GetWriterPos() = 0;

	void WriteBuffer(array_view<const std::byte> buf) {
		if (WriteBytes(buf) != buf.size())
			throw CBinaryIOException {"Unexpected EOF reached"};
	}

	template <typename T, typename U, std::enable_if_t<std::is_integral_v<T>, int> = 0, std::enable_if_t<std::is_integral_v<U>, int> = 0>
	void WriteInt(U x) {
		DoWrite([&] { WriteIntImpl<T>(x); });
	}

	template <typename CharT>
	void WriteStringNull(std::basic_string_view<CharT> sv) {
		DoWrite([&] {
			for (CharT ch : sv) {
				if (!ch)
					break;
				WriteIntImpl<CharT>(ch);
			}
			WriteIntImpl<CharT>(0);
		});
	}

	template <typename CharT>
	void WriteString(std::basic_string_view<CharT> sv) {
		DoWrite([&] {
			WriteIntImpl<std::uint32_t>(static_cast<std::uint32_t>(sv.size()));
			for (CharT ch : sv)
				WriteIntImpl<CharT>(ch);
		});
	}

	template <typename CharT>
	void WriteStringN(std::basic_string_view<CharT> sv, std::size_t count) {
		DoWrite([&] {
			sv = sv.substr(0u, count);
			for (CharT ch : sv)
				WriteIntImpl<CharT>(ch);
			for (std::size_t i = sv.size(); i < count; ++i)
				WriteIntImpl<CharT>(0);
		});
	}

private:
	template <typename T, typename U>
	void WriteIntImpl(U x) {
		if (WriteIntLEImpl<T>(static_cast<T>(x), std::make_index_sequence<sizeof(T)> { }) != sizeof(T))
			throw CBinaryIOException {"Unexpected EOF reached"};
	}

	template <typename T, std::size_t... Is>
	std::size_t WriteIntLEImpl(T x, std::index_sequence<Is...>) {
		const std::byte buf[sizeof(T)] = {static_cast<std::byte>(x >> (Is * 8))...};
		return WriteBytes(buf);
	}

	template <typename F>
	void DoWrite(F f) {
		BeforeWrite();
		f();
		AfterWrite();
	}
};
