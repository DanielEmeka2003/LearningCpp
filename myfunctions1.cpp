#include <cassert>
#include <limits>
#include <stdexcept>
#include <bitset>
#include <regex>

#include "stream/stringoutputstream.h"
#include "stream/stringinputstream.h"
#include "boost/multiprecision/cpp_int.hpp"
#include "boost/multiprecision/cpp_dec_float.hpp"
#include "myfunctions1.h"

namespace Myfcn
{
    void ManipulateStrAsBaseN::equalizeOperands(std::string& lhs, std::string& rhs)
    {
        auto firstNonZero = [](char& x)
        {
            if (x != '0')
            return true;

            return false;
        };
        
        auto it = std::ranges::find_if(lhs, firstNonZero);

        if (it != lhs.end())
        lhs.erase(lhs.begin(), it);

        it = std::ranges::find_if(rhs, firstNonZero);

        if (it != rhs.end())
        rhs.erase(rhs.begin(), it);
    }

    bool ManipulateStrAsBaseN::equal(const std::string& lhs, const std::string& rhs, std::uint8_t base)
    {
        if (lhs.empty() or rhs.empty())
        throw std::runtime_error{ "in function Myfcn::ManipulateStrAsBaseN::equal either lhs or rhs is empty" };

        if (!isValidBaseDigits(lhs, base) or !isValidBaseDigits(rhs, base))
        throw std::runtime_error{"in function MyFcn::ManipulateStrAsBaseN::equal the base given is compatible with either lhs or rhs"};

        auto lhs_copy = lhs, rhs_copy = rhs;
        equalizeOperands(lhs_copy, rhs_copy);

        if (lhs_copy.size() != rhs_copy.size())
        return false;

        for (auto i = 0uz; i < lhs_copy.size(); i++)
        {
            if (digitsMap[ std::toupper(lhs_copy[i]) ] != digitsMap[ std::toupper(rhs_copy[i]) ])
            return false;
        }
            
        return true;
    }

    bool ManipulateStrAsBaseN::notEqual(const std::string& lhs, const std::string& rhs, std::uint8_t base)
    { return !equal(lhs, rhs, base); }

    bool ManipulateStrAsBaseN::lessThan(const std::string& lhs, const std::string& rhs, std::uint8_t base)
    {
        if (lhs.empty() or rhs.empty())
        throw std::runtime_error{ "in function Myfcn::ManipulateStrAsBaseN::lessThan either lhs or rhs is empty" };

        if (!isValidBaseDigits(lhs, base) or !isValidBaseDigits(rhs, base))
        throw std::runtime_error{"in function MyFcn::ManipulateStrAsBaseN::lessThan the base given is compatible with either lhs or rhs"};

        auto lhs_copy = lhs, rhs_copy = rhs;
        equalizeOperands(lhs_copy, rhs_copy);

        // boost::multiprecision::cpp_int real_lhs{ convertBaseNToBase10(lhs_copy, base) };
        // boost::multiprecision::cpp_int real_rhs{ convertBaseNToBase10(rhs_copy, base) };

        // if (real_lhs < real_rhs)
        // return true;
        
        // return false;

        if (lhs_copy.size() < rhs_copy.size())
        return true;

        if (lhs_copy.size() > rhs_copy.size())
        return false;
            
        if (equal(lhs_copy, rhs_copy, base))
        return false;
        
        for (auto i = 0uz; i < lhs_copy.size(); i++)
        {
            if (digitsMap[ std::toupper(lhs_copy[i]) ] < digitsMap[ std::toupper(rhs_copy[i]) ])
            return true;
            else if (digitsMap[ std::toupper(lhs_copy[i]) ] > digitsMap[ std::toupper(rhs_copy[i]) ])
            return false;
        }
        
        return false;
    }

    bool ManipulateStrAsBaseN::greaterThan(const std::string& lhs, const std::string& rhs, std::uint8_t base)
    {
        if (!isValidBaseDigits(lhs, base) or !isValidBaseDigits(rhs, base))
        throw std::runtime_error{"in function MyFcn::ManipulateStrAsBaseN::greaterThan the base given is compatible with either lhs or rhs"};

        if (lhs.empty() or rhs.empty())
        throw std::runtime_error{ "in function Myfcn::ManipulateStrAsBaseN::greaterThan either lhs or rhs is empty" };

        if (equal(lhs, rhs, base))
        return false;

        return !lessThan(lhs, rhs, base);
    }

    bool ManipulateStrAsBaseN::lessThanOrEqual(const std::string& lhs, const std::string& rhs, std::uint8_t base)
    {
        if (equal(lhs, rhs, base) or lessThan(lhs, rhs, base))
        return true;

        return false;
    }

    bool ManipulateStrAsBaseN::greaterThanOrEqual(const std::string& lhs, const std::string& rhs, std::uint8_t base)
    {
        if (equal(lhs, rhs, base) or greaterThan(lhs, rhs, base))
        return true;

        return false;
    }

    bool ManipulateStrAsBaseN::isValidBaseDigits(const std::string& str, std::uint8_t base)
    {
        if (str.empty())
        throw std::runtime_error{ "in function Myfcn::ManipulateStrAsBaseN::isValidBaseDigits str is empty" };

        for (auto &&i : str)
        {
            if (digitsMap[ std::toupper(i) ] >= int(base))
            return false;
        }
            
        return true;
    }

    std::string ManipulateStrAsBaseN::convertBase10ToBaseN(const std::string& str, std::uint8_t base)
    {
        if (!isValidBaseDigits(str, 10))
        throw std::runtime_error{"in function MyFcn::ManipulateStrAsBaseN::convertBase10ToBaseN str contians an invalid base 10 digit"};

        struct DivLarge
        {
            boost::multiprecision::cpp_int quot{};
            boost::multiprecision::cpp_int rem{};

            void perform(boost::multiprecision::cpp_int dividend, boost::multiprecision::cpp_int divisor)
            {
                quot = dividend / divisor;
                rem = dividend % divisor;
            }
        };
            
        DivLarge divLarge{};
        std::string result{};

        divLarge.perform(boost::multiprecision::cpp_int{str.data()}, base);
        result.insert(result.begin(), mapDigits[std::stoi(divLarge.rem.str())]);

        while (true)
        {
            if (divLarge.quot == 0)
            break;

            divLarge.perform(divLarge.quot, base);
            result.insert(result.begin(), mapDigits[std::stoi(divLarge.rem.str())]);    
        }
            
        return result;
    }

    std::string ManipulateStrAsBaseN::convertBaseNToBase10(const std::string& str, std::uint8_t base)
    {
        if (!isValidBaseDigits(str, base))
        throw std::runtime_error{"in function MyFcn::ManipulateStrAsBaseN::convertBaseNToBase10 the base given is incompatible with str"};
            
        boost::multiprecision::cpp_int cppint{ std::to_string(digitsMap[ std::toupper(str[0]) ]).data() };

        for (auto i = 1uz; i < str.size(); ++i)
        cppint = (cppint * base) + digitsMap[ std::toupper(str[i]) ];
            
        return cppint.str();
    }


