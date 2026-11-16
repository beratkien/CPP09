
#include "PmergeMe.hpp"

int main(int argc, char **argv)
{
    try
    {
        PmergeMe p;

        p.sort(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
