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
 * @file vector3d.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of the class Vector3D, which abstracts the concept of 3D vector.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <cmath>
#include <array>
#include <ostream>
#include <vector>
#include <sstream>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Helpers/string_helpers.h>
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
// =====================================================================================================================

/**
 * @brief Generic 3D vector class.
 * @tparam T The type of the vector components.
 */
template <typename T>
class Vector3D
{
public:

    /**
     * @brief Default constructor. Initializes the vector to (0, 0, 0).
     */
    Vector3D() : data_{0, 0, 0} {}

    /**
     * @brief Constructs a 3D vector with the given components.
     * @param x The x-component.
     * @param y The y-component.
     * @param z The z-component.
     */
    Vector3D(T x, T y, T z) : data_{x, y, z} {}

    /**
     * @brief Copy constructor. Constructs a vector by copying another vector.
     * @param other The other vector to copy from.
     */
    Vector3D(const Vector3D&) = default;

    /**
     * @brief Move constructor. Constructs a vector by moving another vector.
     * @param other The other vector to move from.
     */
    Vector3D(Vector3D&&) = default;

    /**
     * @brief Assign operator.
     * @param other The other vector to copy from.
     */
    Vector3D& operator=(const Vector3D&) = default;

    /**
     * @brief Move operator.
     * @param The other vector to move from.
     */
    Vector3D& operator=(Vector3D&&) = default;

    T operator[] (std::size_t i)  {return this->data_[i];}

    const T& operator[] (std::size_t i) const {return this->data_[i];}

    /**
     * @brief Gets the x-component of the vector (as const reference).
     * @return The x-component.
     */
    const T& getX() const {return this->data_[0];}

    /**
     * @brief Gets the y-component of the vector (as const reference).
     * @return The y-component.
     */
    const T& getY() const {return this->data_[1];}

    /**
     * @brief Gets the z-component of the vector (as const reference).
     * @return The z-component.
     */
    const T& getZ() const {return this->data_[2];}

    /**
     * @brief Gets the x-component of the vector (as reference).
     * @return The x-component.
     */
    T& getX() {return this->data_[0];}

    /**
     * @brief Gets the y-component of the vector (as reference).
     * @return The y-component.
     */
    T& getY() {return this->data_[1];}

    /**
     * @brief Gets the z-component of the vector (as reference).
     * @return The z-component.
     */
    T& getZ() {return this->data_[2];}

    template<typename Container = std::array<long double, 3>>
    inline constexpr Container store() const {return Container{this->getX(), this->getY(), this->getZ()};}

    /**
     * @brief Sets the x-component of the vector.
     * @param x The new x-component value.
     */
    void setX(T x) {this->data_[0] = x;}

    /**
     * @brief Sets the y-component of the vector.
     * @param y The new y-component value.
     */
    void setY(T y) {this->data_[1] = y;}

    /**
     * @brief Sets the z-component of the vector.
     * @param z The new z-component value.
     */
    void setZ(T z) {this->data_[2] = z;}

    /**
     * @brief Converts the Vector3D to std::vector.
     * @return The vector representation of the Vector3D.
     */
    std::vector<T> toVector() const
    {
        return {getX(), getY(), getZ()};
    }

    /**
     * @brief Computes the length (magnitude) of the vector.
     * @return The length of the vector.
     */
    long double magnitude() const
    {
        return std::sqrt(this->getX() * this->getX() + this->getY() * this->getY() + this->getZ() * this->getZ());
    }

    /**
     * @brief Computes the normalized version of the vector.
     * @return The normalized vector.
     */
    Vector3D normalize() const
    {
        Vector3D res;
        long double len = this->magnitude();
        if (len != 0)
            res = Vector3D(getX() / len, getY() / len, getZ() / len);
        return res;
    }

    /**
     * @brief Computes the dot product between this vector and another vector.
     * @param v3d The other Vector3D.
     * @return The dot product value.
     */
    T dot(const Vector3D& v3d) const
    {
        return this->getX() * v3d.getX() + this->getY() * v3d.getY() + this->getZ() * v3d.getZ();
    }

    /**
     * @brief Computes the cross product between this vector and another vector.
     * @param v3d The other Vector3D.
     * @return The cross product vector.
     */
    Vector3D cross(const Vector3D& v3d) const
    {
        Vector3D res;
        res.setX(this->getY() * v3d.getZ() - this->getZ() * v3d.getY());
        res.setY(this->getZ() * v3d.getX() - this->getX() * v3d.getZ());
        res.setZ(this->getX() * v3d.getY() - this->getY() * v3d.getX());
        return res;
    }

