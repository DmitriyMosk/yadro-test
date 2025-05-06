#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>

class csv_writer {
public:
    csv_writer() : file_opened(false) {}
    
    ~csv_writer() {
        if (file_opened) {
            file.close();
        }
    }

    void set_file_name(const std::string& filename) {
        if (file_opened) {
            file.close();
        }
        
        file.open(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Не удалось открыть файл: " + filename);
        }
        
        file_opened = true;
    }

    void set_headers(const std::string& headers) {
        if (!file_opened) {
            throw std::runtime_error("Файл не открыт. Сначала вызовите set_file_name()");
        }
        
        file << headers << std::endl;
    }
    
    void push_data(const std::string& data) {
        if (!file_opened) {
            throw std::runtime_error("Файл не открыт. Сначала вызовите set_file_name()");
        }
        
        file << data << std::endl;
    }
    
private:
    std::ofstream file;
    bool file_opened;
};
