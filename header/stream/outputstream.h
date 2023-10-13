#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#pragma once

#include <ostream>
#include <type_traits>
#include <chrono>
#include <utility>

namespace Streams
{
    template <typename CharT>
    class OutputStream final
    {
    private:
        /*A function type that takes a std::ios_base& parameter and returns a std::ios_base&.
        Used in the formatting output functions[printws, printwt, printwl, printws_endl and printwt_endl] to check
        if an argument is a I/O manipulator.*/
        using ios_base_fcn = std::ios_base& (std::ios_base&);

        /*A function template type that takes a std::basic_ios<CharT>& parameter and returns a std::basic_ios<CharT>&.
        Used in the formatting output functions[printws, printwt, printwl, printws_endl and printwt_endl] to check
        if an argument is a I/O manipulator.*/
        using basic_ios_fcn = std::basic_ios<CharT, std::char_traits<CharT>>& (std::basic_ios<CharT, std::char_traits<CharT>>&);

        /*A function template type that takes a std::basic_ostream<CharT>& parameter and returns a std::basic_ostream<CharT>&.
        Used in the formatting output functions[printws, printwt, printwl, printws_endl and printwt_endl] to check
        if an argument is a I/O manipulator.*/
        using basic_ostream_fcn = std::basic_ostream<CharT, std::char_traits<CharT>>& (std::basic_ostream<CharT, std::char_traits<CharT>>&);

        //Holds the output stream object.
        std::basic_ostream<CharT>&& m_out;

        /*An exculsive flag used to signal if an argument of an output function was written out.
        The condition for an agrument to be consider printable:
        1.The argument must not be an I/O manipulator function, i.e: std::boolalpha, std::fixed e.t.c
        It is dubbed exculsive because only a handful of output funtions make use of the flag.*/
        bool m_isPrinted{};

        //A flag used to signal if an output function has been called or not.
        //Used in conjunction with m_printEndTime data member.
        bool m_is_print_fcn_used{};
    	
        //Used to keep record of the endTime of a print operation
        std::chrono::sys_time<std::chrono::nanoseconds> m_printEndTime{};

        /*Used in formatting functions to keep track of the first argument given*/
        bool m_isFirstArg1{true};
    public:
        constexpr OutputStream(std::basic_ostream<CharT>&& out) noexcept;

        //Copying is not allowed.
        OutputStream(const OutputStream&) = default;
        //Moving is not allowed.
        OutputStream(OutputStream&&) = default;

        //Copy assigning is not allowed.
        OutputStream& operator=(const OutputStream&) = delete;
        //Move assigning is not allowed.
        OutputStream& operator=(OutputStream&&) = delete;

        virtual ~OutputStream() noexcept = default;
        
        /*Takes in a variable amount of arguments and prints them as they were(as far as those arguments have overloaded the
        insertion operator[operator<<] to work with it).

        Throws no exception, but the output stream object my throw when there is a formatting error during output operations.*/
        template <typename Arg1, typename... Args>
        void print(const Arg1& arg1, const Args&... args);

        /*printws - print with spacing.
        Takes in a variable amount of arguments and prints each individual argument with the space character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it).

        Throws no exception, but the output stream object my throw when there is a formatting error during output operations.*/
        template <typename Arg1, typename... Args>
        void printws(const Arg1& arg1, const Args&... args);

        /*printwt - print with tabing.
        Takes in a variable amount of arguments and prints each individual argument with the tab character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it).

        Throws no exception, but the output stream object my throw when there is a formatting error during output operations.*/
        template <typename Arg1, typename... Args>
        void printwt(const Arg1& arg1, const Args&... args);

        /*printwl - print with newline.
        Takes in a variable amount of arguments and prints each individual argument with the new line character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it).

        Throws no exception, but the output stream object my throw when there is a formatting error during output operations.*/
        template <typename Arg1, typename... Args>
        void printwl(const Arg1& arg1, const Args&... args);

        /*printws_endl - print with spacing and end in a newline.
        Takes in a variable amount of arguments and outputs each individual argument with the space character appended after it,
        expect the last argument in a *variable list* of arguments,
        and ends the print with the new line character (as far as those arguments have overloaded the insertion operator[operator<<]
        to work with it).

        Throws no exception, but the output stream object my throw when there is a formatting error during output operations.
        [Miscellany] :  sets m_isPrinted.*/
        template <typename Arg1, typename... Args>
        void printws_endl(const Arg1& arg1, const Args&... args);

