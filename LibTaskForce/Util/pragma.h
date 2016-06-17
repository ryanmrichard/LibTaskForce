/*  
 *   LibTaskForce: An open-source library for task-based parallelism
 * 
 *   Copyright (C) 2016 Ryan M. Richard
 * 
 *   This file is part of LibTaskForce.
 *
 *   LibTaskForce is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   LibTaskForce is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with LibTaskForce.  If not, see <http://www.gnu.org/licenses/>.
 */ 

/** \file pragma.h
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 16, 2016
 */

#ifndef LIBTASKFORCE_GUARD_PRAGMA_H
#define LIBTASKFORCE_GUARD_PRAGMA_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__)||defined(__GNUG__)
    #define PRAGMA_WARNING_PUSH                                _Pragma("GCC diagnostic push")
    #define PRAGMA_WARNING_POP                                 _Pragma("GCC diagnostic pop")
    #define PRAGMA_WARNING_IGNORE_UNUSED_PARAMETERS            _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
    #define PRAGMA_WARNING_IGNORE_UNUSED_VARIABLES             _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")
    #define PRAGMA_WARNING_IGNORE_FP_EQUALITY                  _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")
    #define PRAGMA_WARNING_IGNORE_FP_CONVERT                   _Pragma("GCC diagnostic ignored \"-Wfloat-conversion\"")
    #define PRAGMA_WARNING_IGNORE_CONVERT                      _Pragma("GCC diagnostic ignored \"-Wconversion\"")
    #define PRAGMA_WARNING_IGNORE_SWITCH_MISSING_DEFAULT       _Pragma("GCC diagnostic ignored \"-Wswitch-default\"")
    #define PRAGMA_WARNING_IGNORE_POINTLESS_COMPARISON_UINT_0  _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")
    #define PRAGMA_WARNING_IGNORE_STATEMENT_UNREACHABLE        //! \todo Is this a warning in GCC?
    #define PRAGMA_WARNING_IGNORE_SHADOW                       _Pragma("GCC diagnostic ignored \"-Wshadow\"")
    #define PRAGMA_WARNING_IGNORE_SHADOW_MEMBER                //! \todo doesn't seem to warn in GCC, or may be a part of -Wshadow 
    #define PRAGMA_WARNING_IGNORE_EXTRA_SEMICOLON              _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
    #define PRAGMA_WARNING_IGNORE_REDECLARED_INLINE            // does not have warning for GCC
    #define PRAGMA_WARNING_IGNORE_UNUSED_LOCAL_TYPEDEFS        _Pragma("GCC diagnostic ignored \"-Wunused-local-typedefs\"")
    #define PRAGMA_WARNING_IGNORE_GCC_PRAGMA                   // uh... not a warning in gcc
    #define PRAGMA_WARNING_IGNORE_NONVIRTUAL_DTOR              // Doesn't seem to warn in GCC
    #define PRAGMA_WARNING_IGNORE_UNUSED_FUNCTION              _Pragma("GCC diagnostic ignored \"-Wunused-function\"")
    #define PRAGMA_WARNING_IGNORE_UNRECOGNIZED_PRAGMA          _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"")
#endif
    
#ifdef __cplusplus
}
#endif

#endif /* LIBTASKFORCE_GHUARD_PRAGMA_H */

