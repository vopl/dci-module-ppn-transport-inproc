/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "pch.hpp"
#include "acceptor.hpp"
#include <dci/utils/net/url.hpp>

namespace dci::module::ppn::transport::inproc
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::Acceptor()
        : apit::inproc::Acceptor<>::Opposite(idl::interface::Initializer())
    {
        //in address() -> transport::Address;
        methods()->address() += sol() * [this]
        {
            return cmt::readyFuture(_address);
        };

        //in cost() -> real64;
        methods()->cost() += sol() * []
        {
            return cmt::readyFuture(real64{0});
        };

        //in rtt() -> real64;
        methods()->rtt() += sol() * []
        {
            return cmt::readyFuture(real64{0});
        };

        //in bandwidth() -> real64;
        methods()->bandwidth() += sol() * []
        {
            return cmt::readyFuture(std::numeric_limits<real64>::max());
        };

        //in bind(Address) -> void;
        methods()->bind() += sol() * [this](apit::Address&& address)
        {
            if(_started)
            {
                return cmt::readyFuture<void>(exception::buildInstance<api::AlreadyBound>("unable to bind after acceptor started"));
            }

            using namespace std::literals;
            if("inproc"sv != utils::net::url::scheme(address.value))
            {
                return cmt::readyFuture<void>(exception::buildInstance<api::BadAddress>());
            }

            _address = std::move(address);
            methods()->addressChanged(_address);
            return cmt::readyFuture<void>();
        };

        //in start();
        methods()->start() += sol() * [this]
        {
            if(_started)
            {
                //already started
                return;
            }
            _started = true;

            auto p = _registry.try_emplace(_address.value, this);

            if(!p.second)
            {
                methods()->failed(_address, _address, exception::buildInstance<api::AddressAlreadyInUse>());
                return;
            }

            methods()->started(_address, _address);
        };

        //in stop();
        methods()->stop() += sol() * [this]
        {
            if(_started)
            {
                _started = false;
                _registry.erase(_address.value);
                methods()->stopped(_address, _address);
            }
        };

        //out accepted(Channel);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::~Acceptor()
    {
        sol().flush();

        if(_started)
        {
            _registry.erase(_address.value);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor* Acceptor::findAcceptor(const String& address)
    {
        auto iter = _registry.find(address);
        if(_registry.end() == iter)
        {
            return nullptr;
        }

        return iter->second;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::map<String, Acceptor*> Acceptor::_registry;
}
