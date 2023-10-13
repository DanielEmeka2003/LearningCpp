#ifndef STRINGINPUTSTREAM_H
#define STRINGINPUTSTREAM_H

#pragma once

#include <sstream>
#include <utility>
#include <cctype>
#include <algorithm>
#include <chrono>

namespace Streams
{
    template <typename CharT>
    class StringInputStream
    {
    private:
        //Holds string input stream object.
        std::basic_istringstream<CharT>&& m_isstream;
        
        //A flag used to signal if function insert has been called or not.
        //Used in conjunction with m_readEndTime data member.
        bool m_is_read_fcn_used{};

        std::chrono::sys_time<std::chrono::nanoseconds> m_readEndTime{};
    public:
        using StringType = std::basic_string<CharT>;

        static constexpr const char* e_failmessage{"[Invalid: Input type does not match the destination object type]"};
        static constexpr const char* e_eofmessage{"[End of File encountered]"};
        static constexpr const char* e_extraInputmessage{"[Invalid: Extraneous input(s) detected]"};
        

        constexpr StringInputStream(std::basic_istringstream<CharT>&& isstream) noexcept;

        
        StringInputStream(const StringInputStream&) = default;
        StringInputStream(StringInputStream&&) = default;

        StringInputStream& operator=(const StringInputStream&) = delete;
        StringInputStream& operator=(StringInputStream&&) = default;

        ~StringInputStream() noexcept = default;

        
        template <typename Arg1, typename... Args>
        void read(Arg1& arg1, Args&... args);

        
        template <typename Arg1, typename... Args>
        void read_(Arg1& arg1, Args&... args);

        
        void read_str(StringType& arg1, bool is_leadingWS_valid = false);

        /*
        Inserts data(const std::string&) to the string used by the input stream and resets the input stream indicator to the position given
        as the std::ios_base::openmode argument in the std::istringstream constructor.
        Behaves exactly like this:
        { 
            std::string arg{"Emeka Daniel"};
            std::istringstring is{}
            is.str(arg); 
        }*/
        void insert_str(const StringType& data) noexcept;

        /*
        Inserts data(std::string&&) to the string used by the input stream and resets the input stream indicator to the position given
        as the std::ios_base::openmode argument in the std::istringstream constructor.
        Behaves exactly like this:
        { 
            std::string arg{"Emeka Daniel"};
            std::istringstring is{}
            is.str(arg); 
        }*/
        void insert_str(StringType&& data) noexcept;

        /*Returns the string used by the input stream.
        Behaves like this:
        {
            StringInputStream sis{std::istringstream{"Emeka Daniel"}};
            sis.get().str();

            Or..

            std::istringstream is{"Emeka Daniel"};
            is.str();
        }*/
        StringType getStrUsedByStream() noexcept;

        /*Clears the string used by the input stream.
        Behaves exactly like this:
        {
            StringInputStream sis{std::istringstream{"Emeka Daniel"}};
            sis.insert_str("");

            Or..

            std::istringstream is{"Emeka Daniel"};
            is.str("");
        }*/
        void emptyStrUsedByStream() noexcept;

        /*Returns a reference to the string input stream object given.(Preferred)*/
        constexpr std::basic_istringstream<CharT>& get() noexcept;

        /*Returns a reference to the string input stream object given.(Not Preferred)
        This method is not preferred because the underlying concept of deferencing a class object whoose data member is not a pointer is flawed.
        It is only defined here out of convenience - meaning it is easier to write this '*object' than use the preferred version 'object.get()'.*/
        constexpr std::basic_istringstream<CharT>& operator*() noexcept;

        /*Prepares a string for insertion into the string used by the input stream by parsing the string for space characters and assigning them
        the end of extraction/reading indicator character - which is the new line character and returns the parsed and modified string.

        Strings given to this function should generally be in this format: 
        {"$ $ $ $ $ $....."} where $ is any potential fixed or multibyte character.
        Each extractable character[$] should be separated by one or more leading space characters and can optionally include a begin and end
        space character like so: {" $ $ $ $ "}.
        Example:
        {
            using namespace Streams;

            std::string str{"1 Daniel 2 5.6 Emeka"};

            //would equal this: {"1#Daniel#2#5.6#Emeka#"} where # = the newline character
            str = System::cinput.prepare_str_for_insertion(str);
        }
        */
        StringType prepare_str_for_insertion(StringType str) noexcept;

        std::basic_istream<CharT>& 
        moveIndicator
        (std::basic_istream<CharT>::off_type number_of_bytes, std::basic_istream<CharT>::seekdir offset_direction = std::ios_base::beg);        

        std::basic_istream<CharT>::pos_type getIndicatorPos();

        /*Returns the time a read function finished its operation.
        The timezone_name parameter takes in the name of a time zone you wish the time to be in, default is "ETC/UTC".
        If no read function has been called prior to a function call to public member endtime_of_read_fcn,
        the function returns the epoch time of std::chrono::system_clock.*/
        std::chrono::zoned_time<std::chrono::nanoseconds> endtime_of_read_fcn(const char* timezone_name = "Etc/UTC") const noexcept;
    private:
        /*An overload of read that takes no argument.
        Does no extracting/reading but is essential to stopping the extraction operation while recording the time the operation stopped.*/
        void read();

