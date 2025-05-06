#include <iostream>
#include <memory>
#include "phys/qam/mapper.hpp"
#include "file/file.hpp"

void test_qam_mapper() {
    auto qpsk_mapper            = qam_mapper<float, qam_order::QPSK>::make();
    auto qam16_mapper           = qam_mapper<float, qam_order::QAM16>::make();
    auto qam64_mapper           = qam_mapper<float, qam_order::QAM64>::make();

    bool qpsk_data_saved        = file_io::save_constellation_to_file(qpsk_mapper, "qpsk_constellation.dat", true);
    bool qam16_data_saved       = file_io::save_constellation_to_file(qam16_mapper, "qam16_constellation.dat", true);
    bool qam64_data_saved       = file_io::save_constellation_to_file(qam64_mapper, "qam64_constellation.dat", true);

    bool qpsk_script_created    = file_io::create_gnuplot_script(qpsk_mapper, "qpsk_constellation.dat", "qpsk_constellation.plt", true);
    bool qam16_script_created   = file_io::create_gnuplot_script(qam16_mapper, "qam16_constellation.dat", "qam16_constellation.plt", true);
    bool qam64_script_created   = file_io::create_gnuplot_script(qam64_mapper, "qam64_constellation.dat", "qam64_constellation.plt", true);
}

int main() {
    test_qam_mapper();
    return 0;
}
