#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <format>
#include <print>

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream file{path};
    // check if file is opened properly
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main()
{
    const std::filesystem::path path{"./enron1/ham/0002.1999-12-13.farmer.ham.txt"};
    std::string content; // NOTE: file is read properly but printing doesn't show all contents of the file

    try
    {
        content = readFile(path);
    } catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    std::cout << content << '\n';

    return 0;
}