    std::string ManipulateStrAsBaseN::convertFractionalBaseNToBase10(const std::string& str, std::uint8_t base)
    {
        auto convertFractionPart = [&](const std::string& fraction)
        {
            if (!isValidBaseDigits(fraction, base))
            std::cout << "in function convertFractionalBaseNToBase10, the fraction part of str(.[xxxx]) contains invalid base digits\n", std::exit(0);

            boost::multiprecision::cpp_dec_float_50 fractional_part{};
            boost::multiprecision::cpp_dec_float_50 base_alt{int(base)};
            for (std::int64_t exp{-1}; auto&& i : fraction)
            {
                if (digitsMap[std::toupper(i)] != 0)
                fractional_part += boost::multiprecision::cpp_dec_float_50{digitsMap[std::toupper(i)]} * boost::multiprecision::pow(base_alt, {exp});
                
                exp -= 1;
            }

            Streams::StringOutputStream sos{std::ostringstream{}};
            sos.nouppercase().setPrecision(40).scientific().write(fractional_part);

            auto string_fractional_part = sos.get_str();
            
            if (string_fractional_part.contains('e'))
            std::ranges::replace(string_fractional_part, 'e', '@');

            //denoramlization of the calculated fractional number
            string_fractional_part = denormalizeToFraction(string_fractional_part);
            //removal of the leading zero i.e [0].xxxxxxxx
            string_fractional_part.erase(string_fractional_part.begin());
            
            return string_fractional_part;
        };

        if (!str.contains('.'))
        std::cout << "in function convertFractionalBaseNToBase10, str does not contain a radix point", std::exit(0);
        else if (std::ranges::count(str, '.') != 1)
        std::cout << "in function convertFractionalBaseNToBase10, str contains multiple radix point", std::exit(0);

        auto found = std::ranges::find(str, '.');
        return convertBaseNToBase10({str.begin(), found}, base) + convertFractionPart({found + 1, str.end()});
    }

    std::string ManipulateStrAsBaseN::convertFractionalBase10ToBaseN(const std::string& str, std::uint8_t base)
    {
        auto convertFractionPart = [&](const std::string& fraction)
        {
            if (!isValidBaseDigits(fraction, 10))
            std::cout << "in function convertFractionalBase10ToBaseN, the fraction part of str(.[xxxx]) contains invalid base digits", std::exit(0);

            boost::multiprecision::cpp_dec_float_50 fractional_part{};
            boost::multiprecision::cpp_dec_float_50 base_alt{int(base)};
            std::uint32_t max_iterations{50};
            std::string result_str{'.'};

            try { Streams::StringInputStream{ std::istringstream{fraction} }.read(fractional_part); }
            catch(std::exception&){}

            for (decltype(max_iterations) i = {}; i < max_iterations; ++i)
            {
                if (fractional_part == boost::multiprecision::cpp_dec_float_50{"0.0"})
                break;

                fractional_part *= base_alt;
                result_str.push_back(mapDigits[fractional_part.convert_to<int>()]);
                
                if (fractional_part.convert_to<int>() >= 1)
                fractional_part -= boost::multiprecision::cpp_dec_float_50{fractional_part.convert_to<int>()};
            }
            
            if (result_str.empty())
            result_str.push_back('0');

            return result_str;
        };

        if (!str.contains('.'))
        std::cout << "in function convertFractionalBase10ToBaseN, str does not contain a radix point", std::exit(0);
        else if (std::ranges::count(str, '.') != 1)
        std::cout << "in function convertFractionalBase10ToBaseN, str contains multiple radix point", std::exit(0);

        auto found = std::ranges::find(str, '.');
        return convertBase10ToBaseN({str.begin(), found}, base) + convertFractionPart({found, str.end()});
    }

