#include <iostream>
#include <sstream>
#include <string>


int main() {
    int N = 7;
    std::string line;
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        int x;
        while (ss >> x) {
            std::cout << x + N << " ";
        }
    }
    std::cout << std::endl;
    return 0;
}