        /*printwt_endl - print with tabing and end in a newline.
        Takes in a variable amount of arguments and prints each individual argument with the tab character appended after it,
        expect the last argument in a *variable list* of arguments,
        and ends the print with the new line character (as far as those arguments have overloaded the insertion operator[operator<<]
        to work with it).

        Throws no exception, but the output stream object my throw when there is a formatting error during output operations.
        [Miscellany] :  sets m_isPrinted.*/
        template <typename Arg1, typename... Args>
        void printwt_endl(const Arg1& arg1, const Args&... args);

        /*print_endl - print and end in a newline.
        Takes in a variable amount of arguments and prints them as they were, then ends the print in a newline
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it).

        Throws no exception, but the output stream object my throw when there is a formatting error during output operations.
        [Miscellany] :  sets m_isPrinted.*/
        template <typename Arg1, typename... Args>
        void print_endl(const Arg1& arg1, const Args&... args);

        /*Returns a reference to the output stream object given.(Preferred)*/
        constexpr std::basic_ostream<CharT>& get() noexcept;

        /*Returns a reference to the output stream object given.(Not Preferred)
        This method is not preferred because the underlying concept of deferencing a class object whoose data member is not a pointer is flawed.
        It is only defined here out of convenience - meaning it is easier to print this '*object' than use the preferred version 'object.get()'.*/
        constexpr std::basic_ostream<CharT>& operator*() noexcept;

        std::basic_ostream<CharT>& 
        moveIndicator
        (std::basic_ostream<CharT>::off_type number_of_bytes, std::basic_ostream<CharT>::seekdir offset_direction = std::ios_base::beg);        

        std::basic_ostream<CharT>::pos_type getIndicatorPos();

        /*Returns the time a print function finished its operation.
        The timezone_name parameter takes in the name of a timezone you wish the time to be in, default is "ETC/UTC".
        If no output function has been called prior to a function call to public member function endtime_of_print_fcn,
        the function returns the epoch time of std::chrono::system_clock.*/
        std::chrono::zoned_time<std::chrono::nanoseconds> endtime_of_print_fcn(const char* timezone_name = "Etc/UTC") const noexcept;
    private:
        /*An overload of print that takes no argument.
        Does no printing but is essential to stopping the printing operation while recording the time the operation stopped.*/
        void print();
        /*An overload of printws that takes no argument.
        Does no printing but is essential to stopping the printing operation while recording the time the operation stopped.*/
        void printws();
        /*An overload of printwt that takes no argument.
        Does no printing but is essential to stopping the printing operation while recording the time the operation stopped.*/
        void printwt();
        /*An overload of printwl that takes no argument.
        Does no printing but is essential to stopping the printing operation while recording the time the operation stopped.*/
        void printwl();
        /*An overload of printws_endl that takes no argument.
        Does no printing but is essential to stopping the printing operation and calls print_endl for implementing the rest.*/
        void printws_endl();
        /*An overload of printwt_endl that takes no argument.
        Does no printing but is essential to stopping the printing operation and calls print_endl for implementing the rest.*/
        void printwt_endl();
        /*Inserts a newline character to the output stream while implementing m_isPrinted and recording the time printing operations
        stopped. Also called by both printws_endl and printwt_endl overloads that take no arguments.*/
        void print_endl();
    };

