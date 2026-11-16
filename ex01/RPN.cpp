
#include "RPN.hpp"
#include <sstream>
#include <climits>

RPN::RPN() {}

RPN::RPN(const RPN &other)
{
    this->numbers = other.numbers;
}

RPN &RPN::operator=(const RPN &other)
{
    if (this != &other)
        this->numbers = other.numbers;
    return *this;
}

RPN::~RPN() {}

void RPN::calculate(const std::string &expression)
{
    while (!numbers.empty())
        numbers.pop();
    std::stringstream ss(expression);
    std::string token;

    while (ss >> token)
    {
        if (token.length() == 1 && token[0] >= '0' && token[0] <= '9')
        {
            numbers.push(token[0] - '0');
        }
        else if (token == "+" || token == "-" ||
                 token == "*" || token == "/")
        {
            if (numbers.size() < 2)
                throw std::runtime_error("Invalid expression");

            int second = numbers.top();
            numbers.pop();

            int first = numbers.top();
            numbers.pop();

            long long result;

            if (token == "+")
                result = static_cast<long long>(first) + second;
            else if (token == "-")
                result = static_cast<long long>(first) - second;
            else if (token == "*")
                result = static_cast<long long>(first) * second;
            else
            {
                if (second == 0)
                    throw std::runtime_error("Division by zero");
                result = static_cast<long long>(first) / second;
            }

            if (result < INT_MIN || result > INT_MAX)
                throw std::runtime_error("Integer overflow");
            numbers.push(static_cast<int>(result));
        }
        else
            throw std::runtime_error("Invalid token");
    }

    if (numbers.size() != 1)
        throw std::runtime_error("Invalid expression");

    std::cout << numbers.top() << std::endl;
}
