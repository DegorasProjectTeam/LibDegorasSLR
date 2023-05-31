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

template <typename T>
class LIBDPSLR_EXPORT Matrix
{
public:
    Matrix() = default;
    Matrix(const Matrix<T>&) = default;
    Matrix& operator=(const Matrix<T>&) = default;
    Matrix(Matrix<T>&&) = default;
    Matrix& operator=(Matrix<T>&&) = default;
    Matrix(std::initializer_list<T>& list)
    {
        setDataFromContainer(list);
    }
    Matrix(std::size_t row_size, std::size_t col_size, T value = T()) :
        data(row_size, std::vector<T>(col_size, value)) {}
    ~Matrix() = default;
    inline void clear() {data.clear();}
    inline void fill(std::size_t row_size, std::size_t col_size, T value = T())
    {
        data.clear();
        data.insert(data.begin(), row_size, std::vector<T>(col_size, value));
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
                data.clear();
                std::copy(container.begin(), container.end(), data.begin());
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
            data.push_back(std::vector<T>(row.size()));
            std::copy(row.begin(), row.end(), data.back().begin());
        }
        return  size_correct;
    }

    inline std::size_t columnsSize() const { return data.size() == 0 ? 0 : data[0].size(); }
    inline std::size_t rowSize() const { return data.size(); }
    inline std::vector<T>& operator[] (std::size_t row_index)  {return data[row_index];}
    inline const std::vector<T>& operator[] (std::size_t row_index) const {return data[row_index];}

    bool swapRows(std::size_t r1, std::size_t r2)
    {
        bool rows_valid = r1 < this->rowSize() && r1 >= 0 && r2 < this->rowSize() && r2 >= 0;
        if (rows_valid)
            std::swap(this->data[r1], this->data[r2]);
        return rows_valid;
    }

    // TODO: ensure T is swappable.
    bool swapColumns(std::size_t c1, std::size_t c2)
    {
        bool cols_valid = c1 < this->columnsSize() && c1 >= 0 && c2 < this->columnsSize() && c2 >= 0;
        if (cols_valid)
            for (const auto& row : this->data)
                std::swap(row[c1], row[c2]);
        return cols_valid;
    }

    template<typename U>
    Matrix<std::common_type_t<T,U>> operator*(const U& scalar)
    {
        Matrix<std::common_type_t<T,U>> result;
        for (const auto& row : data)
        {
            for (const auto& element : row)
            {
                result = element * scalar;
            }
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

    static Matrix<T> I(std::size_t n)
    {
        Matrix<T> ident(n,n,0);
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
                       std::back_inserter(res.data), [](const auto& e){return std::vector<T>{e};});
        return res;
    }

    static Matrix<T> fromRowVector(const std::vector<T>& row)
    {
        Matrix<T> res;
        res.data.push_back(row);
        return res;
    }

private:
    std::vector<std::vector<T>> data;
};

template<typename T, typename U>
Matrix<std::common_type_t<T,U>> operator *(const Matrix<T>& A, const Matrix<U>& B)
{
    // Result container.
    Matrix<T> result(A.rowSize(), B.columnsSize(), 0);

    // Check colums and rows size.
    if (A.columnsSize() == B.rowSize())
    {
        // Configure OMP.
        omp_set_num_threads(omp_get_max_threads());

        // Parallel multiplication.
        // TODO Improve with transpose.
        #pragma omp parallel for
        for (size_t i = 0; i < A.rowSize(); i++)
        {
            for (size_t j = 0; j < B.columnsSize(); j++)
            {
                for (size_t k = 0; k < A.columnsSize(); k++)
                {
                    result[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }

    // Return the matrix.
    return result;
}

}} // END NAMESPACES
// =====================================================================================================================
