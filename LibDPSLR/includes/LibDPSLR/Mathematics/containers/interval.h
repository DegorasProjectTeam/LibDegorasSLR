/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *                                      *
 *                                                                                                                     *
 *   Copyright (C) 2023 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDPSLR.                                                                                    *
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
 * @file interval.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of the class Interval, which abstracts the concept of interval.
 * @copyright EUPL License
 * @version 2305.1
 * @todo Add a "high precission interval" class if neccesary.
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <cmath>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/libdpslr_global.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
// =====================================================================================================================

/**
 * @brief Represents an interval with customizable inclusion or exclusion of its minimum and maximum values.
 * @tparam T The type of values stored in the interval. Default is 'double'.
 * @warning When comparing floating-point values, precision issues may arise due to the inherent limitations
 *          of floating-point representation. Take care when comparing floating-point values.
 */
template <class T = double>
class LIBDPSLR_EXPORT Interval
{
public:

    /**
     * @brief Default constructor. Creates an empty interval.
     */
    Interval() :
        min_(0.0),
        max_(-1.0),
        incl_min_(true),
        incl_max_(true)
    {}

    /**
     * @brief Constructs an interval with the specified minimum and maximum values.
     * @param min The minimum value of the interval.
     * @param max The maximum value of the interval.
     * @param incl_min True to include the minimum value, false to exclude it.
     * @param incl_max True to include the maximum value, false to exclude it.
     */
    Interval(T min, T max, bool incl_min = true, bool incl_max = true) :
        min_(min),
        max_(max),
        incl_min_(incl_min),
        incl_max_(incl_max)
    {}

    /**
     * @brief Constructs a symmetric interval assuming 0 as the center.
     *
     * @param sym The value representing the absolute maximum/center of the interval.
     * @param incl_min True to include the minimum value, false to exclude it.
     * @param incl_max True to include the maximum value, false to exclude it.
     */
    Interval(T sym, bool incl_min = true, bool incl_max = true) :
        incl_min_(incl_min),
        incl_max_(incl_max)
    {
        this->max_ = std::abs(sym);
        this->min_ = -this->max_;
    }

    // Copy and movement constructors and operators.
    Interval(const Interval&) = default;
    Interval& operator =(Interval&&) = default;
    Interval(Interval&&) = default;
    Interval& operator =(const Interval&) = default;

    // Getters.
    T min() const {return this->min_;}
    T max() const {return this->max_;}
    T center() const {return (this->min_ + (this->max_ - this->min_)/2.0);}
    bool includeMin() const {return incl_min_;}
    bool includeMax() const {return incl_max_;}
    bool endpointsIncluded() const {return this->incl_max_ && this->incl_min_;}

    // Basic seters.
    void setMin(T min){this->min_ = min;}
    void setMax(T max){this->max_ = max;}
    void setIncludeMin(bool flag = true){this->incl_min_ = flag;}
    void setIncludeMax(bool flag = true){this->incl_max_ = flag;}

    // Full interval setter.
    void setInterval(T min, T max, bool incl_min = true, bool incl_max = true)
    {
        this->min_ = min;
        this->max_ = max;
        this->incl_min_ = incl_min;
        this->incl_max_ = incl_max;
    }

    // Symetric interval setter assuming 0 as center.
    void setInterval(T sym, bool incl_min = true, bool incl_max = true)
    {
        this->max_ = std::abs(sym);
        this->min_ = -this->max_;
        this->incl_min_ = incl_min;
        this->incl_max_ = incl_max;
    }

    // Move center method.
    void moveCenter(T distance)
    {
        this->max_ += distance;
        this->min_ += distance;
    }

    // Intervals related methods.

    /**
     * @brief Checks if the interval is valid.
     *
     * An interval is considered valid if it has a non-empty width and the minimum and maximum values are
     * ordered correctly based on the inclusion/exclusion flags.
     *
     * @return True if the interval is valid, false otherwise.
     */
    bool isValid() const
    {
        return !this->endpointsIncluded() ? this->min_ <= this->max_ : this->min_ < this->max_;
    }

    /**
     * @brief Checks if the interval is null. A null interval has zero width.
     * @return True if the interval is null, false otherwise.
     */
    bool isNull() const
    {
        return this->isValid() && this->min_ == this->max_;
    }

    /**
     * @brief Calculates the width of the interval.
     * @return The width of the interval, or 0.0 if the interval is not valid.
     */
    T width() const {return isValid() ? (std::abs(this->max_ - this->min_)) : 0.0;}

    /**
     * @brief Checks if the interval contains the specified value.
     * @tparam D The type of the value to check.
     * @param value The value to check.
     * @return True if the interval contains the value, false otherwise.
     */
    template <class D>
    bool contains(D value) const
    {
        if (!isValid())
            return false;
        if ((value < this->min_) || (value > this->max_ ))
            return false;
        if ((value == this->min_) && !this->incl_min_)
            return false;
        if ((value == this->max_) && !this->incl_max_)
            return false;
        return true;
    }

    /**
     * @brief Calculates the distance between a value and the center of the interval.
     * @tparam D The type of the value.
     * @param value The value.
     * @return The distance between the value and the center of the interval.
     */
    template <class D>
    T toCenter(D value) const
    {
        return std::abs(value - this->center());
    }

    /**
     * @brief Calculates the distance between the maximum value and the center of the interval.
     * @return The distance between the maximum value and the center of the interval.
     */
    T toCenter() const
    {
        return std::abs(this->max_ - this->center());
    }

    /**
     * @brief Calculates the center of the interval.
     * @tparam D The type of the values.
     * @param min The minimum value of the interval.
     * @param max The maximum value of the interval.
     * @return The center of the interval.
     */
    template <class D = double>
    static D center(D min, D max)
    {
        return min + (max - min)/2.0;
    }

private:

    T min_;           /// The minimum value of the interval.
    T max_;           /// The maximum value of the interval.
    bool incl_min_;   /// True to include the minimum value, false to exclude it.
    bool incl_max_;   /// True to include the maximum value, false to exclude it.
};

}} // END NAMESPACES.
// =====================================================================================================================