    std::string toJson() const
    {
        // Result
        std::ostringstream oss;

        // Store the data.
        if(this->data_.empty())
            oss<<"{}";
        else
        {
            oss <<"{";
            oss << "\"x\":" << helpers::strings::numberToStr(this->getX(), 13, 3) << ",";
            oss << "\"y\":" << helpers::strings::numberToStr(this->getY(), 13, 3 )<< ",";
            oss << "\"z\":" << helpers::strings::numberToStr(this->getZ(), 13, 3);
            oss << "}";
        }

        // Return the JSON str.
        return oss.str();
    }

    /**
     * @brief Adds two vectors.
     * @param other The Vector3D to add.
     * @return The resulting vector.
     */
    Vector3D operator+(const Vector3D& v3d) const
    {
        return Vector3D(this->getX() + v3d.getX(), this->getY() + v3d.getY(), this->getZ() + v3d.getZ());
    }

    /**
     * @brief Subtracts another vector from this vector.
     * @param v3d The v3d to subtract.
     * @return The resulting vector.
     */
    Vector3D operator-(const Vector3D& v3d) const
    {
        return Vector3D(this->getX() - v3d.getX(), this->getY() - v3d.getY(), this->getZ() - v3d.getZ());
    }

    /**
     * @brief Multiplies the vector by a scalar value.
     *
     * @param scalar The scalar value.
     * @return The resulting vector.
     */
    Vector3D operator*(T scalar) const
    {
        return Vector3D(this->getX() * scalar, this->getY() * scalar, this->getZ() * scalar);
    }

    /**
     * @brief Divides the vector by a scalar value.
     *
     * @param scalar The scalar value.
     * @return The resulting vector.
     */
    Vector3D operator/(T scalar) const
    {
        Vector3D res;
        if (scalar != 0)
            res = {this->getX() / scalar, this->getY() / scalar, this->getZ() / scalar};
        else
            throw std::overflow_error("Divide by zero exception");
    }

    /**
     * @brief Adds another vector to this vector.
     * @param other The vector to add.
     * @return Reference to the modified vector.
     */
    Vector3D& operator+=(const Vector3D& v3d)
    {
        this->setX(this->getX() + v3d.getX());
        this->setY(this->getY() + v3d.getY());
        this->setZ(this->getZ() + v3d.getZ());
        return *this;
    }

    /**
     * @brief Subtracts another vector from this vector.
     * @param other The vector to subtract.
     * @return Reference to the modified vector.
     */
    Vector3D& operator-=(const Vector3D& v3d)
    {
        setX(getX() - v3d.getX());
        setY(getY() - v3d.getY());
        setZ(getZ() - v3d.getZ());
        return *this;
    }

    /**
     * @brief Multiplies the vector by a scalar value.
     * @param scalar The scalar value.
     * @return Reference to the modified vector.
     */
    Vector3D& operator*=(T scalar)
    {
        setX(this->getX() * scalar);
        setY(this->getY() * scalar);
        setZ(this->getZ() * scalar);
        return *this;
    }

    /**
     * @brief Divides the vector by a scalar value.
     * @param scalar The scalar value.
     * @return Reference to the modified vector.
     */
    Vector3D& operator/=(T scalar)
    {
        if (scalar != 0)
        {
            setX(this->getX() / scalar);
            setY(this->getY() / scalar);
            setZ(this->getZ() / scalar);
        }
        else
            throw std::overflow_error("Divide by zero exception");
        return *this;
    }

    /**
     * @brief Constructs a Vector3D using a std::vector of three positions.
     * @param positions The std::vector containing the x, y, and z positions.
     * @return The constructed Vector3D.
     */
    static Vector3D fromVector(const std::vector<T>& positions)
    {
        if (positions.size() != 3)
            return Vector3D();
        return Vector3D(positions[0], positions[1], positions[2]);
    }

    /**
     * @brief Stream insertion operator for printing the vector.
     */
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& vector)
    {
        os << "(" << vector.getX() << ", " << vector.getY() << ", " << vector.getZ() << ")";
        return os;
    }


private:
    std::array<T, 3> data_;
};


}} // END NAMESPACES.
// =====================================================================================================================
