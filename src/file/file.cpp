#include "file/file.hpp"
#include <fstream>
#include <iostream>
#include "types/complex.hpp"

namespace file_io {

std::string get_order_name(qam_order order) {
    switch (order) {
        case qam_order::QPSK: return "QPSK";
        case qam_order::QAM16: return "QAM16";
        case qam_order::QAM64: return "QAM64";
        default: return "Unknown";
    }
}

template<typename DTYPE, qam_order ORDER>
bool save_constellation_to_file(
    const std::shared_ptr<qam_mapper<DTYPE, ORDER>>& mapper,
    const std::string& filename,
    bool include_indices
) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
            return false;
        }
        
        file << "# QAM Constellation for order " << static_cast<int>(ORDER) << std::endl;
        file << "# Format: ";
        if (include_indices) {
            file << "index i q" << std::endl;
        } else {
            file << "i q" << std::endl;
        }
        
        const auto& constellation = mapper->get_constellation();
        
        for (const auto& [index, symbol] : constellation) {
            if (include_indices) {
                file << index << " " << symbol.i << " " << symbol.q << std::endl;
            } else {
                file << symbol.i << " " << symbol.q << std::endl;
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

template<typename DTYPE, qam_order ORDER>
bool create_gnuplot_script(
    const std::shared_ptr<qam_mapper<DTYPE, ORDER>>& mapper,
    const std::string& data_filename,
    const std::string& script_filename,
    bool include_indices
) {
    try {
        std::ofstream file(script_filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << script_filename << " for writing" << std::endl;
            return false;
        }
        
        std::string order_name = get_order_name(ORDER);
        
        file << "#!/usr/bin/gnuplot -persist\n";
        file << "# Скрипт для визуализации созвездия " << order_name << "\n\n";
        
        file << "# Настройки терминала для Windows\n";
        file << "set terminal wxt enhanced font 'Arial,12' size 800,600 persist\n\n";
        
        file << "# Настройки графика\n";
        file << "set title '" << order_name << " Constellation Diagram'\n";
        file << "set xlabel 'In-phase (I)'\n";
        file << "set ylabel 'Quadrature (Q)'\n";
        file << "set grid\n";
        file << "set size square\n";
        file << "set xrange [-8:8]\n";
        file << "set yrange [-8:8]\n\n";
        
        file << "# Настройки точек\n";
        file << "set style line 1 lc rgb '#0060ad' pt 7 ps 1.5\n\n";
        
        if (include_indices) {
            file << "# Отображение точек с индексами\n";
            file << "plot '" << data_filename << "' using 2:3:1 with points ls 1 title '" << order_name << " points', \\\n";
            file << "     '" << data_filename << "' using 2:3:1 with labels offset 0.5,0.5 title ''\n";
        } else {
            file << "# Отображение точек\n";
            file << "plot '" << data_filename << "' using 1:2 with points ls 1 title '" << order_name << " points'\n";
        }

        file << "pause -1 'Press ENTER for close'\n";
        
        file.close();
        
        std::cout << "Gnuplot script created: " << script_filename << std::endl;
        std::cout << "To run the script, use: gnuplot " << script_filename << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

template<typename DTYPE, qam_order ORDER>
bool save_and_plot_constellation(
    const std::shared_ptr<qam_mapper<DTYPE, ORDER>>& mapper,
    const std::string& base_filename,
    bool include_indices
) {
    std::string data_filename = base_filename + ".dat";
    std::string script_filename = base_filename + ".plt";
    
    bool data_saved = save_constellation_to_file(mapper, data_filename, include_indices);
    if (!data_saved) {
        return false;
    }
    
    bool script_created = create_gnuplot_script(mapper, data_filename, script_filename, include_indices);
    return script_created;
}

template<typename DTYPE>
bool save_modulated_symbols_to_file(
    const complex<DTYPE>& symbols,
    const std::string& filename,
    bool include_indices
) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
            return false;
        }
        
        file << "# Modulated Symbols" << std::endl;
        file << "# Format: ";
        if (include_indices) {
            file << "index i q" << std::endl;
        } else {
            file << "i q" << std::endl;
        }
        
        for (size_t i = 0; i < symbols.size() / 2; ++i) {
            auto symbol = symbols[i];
            if (include_indices) {
                file << i << " " << symbol.i << " " << symbol.q << std::endl;
            } else {
                file << symbol.i << " " << symbol.q << std::endl;
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool create_modulated_gnuplot_script(
    const std::string& data_filename,
    const std::string& script_filename,
    const std::string& title,
    bool include_indices
) {
    try {
        std::ofstream file(script_filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << script_filename << " for writing" << std::endl;
            return false;
        }
        
        file << "set terminal wxt enhanced font 'Arial,12' size 800,600 persist\n\n";
        
        file << "# Настройки графика\n";
        file << "set title '" << title << " Modulated Symbols'\n";
        file << "set xlabel 'In-phase (I)'\n";
        file << "set ylabel 'Quadrature (Q)'\n";
        file << "set grid\n";
        file << "set size square\n";
        file << "set xrange [-8:8]\n";
        file << "set yrange [-8:8]\n\n";
        
        file << "# Настройки точек\n";
        file << "set style line 1 lc rgb '#FF4500' pt 7 ps 1.5\n\n";
        
        if (include_indices) {
            file << "# Отображение точек с индексами\n";
            file << "plot '" << data_filename << "' using 2:3:1 with points ls 1 title '" << title << " modulated symbols', \\\n";
            file << "     '" << data_filename << "' using 2:3:1 with labels offset 0.5,0.5 title ''\n";
        } else {
            file << "# Отображение точек\n";
            file << "plot '" << data_filename << "' using 1:2 with points ls 1 title '" << title << " modulated symbols'\n";
        }
        
        file << "pause -1 'Press ENTER for close'\n";
        
        file.close();
        
        std::cout << "Gnuplot script created: " << script_filename << std::endl;
        std::cout << "To run the script, use: gnuplot " << script_filename << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

template<typename DTYPE>
bool save_and_plot_modulated_symbols(
    const complex<DTYPE>& symbols,
    const std::string& base_filename,
    const std::string& title,
    bool include_indices
) {
    std::string data_filename = base_filename + ".dat";
    std::string script_filename = base_filename + ".plt";
    
    bool data_saved = save_modulated_symbols_to_file(symbols, data_filename, include_indices);
    if (!data_saved) {
        return false;
    }
    
    bool script_created = create_modulated_gnuplot_script(data_filename, script_filename, title, include_indices);
    return script_created;
}

FILE_IO_TEMPLATES(float, qam_order::QPSK)
FILE_IO_TEMPLATES(float, qam_order::QAM16)
FILE_IO_TEMPLATES(float, qam_order::QAM64)
FILE_IO_TEMPLATES(double, qam_order::QPSK)
FILE_IO_TEMPLATES(double, qam_order::QAM16)
FILE_IO_TEMPLATES(double, qam_order::QAM64)

FILE_IO_TEMPLATES_MODULATED(float)
FILE_IO_TEMPLATES_MODULATED(double)

} // namespace file_io
