#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <unordered_map>

/**
 * A function to read the contents of a .txt file into a string given a file
 * path.
 * @param path path to file to read
 * @return string containing content of file
 */
std::string readFile(const std::filesystem::path& path)
{
    std::ifstream file{path};
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
        std::ranges::transform(token, token.begin(), [](const unsigned char c) { return std::toupper(c); });
        tokens.push_back(token);
    }

    return tokens;
}

/**
 * A function to add the contents of a file into a bag of wards.
 * @param path path to txt file to read
 * @param bagOfWords Hashmap to add words into
 */
void addFileToBag(const std::filesystem::path& path, std::unordered_map<std::string, int>& bagOfWords)
{
    std::string content;
    try
    {
        content = readFile(path);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    std::vector<std::string> tokens = tokenize(content);

    for (std::string& token : tokens)
    {
        bagOfWords[token] += 1;
    }
}

/**
 * A function to add the contents of all txt files in a directory into a bag of words.
 * @param dir directory containing the files
 * @param bagOfWords Hashmap to add words into
 */
void addDirectoryToBagOfWords(const std::filesystem::path& dir, std::unordered_map<std::string, int>& bagOfWords)
{
    for (auto itEntry = std::filesystem::recursive_directory_iterator(dir);
         itEntry != std::filesystem::recursive_directory_iterator(); ++itEntry)
    {
        if (!itEntry->is_directory())
        {
            addFileToBag(itEntry->path(), bagOfWords);
        }
    }
}

int totalCount(const std::unordered_map<std::string, int>& bagOfWords)
{
    int count = 0;
    for (const auto& token : bagOfWords)
    {
        count += token.second;
    }
    return count;
}

std::pair<double, double> classifyFile(const std::filesystem::path& filePath,
                                       std::unordered_map<std::string, int>& hamBOW, const double& hamTotalCount,
                                       std::unordered_map<std::string, int>& spamBOW, const double& spamTotalCount,
                                       const double& totalCountOfBags)
{
    std::unordered_map<std::string, int> fileBOW;
    addFileToBag(filePath, fileBOW);

    double dp = 0.0;
    double hamDp = 0.0;
    double spamDp = 0.0;
    const double hamP = std::log(hamTotalCount / totalCountOfBags);
    const double spamP = std::log(spamTotalCount / totalCountOfBags);

    for (auto& key : fileBOW | std::views::keys)
    {
        if (spamBOW[key] != 0)
        {
            spamDp += std::log(static_cast<double>(spamBOW[key]) / spamTotalCount);
        }
        if (hamBOW[key] != 0)
        {
            hamDp += std::log(static_cast<double>(hamBOW[key]) / hamTotalCount);
        }
        const double n = spamBOW[key] + hamBOW[key];
        if (n != 0)
        {
            dp += std::log(n / totalCountOfBags);
        }
    }
    double spam = spamDp + spamP - dp;
    double ham = hamDp + hamP - dp;

    return std::pair{spam, ham};
}

int main()
{
    const std::filesystem::path hamDir{"./enron1/ham"};
    std::unordered_map<std::string, int> hamBOW;
    addDirectoryToBagOfWords(hamDir, hamBOW);

    const std::filesystem::path spamDir{"./enron1/spam"};
    std::unordered_map<std::string, int> spamBOW;
    addDirectoryToBagOfWords(spamDir, spamBOW);

    const auto hamTotalCount = static_cast<double>(totalCount(hamBOW));
    const auto spamTotalCount = static_cast<double>(totalCount(spamBOW));
    double totalCountOfBags = hamTotalCount + spamTotalCount;

    // walk through the test directory
    std::filesystem::path testHamDir{"enron2/ham"};
    int hamOutcomeCount = 0;
    int spamOutcomeCount = 0;

    for (auto itEntry = std::filesystem::recursive_directory_iterator(testHamDir);
         itEntry != std::filesystem::recursive_directory_iterator(); ++itEntry)
    {
        // if it is a file, classify it
        if (!itEntry->is_directory())
        {
            auto [spamP, hamP] =
                classifyFile(itEntry->path(), hamBOW, hamTotalCount, spamBOW, spamTotalCount, totalCountOfBags);

            if (spamP > hamP)
            {
                spamOutcomeCount++;
            }
            else
            {
                hamOutcomeCount++;
            }
        }
    }

    std::cout << "HAM COUNT: " << hamOutcomeCount << '\n';
    std::cout << "SPAM COUNT: " << spamOutcomeCount << '\n';

    return 0;
}
