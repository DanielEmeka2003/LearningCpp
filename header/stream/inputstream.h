#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#pragma once

#include <istream>
#include <type_traits>
#include <chrono>
#include <string>
#include <limits>
#include <utility>
#include <stdexcept>

#include "output.h"

namespace Streams
{
    template <typename CharT>
    class InputStream final
    {
    private:
        //Holds the input stream object.
        std::basic_istream<CharT>&& m_in;

        //A flag used to signal if an input function has been called or not.
        //Used in conjunction with m_readEndTime data member.
        bool m_is_read_fcn_used{};

        std::chrono::sys_time<std::chrono::nanoseconds> m_readEndTime{};

    public:
        static constexpr const char* e_failmessage{"[Invalid: Input type does not match the destination object type]"};
        static constexpr const char* e_eofmessage{"[End of File encountered]"};
        static constexpr const char* e_extraInputmessage{"[Invalid: Extraneous input(s) detected]"};

        using StringType = std::basic_string<CharT>;


        /*Takes in a reference to an input stream object.
        This class models an aggregrate relationship with its data member(m_in)- meaning it is neither responsible for the
        creation or destruction of any input stream object given to it, it only presents an easier inerface on using input stream objects.*/
        constexpr InputStream(std::basic_istream<CharT>&& in) noexcept;
        
        //Copying is not allowed.
        InputStream(const InputStream&) = default;
        //Moving is not allowed.
        InputStream(InputStream&&) = default;

        //Copy assigning is not allowed.
        InputStream& operator=(const InputStream&) = delete;
        //Move assigning is not allowed.
        InputStream& operator=(InputStream&&) = delete;

        ~InputStream() noexcept = default;

        /*
        Takes in a variable amount of arguments(as far as those arguments have overloaded the extraction operator[operator>>] to work with it)
        and attempts to read the data from the input stream to the arguments.
        Behaves exactly like this:
        {
            int a{};
            std::cin >> a;
            //no error handling code
        }
        This function does not perform error handling, but it does however shutsdown the process when an end of file is triggered, 
        which closes the input stream.

        [Note] If extraction to a std::string is needed, public member function read_str is better suited for the job.

        Does not throw an exception, but the input stream object may throw.*/
        template <typename Arg1, typename... Args>
        void read(Arg1& arg1, Args&... args);

        /*
        Takes in a variable amount of arguments(as far as those arguments have overloaded the extraction operator[operator>>] to work with it)
        and attempts to read the data from the input stream to the arguments.
        Behaves exactly like this:
        {
            int a{};
            std::cin >> a;
            //with some error handling code following it
        }
        This function performs error handling for extraction/read operations that aren't met with the expected types or fail for some reason
        by continously prompting the user till he/she enters a valid input.
        It also shutsdown the process when an end of file is triggered, which closes the input stream.

        [Note] If extraction to a std::string is needed, public member function read_str is better suited for the job.

        Does not throw an exception, but the input stream object may throw.*/
        template <typename Arg1, typename... Args>
        void read_(Arg1& arg1, Args&... args);

        /*
        Takes in a reference to a std::string and boolean is_leadingWhiteSpace_valid(set to false by default).
        If is_leadingWhiteSpace_valid is set to true, std::getline(m_in, arg1) is used to read input stream data to the string parameter,
        else std::getline(m_in >> std::ws, arg1) is used.

        Because basic_istream objects or objects inherited from basic_istream ignores leading whitespace by default, 
        leaving parameter is_leadingWhiteSpace_valid set to false is advisable.

        It also shutsdown the process when an end of file is triggered, which closes the input stream.

        Does not throw an exception, but the input stream object may throw.*/
        void read_str(StringType& arg1, bool is_leadingWS_valid = false);

        /*Returns a reference to the input stream object given.(Preferred)*/
        constexpr std::basic_istream<CharT>& get() noexcept;

        /*Returns a reference to the input stream object given.(Not Preferred)
        This method is not preferred because the underlying concept of deferencing a class object whoose data member is not a pointer is flawed.
        It is only defined here out of convenience - meaning it is easier to write this '*object' than use the preferred version 'object.get()'.*/
        constexpr std::basic_istream<CharT>& operator*() noexcept;

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

