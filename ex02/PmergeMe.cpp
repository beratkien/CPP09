
#include "PmergeMe.hpp"

#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <ctime>
#include <iomanip>

struct IndexCompare
{
    const std::vector<int> *values;

    IndexCompare(const std::vector<int> &v) : values(&v) {}

    bool operator()(size_t a, size_t b) const
    {
        return (*values)[a] < (*values)[b];
    }
};

struct PairInfo
{
    size_t small;
    size_t large;
};

struct PendingInfo
{
    size_t value;
    size_t partner;
    bool hasPartner;
};

static std::vector<size_t> getJacobsthalOrder(size_t count)
{
    std::vector<size_t> order;

    size_t previous = 1;
    size_t current = 3;

    while (previous < count + 1)
    {
        size_t upper = current;

        if (upper > count + 1)
            upper = count + 1;

        size_t index = upper;

        while (index > previous)
        {
            order.push_back(index - 2);
            --index;
        }

        size_t next = current + 2 * previous;
        previous = current;
        current = next;
    }

    return order;
}

template <typename Container>
static Container fordJohnson(
    const Container &input,
    const std::vector<int> &values)
{
    if (input.size() <= 1)
        return input;

    IndexCompare comp(values);

    std::vector<PairInfo> pairs;

    typename Container::const_iterator it = input.begin();

    bool hasOdd = false;
    size_t oddValue = 0;

    while (it != input.end())
    {
        size_t first = *it;
        ++it;

        if (it == input.end())
        {
            oddValue = first;
            hasOdd = true;
            break;
        }

        size_t second = *it;
        ++it;

        PairInfo pair;

        if (comp(second, first))
        {
            pair.small = second;
            pair.large = first;
        }
        else
        {
            pair.small = first;
            pair.large = second;
        }

        pairs.push_back(pair);
    }

    Container bigValues;

    for (size_t i = 0; i < pairs.size(); ++i)
        bigValues.push_back(pairs[i].large);

    Container sortedBigValues = fordJohnson(bigValues, values);

    std::vector<PairInfo> sortedPairs;
    std::vector<bool> used(pairs.size(), false);

    typename Container::const_iterator bigIt =
        sortedBigValues.begin();

    for (; bigIt != sortedBigValues.end(); ++bigIt)
    {
        for (size_t i = 0; i < pairs.size(); ++i)
        {
            if (!used[i] && pairs[i].large == *bigIt)
            {
                sortedPairs.push_back(pairs[i]);
                used[i] = true;
                break;
            }
        }
    }

    Container chain;

    chain.push_back(sortedPairs[0].small);

    for (size_t i = 0; i < sortedPairs.size(); ++i)
        chain.push_back(sortedPairs[i].large);

    std::vector<PendingInfo> pending;

    for (size_t i = 1; i < sortedPairs.size(); ++i)
    {
        PendingInfo item;

        item.value = sortedPairs[i].small;
        item.partner = sortedPairs[i].large;
        item.hasPartner = true;

        pending.push_back(item);
    }

    if (hasOdd)
    {
        PendingInfo item;

        item.value = oddValue;
        item.partner = 0;
        item.hasPartner = false;

        pending.push_back(item);
    }

    std::vector<size_t> order =
        getJacobsthalOrder(pending.size());

    for (size_t i = 0; i < order.size(); ++i)
    {
        size_t pendingIndex = order[i];

        if (pendingIndex >= pending.size())
            continue;

        PendingInfo item = pending[pendingIndex];

        typename Container::iterator position;

        if (item.hasPartner)
        {
            typename Container::iterator partnerPosition =
                std::find(
                    chain.begin(),
                    chain.end(),
                    item.partner
                );

            position = std::lower_bound(
                chain.begin(),
                partnerPosition,
                item.value,
                comp
            );
        }
        else
        {
            position = std::lower_bound(
                chain.begin(),
                chain.end(),
                item.value,
                comp
            );
        }

        chain.insert(position, item.value);
    }

    return chain;
}

template <typename Container>
static void printContainer(const Container &container)
{
    typename Container::const_iterator it = container.begin();

    for (; it != container.end(); ++it)
    {
        if (it != container.begin())
            std::cout << " ";

        std::cout << *it;
    }

    std::cout << std::endl;
}

template <typename Container>
static void printSorted(
    const Container &container,
    const std::vector<int> &values)
{
    typename Container::const_iterator it = container.begin();

    for (; it != container.end(); ++it)
    {
        if (it != container.begin())
            std::cout << " ";

        std::cout << values[*it];
    }

    std::cout << std::endl;
}

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe &other)
{
    _vector = other._vector;
    _deque = other._deque;
}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
    if (this != &other)
    {
        _vector = other._vector;
        _deque = other._deque;
    }

    return *this;
}

PmergeMe::~PmergeMe() {}

void PmergeMe::sort(int argc, char **argv)
{
    _vector.clear();
    _deque.clear();

    if (argc < 2)
        throw std::runtime_error("No numbers provided");

    for (int i = 1; i < argc; ++i)
    {
        std::stringstream ss(argv[i]);
        std::string token;
        bool found = false;

        while (ss >> token)
        {
            found = true;

            char *end;
            errno = 0;

            long value = std::strtol(token.c_str(), &end, 10);

            if (errno == ERANGE || end == token.c_str()
                || *end != '\0' || value <= 0
                || value > INT_MAX)
                throw std::runtime_error("Invalid input");

            _vector.push_back(static_cast<int>(value));
            _deque.push_back(static_cast<int>(value));
        }

        if (!found)
            throw std::runtime_error("Invalid input");
    }

    std::cout << "Before: ";
    printContainer(_vector);

    std::vector<size_t> vectorIndexes;

    for (size_t i = 0; i < _vector.size(); ++i)
        vectorIndexes.push_back(i);

    clock_t startVector = clock();

    std::vector<size_t> sortedVector =
        fordJohnson(vectorIndexes, _vector);

    clock_t endVector = clock();

    double vectorTime =
        static_cast<double>(endVector - startVector)
        * 1000000.0 / CLOCKS_PER_SEC;

    std::deque<size_t> dequeIndexes;

    for (size_t i = 0; i < _deque.size(); ++i)
        dequeIndexes.push_back(i);

    clock_t startDeque = clock();

    std::deque<size_t> sortedDeque =
        fordJohnson(dequeIndexes, _vector);

    clock_t endDeque = clock();

    double dequeTime =
        static_cast<double>(endDeque - startDeque)
        * 1000000.0 / CLOCKS_PER_SEC;

    std::cout << "After:  ";
    printSorted(sortedVector, _vector);

    std::cout << std::fixed << std::setprecision(5);

    std::cout << "Time to process a range of "
              << _vector.size()
              << " elements with std::vector : "
              << vectorTime << " us" << std::endl;

    std::cout << "Time to process a range of "
              << _deque.size()
              << " elements with std::deque  : "
              << dequeTime << " us" << std::endl;
}
