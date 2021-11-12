/* This file is part of the the dci project. Copyright (C) 2013-2021 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "pch.hpp"
#include "channelBridge/side.hpp"

namespace dci::module::ppn::transport::inproc
{
    class ChannelBridge
        : private mm::heap::Allocable<ChannelBridge>
    {
    public:
        static std::pair<apit::Channel<>, apit::Channel<>> allocate(const apit::Address& address);

    private:
        ChannelBridge(const apit::Address& address);
        ~ChannelBridge();

        uint32  _useCounter {2};

    private:
        channelBridge::Side _s1;
        channelBridge::Side _s2;
    };
}
