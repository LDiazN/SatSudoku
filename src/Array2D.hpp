#ifndef ARRAY2D_HPP
#define ARRAY2D_HPP
#include <vector>
#include <assert.h>

/// @brief A dynamic 2D array of T
/// @tparam T 
template <typename T>
class Array2D 
{
    public:
    Array2D(size_t rows, size_t cols)
        : _rows(rows)
        , _cols(cols)
        , _elements(std::vector<T>(rows * cols))
    { }

    /// @brief Get element i,j from the matrix
    /// @param i row index
    /// @param j column index
    /// @return element in position i,j
    T get(size_t i, size_t j) const
    {
        return _elements[actual_index(i,j)];
    }

    /// @brief Set element i,j in the matrix to the specified value
    /// @param i row index
    /// @param j col index
    /// @param element value to set 
    void set(size_t i, size_t j, const T& element)
    {
        _elements[actual_index(i,j)] = element;
    }

    size_t get_rows() { return _rows; }
    size_t get_cols() { return _cols; }

    private:
    /// @brief Get actual index inside the stored vector corresponding to this row and column
    /// @param i row index
    /// @param j column index
    /// @return actual index
    size_t actual_index(size_t i, size_t j) const
    {
        assert( i < _rows && j < _cols && "Index out of range of Array2D");
        return i * _cols + j;
    }

    private:
    /// @brief number of rows
    size_t _rows;
    /// @brief number of columns
    size_t _cols;
    /// @brief Actual elements storage
    std::vector<T> _elements;
};

#endif