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
#ifndef ASTSHIM_BASE_H
#define ASTSHIM_BASE_H

#include <limits>
#include <sstream>
#include <stdexcept>

#include "ndarray.h"

extern "C" {
  #include "ast.h"
}

namespace ast {

class notfound_error: public std::runtime_error {
public:
    notfound_error(std::string const & msg) : std::runtime_error(msg) {};
};

typedef ndarray::Array<double, 2, 2> Array2D;

/**
Throw std::runtime_error if AST's state is bad

@param[in] clearStatus  if true, clear AST's status before throwing an exception
*/
inline void assertOK(bool clearStatus=true) {
    if (!astOK) {
        std::ostringstream os;
        os << "failed with AST status = " << astStatus;
        if (clearStatus) {
            astClearStatus;
        }
        throw std::runtime_error(os.str());
    }
}


/**
Control whether graphical escape sequences are included in strings.

The `Plot` class defines a set of escape sequences which can be
included within a text string in order to control the appearance of
sub-strings within the text. (See the Escape attribute for a
description of these escape sequences). It is usually inappropriate
for AST to return strings containing such escape sequences when
called by application code. For instance, an application which
displays the value of the @ref Frame_Title "Title" attribute
of a Frame usually does
not want the displayed string to include potentially long escape
sequences which a human read would have difficuly interpreting.
Therefore the default behaviour is for AST to strip out such escape
sequences when called by application code. This default behaviour
can be changed using this function.

@param[in] include  Possible values are:
                -  -1 (or any negative value) to return the current value without changing it.
                -   0 to not include escape sequences,
                -   1 (or any positive value) to include escape sequences,
@return the previous value (or current value if `include` is negative).

### Notes:

- This function also controls whether the AST function `astStripEscapes` removes escape sequences
    from the supplied string, or returns the supplied string without change.
- Unlike the AST function `astEscapes`, this function will not attempt to execute
    if an error has already occurred.
*/
inline bool escapes(int include=-1) {
    assertOK();
    int ret = astEscapes(include);
    assertOK();
    return ret;
}

}  // namespace ast

#endif