
#include <cassert>
#include <iostream>

#include <LibDPSLR/Mathematics/Matrix>

void testMatrixTranspose()
{
    // Create a matrix for testing
    dpslr::math::Matrix<int> matrix1({{1, 2, 3},{4, 5, 6},{7, 8, 9}});

    std::cout<<matrix1.toString();

    // Transpose the matrix
    auto transposed = matrix1.transpose();

    // Check the dimensions
    assert(transposed.rowSize() == matrix1.columnsSize());
    assert(transposed.columnsSize() == matrix1.rowSize());

    // Check the values
    assert(transposed[0][0] == 1);
    assert(transposed[1][0] == 2);
    assert(transposed[2][0] == 3);
    assert(transposed[0][1] == 4);
    assert(transposed[1][1] == 5);
    assert(transposed[2][1] == 6);
    assert(transposed[0][2] == 7);
    assert(transposed[1][2] == 8);
    assert(transposed[2][2] == 9);
}

int main()
{
    std::cout << "Unit Test: Class dpslr::math::Matrix" << std::endl;

    testMatrixTranspose();

    // All assertions passed
    std::cout << "All tests passed!\n";

    return 0;
}
