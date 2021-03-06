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

#include <complex>

#include "astshim/base.h"
#include "astshim/detail.h"
#include "astshim/Object.h"
#include "astshim/Stream.h"
#include "astshim/FitsChan.h"

namespace ast {

    FitsChan::FitsChan(Stream & stream, std::string const & options)
    :
        Channel(
            reinterpret_cast<AstChannel *>(astFitsChan(detail::source, detail::sink, options.c_str())),
            stream)
    {
        _stream.setIsFits(true);
    }

    FitsChan::~FitsChan() {
        // when an astFitsChan is destroyed it first writes out any cards, but if I let astFitsChan
        // do this automatically then it occurs while the Channel and its Source are being destroyed,
        // which is too late
        astWriteFits(getRawPtr());
    }

    FoundValue<std::complex<double>> FitsChan::getFitsCF(
        std::string const & name,
        std::complex<double> defval
    ) {
        std::complex<double> val = defval;
        // this use of reinterpret_cast is explicitly permitted, for C compatibility
        double * rawval = reinterpret_cast<double(&)[2]>(val);
        bool found = astGetFitsCF(getRawPtr(), name.c_str(), rawval);
        assertOK();
        return FoundValue<std::complex<double>>(found, val);
    }

    FoundValue<std::string> FitsChan::getFitsCN(
            std::string const & name,
            std::string defval
    ) {
        char * rawval;  // astGetFitsCN has its own static buffer for the value
        bool found = astGetFitsCN(getRawPtr(), name.c_str(), &rawval);
        assertOK();
        std::string val = found ? rawval : defval;
        return FoundValue<std::string>(found, val);
    }

    FoundValue<double> FitsChan::getFitsF(
        std::string const & name,
        double defval
    ) {
        double val = defval;
        bool found = astGetFitsF(getRawPtr(), name.c_str(), &val);
        assertOK();
        return FoundValue<double>(found, val);
    }

    FoundValue<int> FitsChan::getFitsI(
        std::string const & name,
        int defval
    ) {
        int val = defval;
        bool found = astGetFitsI(getRawPtr(), name.c_str(), &val);
        assertOK();
        return FoundValue<int>(found, val);
    }

    FoundValue<bool> FitsChan::getFitsL(
        std::string const & name,
        bool defval
    ) {
        int val = static_cast<int>(defval);
        bool found = astGetFitsL(getRawPtr(), name.c_str(), &val);
        assertOK();
        return FoundValue<bool>(found, static_cast<bool>(val));
    }

    FoundValue<std::string> FitsChan::getFitsS(
        std::string const & name,
        std::string defval
    ) {
        char * rawval;  // astGetFitsCN has its own static buffer for the value
        bool found = astGetFitsS(getRawPtr(), name.c_str(), &rawval);
        assertOK();
        std::string val = found ? rawval : defval;
        return FoundValue<std::string>(found, val);
    }

    FoundValue<std::string> FitsChan::findFits(std::string const & name, bool inc) {
        char fitsbuf[detail::FITSLEN + 1];  // leave room for a terminating \0
        fitsbuf[0] = '\0';  // in case nothing is found
        bool success = static_cast<bool>(astFindFits(getRawPtr(), name.c_str(), fitsbuf, inc));
        assertOK();
        return FoundValue<std::string>(success, std::string(fitsbuf));
    }

    /**
    Determine if a named keyword is present, and if so, whether it has a value.

    ### Notes

    - This function does not change the current card.
    - The card following the current card is checked first. If this is not the required card,
      then the rest of the FitsChan is searched, starting with the first card added to the FitsChan.
      Therefore cards should be accessed in the order they are stored in the FitsChan (if possible)
      as this will minimise the time spent searching for cards.
    - An error will be reported if the keyword name does not conform to FITS requirements.
    */
    FitsKeyState FitsChan::testFits(std::string const & name) const {
        int there;
        int hasvalue = astTestFits(getRawPtr(), name.c_str(), &there);
        assertOK();
        if (hasvalue) {
            return FitsKeyState::PRESENT;
        }
        return there ? FitsKeyState::NOVALUE : FitsKeyState::ABSENT;
    }

}  // namespace ast
