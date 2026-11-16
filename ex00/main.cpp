#include "BitcoinExchange.hpp"

static std::string getDatabasePath(const char *program)
{
    std::string executable(program);
    std::string::size_type separator = executable.find_last_of("/\\");

    if (separator == std::string::npos)
        return "data.csv";
    return executable.substr(0, separator + 1) + "data.csv";
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: could not open file." << std::endl;
        return 1;
    }

    try
    {
        BitcoinExchange btc;

        btc.addToDb(getDatabasePath(argv[0]));
        btc.processInput(argv[1]);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}