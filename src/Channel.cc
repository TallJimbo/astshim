/* 
 * LSST Data Management System
 * Copyright 2016  AURA/LSST.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */

#include "astshim/base.h"
#include "astshim/FrameSet.h"
#include "astshim/Object.h"
#include "astshim/Stream.h"
#include "astshim/Channel.h"

namespace ast {

    Channel::Channel(Stream & stream, std::string const & options)
    :
        Channel(astChannel(detail::source, detail::sink, options.c_str()), stream)
    {}

    Channel::Channel(AstChannel * chan, Stream & stream)
    :
        Object(reinterpret_cast<AstObject *>(chan)),
        _stream(stream)
    {
        astPutChannelData(getRawPtr(), &_stream);
        assertOK();
    }

    Channel::Channel(AstChannel * chan)
    :
        Object(reinterpret_cast<AstObject *>(chan)),
        _stream()
    {
        assertOK();
    }

    Channel::~Channel() {
        if (_stream.hasStdStream()) {
            // avoid any attempt to read or write while the stream is being destroyed
            astPutChannelData(getRawPtr(), nullptr);
        }
    }

    Object Channel::read() {
        AstObject * rawret = reinterpret_cast<AstObject *>(astRead(getRawPtr()));
        if (!rawret) {
            throw std::runtime_error("Could not read an AST object from this channel");
        }
        return Object(rawret);
    }

    int Channel::write(Object const & obj) {
        int ret = astWrite(getRawPtr(), obj.getRawPtr());
        assertOK();
        return ret;
    }

}  // namespace ast
