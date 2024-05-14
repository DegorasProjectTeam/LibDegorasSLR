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
 * @file matrix.h
 * @brief This file contains the declarations of the mathematical class Matrix.
 * @author Degoras Project Team
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <string>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <utility>
#include <cmath>
#include <omp.h>
#include <string.h>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
#include "LibDegorasSLR/Mathematics/utils/math_utils.h"
#include "LibDegorasSLR/Helpers/type_traits.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
#include "LibDegorasSLR/libdegorasslr_global.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
namespace types{
// =====================================================================================================================

/**
 * @class Matrix
 * @brief A template class representing a mathematical matrix.
 * @tparam T The type of the elements in the matrix.
 */
template <typename T = long double>
class LIBDPSLR_EXPORT Matrix
{
public:

    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(Matrix)

    /**
     * @brief Construct a matrix from an initializer list.
     * @param list The initializer list containing the matrix elements.
     */
    Matrix(const std::initializer_list<std::initializer_list<T>>& list);

    /**
     * @brief Constructor with row and column size and all elements initializated to a specified value.
     * @param row_size The number of rows in the matrix.
     * @param col_size The number of columns in the matrix.
     * @param value The initial value for all elements (default is T()).
     */
    Matrix(std::size_t row_size, std::size_t col_size, T value = T());

    /**
     * @brief Removes all elements from the matrix, resulting in an empty matrix.
     */
    void clear();

    /**
     * @brief Sets the size of the matrix and fills all elements with the specified value.
     * @param row_size The number of rows in the matrix.
     * @param col_size The number of columns in the matrix.
     * @param value The value to fill the matrix with.
     */
    void fill(std::size_t row_size, std::size_t col_size, T value = T());

    /**
     * @brief Fill all the current matrix space with the specified value.
     * @param value The value to fill the matrix with.
     */
    void fill(T value = T());

    /**
     * @brief Fill the matrix with the contents of a container.
     * @param container The container ith the values to fill the matrix with.
     *        This must have a matrix form, i.e., container of containers, with a proper size.
     * @return True if matrix was filled successfully, false otherwise.
     */
    template<typename Container>
    bool setDataFromContainer(const Container& container);


    /**
     * @brief Sets the values of a specific column in the matrix.
     *
     * @param col_index The index of the column to set.
     * @param column The vector containing the values for the column.
     *
     * @pre `col_index` must be a valid column index (i.e., less than `columnsSize()`).
     * @pre The size of `column` must be equal to `rowSize()`.
     */
    void setColumn(std::size_t col_index, const std::vector<T>& column);

    /**
     * @brief Push back a new row in the matrix, increasing the row number by one. The row must have the same column
     *        size as the matrix.
     * @param row The row to push back to the matrix.
     * @return True if row was successfully pushed back, false otherwise.
     */
    template <typename Container>
    bool pushBackRow(const Container& row);

    /**
     * @brief Get current columns size of the matrix.
     * @return The columns size of the matrix.
     */
    std::size_t columnsSize() const;

    /**
     * @brief Get current rows size of the matrix.
     * @return The rows size of the matrix.
     */
    std::size_t rowSize() const;

    /**
     * @brief Checks if the matrix is square.
     * @return True if the matrix is square, false otherwise.
     */
    bool isSquare() const;

    /**
     * @brief Checks if the matrix is empty.
     * @return True if the matrix is empty, false otherwise.
     */
    bool isEmpty() const;
    /**
     * @brief Check if the matrix is an identity matrix.
     *
     * Check if the current matrix is an identity matrix, a square matrix with diagonal elements equal to 1 and all
     * other elements equal to 0.
     *
     * @return True if the matrix is an identity matrix, False otherwise.
     */
    std::enable_if_t<helpers::traits::is_numeric_v<T>, bool>
    isIdentity() const;