    std::string ManipulateStrAsBaseN::normalizeToScientificN(const std::string& str)
    {
        std::regex reg{R"(^[1-9]{1}(\.[[:digit:]]*)?[@][+-]?[[:digit:]]+$)"};
        std::string newStr{};

        /*
        [PreCondition]
        1. string input digits must be base(10)
        2. the exponent identifier is not [E|e] but [@]

        [Invariant]
        1. the input string is remains unchanged, regardless of the operation that takes place

        [PostCondition]
        1. the string input must return with a fractional part of atleast [0] and an exponent of atleast/atmost [0]
        2. the returned rvalue string would not habour any leading or trailing implicit zeros, even if it is the result of an operation
        */

        //removal of impilcit leading zeros meaning someting like this: ([000000000]x.xxxxx) is discarded
        if (str.starts_with('0'))
        {
            for (auto i = 0uz, count = 0uz; i != str.size(); ++i)
            {
                if (str[i] != '0')
                {
                    if (count != 0)
                    newStr.assign(str.begin() + count, str.end());

                    break;
                }

                if (str[i] == '0' and !(str[i+1] == '.' or str[i+1] == '@'))
                ++count;
            }
        }
        
        if (newStr.empty())
        newStr.assign(str);

        //removal of implicit trailing zeros
        auto removal_of_implicit_trailing_zeros = [&]
        {
            if (auto radix_pos = newStr.find('.'); radix_pos != std::string::npos)
            {
                auto begin = newStr.find('@');
                if (begin == std::string::npos)
                begin = newStr.size() - 1; //example: so it would point to the character before the end
                else
                --begin;
                //all to ensure that only the fractional part is iterated over x.(xxxxx)@xx

                auto i = begin, leadingZerosCount = 0uz;
                for (; i > radix_pos; i--)
                {
                    if (newStr[i] != '0')
                    break;
                    else if (newStr[i] == '0' and i - 1 == radix_pos) //to save for something like this: (1.0) instead of (1.)
                    break;
                    else
                    ++leadingZerosCount;
                }
                if (leadingZerosCount != 0)
                newStr.erase(i + 1, leadingZerosCount);   
            }
        };
        
        removal_of_implicit_trailing_zeros();

        //enforcing PostCondition(0)
        {
            auto begin = newStr.find('@');
            if (begin == std::string::npos)
            begin = newStr.size();
            
            if (newStr.contains('.'))
            {
                //PostCondition(0) when a radix separator is found
                if (newStr.find('.') == begin - 1)
                newStr.insert(begin, 1, '0');
            }
            else //PostCondition(0) when a radix separator is not found
            newStr.insert(begin, ".0");
        }


        if (std::regex_match(newStr, reg))
        {
            return newStr;
        }
        else if (std::regex_match(newStr, reg = R"(^[0-9]{1}(\.[[:digit:]]*)?$)"))//first case for matching something like this: 0, 0., 0.1234, 1, 1., 1.2345
        {
            if (newStr.starts_with('0'))
            {
                if (newStr.contains('.')) //for normalizing something like this: 0.12345 => 1.2345@-1
                {
                    if (auto non_zero_pos = std::string{newStr.begin() + 2, newStr.end()}.find_first_not_of('0'); non_zero_pos != std::string::npos)
                    {
                        newStr.insert((non_zero_pos + 2) + 1, 1, '.');
                        return std::string{newStr.begin()+(non_zero_pos + 2), newStr.end()}.append("@-") + std::to_string(non_zero_pos + 1);
                    }
                }
            }
            return newStr.append("@0");
        }
        else if (std::regex_match(newStr, reg = R"(^[0-9]{2,}(\.[[:digit:]]*)?$)"))//second case for matching something like this: 12, 12., 12.34
        {
            if (newStr.contains('.'))
            {
                auto radix_loc = newStr.find('.');
                newStr.erase(newStr.begin() + radix_loc);
                newStr.insert(newStr.begin() + 1, '.');
                return removal_of_implicit_trailing_zeros(), newStr.append({'@'}) + std::to_string(radix_loc - 1);
            }
            else
            {
                newStr.insert(newStr.begin() + 1, '.');
                return removal_of_implicit_trailing_zeros(), newStr.append({'@'}) + std::to_string(newStr.size() - 2);
            }
        }
        else if (std::regex_search(newStr, reg = R"([@][+-]?[[:digit:]]+)"))//third and final case for any fallout that has an exponent, 12.34@1, 123.4@-23
        {
            std::string exponent{};
            //extracting the exponent
            for (auto i = newStr.size() - 1; i != 0; --i)
            {
                if (newStr[i] == '@')
                {
                    exponent.append(newStr.begin() + (i + 1), newStr.end());
                    newStr.erase(newStr.begin() + i, newStr.end());
                    break;
                }
            }
            
            if (std::regex_match(newStr, reg = R"(^[0]{1}(\.[[:digit:]]*)?$)"))//a reiteration of the first case, but one which has an exponent
            {
                if (newStr.contains('.'))
                {
                    if (auto non_zero_pos = std::string{newStr.begin() + 2, newStr.end()}.find_first_not_of('0'); non_zero_pos != std::string::npos)
                    {
                        newStr.insert((non_zero_pos+2) + 1, 1, '.');
                        return std::string{newStr.begin()+(non_zero_pos+2), newStr.end()}.append({'E'}) + std::to_string(std::stoi(exponent) - int(non_zero_pos+1));
                    }
                }
                
                return newStr.append({'@'}).append(exponent.begin(), exponent.end());
            }
            else if (std::regex_match(newStr, reg = R"(^[0-9]{2,}(\.[[:digit:]]*)?$)"))//a reiteration of the second case, but one which has an exponent
            {
                if (newStr.contains('.'))
                {
                    auto radix_loc = newStr.find('.');
                    newStr.erase(newStr.begin() + radix_loc);
                    newStr.insert(newStr.begin()+1, '.');
                    return removal_of_implicit_trailing_zeros(), newStr.append({'@'}) + std::to_string(std::stoi(exponent) + int(radix_loc - 1));
                }
                else
                {
                    newStr.insert(newStr.begin()+1, '.');
                    return removal_of_implicit_trailing_zeros(), newStr.append({'@'}) + std::to_string(std::stoi(exponent) + int(newStr.size() - 2));
                }
            }
        }

        //if failure occurs
        std::cout << "in function normalizeToScientificN, the Exponent character is [ @ ](also exponent digits must be specified when the [ @ ] is found) and base(10) digits are expected", std::exit(0);
        
        return "";
    }

    std::string ManipulateStrAsBaseN::denormalizeToFraction(const std::string& str)
    {
        std::regex reg{R"(^[[:alnum:]]{1,}(\.[[:alnum:]]*)?[@]([+-]?[[:digit:]]+)$)"};
        std::smatch smatch{};

        if (std::regex_match(str, smatch, reg))
        {
            std::string newStr{};

            //removal of implicit leading zeros
            if (str.starts_with('0'))
            {
                for (auto i = 0uz, count = 0uz; i != str.size(); ++i)
                {
                    if (str[i] != '0')
                    {
                        if (count != 0)
                        newStr.assign(str.begin() + count, str.end());

                        break;
                    }

                    if (str[i] == '0' and !(str[i+1] == '.' or str[i+1] == '@'))
                    ++count;
                }
            }
            
            if (newStr.empty())
            newStr.assign(str);

            //removal of the exponent
            newStr.erase(newStr.begin() + newStr.find('@'), newStr.end());

            auto exponent = std::stoi(smatch[2]);

            if (smatch[1].str().empty()) //smatch[i] contains the optional fractional part, inculding the radix point
            {
                if (exponent >= 0)
                {
                    for (auto&& i = 0; i < exponent; ++i)
                    newStr.push_back('0');

                    newStr.append(".0");
                }
                else
                {
                    exponent *= -1;

                    if (std::size_t(exponent) == newStr.size())
                    {
                        newStr.insert(newStr.begin(), {'0', '.'});
                    }
                    else if (std::size_t(exponent) < newStr.size())
                    {   
                        newStr.insert(newStr.begin() + (newStr.size() - std::size_t(exponent)), '.');
                    }
                    else //if (exponent > newStr.size())
                    {
                        auto number_of_fractionalzeros = std::max(exponent, int(newStr.size())) - std::min(exponent, int(newStr.size()));

                        for (auto i = 0; i < number_of_fractionalzeros; ++i)
                        newStr.insert(newStr.begin(), '0');

                        newStr.insert(newStr.begin(), {'0', '.'});
                    }
                }
            }
            else
            {
                auto radix_pos = newStr.find('.');
                
                //removal of implicit trailing zeros
                {
                    std::size_t i = newStr.size() - 1, leadingZerosCount = {};
                    for (; i > radix_pos; i--)
                    {
                        if (newStr[i] != '0')
                        break;
                        else ++leadingZerosCount;
                    }
                    if (leadingZerosCount != 0)
                    newStr.erase(i + 1, leadingZerosCount);
                }
                
                auto whole_number_size = radix_pos;
                newStr.erase(newStr.begin() + radix_pos); //also removing the radix separator
                auto fractional_part_size = newStr.size() - whole_number_size; //fractional_part_size depends on newStr devoid of the radix seperator

                if (exponent >= 0)
                {
                    if (std::size_t(exponent) > fractional_part_size)
                    {
                        auto newExponent = std::max(exponent, int(fractional_part_size)) - std::min(exponent, int(fractional_part_size));
                        
                        for (auto&& i = 0; i < newExponent; ++i)
                        newStr.push_back('0');

                        newStr.append(".0");
                    }
                    else if (std::size_t(exponent) < fractional_part_size)
                    {
                        auto newRadixPos = exponent + radix_pos;
                        newStr.insert(newStr.begin() + newRadixPos, '.');
                    }
                    else
                    {
                        newStr.append(".0");
                    }
                }
                else
                {
                    exponent *= -1;

                    if (std::size_t(exponent) == whole_number_size)
                    {
                        newStr.insert(newStr.begin(), {'0', '.'});
                    }
                    else if (std::size_t(exponent) < whole_number_size)
                    {   
                        newStr.insert(newStr.begin() + (whole_number_size - std::size_t(exponent)), '.');
                    }
                    else //if (exponent > newStr.size())
                    {
                        auto number_of_fractionalzeros = std::max(exponent, int(whole_number_size)) - std::min(exponent, int(whole_number_size));

                        for (auto i = 0; i < number_of_fractionalzeros; ++i)
                        newStr.insert(newStr.begin(), '0');

                        newStr.insert(newStr.begin(), {'0', '.'});
                    }
                }
            }

            return newStr;
        }
        else
        std::cout << R"(in function denormalizeToFraction format for string was not met, i.e [[:alnum:]]{1,}(\.[[:alnum:]]*)?[@]([+-]?[[:digit:]]+)", std::exit(0);

        return "";
    }

