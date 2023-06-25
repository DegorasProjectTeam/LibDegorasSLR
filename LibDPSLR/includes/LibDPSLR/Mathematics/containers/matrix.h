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
 * @file matrix.h
 * @brief This file contains the declarations of the mathematical class Matrix.
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <vector>
#include <cstddef>
#include <algorithm>
#include <omp.h>
#include <math.h>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/libdpslr_global.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
// =====================================================================================================================

/**
 * @class Matrix
 * @brief A template class representing a mathematical matrix.
 * @tparam T The type of the elements in the matrix.
 */
template <typename T>
class LIBDPSLR_EXPORT Matrix
{
public:

    /**
     * @brief Default constructor.
     */
    Matrix() = default;

    /**
     * @brief Copy constructor.
     * @param other The matrix to copy.
     */
    Matrix(const Matrix<T>& other) = default;

    /**
     * @brief Move constructor.
     * @param other The matrix to move.
     */
    Matrix(Matrix<T>&& other) = default;

    /**
     * @brief Construct a matrix from an initializer list.
     * @param list The initializer list containing the matrix elements.
     */
    Matrix(std::initializer_list<T>& list)
    {
        this->setDataFromContainer(list);
    }

    /**
     * @brief Constructor with row and column size and all elements initializated to a specified value.
     * @param row_size The number of rows in the matrix.
     * @param col_size The number of columns in the matrix.
     * @param value The initial value for all elements (default is T()).
     */
    Matrix(std::size_t row_size, std::size_t col_size, T value = T()) :
        data_(row_size, std::vector<T>(col_size, value)) {}

    /**
     * @brief Destructor.
     */
    ~Matrix() = default;

    /**
     * @brief Copy assignment operator.
     * @param other The matrix to assign.
     * @return The assigned matrix.
     */
    Matrix& operator=(const Matrix<T>& other) = default;

    /**
     * @brief Removes all elements from the matrix, resulting in an empty matrix.
     */
    inline void clear() {data_.clear();}

    /**
     * @brief Sets the size of the matrix and fills all elements with the specified value.
     * @param row_size The number of rows in the matrix.
     * @param col_size The number of columns in the matrix.
     * @param value The value to fill the matrix with (default is T()).
     */
    inline void fill(std::size_t row_size, std::size_t col_size, T value = T())
    {
        this->data_.clear();
        this->data_.insert(data_.begin(), row_size, std::vector<T>(col_size, value));
    }

    template<typename Container>
    bool setDataFromContainer(const Container& container)
    {
        if (container.size() > 0)
        {
            // Check if every column has the same size, otherwise, the matrix is ill-formed
            std::size_t col_size = container[0].size();
            bool valid_column_size = true;
            std::size_t i = 1;
            while(i < container.size() && valid_column_size)
            {
                valid_column_size = container[i].size() == col_size;
            }
            if (valid_column_size)
            {
                this->data_.clear();
                std::copy(container.begin(), container.end(), this->data_.begin());
                return true;
            }
        }
        return false;
    }

    template <typename Container>
    bool push_back_row(const Container& row)
    {
        bool size_correct = row.size() == this->columnsSize() || this->columnsSize() == 0;
        if (size_correct)
        {
            this->data_.push_back(std::vector<T>(row.size()));
            std::copy(row.begin(), row.end(), this->data_.back().begin());
        }
        return  size_correct;
    }

    inline std::size_t columnsSize() const { return this->data_.size() == 0 ? 0 : this->data_[0].size(); }

    inline std::size_t rowSize() const { return this->data_.size(); }

    /**
     * @brief Checks if the matrix is square.
     * @return True if the matrix is square, false otherwise.
     */
    bool isSquare() const
    {
        return this->rowSize() == this->columnsSize();
    }

    inline std::vector<T>& operator[] (std::size_t row_index)  {return this->data_[row_index];}

    inline const std::vector<T>& operator[] (std::size_t row_index) const {return this->data_[row_index];}