    /**
     * @brief Access operator that gives access to the row vector specified by index.
     * @param row_index The index of the row returned.
     * @return The row specified by index.
     * @warning This operator is unsafe. Index must be checked before using it.
     */
    std::vector<T>& operator[] (std::size_t row_index);

    /**
     * @brief Access operator that gives access to the row vector specified by index. Constant override.
     * @param row_index The index of the row returned.
     * @return The row specified by index.
     * @warning This operator is unsafe. Index must be checked before using it.
     */
    const std::vector<T>& operator[] (std::size_t row_index) const;

    /**
     * @brief Element access operator that gives access to the element specified by indexes. Constant override.
     * @param row_index The row index of the element.
     * @param col_index The column index of the element
     * @return The element specified by the indexes.
     * @warning This operator is unsafe. Indexes must be checked before using it.
     */
    const T& operator()(std::size_t row_index, std::size_t col_index) const;

    /**
     * @brief Element access operator that gives access to the element specified by indexes.
     * @param row_index The row index of the element.
     * @param col_index The column index of the element
     * @return The element specified by the indexes.
     * @warning This operator is unsafe. Indexes must be checked before using it.
     */
    T& operator()(std::size_t row_index, std::size_t col_index);

    // TODO: this versions should throw exception?
    /**
     * @brief Retrieves a specific row of the matrix.
     * @param row_index The index of the row to retrieve.
     * @return The vector representing the requested row.
     */
    const std::vector<T>& getRow(std::size_t row_index) const;

    /**
     * @brief Retrieves a specific column of the matrix.
     * @param col_index The index of the column to retrieve.
     * @return The vector representing the requested column.
     */
    std::vector<T> getColumn(std::size_t col_index) const;

    /**
     * @brief Set element value at the specified position.
     * @param row_index The row index of the element.
     * @param col_index The column index of the element.
     * @param value The value to set to the element.
     */
    void setElement(std::size_t row_index, std::size_t col_index, const T &value);

    /**
     * @brief Get the element value at the specified position.
     * @param row_index The row index of the element.
     * @param col_index The column index of the element.
     */
    const T& getElement(std::size_t row_index, std::size_t col_index) const;

    /**
     * @brief Convert matrix to string.
     * @return A string representing the matrix.
     */
    std::string toString() const;

    /**
     * @brief Swap two rows of the matrix.
     * @param r1 The row index of the first row.
     * @param r2 The row index of the second row.
     * @return True if swap was successfully done, false otherwise.
     * @note This function is safe. It will fail if indexes are not valid.
     */
    std::enable_if_t<std::is_swappable_v<T>, bool>
    swapRows(std::size_t r1, std::size_t r2);

    /**
     * @brief Swap two columns of the matrix.
     * @param r1 The column index of the first column.
     * @param r2 The column index of the second column.
     * @return True if swap was successfully done, false otherwise.
     * @note This function is safe. It will fail if indexes are not valid.
     */
    std::enable_if_t<std::is_swappable_v<T>, bool>
    swapColumns(std::size_t c1, std::size_t c2);

    /**
     * @brief Transposes the matrix.
     * @return Transposed matrix.
     */
    Matrix<T> transpose() const;

    /**
     * @brief Calculates the inverse of a square matrix using Cholesky decomposition.
     * @note The matrix must be square and positive definite for the inverse to exist.
     * @return The inverse matrix if it exists, otherwise an empty matrix.
     */
    Matrix<long double> inverse() const;

    std::pair<Matrix<long double>, std::vector<size_t>> decomposeLU() const;

    static Matrix<long double> solveLU(const Matrix<long double>& LU, const std::vector<size_t>& pivot,
                                       const std::vector<long double>& b);