    std::string ManipulateStrAsBaseN::approximation(const std::string& str, std::uint32_t desired_digits)
    {
        /*
            [PreCondition]
            1. input string must the return value of normalizeScientificN
        */

        std::regex reg{R"(^([0-9]{1})(\.[[:digit:]]*)([@][+-]?[[:digit:]]+)$)"};
        std::smatch smatch{};

        if (std::regex_match(str, smatch, reg))
        {
            if (str.front() == '0')
            return str;
            
            std::string fractional_part = smatch[2];
            fractional_part.erase(fractional_part.begin()); //removing the radix separator

            if (fractional_part.size() > desired_digits)
            {
                std::string wholeNumber_part = smatch[1];
                std::string exponent_part = smatch[3];

                auto index_of_the_first_digit_to_remove = desired_digits;

                if (digitsMap[fractional_part[index_of_the_first_digit_to_remove]] >= 5)
                {
                    auto carry_digit = 0;
                    auto temp_insert_digit = 0;
                    for (int i = index_of_the_first_digit_to_remove - 1 /*to get the last safe digit index*/; i >= 0; --i)
                    {
                        temp_insert_digit = 1 + digitsMap[fractional_part[i]];
                        carry_digit = temp_insert_digit / 10, temp_insert_digit %= 10;

                        fractional_part.replace((std::size_t)i, 1uz, std::string{mapDigits[temp_insert_digit]});
                        
                        if (carry_digit == 0)
                        break;
                    }
                    
                    if (carry_digit != 0)
                    wholeNumber_part.replace(0uz, 1uz, "10");
                }
                //removing the undesired digits
                fractional_part.erase((std::size_t)index_of_the_first_digit_to_remove);
                //insertion of the radix separator again
                fractional_part.insert(fractional_part.begin(), '.');

                return normalizeToScientificN(wholeNumber_part + fractional_part + exponent_part);
            }
            return str;
        }
        else
        std::cout << R"(in function approximation, the PreCondition ditates that the str must be in this format: ^[1-9]{1}\.[[:digit:]]*?[@][+-]?[[:digit:]]+$)", std::exit(0);

        return "";
    }

    bool ManipulateStrAsBaseN::isFPNumberEqualTo(const std::string& str1, const std::string& str2)
    {
        std::regex reg{R"(^([0-9]){1}(\.[[:digit:]]*)?[@]([+-]?[[:digit:]]+)$)"};
        std::smatch smatch1{};
        std::smatch smatch2{};

        if (std::regex_match(str1, smatch1, reg) and std::regex_match(str2, smatch2, reg)) 
        {
            if (std::stoi(smatch1[3]) != std::stoi(smatch2[3]))
            return false;

            if (!equal(smatch1[1], smatch2[1], 10))
            return false;
            else
            {
                std::string fractional_part_1 = smatch1[2].str();
                std::string fractional_part_2 = smatch2[2].str();
                
                if (fractional_part_1.contains('.'))
                fractional_part_1.erase(fractional_part_1.begin());

                if (fractional_part_2.contains('.'))
                fractional_part_2.erase(fractional_part_2.begin());
                
                if (fractional_part_1.empty())
                fractional_part_1.push_back('0');
                
                if (fractional_part_2.empty())
                fractional_part_2.push_back('0');

                if (fractional_part_1.size() != fractional_part_2.size())
                {
                    bool fp1_isMax{};

                    if (fractional_part_1.size() > fractional_part_2.size())
                    fp1_isMax = true;
                    else
                    fp1_isMax = false;
                    
                    auto maxSize = (fp1_isMax? fractional_part_1.size() : fractional_part_2.size()) - (fp1_isMax? fractional_part_2.size() : fractional_part_1.size());

                    for (auto i = 0uz; i < maxSize; i++)
                    {
                        if (!fp1_isMax)
                        fractional_part_1.push_back('0');
                        else
                        fractional_part_2.push_back('0');
                    }   
                }
                
                if (equal(fractional_part_1, fractional_part_2, 10))
                return true;

                return false;
            }
        }
        else
        std::cout << "in function isFPNumberEqualTo Don't do that!\n\n", std::exit(0);
        
        return false;
    }

    bool ManipulateStrAsBaseN::isFPNumberNotEqualTo(const std::string& str1, const std::string& str2)
    { return !isFPNumberEqualTo(str1, str2); }

    bool ManipulateStrAsBaseN::isFPNumberLessThan(const std::string& str1, const std::string& str2)
    {
        std::regex reg{R"(^([0-9]){1}(\.[[:digit:]]*)?[@]([+-]?[[:digit:]]+)$)"};
        std::smatch smatch1{};
        std::smatch smatch2{};

        if (std::regex_match(str1, smatch1, reg) and std::regex_match(str2, smatch2, reg)) 
        {
            if (std::stoi(smatch1[3]) < std::stoi(smatch2[3]))
            return true;
            else if (std::stoi(smatch1[3]) > std::stoi(smatch2[3]))
            return false;

            if (lessThan(smatch1[1], smatch2[1], 10))
            return true;
            else if (greaterThan(smatch1[1], smatch2[1], 10))
            return false;
            else
            {
                std::string fractional_part_1 = smatch1[2].str();
                std::string fractional_part_2 = smatch2[2].str();
                
                if (fractional_part_1.contains('.'))
                fractional_part_1.erase(fractional_part_1.begin());

                if (fractional_part_2.contains('.'))
                fractional_part_2.erase(fractional_part_2.begin());
                
                if (fractional_part_1.empty())
                fractional_part_1.push_back('0');
                
                if (fractional_part_2.empty())
                fractional_part_2.push_back('0');

                if (fractional_part_1.size() != fractional_part_2.size())
                {
                    bool fp1_isMax{};

                    if (fractional_part_1.size() > fractional_part_2.size())
                    fp1_isMax = true;
                    else
                    fp1_isMax = false;
                    
                    auto maxSize = (fp1_isMax? fractional_part_1.size() : fractional_part_2.size()) - (fp1_isMax? fractional_part_2.size() : fractional_part_1.size());

                    for (auto i = 0uz; i < maxSize; i++)
                    {
                        if (!fp1_isMax)
                        fractional_part_1.push_back('0');
                        else
                        fractional_part_2.push_back('0');
                    }   
                }

                if (lessThan(fractional_part_1, fractional_part_2, 10))
                return true;
                else if (greaterThan(fractional_part_1, fractional_part_2, 10))
                return false;

                return false;
            }
        }
        else
        std::cout << "in function isFPNumberLessThan Don't do that!\n\n", std::exit(0);
        
        return false;
    }

