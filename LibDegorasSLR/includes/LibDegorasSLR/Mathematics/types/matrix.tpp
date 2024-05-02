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
 * @file matrix.tpp
 * @brief This file contains the implementation of the mathematical class Matrix.
 * @author Degoras Project Team
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <vector>
#include <cstddef>
#include <algorithm>
#include <omp.h>
#include <math.h>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
// #include "LibDegorasSLR/Mathematics/types/matrix.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
namespace types{
// =====================================================================================================================

template <typename T>
Matrix<T>::Matrix(const std::initializer_list<std::initializer_list<T>>& list)
{
    std::vector<std::vector<T>> data;
    data.reserve(list.size());
    for (const auto& row : list)
        data.emplace_back(row.begin(), row.end());
    this->setDataFromContainer(data);
}

template <typename T>
Matrix<T>::Matrix(std::size_t row_size, std::size_t col_size, T value) :
    data_(row_size, std::vector<T>(col_size, value)) {}


template <typename T>
void Matrix<T>::clear() {data_.clear();}


template <typename T>
void Matrix<T>::fill(std::size_t row_size, std::size_t col_size, T value)
{
    this->data_.clear();
    this->data_.insert(data_.begin(), row_size, std::vector<T>(col_size, value));
}

template <typename T>
void Matrix<T>::fill(T value)
{
    std::size_t r_size = this->rowSize();
    std::size_t c_size = this->columnsSize();

    this->data_.clear();
    this->data_.insert(data_.begin(), r_size, std::vector<T>(c_size, value));
}

template <typename T>
template <typename Container>
bool Matrix<T>::setDataFromContainer(const Container& container)
{
    bool result = false;

    if (container.size() > 0)
    {
        // Check if every column has the same size, otherwise, the matrix is ill-formed
        std::size_t col_size = container[0].size();
        bool valid_column_size = true;
        std::size_t i = 1;
        while(i < container.size() && valid_column_size)
        {
            valid_column_size = container[i].size() == col_size;
            i++;
        }
        if (valid_column_size)
        {
            this->data_.clear();
            this->data_.reserve(container.size());
            for (const auto& row : container)
                this->data_.emplace_back(row.begin(), row.end());
            result = true;
        }
    }
    return result;
}


template <typename T>
void Matrix<T>::setColumn(std::size_t col_index, const std::vector<T>& column)
{
    assert(col_index < this->columnsSize());
    assert(column.size() == this->rowSize());

    for (std::size_t i = 0; i < this->rowSize(); ++i)
        this->data_[i][col_index] = column[i];
}

template <typename T>
template <typename Container>
bool Matrix<T>::pushBackRow(const Container& row)
{
    bool size_correct = row.size() == this->columnsSize() || this->columnsSize() == 0;
    if (size_correct)
    {
        this->data_.push_back(std::vector<T>(row.size()));
        std::copy(row.begin(), row.end(), this->data_.back().begin());
    }
    return  size_correct;
}

template <typename T>
std::size_t Matrix<T>::columnsSize() const { return this->data_.size() == 0 ? 0 : this->data_[0].size(); }

template <typename T>
std::size_t Matrix<T>::rowSize() const { return this->data_.size(); }

template <typename T>
bool Matrix<T>::isSquare() const
{
    return this->rowSize() == this->columnsSize();
}

template <typename T>
bool Matrix<T>::isEmpty() const
{
    return this->data_.empty();
}

template <typename T>
typename std::enable_if_t<helpers::traits::is_numeric_v<T>, bool>
Matrix<T>::isIdentity() const
{
    // Check if the matrix is square
    if(!this->isSquare())
        return false;

    // Auxiliar result.
    bool identity = true;

    // Check if the diagonal elements are 1 and all other elements are 0
    for (size_t i = 0; i < this->rowSize() && identity; i++)
        for (size_t j = 0; j < this->columnsSize(); j++)
        {
            if (i == j && compareFloating(static_cast<double>(this->data_[i][j]), 1.0))
                identity = false;
            else if (i != j && compareFloating(static_cast<double>(this->data_[i][j]), 0.0))
                identity = false;
        }

    // Return the result.
    return identity;
}

template <typename T>
std::vector<T>& Matrix<T>::operator[] (std::size_t row_index)  {return this->data_[row_index];}

template <typename T>
const std::vector<T>& Matrix<T>::operator[] (std::size_t row_index) const {return this->data_[row_index];}

template <typename T>
const T& Matrix<T>::operator()(std::size_t row_index, std::size_t col_index) const
{
    return this->data_[row_index][col_index];
}

template <typename T>
T& Matrix<T>::operator()(std::size_t row_index, std::size_t col_index) {return this->data_[row_index][col_index];}

template <typename T>
const std::vector<T>& Matrix<T>::getRow(std::size_t row_index) const
{
    return this->data_[row_index];
}

template <typename T>
std::vector<T> Matrix<T>::getColumn(std::size_t col_index) const
{
    std::vector<T> column;
    for (const auto& row : this->data_)
        column.push_back(row[col_index]);
    return column;
}

template <typename T>
void Matrix<T>::setElement(std::size_t row_index, std::size_t col_index, const T& value)
{
    data_[row_index][col_index] = value;
}

template <typename T>
const T& Matrix<T>::getElement(std::size_t row_index, std::size_t col_index) const
{
    return this->data_[row_index][col_index];
}

template <typename T>
std::string Matrix<T>::toString() const
{
    std::string str;
    for (std::size_t j = 0; j < this->columnsSize(); j++)
    {
        for (std::size_t i = 0; i < this->rowSize(); i++)
        {
            str += std::to_string(this->data_[i][j]) + " ";
        }
        str += '\n';
    }
    return str;
}

template <typename T>
std::enable_if_t<std::is_swappable_v<T>, bool>
Matrix<T>::swapRows(std::size_t r1, std::size_t r2)
{
    bool rows_valid = r1 < this->rowSize() && r1 >= 0 && r2 < this->rowSize() && r2 >= 0;
    if (rows_valid)
        std::swap(this->data_[r1], this->data_[r2]);
    return rows_valid;
}

template <typename T>
std::enable_if_t<std::is_swappable_v<T>, bool>
Matrix<T>::swapColumns(std::size_t c1, std::size_t c2)
{
    bool cols_valid = c1 < this->columnsSize() && c1 >= 0 && c2 < this->columnsSize() && c2 >= 0;
    if (cols_valid)
        for (const auto& row : this->data_)
            std::swap(row[c1], row[c2]);
    return cols_valid;
}

template <typename T>
Matrix<T> Matrix<T>::transpose() const
{
    // Create a new matrix with swapped dimensions.
    Matrix<T> result(this->columnsSize(), this->rowSize(), 0);

    // Transpose elements in parallel using OpenMP.
    omp_set_num_threads(omp_get_max_threads());
    #pragma omp parallel
    {
        // Get data from omp.
        const size_t n_th = static_cast<size_t>(omp_get_num_threads());
        const size_t th_id = static_cast<size_t>(omp_get_thread_num());
        const size_t block_size = this->rowSize() / n_th;
        const size_t start_idx = th_id * block_size;
        const size_t end_idx = (th_id == n_th - 1) ? this->rowSize() : start_idx + block_size;

        // Private transpose.
        Matrix<T> priv_res(columnsSize(), rowSize(), 0);

        for (size_t i = start_idx; i < end_idx; i++)
            for (size_t j = 0; j < this->columnsSize(); j++)
                priv_res[j][i] = data_[i][j];

                // Update the result.
        #pragma omp critical
        {
            result += priv_res;
        }
    }

    // Return the transpose matrix.
    return result;
}

template <typename T>
template <typename U>
typename std::enable_if_t<helpers::traits::is_numeric_v<U>, Matrix<T>>
Matrix<T>::operator*(const U& scalar) const
{
    Matrix<T> result(rowSize(), columnsSize());

    #pragma omp parallel for
    for (size_t i = 0; i < rowSize(); i++)
    {
        for (size_t j = 0; j < columnsSize(); j++)
            result(i, j) = static_cast<T>(this->data_[i][j]) * static_cast<T>(scalar);
    }

    return result;
}

template <typename T>
template <typename U>
typename std::enable_if_t<helpers::traits::same_arithmetic_category_v<T,U>, Matrix<T>>
Matrix<T>::operator*(const Matrix<U>& B) const
{
    // Check dimensions.
    if (this->columnsSize() != B.rowSize())
        return Matrix<T>();

    // Transpose the rhs matrix for more efficient multiplication.
    Matrix<U> B_transposed = B.transpose();

    // Prepare the result container.
    Matrix<T> result(this->rowSize(), B.columnsSize());

    // Do the multiplication.
    #pragma omp parallel for
    for (std::size_t i = 0; i < this->rowSize(); ++i)
    {
        for (std::size_t j = 0; j < B_transposed.rowSize(); ++j)
        {
            long double sum = 0;
            #pragma omp parallel for reduction(+:sum)
            for (std::size_t k = 0; k < this->columnsSize(); ++k)
            {
                sum += static_cast<long double>(this->data_[i][k]) *
                       static_cast<long double>(B_transposed.getElement(j,k));
            }
            result(i, j) = static_cast<T>(sum);
        }
    }

    // Return the result matrix.
    return result;
}

template <typename T>
template <typename U>
Matrix<T>& Matrix<T>::operator *=(const Matrix<U>& B)
{
    return *this = *this * B;
    return *this;
}

template <typename T>
template <typename U>
Matrix<T>& Matrix<T>::operator *=(const U& scalar)
{
    *this = *this * scalar;
    return *this;
}

template <typename T>
std::pair<Matrix<long double>, std::vector<size_t>> Matrix<T>::decomposeLU() const
{
    size_t row_s = this->rowSize();
    size_t col_s = this->columnsSize();

    // Create a copy of the input matrix to preserve its values
    Matrix<long double> LU = *this;
    std::vector<size_t> pivot(col_s);

    // Initialize the pivot vector
    for (size_t i = 0; i < col_s; i++)
    {
        pivot[i] = i;
    }

    for (size_t k = 0; k < std::min(row_s, col_s); k++)
    {
        // Find the pivot element and swap rows
        size_t maxIndex = k;
        long double maxValue = std::abs(LU(k, k));

        for (size_t i = k + 1; i < row_s; i++)
        {
            if (std::abs(LU(i, k)) > maxValue)
            {
                maxIndex = i;
                maxValue = std::abs(LU(i, k));
            }
        }

        // WARNING TODO Comparacion flotante???
        if (0 == compareFloating(maxValue, 0.L))
        {
            // Matrix is singular
            return std::make_pair(LU, pivot);
        }

        if (maxIndex != k)
        {
            // Swap rows in LU matrix
            LU.swapRows(maxIndex, k);

            // Update the pivot vector
            std::swap(pivot[maxIndex], pivot[k]);
        }

        for (size_t i = k + 1; i < row_s; i++)
        {
            LU(i, k) /= LU(k, k);
            for (size_t j = k + 1; j < col_s; j++)
            {
                LU(i, j) -= LU(i, k) * LU(k, j);
            }
        }
    }

    return std::make_pair(LU, pivot);
}

template <typename T>
Matrix<long double> Matrix<T>::solveLU(
    const Matrix<long double>& LU, const std::vector<size_t>& pivot,const std::vector<long double>& b)
{
    size_t M = LU.rowSize();
    size_t N = LU.columnsSize();

    // Create a matrix from the vector b

    Matrix<long double> x(M, N);

    for (size_t col = 0; col < N; col++)
    {
        // Create a copy of the right-hand side vector for the current column
        Matrix<long double> y(M, 1);
        y.setColumn(0, b);

        // Apply row permutations to the right-hand side vector
        for (size_t i = 0; i < M; i++)
        {
            std::swap(y(i, 0), y(pivot[i], 0));
        }

        // Solve Ly = b using forward substitution
        for (size_t i = 1; i < M; i++)
        {
            for (size_t j = 0; j < i; j++)
            {
                y(i, 0) -= LU(i, j) * y(j, 0);
            }
        }

        // Solve Ux = y using backward substitution
        for (int i = M - 1; i >= 0; i--)
        {
            for (size_t j = static_cast<size_t>(i) + 1; j < N; j++)
            {
                y(static_cast<size_t>(i), 0) -= LU(static_cast<size_t>(i), j) * y(j, 0);
            }
            y(static_cast<size_t>(i), 0) /= LU( static_cast<size_t>(i), static_cast<size_t>(i));
        }

        // Assign the solution to the corresponding column of the result matrix
        x.setColumn(pivot[col], y.getColumn(0));
    }

    return x;
}

template <typename T>
Matrix<long double> Matrix<T>::inverse() const
{
    // TODO
    // Algo esta mal, retorna los datos bien pero las columnas no estan en el orden correcto.

    // Check if the matrix is square.
    if (!this->isSquare())
        return Matrix<long double>();

    size_t m = this->rowSize();
    Matrix<long double> identity = Matrix<long double>::I(m);

    // Perform LU decomposition
    Matrix<long double> lu_m = *this;
    std::vector<size_t> pivot;
    std::pair<Matrix<long double>, std::vector<size_t>> lu = lu_m.decomposeLU();

    // Solve for each column of the inverse
    Matrix<long double> inv(m, m);
    for (size_t col = 0; col < m; col++)
    {
        Matrix<long double> x = Matrix<long double>::solveLU(lu.first, lu.second, identity.getColumn(col));
        inv.setColumn(lu.second[col], x.getColumn(0));
    }

    return inv;
}

template <typename T>
template <typename U>
Matrix<T> Matrix<T>::operator+(const Matrix<U>& B) const
{
    // Check dimensions.
    if (this->rowSize() != B.rowSize() || this->columnsSize() != B.columnsSize())
        return Matrix<T>();

    // Create a new matrix for the result.
    Matrix<T> result(this->rowSize(), this->columnsSize(), 0);

    // Add corresponding elements.
    omp_set_num_threads(omp_get_max_threads());
    #pragma omp parallel for
    for (size_t i = 0; i < this->rowSize(); i++)
        for (size_t j = 0; j < this->columnsSize(); j++)
            result[i][j] = this->data_[i][j] + B.data_[i][j];

    // Return the result.
    return result;
}

template <typename T>
template <typename U>
Matrix<T> Matrix<T>::operator-(const Matrix<U>& B) const
{
    return this + (-B);
}

template <typename T>
Matrix<T> Matrix<T>::operator-() const
{
    return static_cast<T>(-1) * this;
}

template <typename T>
template <typename U>
Matrix<T>& Matrix<T>::operator+=(const Matrix<U>& other)
{
    *this = *this + other;
    return *this;
}

template <typename T>
template <typename U>
Matrix<T>& Matrix<T>::operator-=(const Matrix<U>& other)
{
    *this = *this - other;
    return *this;
}

template <typename T>
Matrix<T> Matrix<T>::I(std::size_t n)
{
    Matrix<T> ident(n,n,0);
    omp_set_num_threads(omp_get_max_threads());
#pragma omp parallel for
    for (size_t i = 0; i < n; i++)
    {
        ident[i][i] = 1;
    }
    return ident;
}

template <typename T>
Matrix<T> Matrix<T>::fromColumnVector(const std::vector<T>& col)
{
    Matrix<T> res;
    std::transform(col.begin(), col.end(),
                   std::back_inserter(res.data_), [](const auto& e){return std::vector<T>{e};});
    return res;
}

template <typename T>
Matrix<T> Matrix<T>::fromRowVector(const std::vector<T>& row)
{
    Matrix<T> res;
    res.data_.push_back(row);
    return res;
}

template <typename T>
std::enable_if_t<helpers::traits::is_floating_v<T>, void>
Matrix<T>::euclidian3DRotation(unsigned axis, const math::units::Radians& angle)
{
    // Checks.
    if (axis < 1)
    {
        std::string submodule("[LibDegorasSLR,Mathematics,Matrix]");
        std::string error("Invalid axis for 3D rotation, axis must be > 1.");
        throw std::invalid_argument(submodule + " " + error);
    }
    // Generate the rotation matrix.
    Matrix<T> rotation;
    T s, c;
    unsigned caxis = axis - 1;
    rotation.fill(3,3,0);
    s = std::sin(angle);
    c = std::cos(angle);
    rotation[0][0]=c;
    rotation[1][1]=c;
    rotation[2][2]=c;
    rotation[0][1]=-s;
    rotation[1][2]=-s;
    rotation[2][0]=-s;
    rotation[1][0]=s;
    rotation[2][1]=s;
    rotation[0][2]=s;
    for (unsigned i=0; i<3; i++)
    {
        rotation[i][caxis] = static_cast<T>(0);
        rotation[caxis][i] = static_cast<T>(0);
    }
    rotation[caxis][caxis]= static_cast<T>(1);
    // Do the rotation.
    *this *= rotation;
}

template <typename T, typename U>
bool operator==(const Matrix<T>& A, const Matrix<U>& B)
{
    // Check if the matrices have the same dimensions
    if (A.rowSize() != B.rowSize() || A.columnsSize() != B.columnsSize())
        return false;

    // Check if both are empty.
    if(A.isEmpty() && B.isEmpty())
        return true;

    bool res = true;

    // Check element-wise equality
    for (size_t i = 0; i < A.rowSize() && res; ++i)
        for (size_t j = 0; j < A.columnsSize() && res; ++j)
            if(math::compareFloating(A(i, j), B(i, j)) != 0)
                res = false;

    return res;
}

template <typename T, typename U>
bool operator != (const Matrix<T>& A, const Matrix<U>& B)
{
    return !(A == B);
}

}}} // END NAMESPACES
// =====================================================================================================================