    /**
     * @brief Performs a 3D Euclidean rotation on the matrix.
     *
     * This function generates a rotation matrix based on the provided axis and angle, then applies this rotation to
     * the current matrix. It is templated to work with floating point types directly or types defined as strong
     * floating point types via a helper structure. An exception is thrown if an invalid axis is provided.
     *
     * @param axis The axis of rotation. Must be greater than 1.
     * @param angle The angle of rotation in radians.
     *
     * @throws std::invalid_argument If the provided axis is < 1.
     *
     * @warning This rotation is only suitable for floating point types.
     */
    typename std::enable_if_t<helpers::traits::is_floating_v<T>, void>
    euclidian3DRotation(unsigned axis, const math::units::Radians& angle);

    /**
     * @brief Multiply by scalar operator.
     * @param scalar The scalar to multiply the matrix by.
     * @return The matrix multiplied by scalar.
     */
    template<typename U>
    typename std::enable_if_t<helpers::traits::is_numeric_v<U>, Matrix<T>>
    operator*(const U& scalar) const;

    /**
     * @brief Multiply this matrix by other matrix.
     * @param B The other matrix.
     * @return this * B, if possible. Empty matrix otherwise.
     */
    template<typename U>
    typename std::enable_if_t<helpers::traits::same_arithmetic_category_v<T,U>,Matrix<T>>
    operator*(const Matrix<U>& B) const;

    /**
     * @brief Multiply and assign operator.
     * @param B The other matrix to multiply by.
     * @return A reference to this.
     */
    template<typename U>
    Matrix<T>& operator *=(const Matrix<U>& B);

    /**
     * @brief Multiply and assign operator.
     * @param scalar The scalar to multiply by.
     * @return A reference to this.
     */
    template<typename U>
    Matrix<T>& operator *=(const U& scalar);

    /**
     * @brief Sum operator for matrices.
     * @param B The other matrix.
     * @return this + B if possible. An empty matrix otherwise.
     */
    template<typename U>
    Matrix<T> operator+(const Matrix<U>& B) const;

    /**
     * @brief Unary minus operator for matrix. Changes sign of elements.
     * @return -this.
     */
    Matrix<T> operator-() const;

    /**
     * @brief Substract operator for matrices.
     * @param B The other matrix.
     * @return this - B if possible. An empty matrix otherwise.
     */
    template<typename U>
    Matrix<T> operator-(const Matrix<U>& B) const;

    /**
     * @brief Sum and assign operator for matrices.
     * @param B The other matrix.
     * @return A reference to this.
     */
    template<typename U>
    Matrix<T>& operator+=(const Matrix<U>& other);

    /**
     * @brief Substract and assign operator for matrices.
     * @param B The other matrix.
     * @return A reference to this.
     */
    template<typename U>
    Matrix<T>& operator-=(const Matrix<U>& other);

    /**
     * @brief Create an identity matrix of order n.
     * @param n The order of the identity matrix.
     * @return The identity matrix of order n.
     */
    static Matrix<T> I(std::size_t n);

    /**
     * @brief Create a matrix from a column vector.
     * @param col The column vector.
     * @return A matrix created from a column vector.
     */
    static Matrix<T> fromColumnVector(const std::vector<T>& col);

    /**
     * @brief Create a matrix from a row vector.
     * @param row The row vector.
     * @return A matrix created from a row vector.
     */
    static Matrix<T> fromRowVector(const std::vector<T>& row);

private:

    std::vector<std::vector<T>> data_;
};

/**
 * @brief Equal operator for matrices.
 * @param A One matrix.
 * @param B The other matrix.
 * @return True if matrices are equal, false otherwise.
 */
template <typename T, typename U>
bool operator==(const Matrix<T>& A, const Matrix<U>& B);

/**
 * @brief Different operator for matrices.
 * @param A One matrix.
 * @param B The other matrix.
 * @return True if matrices are different, false otherwise.
 */
template <typename T, typename U>
bool operator!=(const Matrix<T>& A, const Matrix<U>& B);

}}} // END NAMESPACES
// =====================================================================================================================

// TEMPLATES INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Mathematics/types/matrix.tpp"
// =====================================================================================================================
