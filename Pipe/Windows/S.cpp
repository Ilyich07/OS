#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string line;
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        int x;
        long long sum = 0;
        while (ss >> x) {
            sum += x;
        }
        std::cout << sum << std::endl;
    }
    return 0;
}