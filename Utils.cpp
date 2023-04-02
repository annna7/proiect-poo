#include <iostream>
#include <string>
#include <random>

int promptUser(const std::string& message, int mn, int mx) {
    int input;
    do {
        std::cout << message << " (" << mn << "-" << mx << "): ";
        std::cin >> input;
    } while (input < mn || input > mx);
    return input;
}

int randomIntegerFromInterval(int mn, int mx) {
    // generate random integer in interval mn, max using <random>
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(mn, mx);
    return dis(gen);
}