    bool ManipulateStrAsBaseN::isFPNumberGreaterThan(const std::string& str1, const std::string& str2)
    { return !isFPNumberEqualTo(str1, str2) and !isFPNumberLessThan(str1, str2); }

    bool ManipulateStrAsBaseN::isFPNumberLessThanEqualTo(const std::string& str1, const std::string& str2)
    { return isFPNumberEqualTo(str1, str2) or isFPNumberLessThan(str1, str2); }

    bool ManipulateStrAsBaseN::isFPNumberGreaterThanEqualTo(const std::string& str1, const std::string& str2)
    { return isFPNumberEqualTo(str1, str2) or isFPNumberGreaterThan(str1, str2); }



    U8char::U8char(const std::string& u8)
    {
        if (u8.empty())
        return;
        
        calculate_byteEncoding(u8);

        if (!isU8charValid(u8))
        throw std::invalid_argument{"in function U8char::U8char parameter u8charholder contains an invalid utf8 character"};

        for (std::size_t index{}; auto&& i : u8)
        {
            m_u8[index] = std::uint8_t(i);
            ++index;
        }
    }

    bool U8char::isU8charValid(const std::string u8) const
    {
        if (m_byteEncoding == ByteEncoding::one and u8.size() > 1)
        return false;
        else if (m_byteEncoding == ByteEncoding::two and u8.size() > 2)
        return false;
        else if (m_byteEncoding == ByteEncoding::three and u8.size() > 3)
        return false;
        else if (m_byteEncoding == ByteEncoding::four and u8.size() > 4)
        return false;

        return true;
    }

    void U8char::calculate_byteEncoding(const std::string& u8)
    {
        if (u8.empty())
        m_byteEncoding = ByteEncoding::empty;

        std::bitset<8> byte{ std::uint8_t(u8[0]) };
        
        if (!byte.test(7))
        m_byteEncoding = ByteEncoding::one;
        else
        {
            std::size_t i{7};
            std::size_t count{};

            while (byte.test(i))
            { --i, ++count; }

            switch(count)
            {
                case 2: m_byteEncoding = ByteEncoding::two; break;
                case 3: m_byteEncoding = ByteEncoding::three; break;
                case 4: m_byteEncoding = ByteEncoding::four; break;
            }
        }
    }

    std::string U8char::to_string() const
    {
        std::string temp{};

        for (std::uint16_t i = 0; i < std::uint16_t(m_byteEncoding); ++i)
        temp.push_back(char(m_u8[i]));

        return temp;
    }

    std::string U8char::convertToUnicodeCodePoint() const
    {
        std::string unicodeCodePoint{};
        std::bitset<8> byte{};
        std::string u8{to_string()};

        switch (m_byteEncoding)
        {
        case ByteEncoding::one:
            byte = std::uint8_t(u8[0]);

            unicodeCodePoint = byte.to_string().erase(0, 1);
            break;
        case ByteEncoding::two:
            byte = std::uint8_t(u8[0]);
            unicodeCodePoint.append(byte.to_string().erase(0, 3));

            byte = std::uint8_t(u8[1]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));
            break;
        case ByteEncoding::three:
            byte = std::uint8_t(u8[0]);
            unicodeCodePoint.append(byte.to_string().erase(0, 4));

            byte = std::uint8_t(u8[1]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));

