#ifndef SATSUDOKU_HPP
#define SATSUDOKU_HPP
#include <string>

/// @brief Maion class wrapping application flow
class SatSudoku
{
    public:
        SatSudoku(float max_time, const std::string& file);

        /// @brief Run application logic
        void run();

    private: 
        std::string _file;
        float _time;

};

#endif