#include "BitcoinExchange.hpp"
#include <sstream>
#include <fstream>
#include <cctype>

BitcoinExchange::BitcoinExchange()
{

}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other)
{
    this->db = other.db;
}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
    if(this != &other)
    {
        this->db = other.db;
    }
    return *this;
}

BitcoinExchange::~BitcoinExchange()
{

}

std::string BitcoinExchange::stringTrim(const std::string &str)
{   
    size_t first = str.find_first_not_of(" \t");
    if(first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}


void BitcoinExchange::addToDb(const std::string &file)
{
    std::ifstream f(file.c_str());
    if(!f.is_open())
    throw std::runtime_error("Could not open database.");
    std::string line;
    std::getline(f, line);
    while(std::getline(f, line))
    {
        size_t comma = line.find(',');
        if(comma == std::string::npos)
            continue;
        std::string date = stringTrim(line.substr(0, comma));
        std::string price = stringTrim(line.substr(comma + 1));
        
        std::stringstream ss(price);
        float rate;
        char extra;
        if(!(ss >> rate) || (ss >> extra) || rate < 0)
            continue;

        db[date] = rate;
    }
    
    f.close();
}

float BitcoinExchange::getExchangeRate(const std::string &date) const
{
    std::map<std::string, float>::const_iterator it;
    it = this->db.lower_bound(date);
    if(it == db.end())
    {
        if(db.empty())
        throw std::runtime_error("Database is empty.");
        --it;
        return it->second;
    }
    if(it->first == date)
    return it->second;
    if(it == db.begin())
    throw std::runtime_error("No earlier date found.");
    --it;
    return it->second;
}

static bool isValidDate(const std::string &date)
{
    if (date.length() != 10)
        return false;

    if (date[4] != '-' || date[7] != '-')
        return false;

    for (int i = 0; i < 10; ++i)
    {
        if (i == 4 || i == 7)
            continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i])))
            return false;
    }

    std::stringstream ssYear(date.substr(0, 4));
    std::stringstream ssMonth(date.substr(5, 2));
    std::stringstream ssDay(date.substr(8, 2));

    int year, month, day;

    if (!(ssYear >> year) || !(ssMonth >> month) || !(ssDay >> day))
        return false;

    if (year < 1)
        return false;

    if (month < 1 || month > 12)
        return false;

    if (day < 1 || day > 31)
        return false;

    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
        return false;

    if (month == 2)
    {
        bool isLeap = (year % 4 == 0 && year % 100 != 0)
                    || (year % 400 == 0);

        if ((isLeap && day > 29) || (!isLeap && day > 28))
            return false;
    }

    return true;
}

static bool isValidNumberFormat(const std::string &value)
{
    bool hasDigit = false;
    bool hasDot = false;
    std::string::size_type i = 0;

    if (value.empty())
        return false;
    if (value[i] == '+' || value[i] == '-')
        ++i;
    for (; i < value.length(); ++i)
    {
        if (value[i] >= '0' && value[i] <= '9')
            hasDigit = true;
        else if (value[i] == '.' && !hasDot)
            hasDot = true;
        else
            return false;
    }
    return hasDigit;
}

void BitcoinExchange::processInput(const std::string &file) const
{
    
    std::ifstream f(file.c_str());
    if(!f.is_open())
    throw std::runtime_error("Could not open input file.");
    std::string line;
    std::getline(f, line);
    while(std::getline(f, line))
    {
        if(line.empty())
        continue;
        size_t seperator = line.find('|');
        if(seperator == std::string::npos)
        {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;

        }
        std::string date = stringTrim(line.substr(0, seperator));
        std::string value = stringTrim(line.substr(seperator + 1));

        if(!isValidDate(date))
        {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        if(!isValidNumberFormat(value))
        {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        std::stringstream ss(value);
        float amount;
        char extra;
        if(!(ss >> amount) || (ss >> extra))
        {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        if(amount != amount)
        {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        if(amount <= 0)
        {
            std::cout << "Error: not a positive number." << std::endl;
            continue;
        }
        if(amount > 1000)
        {
            std::cout << "Error: too large a number." << std::endl;
            continue;
        }
        try
        {
            float rate = getExchangeRate(date);
            std::cout << date << " => " << amount << " = " << (amount * rate) << std::endl;
        }
        catch(const std::exception &e)
        {
            std::cout << "Error: " << e.what() << std::endl;
            continue;
        }

    }
    f.close();
}