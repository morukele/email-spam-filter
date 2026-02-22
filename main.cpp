#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <unordered_map>

constexpr int THRESHOLD = 300;


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

int totalCount(std::unordered_map<std::string, int>& bagOfWords)
{
    int count = 0;
    for (const auto& [key, value] : bagOfWords)
    {
        if (const int f = bagOfWords[key]; f < THRESHOLD)
        {
            continue;
        }
        count += value;
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
        const double n = spamBOW[key] + hamBOW[key];
        if (n < THRESHOLD)
        {
            continue;
        }

        if (spamBOW[key] != 0)
        {
            spamDp += std::log(static_cast<double>(spamBOW[key]) / spamTotalCount);
        }
        if (hamBOW[key] != 0)
        {
            hamDp += std::log(static_cast<double>(hamBOW[key]) / hamTotalCount);
        }
        if (n != 0)
        {
            dp += std::log(n / totalCountOfBags);
        }
    }
    double spam = spamDp + spamP - dp;
    double ham = hamDp + hamP - dp;

    return std::pair{spam, ham};
}

std::pair<int, int> classifyDir(const std::filesystem::path& dirPath, std::unordered_map<std::string, int>& hamBOW,
                                std::unordered_map<std::string, int>& spamBOW, const double& hamTotalCount,
                                const double& spamTotalCount, const double& totalCountOfBags)
{
    int hamOutcomeCount = 0;
    int spamOutcomeCount = 0;

    for (auto itEntry = std::filesystem::recursive_directory_iterator(dirPath);
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

    return std::pair{spamOutcomeCount, hamOutcomeCount};
}

int main()
{
    std::cout << "Training..." << '\n';
    std::unordered_map<std::string, int> hamBOW;
    std::unordered_map<std::string, int> spamBOW;
    for (int i{1}; i <= 5; i++)
    {
        const std::filesystem::path hamDir{"./enron" + std::to_string(i) + "/ham"};
        addDirectoryToBagOfWords(hamDir, hamBOW);

        const std::filesystem::path spamDir{"./enron" + std::to_string(i) + "/spam"};
        addDirectoryToBagOfWords(spamDir, spamBOW);
    }

    const auto hamTotalCount = static_cast<double>(totalCount(hamBOW));
    const auto spamTotalCount = static_cast<double>(totalCount(spamBOW));
    double totalCountOfBags = hamTotalCount + spamTotalCount;

    std::cout << "Classifying ham..." << '\n';

    std::filesystem::path testHamDir{"enron6/ham"};
    auto [spamOutcomeCount, hamOutcomeCount] =
        classifyDir(testHamDir, hamBOW, spamBOW, hamTotalCount, spamTotalCount, totalCountOfBags);

    std::cout << '\t' << "SPAM COUNT    = " << spamOutcomeCount << '\n';
    std::cout << '\t' << "HAM COUNT     = " << hamOutcomeCount << '\n';

    std::cout << "Classifying spam..." << '\n';
    std::filesystem::path testSpamDir{"enron6/spam"};
    auto [spamOutcomeCount2, hamOutcomeCount2] =
        classifyDir(testSpamDir, hamBOW, spamBOW, hamTotalCount, spamTotalCount, totalCountOfBags);

    std::cout << '\t' << "SPAM COUNT    = " << spamOutcomeCount2 << '\n';
    std::cout << '\t' << "HAM COUNT     = " << hamOutcomeCount2 << '\n';

    return 0;
}
