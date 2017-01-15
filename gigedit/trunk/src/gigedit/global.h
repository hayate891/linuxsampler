/*                                                         -*- c++ -*-
 * Copyright (C) 2007-2017 Andreas Persson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with program; see the file COPYING. If not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#ifndef GIGEDIT_GLOBAL_H
#define GIGEDIT_GLOBAL_H

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if !defined(WIN32)
# include <unistd.h>
# include <errno.h>
#endif

#include <sstream>
#include <map>

#ifdef LIBGIG_HEADER_FILE
# include LIBGIG_HEADER_FILE(gig.h)
#else
# include <gig.h>
#endif

//FIXME: for some reason AC GETTEXT check fails on the Mac cross compiler?
#if (HAVE_GETTEXT || defined(__APPLE__))
# include <libintl.h>
# define _(String) gettext(String)
#else
# define _(String) String
#endif

#if defined(WIN32) && !HAVE_CONFIG_H
# include "../../win32/libgigedit_private.h" // like config.h, automatically generated by Dev-C++
# define PACKAGE "gigedit"
# define VERSION VER_STRING // VER_STRING defined in libgig_private.h
#endif // WIN32

#define UNICODE_RIGHT_ARROW     Glib::ustring(1, gunichar(0x2192))
#define UNICODE_LEFT_ARROW      Glib::ustring(1, gunichar(0x2190))

template<class T> inline std::string ToString(T o) {
    std::stringstream ss;
    ss << o;
    return ss.str();
}

inline int getDimensionIndex(gig::dimension_t type, gig::Region* rgn) {
    for (uint i = 0; i < rgn->Dimensions; ++i)
        if (rgn->pDimensionDefinitions[i].dimension == type)
            return i;
    return -1;
}

inline int getDimensionRegionIndex(gig::DimensionRegion* dr) {
    if (!dr) return -1;
    gig::Region* rgn = (gig::Region*)dr->GetParent();
    for (uint i = 0; i < 256; ++i)
        if (rgn->pDimensionRegions[i] == dr)
            return i;
    return -1;
}

/// Find the number of bits required to hold the specified amount of zones.
inline int zoneCountToBits(int nZones) {
    if (!nZones) return 0;
    int iFinalBits = 0;
    int zoneBits = nZones - 1;
    for (; zoneBits > 1; iFinalBits += 2, zoneBits >>= 2);
    iFinalBits += zoneBits;
    return iFinalBits;
}

/**
 * Returns the sum of all bits of all dimensions defined before the given
 * dimensions (@a type). This allows to access cases of that particular
 * dimension directly. If the supplied dimension @a type does not exist in the
 * the supplied @a region, then this function returns -1 instead!
 *
 * @param type - dimension that shall be used
 * @param rgn - parent region of that dimension
 */
inline int baseBits(gig::dimension_t type, gig::Region* rgn) {
    int previousBits = 0;
    for (uint i = 0; i < rgn->Dimensions; ++i) {
        if (rgn->pDimensionDefinitions[i].dimension == type) return previousBits;
        previousBits += rgn->pDimensionDefinitions[i].bits;
    }
    return -1;
}

// key: dimension type, value: dimension's zone index
class DimensionCase : public std::map<gig::dimension_t,int> {
public:
    bool isViolating(const DimensionCase& c) const {
        for (DimensionCase::const_iterator it = begin(); it != end(); ++it) {
            if (c.find(it->first) == c.end()) continue;
            if (c.find(it->first)->second != it->second) return true;
        }
        return false;
    }
};

inline DimensionCase dimensionCaseOf(gig::DimensionRegion* dr) {
    DimensionCase dimCase;
    int idr = getDimensionRegionIndex(dr);
    if (idr < 0) return dimCase;
    gig::Region* rgn = (gig::Region*)dr->GetParent();
    int bitpos = 0;
    for (int d = 0; d < rgn->Dimensions; ++d) {
        const gig::dimension_def_t& dimdef = rgn->pDimensionDefinitions[d];
        const int zone = (idr >> bitpos) & ((1 << dimdef.bits) - 1);
        dimCase[dimdef.dimension] = zone;
        bitpos += rgn->pDimensionDefinitions[d].bits;
    }
    return dimCase;
}

inline std::vector<gig::DimensionRegion*> dimensionRegionsMatching(const DimensionCase& dimCase, gig::Region* rgn) {
    std::vector<gig::DimensionRegion*> v;
    for (int idr = 0; idr < 256; ++idr) {
        if (!rgn->pDimensionRegions[idr]) continue;
        DimensionCase c = dimensionCaseOf(rgn->pDimensionRegions[idr]);
        if (!dimCase.isViolating(c)) v.push_back(rgn->pDimensionRegions[idr]);
    }
    return v;
}

#endif // GIGEDIT_GLOBAL_H
