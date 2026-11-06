#include "BitcoinExchange.hpp"
#include <sstream>
#include <fstream>

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
        ss >> rate;
        if(ss.fail())
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
        size_t seperator = line.find("|");
        if(seperator == std::string::npos)
        {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;

        }
        std::string date = stringTrim(line.substr(0, seperator));
        std::string value = stringTrim(line.substr(seperator + 1));

        std::stringstream ss(value);
        float amount;
        char extra;
        if(!(ss >> amount) || (ss >> extra))
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