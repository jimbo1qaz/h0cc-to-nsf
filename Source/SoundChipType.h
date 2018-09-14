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

#include "APU/Types_fwd.h"
#include <string_view>
#include <memory>

class CSoundChip;
class CMixer;
class CChipHandler;
namespace ft0cc::doc {
enum class effect_type : std::uint8_t;
} // namespace ft0cc::doc

class CSoundChipType {
public:
	virtual sound_chip_t GetID() const = 0;
	virtual std::size_t GetSupportedChannelCount() const = 0;

	virtual std::string_view GetShortName() const = 0;
	virtual std::string_view GetFullName() const = 0;
	virtual std::string_view GetChannelShortName(std::size_t subindex) const = 0;
	virtual std::string_view GetChannelFullName(std::size_t subindex) const = 0;

#ifndef NSF_ONLY
	virtual std::unique_ptr<CSoundChip> MakeSoundDriver(CMixer &mixer, std::uint8_t nInstance) const = 0;
	virtual std::unique_ptr<CChipHandler> MakeChipHandler(std::uint8_t nInstance) const = 0;
#endif

	virtual ft0cc::doc::effect_type TranslateEffectName(char name, sound_chip_t chip) const = 0;
};