        // Calls std::cin.ignore(std::numeric<std::streamsize>::max(), '\n') to clear input stream.
        //For error handling.
        void ignoreRemainingInputs();

        //Checks if the input stream is alright.
        //For error handling
        void isInputStreamStillValid();
    };

    template <typename CharT>
    constexpr InputStream<CharT>::InputStream(std::basic_istream<CharT>&& in) noexcept
    : m_in{ std::forward<std::basic_istream<CharT>>(in) }
    {}
    

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void InputStream<CharT>::read(Arg1& arg1, Args&... args)
    {
        m_in >> arg1;

        if (m_in.eof())
        {
            m_in.clear();

            System::cerror.print_endl(e_eofmessage);
            std::exit(EXIT_SUCCESS);
        }

        read(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void InputStream<CharT>::read_(Arg1& arg1, Args&... args)
    {
        while (true)
        {
            try
            {
                m_in >> arg1;

                ignoreRemainingInputs();
                isInputStreamStillValid();
                
                break; //need here, do not move it
            }
            catch(const std::runtime_error& e)
            {
                System::cerror.print_endl(e.what());
                System::cerror.printws("<Try Again>:");
            }
        }
        
        read(args...);
    }

    template <typename CharT>
    void InputStream<CharT>::read_str(StringType& arg1, bool is_leadingWS_valid)
    {
        if (is_leadingWS_valid)
        std::getline(m_in, arg1);
        else //std::ws to ignore leading whitespace.
        std::getline(m_in >> std::ws, arg1);

        if (m_in.eof())
        {
            m_in.clear();

            System::cerror.print_endl(e_eofmessage);
            std::exit(EXIT_SUCCESS);
        }

        m_readEndTime = std::chrono::system_clock::now();
        m_is_read_fcn_used = true;
    }

    template <typename CharT>
    constexpr std::basic_istream<CharT>& InputStream<CharT>::get() noexcept
    { return m_in; }

    template <typename CharT>
    constexpr std::basic_istream<CharT>& InputStream<CharT>::operator*() noexcept
    { return m_in; }

    template <typename CharT>
    std::basic_istream<CharT>&
    InputStream<CharT>::moveIndicator
    (std::basic_istream<CharT>::off_type number_of_bytes, std::basic_istream<CharT>::seekdir offset_direction)
    { return m_in.seekg(number_of_bytes, offset_direction); }   

    template <typename CharT>
    std::basic_istream<CharT>::pos_type InputStream<CharT>::getIndicatorPos()
    { return m_in.tellg(); }


    template <typename CharT>
    std::chrono::zoned_time<std::chrono::nanoseconds> 
    InputStream<CharT>::endtime_of_read_fcn(const char* timezone_name) const noexcept
    {
        if (!m_is_read_fcn_used)
        return std::chrono::zoned_time<std::chrono::nanoseconds>{timezone_name};

        return std::chrono::zoned_time{timezone_name, m_readEndTime};
    }


/*--------------------------------------------------Private------------------------------------------------------------*/

    template <typename CharT>
    void InputStream<CharT>::read()
    {
        m_readEndTime = std::chrono::system_clock::now();
        m_is_read_fcn_used = true;
    }

    template <typename CharT>
    void InputStream<CharT>::read_()
    {
        m_readEndTime = std::chrono::system_clock::now();
        m_is_read_fcn_used = true;
    }

    template <typename CharT>
    void InputStream<CharT>::ignoreRemainingInputs()
    {
        //Ignore all data still left in the input stream.
        m_in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    template <typename CharT>
    void InputStream<CharT>::isInputStreamStillValid()
    {
        if (m_in.eof())
        {
            m_in.clear();

            System::cerror.print_endl(e_eofmessage);
            std::exit(EXIT_SUCCESS);
        }
        
        if (m_in.fail())
        {
            m_in.clear();

            ignoreRemainingInputs();
            throw std::runtime_error{e_failmessage};
        }
        
        if (m_in.gcount() > 1)
        {
            throw std::runtime_error{e_extraInputmessage};
        }
    }


} // namespace Myfcn::System



#endif