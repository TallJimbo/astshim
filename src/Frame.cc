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
#include <stdexcept>

#include "astshim/CmpFrame.h"
#include "astshim/Frame.h"
#include "astshim/FrameSet.h"

namespace ast {

    FrameSet Frame::convert(Frame const & to, std::string const & domainlist) {
        auto * rawframeset = reinterpret_cast<AstFrameSet *>(
            astConvert(getRawPtr(), to.getRawPtr(), domainlist.c_str())
        );
        assertOK();
        if (!rawframeset) {
            throw notfound_error("convert found no suitable frame set");
        }
        return FrameSet(rawframeset);
    }

    FrameSet Frame::findFrame(Frame & tmplt, std::string const & domainlist) {
        auto * rawframeset = reinterpret_cast<AstFrameSet *>(
            astFindFrame(getRawPtr(), tmplt.getRawPtr(), domainlist.c_str())
        );
        assertOK();
        if (!rawframeset) {
            throw notfound_error("findFrame found no suitable frame set");
        }
        return FrameSet(rawframeset);
    }

    std::vector<double> Frame::intersect(std::vector<double> const & a1,
                                          std::vector<double> const & a2,
                                          std::vector<double> const & b1,
                                          std::vector<double> const & b2
                                          ) const {
        int const naxes = 2;
        detail::assertEqual(getNaxes(), "# axes", naxes, "");
        detail::assertEqual(a1.size(), "a1.size()", naxes, "");
        detail::assertEqual(a2.size(), "a2.size()", naxes, "");
        detail::assertEqual(b1.size(), "b1.size()", naxes, "");
        detail::assertEqual(b2.size(), "b2.size()", naxes, "");
        std::vector<double> ret(naxes);
        astIntersect(getRawPtr(), a1.data(), a2.data(), b1.data(), b2.data(), ret.data());
        assertOK();
        detail::astBadToNan(ret);
        return ret;
    }

    CmpFrame Frame::over(Frame const & first) const {
        return CmpFrame(first, *this);
    }

    FrameMapping Frame::pickAxes(std::vector<int> const & axes) const {
        AstMapping * rawmap;
        auto rawframe = reinterpret_cast<AstFrame *>(astPickAxes(
            getRawPtr(), axes.size(), axes.data(), &rawmap));
        assertOK();
        return FrameMapping(Frame(rawframe), Mapping(rawmap));
    }

    ResolvedPoint Frame::resolve(std::vector<double> const & point1,
                                 std::vector<double> const & point2,
                                 std::vector<double> const & point3) const {
        int const naxes = getNaxes();
        detail::assertEqual(point1.size(), "a1.size()", naxes, "");
        detail::assertEqual(point2.size(), "a2.size()", naxes, "");
        detail::assertEqual(point3.size(), "b1.size()", naxes, "");
        ResolvedPoint ret(naxes);
        astResolve(getRawPtr(), point1.data(), point2.data(), point3.data(),
                   ret.point.data(), &ret.d1, &ret.d2);
        assertOK();
        detail::astBadToNan(ret.point);
        return ret;
    }

}  // namespace ast