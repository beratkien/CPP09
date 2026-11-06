#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <iostream>
#include <string>
#include <stdexcept>
class BitcoinExchange
{
    private:
        std::map<std::string, float> db;
    public:
        BitcoinExchange();
        BitcoinExchange(const BitcoinExchange &other);
        BitcoinExchange &operator=(const BitcoinExchange &other);
        ~BitcoinExchange();
        static std::string stringTrim(const std::string &str);
        void addToDb(const std::string &file);
        float getExchangeRate(const std::string &date) const;
        void processInput(const std::string &file) const;
        
};
#endif