            byte = std::uint8_t(u8[2]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));
            break;
        case ByteEncoding::four:
            byte = std::uint8_t(u8[0]);
            unicodeCodePoint.append(byte.to_string().erase(0, 5));

            byte = std::uint8_t(u8[1]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));

            byte = std::uint8_t(u8[2]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));

            byte = std::uint8_t(u8[3]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));
            break;
        default:
            unicodeCodePoint  = "[NO UTF8 TO STRIP TO UNICODE CODE POINTS]";
            break;
        }

        if (m_byteEncoding != ByteEncoding::empty)
        unicodeCodePoint = ManipulateStrAsBaseN::convertBase10ToBaseN(ManipulateStrAsBaseN::convertBaseNToBase10(unicodeCodePoint, 2), 16);

        return unicodeCodePoint;
    }

    std::string U8char::convertToUtf8CodePoint() const
    {
        std::string u8CodePoint{};

        for (auto&& i : to_string())
        {
            u8CodePoint.
            append(ManipulateStrAsBaseN::convertBase10ToBaseN(ManipulateStrAsBaseN::convertBaseNToBase10(std::bitset<8>{std::uint8_t(i)}.to_string(), 2), 16));
        }

        return u8CodePoint;
    }

    U8char::ByteEncoding U8char::getByteEncoding() const
    { return m_byteEncoding; }

    void U8char::info() const
    {
        System::coutput.setFillc('=').setWidth(42).write_endl("");
        System::coutput.writews_endl("ByteEncoding:", m_byteEncoding);
        System::coutput.writews_endl("UTF8 character:", to_string());
        System::coutput.write_endl("");

        System::coutput.write_endl("UTF8 CODE POINT:");
        
        System::coutput.writews("Base2  :");
        for(auto&& i : to_string())
        {
            System::coutput.writews(std::bitset<8>{std::uint8_t(i)});
        }
        System::coutput.write_endl("");

        System::coutput.hex().writews("Base16 :");
        for(auto&& i : to_string())
        {
            System::coutput.write(std::bitset<8>{std::uint8_t(i)}.to_ulong());
        }
        System::coutput.dec();
        System::coutput.write_endl("");

        System::coutput.write_endl("");

        System::coutput.write_endl("UNICODE CODE POINT:");
        std::string unicodeCodePoints = convertToUnicodeCodePoint();
        System::coutput.writews_endl("Base2  :", ManipulateStrAsBaseN::convertBase10ToBaseN(ManipulateStrAsBaseN::convertBaseNToBase10(unicodeCodePoints, 16), 2));        
        System::coutput.writews_endl("Base16 :", unicodeCodePoints);

        System::coutput.setFillc('=').setWidth(42).write_endl("");
    }

    std::size_t U8char::size() const
    { return 1; }

    bool U8char::isEmpty() const
    {
        if (to_string().empty())
        return true;

        return false;
    }

    U8char::operator std::uint32_t() const
    { return std::stoul(convertToUtf8CodePoint(), nullptr, 16); }

    U8char U8char::intToU8char(std::uint32_t x)
    {
        std::string str{ManipulateStrAsBaseN::convertBase10ToBaseN(std::to_string(x), 16)};
        std::string u8str{};

        struct CharByte
        {
            std::string byte{};
            void inc(std::string::iterator& it)
            {
                byte.push_back(*it);
                byte.push_back(*(it + 1));
                it += 2;
            }
        };
        
        CharByte charByte{};
        for (auto i = str.begin(); i != str.end(); )
        {
            charByte.inc(i);
            u8str.push_back(char(std::stoul(charByte.byte, nullptr, 16)));
        }
        
        return u8str;
    }


    bool U8char::isBasicLatinDigit() const
    {
        if (!isBasicLatin())
        return false;

        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "30", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "39", 16))
        return true;

        return false;
    }

    bool U8char::isBasicLatinAlphabet() const
    {
        if (isBasicLatinLowerCaseAlphabet() or isBasicLatinUpperCaseAlphabet())
        return true;

        return false;
    }

    bool U8char::isBasicLatinAlphabetNumeral() const
    {
        if (isBasicLatinAlphabet() or isBasicLatinDigit())
        return true;

        return false;
    }

    bool U8char::isBasicLatinLowerCaseAlphabet() const
    {
        if (!isBasicLatin())
        return false;

        std::string unicode{ convertToUnicodeCodePoint() };
        
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "61", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "7a", 16))
        return true;

        return false;
    }

    bool U8char::isBasicLatinUpperCaseAlphabet() const
    {
        if (!isBasicLatin())
        return false;

        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "41", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "5a", 16))
        return true;

        return false;
    }

    bool U8char::isBasicLatinWhiteSpace() const
    {
        if (!isBasicLatin())
        return false;

        std::string unicode{ convertToUnicodeCodePoint() };
        if (
            ManipulateStrAsBaseN::equal(unicode, "20", 16) or
            (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "9", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "d", 16))
            )
        return true;

        return false;
    }

    U8char& U8char::basicLatinToLowerCase()
    { 
        if (!isBasicLatinAlphabet() or isBasicLatinLowerCaseAlphabet())
        return *this;
        
        m_u8[0] += 32;

        return *this;
    }

    U8char& U8char::basicLatinToUpperCase()
    {
        if (!isBasicLatinAlphabet() or isBasicLatinUpperCaseAlphabet())
        return *this;

        m_u8[0] -= 32;
        return *this;
    }

    bool U8char::basicLatinAlphabetCaseInsensitiveCompare(const U8char& x) const
    {
        U8char a{x};
        U8char b{*this};

        if (a.basicLatinToLowerCase() == b.basicLatinToLowerCase())
        return true;

        return false;
    }


    bool U8char::isBasicLatin() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::lessThanOrEqual(unicode, "7f", 16))
        return true;

        return false;
    }

    bool U8char::isLatin1Supplement() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "80", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "ff", 16))
        return true;

        return false;
    }

    bool U8char::isLatinExtendedA() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "100", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "17f", 16))
        return true;

        return false;
    }

    bool U8char::isLatinExtendedB() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "180", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "24f", 16))
        return true;

        return false;
    }

    bool U8char::isIPAExtensions() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "250", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "2af", 16))
        return true;

        return false;
    }

    bool U8char::isSpacingModifierLetters() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2b0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "2ff", 16))
        return true;

        return false;
    }

    bool U8char::isCombiningDiacriticalMarks() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "300", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "36f", 16))
        return true;

        return false;
    }

    bool U8char::isGreekAndCoptic() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "370", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "3ff", 16))
        return true;

        return false;
    }

    bool U8char::isCyrillic() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "400", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "4ff", 16))
        return true;

        return false;
    }

    bool U8char::isArmenian() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "530", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "58f", 16))
        return true;

        return false;
    }

    bool U8char::isHebrew() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "590", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "5ff", 16))
        return true;

        return false;
    }

    bool U8char::isArabic() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "600", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "6ff", 16))
        return true;

        return false;
    }

    bool U8char::isSyriac() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "700", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "74f", 16))
        return true;

        return false;
    }

    bool U8char::isArabicSupplement() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "750", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "77f", 16))
        return true;

        return false;
    }

    bool U8char::isThaana() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "780", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "7bf", 16))
        return true;

        return false;
    }

    bool U8char::isNKo() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "7c0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "7ff", 16))
        return true;

        return false;
    }

    bool U8char::isSamaritan() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "800", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "83f", 16))
        return true;

        return false;
    }

    bool U8char::isMandaic() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "840", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "85f", 16))
        return true;

        return false;
    }

    bool U8char::isSyriacSupplement() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "860", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "86f", 16))
        return true;

        return false;
    }

    bool U8char::isArabicExtendedB() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "870", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "89f", 16))
        return true;

        return false;
    }

    bool U8char::isArabicExtendedA() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "8a0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "8ff", 16))
        return true;

        return false;
    }

    bool U8char::isDevanagari() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "900", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "97f", 16))
        return true;

        return false;
    }

    bool U8char::isBengali() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "980", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "9ff", 16))
        return true;

        return false;
    }

    bool U8char::isGurmukhi() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "a00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "a7f", 16))
        return true;

        return false;
    }

    bool U8char::isGujarati() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "a80", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "aff", 16))
        return true;

        return false;
    }

    bool U8char::isOriya() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "b00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "b7f", 16))
        return true;

        return false;
    }

    bool U8char::isTamil() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "b80", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "bff", 16))
        return true;

        return false;
    }
    
    bool U8char::isTelugu() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "c00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "c7f", 16))
        return true;

        return false;
    }

    bool U8char::isKannada() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "c80", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "cff", 16))
        return true;

        return false;
    }

    bool U8char::isMalayalam() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "d00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "d7f", 16))
        return true;

        return false;
    }

    bool U8char::isSinhala() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "d80", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "dff", 16))
        return true;

        return false;
    }

    bool U8char::isThai() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "e00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "e7f", 16))
        return true;

        return false;
    }

    bool U8char::isLao() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "e80", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "eff", 16))
        return true;

        return false;
    }

    bool U8char::isTibetan() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "f00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "fff", 16))
        return true;

        return false;
    }

    bool U8char::isMyanmar() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1000", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "109f", 16))
        return true;

        return false;
    }

    bool U8char::isGeorgian() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "10a0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "10ff", 16))
        return true;

        return false;
    }

    bool U8char::isHangulJamo() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1100", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "11ff", 16))
        return true;

        return false;
    }

    bool U8char::isEthiopic() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1200", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "137f", 16))
        return true;

        return false;
    }

    bool U8char::isEthiopicSupplement() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1380", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "139f", 16))
        return true;

        return false;
    }

    bool U8char::isCherokee() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "13a0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "13ff", 16))
        return true;

        return false;
    }

    bool U8char::isUnifiedCanadianAboriginalSyllabics() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1400", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "167f", 16))
        return true;

        return false;
    }

    bool U8char::isOgham() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1680", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "169f", 16))
        return true;

        return false;
    }

    bool U8char::isRunic() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "16a0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "16ff", 16))
        return true;

        return false;
    }

    bool U8char::isTagalog() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1700", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "171f", 16))
        return true;

        return false;
    }

    bool U8char::isHanunoo() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1720", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "173f", 16))
        return true;

        return false;
    }

    bool U8char::isBuhid() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1740", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "175f", 16))
        return true;

        return false;
    }

    bool U8char::isTagbanwa() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1760", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "177f", 16))
        return true;

        return false;
    }

    bool U8char::isKhmer() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1780", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "17ff", 16))
        return true;

        return false;
    }

    bool U8char::isMongolian() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1800", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "18af", 16))
        return true;

        return false;
    }

    bool U8char::isCombiningDiacriticalMarksExtended() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1ab0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "1aff", 16))
        return true;

        return false;
    }

    bool U8char::isCombiningDiacriticalMarksSupplement() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1dc0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "1dff", 16))
        return true;

        return false;
    }

    bool U8char::isLatinExtendedAdditional() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1e00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "1eff", 16))
        return true;

        return false;
    }

    bool U8char::isGreekExtended() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1f00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "1fff", 16))
        return true;

        return false;
    }

    bool U8char::isGeneralPunctuation() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2000", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "206f", 16))
        return true;

        return false;
    }

    bool U8char::isSuperscriptsAndSubscripts() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2070", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "209f", 16))
        return true;

        return false;
    }

    bool U8char::isCurrencySymbols() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "20a0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "20cf", 16))
        return true;

        return false;
    }

    bool U8char::isCombiningDiacriticalMarksForSymbols() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "20d0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "20ff", 16))
        return true;

        return false;
    }

    bool U8char::isLetterLikeSymbols() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2100", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "214f", 16))
        return true;

        return false;
    }

    bool U8char::isNumberForms() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2150", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "218f", 16))
        return true;

        return false;
    }

    bool U8char::isArrows() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2190", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "21ff", 16))
        return true;

        return false;
    }

    bool U8char::isMathematicalOperators() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2200", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "22ff", 16))
        return true;

        return false;
    }

    bool U8char::isMiscellaneousTechnical() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2300", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "23ff", 16))
        return true;

        return false;
    }

    bool U8char::isControlPictures() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2400", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "243f", 16))
        return true;

        return false;
    }

    bool U8char::isOpticalCharacterRecognition() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2440", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "245f", 16))
        return true;

        return false;
    }

    bool U8char::isEncolsedAlphanumerics() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2460", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "24ff", 16))
        return true;

        return false;
    }

    bool U8char::isBoxDrawing() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2500", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "257f", 16))
        return true;

        return false;
    }

    bool U8char::isBlockElements() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2580", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "259f", 16))
        return true;

        return false;
    }

    bool U8char::isGeometricShapes() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "25a0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "25ff", 16))
        return true;

        return false;
    }

    bool U8char::isMiscellaneousMathematicalSymbolsA() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "27c0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "27ef", 16))
        return true;

        return false;
    }

    bool U8char::isSupplementalArrowsA() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "27f0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "27ff", 16))
        return true;

        return false;
    }

    bool U8char::isBraillePatterns() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2800", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "28ff", 16))
        return true;

        return false;
    }

    bool U8char::isSupplementalArrowsB() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2900", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "297f", 16))
        return true;

        return false;
    }

    bool U8char::isMiscellaneousMathematicalSymbolsB() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2980", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "29ff", 16))
        return true;

        return false;
    }

    bool U8char::isSupplementalMathematicalOperators() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2a00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "2aff", 16))
        return true;

        return false;
    }

    bool U8char::isMiscellaneousSymbolsAndArrows() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2b00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "2bff", 16))
        return true;

        return false;
    }

    bool U8char::isMiscellaneousSymbols() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2600", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "26ff", 16))
        return true;

        return false;
    }

    bool U8char::isDingbats() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2700", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "27bf", 16))
        return true;

        return false;
    }

    bool U8char::isCjkRadicalsSupplement() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2e80", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "2eff", 16))
        return true;

        return false;
    }

    bool U8char::isKangxiRadicals() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2f00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "2fdf", 16))
        return true;

        return false;
    }

    bool U8char::isIdeographicsDescription() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2ff0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "2fff", 16))
        return true;

        return false;
    }

    bool U8char::isCjkSymbols_Punctuation() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "3000", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "303f", 16))
        return true;

        return false;
    }

    bool U8char::isHiragana() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "3040", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "309f", 16))
        return true;

        return false;
    }

    bool U8char::isKatakana() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "30a0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "30ff", 16))
        return true;

        return false;
    }

    bool U8char::isBopomofo() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "3100", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "312f", 16))
        return true;

        return false;
    }

    bool U8char::isHangulCompatibilityJamo() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "3130", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "318f", 16))
        return true;

        return false;
    }

    bool U8char::isKanbun() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "3190", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "319f", 16))
        return true;

        return false;
    }

    bool U8char::isBopomofoExtended() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "31a0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "31bf", 16))
        return true;

        return false;
    }

    bool U8char::isEnclosedCjkLetters_Months() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "3200", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "32ff", 16))
        return true;

        return false;
    }

    bool U8char::isCjkCompatibility() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "3300", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "33ff", 16))
        return true;

        return false;
    }

    bool U8char::isCjkUnifiedIdeographsExtensionA() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "3400", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "4db5", 16))
        return true;

        return false;
    }

    bool U8char::isCjkUnifiedIdeographs() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "4e00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "9fff", 16))
        return true;

        return false;
    }

    bool U8char::isYiSyllables() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "a000", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "a48f", 16))
        return true;

        return false;
    }

    bool U8char::isYiRadicals() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "a490", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "a4cf", 16))
        return true;

        return false;
    }

    bool U8char::isHangulSyllables() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "ac00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "d7a3", 16))
        return true;

        return false;
    }

    bool U8char::isHighSurrogates() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "d800", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "db7f", 16))
        return true;

        return false;
    }

    bool U8char::isHighPrivateUseSurrogates() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "db80", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "dbff", 16))
        return true;

        return false;
    }

    bool U8char::isLowSurrogates() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "dc00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "dfff", 16))
        return true;

        return false;
    }

    bool U8char::isPrivateUse() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "e000", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "f8ff", 16))
        return true;

        return false;
    }

    bool U8char::isCjkCompatibilityIdeographs() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "f900", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "faff", 16))
        return true;

        return false;
    }

    bool U8char::isAlphabeticPresentationForms() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "fb00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "fb4f", 16))
        return true;

        return false;
    }

    bool U8char::isArabicPresentationFormsA() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "fb50", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "fdff", 16))
        return true;

        return false;
    }

    bool U8char::isCombiningHalfMarks() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "fe20", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "fe2f", 16))
        return true;

        return false;
    }

    bool U8char::isCjkCompatibilityForms() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "fe30", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "fe4f", 16))
        return true;

        return false;
    }

    bool U8char::isSmallFormVariants() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "fe50", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "fe6f", 16))
        return true;

        return false;
    }

    bool U8char::isArabicPresentationFormsB() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "fe70", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "fefe", 16))
        return true;

        return false;
    }

    bool U8char::isSpecials() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if
        (
            ManipulateStrAsBaseN::equal(unicode, "feff", 16) or
            (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "fff0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "fffd", 16))
        )
        return true;

        return false;
    }

    bool U8char::isHalfwidth_Fullwidth_Forms() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "ff00", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "ffef", 16))
        return true;

        return false;
    }

    bool U8char::isIdeographicSymbolsAndPunctuation() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "16fe0", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "16fff", 16))
        return true;

        return false;
    }

    bool U8char::isSupplementalArrowsC() const
    {
        std::string unicode{ convertToUnicodeCodePoint() };
        if (ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "1f800", 16) and ManipulateStrAsBaseN::lessThanOrEqual(unicode, "1f8ff", 16))
        return true;

        return false;
    }


    bool operator==(const U8char& x, const U8char& y)
    {
        if (x.isEmpty() and y.isEmpty())
        return true;

        if ((x.isEmpty() and !y.isEmpty()) or (!x.isEmpty() and y.isEmpty()))
        return false;

        if (x.m_byteEncoding != y.m_byteEncoding)
        return false;
        
        if (ManipulateStrAsBaseN::notEqual(x.convertToUnicodeCodePoint(), y.convertToUnicodeCodePoint(), 16))
        return false;

        return true;
    }

    bool operator!=(const U8char& x, const U8char& y)
    { return !(x == y); }

    bool operator<(const U8char& x, const U8char& y)
    {
        if (x == y)
        return false;

        if (x.isEmpty() and !y.isEmpty())
        return true;
        else if (!x.isEmpty() and y.isEmpty())
        return false;

        if (x.m_byteEncoding > y.m_byteEncoding)
        return false;

        if (ManipulateStrAsBaseN::greaterThanOrEqual(x.convertToUnicodeCodePoint(), y.convertToUnicodeCodePoint(), 16))
        return false;

        return true;
    }

    bool operator>(const U8char& x, const U8char& y)
    {
        if (x == y)
        return false;

        if (x.isEmpty() and !y.isEmpty())
        return false;
        else if (!x.isEmpty() and y.isEmpty())
        return true;

        if (x.m_byteEncoding > y.m_byteEncoding)
        return true;

        if (ManipulateStrAsBaseN::lessThanOrEqual(x.convertToUnicodeCodePoint(), y.convertToUnicodeCodePoint(), 16))
        return false;

        return true;
    }

    bool operator<=(const U8char& x, const U8char& y)
    {
        if ((x == y) or (x < y))
        return true;

        return false;
    }

    bool operator>=(const U8char& x, const U8char& y)
    {
        if ((x == y) or (x > y))
        return true;

        return false;
    }


    std::ostream& operator<<(std::ostream& os, const U8char& d)
    { return os << d.to_string(); }

    std::ostream& operator<<(std::ostream& os, U8char::ByteEncoding d)
    {
        switch(d)
        {
            case U8char::ByteEncoding::one: return os << "[1 byte encoding]";
            case U8char::ByteEncoding::two: return os << "[2 byte encoding]";
            case U8char::ByteEncoding::three: return os << "[3 byte encoding]";
            case U8char::ByteEncoding::four: return os << "[4 byte encoding]";
            default: return  os << "[No Encoding]";
        }

        return os;
    }

    std::ostream& operator<<(std::ostream& os, const U8string& d)
    {
        for (auto&& i : d)
        os << i;

        return os;
    }

    std::ostream& operator<<(std::ostream& os, const U8string_view& d)
    {
        for (auto&& i : d)
        os << i;

        return os;
    }

    U8string convertAsciiStrToU8Str(const std::string& str)
    {
        U8string u8str{};
        struct makeU8Char
        {
            U8char inc(std::string::const_iterator& it)
            {
                U8char::ByteEncoding byteEncoding{U8char{std::string{*it}}.getByteEncoding()};
                std::string str{};
                
                for (auto i = 0; i < int(byteEncoding); ++i)
                {
                    str.push_back(*(it + i));
                }
                it += std::size_t(byteEncoding);

                return str;
            }
        };

        makeU8Char mku8{};
        for (auto i = str.begin(); i != str.end(); u8str.push_back(mku8.inc(i)))
        ;

        return u8str;
    }

    std::string convertU8StrToAsciiStr(const U8string& u8str)
    {
        std::string str{};

        for (auto&& i : u8str)
        str += i.to_string();
        
        return str;
    }

    namespace u8literals
    {
        U8char operator ""_u8(const char* x, std::size_t s)
        {
            if (s > 4)
            throw std::invalid_argument{R"(in function operator ""_u8, s of type std::size_t is greater than 4)"};
            return U8char{std::string{x, s}};
        }
        U8char operator ""_u8(char x)
        { return U8char{std::string{x}}; }

        U8string operator ""_u8str(const char* x, std::size_t s)
        { return convertAsciiStrToU8Str(std::string{x, s}); }
    }


    bool isPrime(int n) noexcept
    {
        if (n <= 1)
        return false;

        for (size_t i = 2; i < size_t(n); i++)
        {
            if(size_t(n) % i == 0)
            return false;
        }

        return true;
    }

    int sum_ofposbase10digits(int base10number)
    {
        //Throw an exception if base10number is negative.
        if(base10number < 0)
        throw std::invalid_argument{"Error base10number must be positive!"};
        
        /*Recursive termination condition.
        Returns the base10number back if it is not a base10 digit.*/
        if (base10number >= 0 and base10number <= 9)
        return base10number;
        //Holds the quotient and remainder of input base10number/divisor where the divisor is equal to 10.
        auto quot_rem{std::div(base10number, 10)};
        //The quotient now becomes the base10number.
        return sum_ofposbase10digits(quot_rem.quot) + quot_rem.rem;
    }

} // namespace Myfcn