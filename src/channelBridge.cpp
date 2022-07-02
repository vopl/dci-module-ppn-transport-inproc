/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "pch.hpp"
#include "channelBridge.hpp"

namespace dci::module::ppn::transport::inproc
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<apit::Channel<>, apit::Channel<>> ChannelBridge::allocate(const apit::Address& address)
    {
        ChannelBridge* cb = new ChannelBridge(address);

        cb->_s1.ch().involvedChanged() += cb->_s1 * [cb](bool v)
        {
            if(!v)
            {
                cb->_s1.stop();

                if(!(--cb->_useCounter))
                {
                    delete cb;
                }
            }
        };

        cb->_s2.ch().involvedChanged() += cb->_s2 * [cb](bool v)
        {
            if(!v)
            {
                cb->_s2.stop();

                if(!(--cb->_useCounter))
                {
                    delete cb;
                }
            }
        };

        return std::make_pair(cb->_s1.ch().opposite(), cb->_s2.ch().opposite());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChannelBridge::ChannelBridge(const apit::Address& address)
    {
        _s1.start(&_s2, address);
        _s2.start(&_s1, address);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChannelBridge::~ChannelBridge()
    {
        _s1.stop();
        _s2.stop();
    }
}