        /*An overload of read that takes no argument.
        Does no extracting/reading but is essential to stopping the extraction operation while recording the time the operation stopped.*/
        void read_();

        //Checks if the input stream is alright.
        //For error handling.
        void validateStringInputStream();

        // Calls std::cin.ignore(std::numeric<std::streamsize>::max(), '\n') to clear input stream.
        //For error handling.
        void ignoreRemainingInputs();
    };

    template <typename CharT>
    constexpr StringInputStream<CharT>::StringInputStream(std::basic_istringstream<CharT>&& isstream) noexcept 
    :m_isstream{ std::forward<std::basic_istringstream<CharT>>(isstream) }
    {}


    template <typename CharT>
    template <typename Arg1, typename... Args>
    void StringInputStream<CharT>::read(Arg1& arg1, Args&... args)
    {
        m_isstream >> arg1;

        if (m_isstream.eof())
        {
            m_isstream.clear();
            throw std::runtime_error{ StringInputStream<CharT>::e_eofmessage };
        }
        
        read(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void StringInputStream<CharT>::read_(Arg1& arg1, Args&... args)
    {   
        m_isstream >> arg1;
        
        InputStream<CharT>::ignoreRemainingInputs();
        validateStringInputStream();
        
        read(args...);
    }

    template <typename CharT>
    void StringInputStream<CharT>::read_str(StringType& arg1, bool is_leadingWS_valid)
    {
        if (is_leadingWS_valid)
        std::getline(m_isstream, arg1);
        else //std::ws to ignore leading whitespace.
        std::getline(m_isstream >> std::ws, arg1);

        if (m_isstream.eof())
        {
            m_isstream.clear();
            throw std::runtime_error{ StringInputStream<CharT>::e_eofmessage };
        }

        m_readEndTime = std::chrono::system_clock::now();
        m_is_read_fcn_used = true;
    }

        
    template <typename CharT>
    void StringInputStream<CharT>::insert_str(const StringType& data) noexcept
    { m_isstream.str(data);}

        
    template <typename CharT>
    void StringInputStream<CharT>::insert_str(StringType&& data) noexcept
    { m_isstream.str(data); }
        
    template <typename CharT>
    StringInputStream<CharT>::StringType StringInputStream<CharT>::getStrUsedByStream() noexcept
    { return m_isstream.str(); }

    template <typename CharT>
    void StringInputStream<CharT>::emptyStrUsedByStream() noexcept
    { return m_isstream.str(""); }

    template <typename CharT>
    constexpr std::basic_istringstream<CharT>& StringInputStream<CharT>::get() noexcept
    { return m_isstream; }

    template <typename CharT>
    constexpr std::basic_istringstream<CharT>& StringInputStream<CharT>::operator*() noexcept
    { return m_isstream; }

    template <typename CharT>
    StringInputStream<CharT>::StringType StringInputStream<CharT>::prepare_str_for_insertion
    (StringType str) noexcept
    {
        //If the last element is not a space character, push_back a space character to str
        if (!std::isspace(str.back()))
        {
            str.push_back(' ');
        }
        //Replace space characters with the new line character
        //Neccessary for making the string extractable
        std::ranges::replace(str, ' ', '\n');
        
        return str;
    }

    template <typename CharT>
    std::basic_istream<CharT>&
    StringInputStream<CharT>::moveIndicator
    (std::basic_istream<CharT>::off_type number_of_bytes, std::basic_istream<CharT>::seekdir offset_direction)
    { return m_isstream.seekg(number_of_bytes, offset_direction); }   

    template <typename CharT>
    std::basic_istream<CharT>::pos_type StringInputStream<CharT>::getIndicatorPos()
    { return m_isstream.tellg(); }


    template <typename CharT>
    std::chrono::zoned_time<std::chrono::nanoseconds> 
    StringInputStream<CharT>::endtime_of_read_fcn(const char* timezone_name) const noexcept
    {
        if (!m_is_read_fcn_used)
        return std::chrono::zoned_time<std::chrono::nanoseconds>{timezone_name};

        return std::chrono::zoned_time{timezone_name, m_readEndTime};
    }


/*--------------------------------------------------Private------------------------------------------------------------*/
    
    template <typename CharT>
    void StringInputStream<CharT>::read()
    {
        m_readEndTime = std::chrono::system_clock::now();
        m_is_read_fcn_used = true;
    }

    template <typename CharT>
    void StringInputStream<CharT>::read_()
    {
        m_readEndTime = std::chrono::system_clock::now();
        m_is_read_fcn_used = true;
    }

    template <typename CharT>
    void StringInputStream<CharT>::validateStringInputStream()
    {
        if (m_isstream.eof())
        {
            m_isstream.clear();
            throw std::runtime_error{ StringInputStream<CharT>::e_eofmessage };
        }

        if (m_isstream.fail())
        {  
            
            m_isstream.clear();
            throw std::runtime_error
            { StringInputStream<CharT>::e_failmessage };      
        }

        if (m_isstream.gcount() > 1)
        {
            throw std::runtime_error{ StringInputStream<CharT>::e_extraInputmessage };
        }
    }
    
    template <typename CharT>
    void StringInputStream<CharT>::ignoreRemainingInputs()
    {
        //Ignore all data still left in the input stream.
        m_isstream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

} // namespace Myfcn


#endif