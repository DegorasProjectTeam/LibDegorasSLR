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
#include <ostream>
#include <string>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <string.h>
#include <omp.h>
#include <math.h>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/libdpslr_global.h"
#include "LibDPSLR/libdpslr_init.h"
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
    Matrix(const std::initializer_list<std::initializer_list<T>>& list)
    {
        std::vector<std::vector<T>> data;
        data.reserve(list.size());
        for (const auto& row : list)
            data.emplace_back(row.begin(), row.end());
        this->setDataFromContainer(data);
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

    inline void fill(T value = T())
    {
        std::size_t r_size = this->rowSize();
        std::size_t c_size = this->columnsSize();

        this->data_.clear();
        this->data_.insert(data_.begin(), r_size, std::vector<T>(c_size, value));
    }

    template<typename Container>
    bool setDataFromContainer(const Container& container)
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


    /**
     * @brief Sets the values of a specific column in the matrix.
     *
     * @param col_index The index of the column to set.
     * @param column The vector containing the values for the column.
     *
     * @pre `col_index` must be a valid column index (i.e., less than `columnsSize()`).
     * @pre The size of `column` must be equal to `rowSize()`.
     */
    void setColumn(std::size_t col_index, const std::vector<T>& column)
    {
        assert(col_index < this->columnsSize());
        assert(column.size() == this->rowSize());

        for (std::size_t i = 0; i < this->rowSize(); ++i)
            this->data_[i][col_index] = column[i];
    }

    template <typename Container>
    bool pushBackRow(const Container& row)
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

    bool isEmpty() const
    {
        return this->data_.empty();
    }

    /**
     * @brief Check if the matrix is an identity matrix.
     *
     * Check if the current matrix is an identity matrix, a square matrix with diagonal elements equal to 1 and all
     * other elements equal to 0.
     *
     * @return True if the matrix is an identity matrix, False otherwise.
     */
    bool isIdentity() const
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
                if (i == j && this->data_[i][j] != 1.0)
                        identity = false;
                else if (i != j && this->data_[i][j] != 0.0)
                        identity = false;
            }

        // Return the result.
        return identity;
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

    void setElement(std::size_t row_index, std::size_t col_index, int value)
    {
        data_[row_index][col_index] = value;
    }

    const T& getElement(std::size_t row_index, std::size_t col_index) const
    {
        return this->data_[row_index][col_index];
    }

    const T& operator()(std::size_t row_index, std::size_t col_index) const
    {
        return this->data_[row_index][col_index];
    }

    T& operator()(std::size_t row_index, std::size_t col_index)
    {
        return this->data_[row_index][col_index];
    }

    std::string toString() const
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
     * @brief Transposes the matrix.
     * @return Transposed matrix.
     */
    Matrix<T> transpose() const
    {
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

    template<typename U>
    Matrix<std::common_type_t<T,U>> operator*(const U& scalar) const
    {
        using ResultType = std::common_type_t<T, U>;
        Matrix<ResultType> result(rowSize(), columnsSize());

        #pragma omp parallel for
        for (size_t i = 0; i < rowSize(); i++)
        {
            for (size_t j = 0; j < columnsSize(); j++)
                result(i, j) = static_cast<ResultType>(this->data_[i][j]) * scalar;
        }

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

    Matrix<T> operator*(const Matrix<T>& B) const
    {
        // Check dimensions.
        if (this->columnsSize() != B.rowSize())
            return Matrix<T>();

        // Transpose the rhs matrix for more efficient multiplication.
        Matrix<T> B_transposed = B.transpose();

        Matrix<T> result(this->rowSize(), B.columnsSize());

        #pragma omp parallel for
        for (std::size_t i = 0; i < this->rowSize(); ++i)
        {
            for (std::size_t j = 0; j < B_transposed.rowSize(); ++j)
            {
                T sum = 0;
                #pragma omp parallel for reduction(+:sum)
                for (std::size_t k = 0; k < this->columnsSize(); ++k)
                {
                    sum += this->data_[i][k] * B_transposed.data_[j][k];
                }
                result(i, j) = sum;
            }
        }

        return result;
    }

    std::pair<Matrix<long double>, std::vector<size_t>> decomposeLU() const
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

            if (maxValue == 0)
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

    static Matrix<long double> solveLU(const Matrix<long double>& LU, const std::vector<size_t>& pivot, const std::vector<long double>& b)
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
                for (size_t j = i + 1; j < N; j++)
                {
                    y(i, 0) -= LU(i, j) * y(j, 0);
                }
                y(i, 0) /= LU(i, i);
            }

            // Assign the solution to the corresponding column of the result matrix
            x.setColumn(pivot[col], y.getColumn(0));
        }

        return x;
    }
    /**
     * @brief Calculates the inverse of a square matrix using Cholesky decomposition.
     * @note The matrix must be square and positive definite for the inverse to exist.
     * @return The inverse matrix if it exists, otherwise an empty matrix.
     */
    Matrix<long double> inverse() const
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
    // Return the multiplication.
    return A.operator*(B);
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
            if (std::fabs(A(i, j)-B(i, j) >= kFloatingCompEpsilon))
                res = false;

    return res;
}

}} // END NAMESPACES
// =====================================================================================================================

// TEMPLATES INCLUDES
// =====================================================================================================================
#include "LibDPSLR/Mathematics/containers/matrix.tpp"
// =====================================================================================================================
