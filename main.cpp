#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <unordered_map>

/**
 * A function to read the contents of a .txt file into a string given a file path.
 * @param path path to file to read
 * @return string containing content of file
 */
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

/**
 * A function to split a string into tokens with a given delimiter
 * @param s string to split
 * @return A vector to split strings
 */
std::vector<std::string> tokenize(const std::string& s)
{
    std::vector<std::string> tokens;
    std::istringstream iss{s};
    std::string token;

    while (iss >> token)
    {
        tokens.push_back(token);
    }

    return tokens;
}

int main()
{
    const std::filesystem::path path{"./enron1/ham/0002.1999-12-13.farmer.ham.txt"};
    std::string content; // NOTE: file is read properly but printing doesn't show all contents of the file

    try
    {
        content = readFile(path);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    std::vector<std::string> tokens = tokenize(content);
    std::unordered_map<std::string, int> freqs;
    const size_t totalCount = tokens.size(); // better to take the size here

    for (std::string& token : tokens)
    {
        std::ranges::transform(token, token.begin(),
            [](const unsigned char c) { return std::toupper(c); });
        freqs[token] += 1;
    }

    for (auto& [key, value] : freqs)
    {
        std::cout << "|" << key << "|" << " => " << static_cast<double>(value) / static_cast<double>(totalCount)
                  << '\n';
    }

    return 0;
}
