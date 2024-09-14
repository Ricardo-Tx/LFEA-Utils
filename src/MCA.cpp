#include <iostream>
#include <fstream>
#include <string>

namespace LFEA {
    class MCASave {
    public:
        ushort year, month, day, hour, minute, second;
        uint elapsedSeconds, realTime;

    private:
    };

    int main() {
        std::string filename;
        std::cout << "Enter the name of the file to read: ";
        std::cin >> filename;

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file." << std::endl;
            return 1;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }

        file.close();

        return 0;
    }
    
}