    /**
     * @brief Retrieves a specific row of the matrix.
     * @param row_index The index of the row to retrieve.
     * @return The vector representing the requested row.
     */
    const std::vector<T>& getRow(std::size_t row_index) const
    {
        return this->data_[row_index];
    }

    /**
     * @brief Retrieves a specific column of the matrix.
     * @param col_index The index of the column to retrieve.
     * @return The vector representing the requested column.
     */
    std::vector<T> getColumn(std::size_t col_index) const
    {
        std::vector<T> column;
        for (const auto& row : this->data_)
            column.push_back(row[col_index]);
        return column;
    }

    // TODO: ensure T is swappable.
    bool swapRows(std::size_t r1, std::size_t r2)
    {
        bool rows_valid = r1 < this->rowSize() && r1 >= 0 && r2 < this->rowSize() && r2 >= 0;
        if (rows_valid)
            std::swap(this->data_[r1], this->data_[r2]);
        return rows_valid;
    }

    // TODO: ensure T is swappable.
    bool swapColumns(std::size_t c1, std::size_t c2)
    {
        bool cols_valid = c1 < this->columnsSize() && c1 >= 0 && c2 < this->columnsSize() && c2 >= 0;
        if (cols_valid)
            for (const auto& row : this->data_)
                std::swap(row[c1], row[c2]);
        return cols_valid;
    }

