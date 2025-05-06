#pragma once

#include <string>
#include <memory>
#include "phys/qam/qam.hpp"
#include "phys/qam/mapper.hpp"
#include "types/complex.hpp"

/**
 * @namespace file_io
 * @brief Namespace for file I/O operations
 */
namespace file_io {

/**
 * @brief Gets a string representation of the modulation order
 * @param order Modulation order
 * @return String representation of the modulation order
 */
std::string get_order_name(qam_order order);

/**
 * @brief Saves the constellation map to a text file
 * @tparam DTYPE Data type for components of a complex number
 * @tparam ORDER Modulation order
 * @param mapper Pointer to mapper
 * @param filename File name
 * @param include_indices Whether to include point indices in the file
 * @return true if saving is successful, false otherwise
 */
template<typename DTYPE, qam_order ORDER>
bool save_constellation_to_file(
    const std::shared_ptr<qam_mapper<DTYPE, ORDER>>& mapper,
    const std::string& filename,
    bool include_indices = false
);

/**
 * @brief Creates a script for gnuplot to visualize the constellation
 * @tparam DTYPE Data type for the components of a complex number
 * @tparam ORDER Modulation order
 * @param mapper Pointer to the mapper
 * @param data_filename Data file name
 * @param script_filename File name for the script
 * @param include_indices Whether to include point indices in the visualization
 * @return true if creation succeeds, false otherwise
 */
template<typename DTYPE, qam_order ORDER>
bool create_gnuplot_script(
    const std::shared_ptr<qam_mapper<DTYPE, ORDER>>& mapper,
    const std::string& data_filename,
    const std::string& script_filename,
    bool include_indices = true
);

/**
 * @brief Saves the constellation and creates a gnuplot script
 * @tparam DTYPE Data type for the components of a complex number
 * @tparam ORDER Modulation order
 * @param mapper Pointer to the mapper
 * @param base_filename Base file name (without extension)
 * @param include_indices Whether to include point indices
 * @return true if operation succeeds, false otherwise
 */
template<typename DTYPE, qam_order ORDER>
bool save_and_plot_constellation(
    const std::shared_ptr<qam_mapper<DTYPE, ORDER>>& mapper,
    const std::string& base_filename,
    bool include_indices = true
);

/**
 * @brief Saves the modulated symbols to a text file
 * @tparam DTYPE Data type for the components of a complex number
 * @param symbols Modulated symbols
 * @param filename File name
 * @param include_indices Whether to include point indices in the file
 * @return true if saving was successful, false otherwise
 */
template<typename DTYPE>
bool save_modulated_symbols_to_file(
    const complex<DTYPE>& symbols,
    const std::string& filename,
    bool include_indices = true
);

/**
 * @brief Creates a script for gnuplot to visualize the modulated symbols
 * @param data_filename The name of the data file
 * @param script_filename The name of the file for the script
 * @param title The title for the plot
 * @param include_indices Whether to include point indices in the visualization
 * @return true if creation succeeds, false otherwise
 */
bool create_modulated_gnuplot_script(
    const std::string& data_filename,
    const std::string& script_filename,
    const std::string& title,
    bool include_indices = true
);

/**
 * @brief Saves the modulated symbols and creates a gnuplot script
 * @tparam DTYPE Data type for the components of a complex number
 * @param symbols The modulated symbols
 * @param base_filename The base file name (without extension)
 * @param title The title for the plot
 * @param include_indices Whether to include point indices
 * @return true if the operation succeeds, false otherwise
 */
template<typename DTYPE>
bool save_and_plot_modulated_symbols(
    const complex<DTYPE>& symbols,
    const std::string& base_filename,
    const std::string& title,
    bool include_indices = true
);

}

#define FILE_IO_TEMPLATES(dtype, order)                             \
    template bool file_io::save_constellation_to_file(              \
        const std::shared_ptr<qam_mapper<dtype, order>>& mapper,    \
        const std::string& filename,                                \
        bool include_indices                                        \
    );                                                              \
    template bool file_io::create_gnuplot_script(                   \
        const std::shared_ptr<qam_mapper<dtype, order>>& mapper,    \
        const std::string& data_filename,                           \
        const std::string& script_filename,                         \
        bool include_indices                                        \
    );                                                              \
    template bool file_io::save_and_plot_constellation(             \
        const std::shared_ptr<qam_mapper<dtype, order>>& mapper,    \
        const std::string& base_filename,                           \
        bool include_indices                                        \
    );                                                              

#define FILE_IO_TEMPLATES_MODULATED(dtype)                          \
    template bool file_io::save_modulated_symbols_to_file(          \
        const complex<dtype>& symbols,                              \
        const std::string& filename,                                \
        bool include_indices                                        \
    );                                                              \
    template bool file_io::save_and_plot_modulated_symbols(         \
        const complex<dtype>& symbols,                              \
        const std::string& base_filename,                           \
        const std::string& title,                                   \
        bool include_indices                                        \
    );