    template <typename CharT>
    constexpr OutputStream<CharT>::OutputStream(std::basic_ostream<CharT>&& out) noexcept
    : m_out{ std::forward<std::basic_ostream<CharT>>(out) }
    {}

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::print(const Arg1& arg1, const Args&... args)
    {   
        m_out << arg1;
        print(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::printws(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_out << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_out << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_out << arg1;
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_out << arg1 << static_cast<CharT>(' ');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: printw#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_out << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_out << arg1 << static_cast<CharT>(' ');
            }
            
        }
        

        printws(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::printwt(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_out << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_out << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_out << arg1;
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_out << arg1 << static_cast<CharT>('\t');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: printw#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_out << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_out << arg1 << static_cast<CharT>('\t');
            }
        }
        
        printwt(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::printwl(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_out << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_out << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_out << arg1;
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_out << arg1 << static_cast<CharT>('\n');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: printw#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_out << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_out << arg1 << static_cast<CharT>('\n');
            }
        }
        
        printwl(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::printws_endl(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        {
            m_out << arg1;
            m_isPrinted = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        {
            m_out << arg1;
            m_isPrinted = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        {
            m_out << arg1;
            m_isPrinted = false;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/   
            if (m_isFirstArg1)
            {
                m_out << arg1 << static_cast<CharT>(' ');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: printw#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_out << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_out << arg1 << static_cast<CharT>(' ');
            }

            m_isPrinted = true;
        }

        printws_endl(args...);  
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::printwt_endl(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        {
            m_out << arg1;
            m_isPrinted = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        {
            m_out << arg1;
            m_isPrinted = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        {
            m_out << arg1;
            m_isPrinted = false;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_out << arg1 << static_cast<CharT>('\t');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: printw#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_out << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_out << arg1 << static_cast<CharT>('\t');
            }

            m_isPrinted = true;
        }
        
        printwt_endl(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::print_endl(const Arg1& arg1, const Args&... args)
    {
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_isPrinted = false;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_isPrinted = false;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_isPrinted = false;
        else
        m_isPrinted = true;

        m_out << arg1;

        print_endl(args...);
    }

    template <typename CharT>
    constexpr std::basic_ostream<CharT>& OutputStream<CharT>::get() noexcept
    { return m_out; }

    template <typename CharT>
    constexpr std::basic_ostream<CharT>& OutputStream<CharT>::operator*() noexcept
    { return m_out; }

    template <typename CharT>
    std::basic_ostream<CharT>&
    OutputStream<CharT>::moveIndicator
    (std::basic_ostream<CharT>::off_type number_of_bytes, std::basic_ostream<CharT>::seekdir offset_direction)
    { return m_out.seekp(number_of_bytes, offset_direction); }   

    template <typename CharT>
    std::basic_ostream<CharT>::pos_type OutputStream<CharT>::getIndicatorPos()
    { return m_out.tellp(); }

    template <typename CharT>
    std::chrono::zoned_time<std::chrono::nanoseconds> 
    OutputStream<CharT>::endtime_of_print_fcn(const char* timezone_name) const noexcept
    {
        if (!m_is_print_fcn_used)
        return std::chrono::zoned_time<std::chrono::nanoseconds>{timezone_name};

        return std::chrono::zoned_time{timezone_name, m_printEndTime};
    }

    /*--------------------------------------------------Private------------------------------------------------------------*/

    template <typename CharT>
    void OutputStream<CharT>::print()
    {
        m_printEndTime = std::chrono::system_clock::now();
        m_is_print_fcn_used = true;
    }

    template <typename CharT>
    void OutputStream<CharT>::printws()
    {
        m_isFirstArg1 = true;
        /*Setting m_isFirst to true here is needed to make sure that even if only one argument is given to the overload that takes in
        a variable amount of arguments, m_isFisrtArg1 is still set to true.
        Example(where # is any format specification):
        print#(3); would equal 3#,

        The print# would set m_isFirstArg1 to false, and because the argument is only one, m_isFirstArg1 is never set to back to true,
        so using the same print#(or another format function since they all use the same boolean m_isFirstArg1) would have m_isFirstArg1
        set to false and therefore print the argument without any format specification.
        
        So calling this again:
        print#(3); would equal 3.

        [Note] Check the implementation of the overload that takes in a variable amount of arguments to understand more clearly.
        Appicable to any overload of a format function that takes no arguments, expect print_endl.*/
        m_printEndTime = std::chrono::system_clock::now();
        m_is_print_fcn_used = true;
    }

    template <typename CharT>
    void OutputStream<CharT>::printwt()
    {
        m_isFirstArg1 = true;
        m_printEndTime = std::chrono::system_clock::now();
        m_is_print_fcn_used = true;
    }

    template <typename CharT>
    void OutputStream<CharT>::printwl()
    {
        m_isFirstArg1 = true;
        m_printEndTime = std::chrono::system_clock::now();
        m_is_print_fcn_used = true;
    }

    template <typename CharT>
    void OutputStream<CharT>::printws_endl()
    { 
        m_isFirstArg1 = true;
        print_endl();
    }

    template <typename CharT>
    void OutputStream<CharT>::printwt_endl()
    { 
        m_isFirstArg1 = true;
        print_endl();
    }

    template <typename CharT>
    void OutputStream<CharT>::print_endl()
    {
        if (m_isPrinted)
        m_out << static_cast<CharT>('\n');

        m_printEndTime = std::chrono::system_clock::now();
        m_is_print_fcn_used = true;
    }

    
    
} // namespace Myfcn::System


#endif