    /**
     * @brief Transposes the matrix. If the matrix is not square, an empty matrix is returned.
     * @return Transposed matrix if the matrix is square, otherwise an empty matrix.
     */
    Matrix<T> transpose() const
    {
        // Check the matrix.
        if(!this->isSquare())
            return Matrix<T>();

        // Create a new matrix with swapped dimensions.
        Matrix<T> result(this->columnsSize(), this->rowSize(), 0);

        // Transpose elements in parallel using OpenMP.
        omp_set_num_threads(omp_get_max_threads());
        #pragma omp parallel
        {
            // Get data from omp.
            const size_t n_th = omp_get_num_threads();
            const size_t th_id = omp_get_thread_num();
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

    /**
     * @brief Calculates the inverse of a square matrix using Cholesky decomposition.
     * @note The matrix must be square and positive definite for the inverse to exist.
     * @return The inverse matrix if it exists, otherwise an empty matrix.
     */
    Matrix<T> inverse() const
    {
        // Check if the matrix is square.
        if (!this->isSquare())
            return Matrix<T>();

        size_t m = this->rowSize();
        Matrix<T> s(m, m, 0);
        Matrix<T> b(m, m, 0);
        Matrix<T> x(m, 1, 0);
        Matrix<T> reta(m, m, 0);

        // Perform Cholesky matrix inversion
        for (size_t i = 0; i < m; i++)
        {
            T sum_val = data_[i][i];
            if (i > 0)
            {
                for (size_t k = 0; k < i; k++)
                    sum_val -= b[k][i] * b[k][i];
            }

            if (sum_val <= 0.0)
                return Matrix<T>();

            b[i][i] = std::sqrt(sum_val);

            if (i != m - 1)
            {
                for (size_t j = i + 1; j < m; j++)
                {
                    sum_val = data_[j][i];
                    if (i > 0)
                    {
                        for (size_t k = 0; k < i; k++)
                            sum_val -= b[k][i] * b[k][j];
                    }
                    b[i][j] = sum_val / b[i][i];
                }
            }
        }

        for (size_t i = 0; i < m; i++)
        {
            x.fill(0.0);
            x[i][0] = 1.0;
            for (size_t j = 0; j < m; j++)
            {
                T sum_val = x[j][0];
                if (j > 0)
                {
                    for (size_t k = 0; k < j; k++)
                        sum_val -= b[k][j] * x[k][0];
                }
                x[j][0] = sum_val / b[j][j];
            }

            for (size_t j = 0; j < m; j++)
            {
                size_t m1 = m - 1 - j;
                T sum_val = x[m1][0];
                if (j > 0)
                {
                    for (size_t k = m1 + 1; k < m; k++)
                        sum_val -= b[m1][k] * x[k][0];
                }
                x[m1][0] = sum_val / b[m1][m1];
            }

            reta.setColumn(i, x.getColumn(0));
        }

        Matrix<T> s_transposed = s.transpose();
        reta = reta * (s * s_transposed);

        return reta;
    }

    template<typename U>
    Matrix<std::common_type_t<T, U>> operator+(const Matrix<U>& B) const
    {
        // Check dimensions.
        if (this->rowSize() != B.rowSize() || this->columnsSize() != B.columnsSize())
            return Matrix<std::common_type_t<T, U>>();

        // Create a new matrix for the result.
        Matrix<std::common_type_t<T, U>> result(this->rowSize(), this->columnsSize(), 0);

        // Add corresponding elements.
        omp_set_num_threads(omp_get_max_threads());
        #pragma omp parallel for
        for (size_t i = 0; i < this->rowSize(); i++)
            for (size_t j = 0; j < this->columnsSize(); j++)
                result[i][j] = this->data_[i][j] + B.data_[i][j];

        // Return the result.
        return result;
    }

    template<typename U>
    Matrix<T>& operator+=(const Matrix<U>& other)
    {
        *this = *this + other;
        return *this;
    }

    template<typename U>
    Matrix<std::common_type_t<T,U>> operator*(const U& scalar)
    {
        Matrix<std::common_type_t<T,U>> result;
        // TODO Parallelize
        for (const auto& row : data_)
            for (const auto& element : row)
                result = element * scalar;
        return result;
    }

    template<typename U>
    Matrix<T>& operator *=(const Matrix<U>& B)
    {
        *this = *this * B;
        return *this;
    }

    template<typename U>
    Matrix<T>& operator *=(const U& scalar)
    {
        *this = *this * scalar;
        return *this;
    }

    static Matrix<T> I(std::size_t n)
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

    static Matrix<T> fromColumnVector(const std::vector<T>& col)
    {
        Matrix<T> res;
        std::transform(col.begin(), col.end(),
                       std::back_inserter(res.data_), [](const auto& e){return std::vector<T>{e};});
        return res;
    }

    static Matrix<T> fromRowVector(const std::vector<T>& row)
    {
        Matrix<T> res;
        res.data_.push_back(row);
        return res;
    }

private:

    std::vector<std::vector<T>> data_;
};

/**
 * @brief Multiplies two matrices.
 *
 * The number of columns in matrix A must be equal to the number of rows in matrix B
 * for the multiplication to be valid. Otherwise, an empty matrix is returned.
 *
 * @param A The first matrix.
 * @param B The second matrix.
 * @return The resulting matrix after multiplication, or an empty matrix if the matrices cannot be multiplied.
 */
template<typename T, typename U>
Matrix<std::common_type_t<T,U>> operator *(const Matrix<T>& A, const Matrix<U>& B)
{
    // Check if matrices can be multiplied.
    if (A.columnsSize() != B.rowSize())
        return Matrix<std::common_type_t<T, U>>();

    // Result container.
    Matrix<std::common_type_t<T, U>> result(A.rowSize(), B.columnsSize(), 0);

    // Transpose matrix B.
    Matrix<U> B_transposed = B.transpose();

    // Parallel multiplication.
    omp_set_num_threads(omp_get_max_threads());
    #pragma omp parallel for
    for (size_t i = 0; i < A.rowSize(); i++)
        for (size_t j = 0; j < B_transposed.rowSize(); j++)
            for (size_t k = 0; k < A.columnsSize(); k++)
                result[i][j] += A[i][k] * B_transposed[j][k];

    // Return the matrix.
    return result;
}


}} // END NAMESPACES
// =====================================================================================================================

// TEMPLATES INCLUDES
// =====================================================================================================================
#include "LibDPSLR/Mathematics/containers/matrix.tpp"
// =====================================================================================================================
