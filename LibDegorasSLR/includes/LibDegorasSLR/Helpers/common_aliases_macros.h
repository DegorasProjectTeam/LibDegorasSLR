/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   A modern and efficient C++ base library for Satellite Laser Ranging (SLR) software and real-time hardware         *
 *   related developments. Developed as a free software under the context of Degoras Project for the Spanish Navy      *
 *   Observatory SLR station (SFEL) in San Fernando and, of course, for any other station that wants to use it!        *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                               *
 *                                                                                                                     *
 *   Licensed under the European Union Public License (EUPL), Version 1.2 or subsequent versions of the EUPL license   *
 *   as soon they will be approved by the European Commission (IDABC).                                                 *
 *                                                                                                                     *
 *   This project is free software: you can redistribute it and/or modify it under the terms of the EUPL license as    *
 *   published by the IDABC, either Version 1.2 or, at your option, any later version.                                 *
 *                                                                                                                     *
 *   This project is distributed in the hope that it will be useful. Unless required by applicable law or agreed to in *
 *   writing, it is distributed on an "AS IS" basis, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY KIND; without even the  *
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the EUPL license to check specific   *
 *   language governing permissions and limitations and more details.                                                  *
 *                                                                                                                     *
 *   You should use this project in compliance with the EUPL license. You should have received a copy of the license   *
 *   along with this project. If not, see the license at < https://eupl.eu/ >.                                         *
 **********************************************************************************************************************/

/** ********************************************************************************************************************
 * @file common_aliases_macros.h
 * @brief This file contains several generic aliases and macros for the library.
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include<vector>
#include<string>

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    #include <optional>
#else
    #include <experimental/optional>
#endif
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
// =====================================================================================================================

// Generic aliases
// ---------------------------------------------------------------------------------------------------------------------

/// Convenient optional (from std::experimental) type alias.
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    template <class T> using Optional = std::optional<T>;
#else
    template <class T> using Optional = std::experimental::optional<T>;
#endif

/// Alias for a std::string vector.
using StringV = std::vector<std::string>;

// ---------------------------------------------------------------------------------------------------------------------

// Generic macros
// ---------------------------------------------------------------------------------------------------------------------

/// Macro for generate copy and move constructors.
#define M_DEFINE_CTOR_COPY_MOVE(CLASS) \
/** @brief Default copy constructor. */ \
CLASS(const CLASS&) = default; \
/** @brief Default move constructor. */ \
CLASS(CLASS&&) = default;

/// Macro for generate default copy and move operators.
#define M_DEFINE_OP_COPY_MOVE(CLASS) \
/** \
 * @brief Default copy assingment operator. \
 * @return Reference to itself. \
 */ \
CLASS& operator=(const CLASS&) = default; \
/** \
 * @brief Default move assingment operator. \
 * @return Reference to itself. \
 */ \
CLASS& operator=(CLASS&&) = default;

/// Macro for generate default, copy and move constructors.
#define M_DEFINE_CTOR_DEF_COPY_MOVE(CLASS) \
/** @brief Default constructor. */ \
CLASS() = default; \
M_DEFINE_CTOR_COPY_MOVE(CLASS)

/// Macro for generate copy and move constructors and operators.
#define M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(CLASS) \
M_DEFINE_CTOR_COPY_MOVE(CLASS) \
M_DEFINE_OP_COPY_MOVE(CLASS)

/// Macro for generator default, copy and move constructors and copy and move operators.
#define M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(CLASS) \
/** @brief Default constructor. */ \
CLASS() = default; \
M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(CLASS)

/// Macro for generator default, copy and move constructors and copy and move operators, and default destructor.
#define M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(CLASS) \
M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(CLASS) \
/** @brief Default destructor. */ \
~CLASS() = default; \

/// Macro for generator copy and move constructors; copy and move operators, and default destructor.
#define M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(CLASS) \
M_DEFINE_CTOR_COPY_MOVE(CLASS) \
M_DEFINE_OP_COPY_MOVE(CLASS) \
/** @brief Default destructor. */ \
~CLASS() = default; \

// =====================================================================================================================

} // END NAMESPACES
// =====================================================================================================================
