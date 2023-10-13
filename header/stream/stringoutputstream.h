#ifndef STRINGOUTPUTSTREAM_H
#define STRINGOUTPUTSTREAM_H

#pragma once

#include <sstream>
#include <utility>
#include <chrono>

namespace Streams
{
    template <typename CharT>
    class StringOutputStream
    {
    private:
    /*A function type that takes a std::ios_base& parameter and returns a std::ios_base&.
        Used in the formatting output functions[writews, writewt, writewl, writews_endl and writewt_endl] to check
        if an argument is a I/O manipulator.*/
        using ios_base_fcn = std::ios_base& (std::ios_base&);

        /*A function template type that takes a std::basic_ios<CharT>& parameter and returns a std::basic_ios<CharT>&.
        Used in the formatting output functions[writews, writewt, writewl, writews_endl and writewt_endl] to check
        if an argument is a I/O manipulator.*/
        using basic_ios_fcn = std::basic_ios<CharT, std::char_traits<CharT>>& (std::basic_ios<CharT, std::char_traits<CharT>>&);

        /*A function template type that takes a std::basic_ostream<CharT>& parameter and returns a std::basic_ostream<CharT>&.
        Used in the formatting output functions[writews, writewt, writewl, writews_endl and writewt_endl] to check
        if an argument is a I/O manipulator.*/
        using basic_ostream_fcn = std::basic_ostream<CharT, std::char_traits<CharT>>& (std::basic_ostream<CharT, std::char_traits<CharT>>&);

        //Holds string output stream object.
        std::basic_ostringstream<CharT>&& m_osstream;

        /*An exculsive flag used to signal if an argument of a write function was writeed out.
        The condition for an agrument to be consider writeable:
        1.The argument must not be an I/O manipulator function, i.e: std::boolalpha, std::fixed e.t.c
        It is dubbed exculsive because only a handful of write funtions make use of the flag.*/
        bool m_isWritten{};
        
        //A flag used to signal if any of the write functions has been called or not.
        //Used in conjunction with m_writeEndTime data member.
        bool m_is_write_fcn_used{};

        //Used to keep record of the endTime of a write operation
        std::chrono::sys_time<std::chrono::nanoseconds> m_writeEndTime{};

        /*Used in formatting functions to keep track of the first argument given*/
        bool m_isFirstArg1{true};
    public:
        /*Uses std::forward from the utility library to forward rvalue reference osstream to data member m_osstream as a rvalue.
        [Miscellany] It would have been just fine to use std::move instead, but the semantics behind what i am trying to do favours forwarding.*/
        constexpr StringOutputStream(std::basic_ostringstream<CharT>&& osstream) noexcept;

        //Copying not allowed.
        StringOutputStream(const StringOutputStream&) = default;
        StringOutputStream(StringOutputStream&&) = default;

        //Copy assigning not allowed.
        StringOutputStream& operator=(const StringOutputStream&) = delete;
        StringOutputStream& operator=(StringOutputStream&&) = default;

        ~StringOutputStream() noexcept = default;

        /*
        Takes in a variable amount of arguments and writes them
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it). to the string used by the 
        output stream.
        Behaves exactly like this:
        { 
            std::basic_ostringstream<CharT> os{};
            os << argument;
        }
        [Note]: This function writes data to the output stream and moves the output stream indicator, 
        which means it is dependant on the position of the output stream indicator.
        Example:
        {
            using namespace Streams;

            //std::ios_base::out for always keep the indicator on the begining of the stream.
            StringOutputStream sos{std::ostringstream{"Emeka Daniel", std::ios_base::out}};

            //start a write where the indicator(which is at the begining of the stream) is and move it while writing.
            sos.write(123);

            System::coutput.printwl(std::boolalpha, sos.get_streamdata() == "123ka Daniel"); //prints true.
        }
        Throws no exception, but the output stream object may throw when formatting goes wrong.
        */
        template <typename Arg1, typename... Args>
        void write(const Arg1& arg1, const Args&... args);

        /*writews - write with spacing.
        Takes in a variable amount of arguments and writes each individual argument with the space character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it) to the string used by the 
        output stream.
        Behaves exactly like this:
        { 
            std::basic_ostringstream<CharT> os{};
            os << argument << ' ';
            //and this:
            os << argument1 << ' ' << argument2 << ' ' << argument3;
        }
        [Note]: This function writes data to the output stream and moves the output stream indicator, 
        which means it is dependant on the position of the output stream indicator.
        }
        
        Throws no exception, but the file output stream object my throw when there is a formatting error during write operations.
        */
        template <typename Arg1, typename... Args>
        void writews(const Arg1& arg1, const Args&... args);

        template <typename Arg1, typename... Args>
        void writewt(const Arg1& arg1, const Args&... args);

        template <typename Arg1, typename... Args>
        void writewl(const Arg1& arg1, const Args&... args);

        template <typename Arg1, typename... Args>
        void writews_endl(const Arg1& arg1, const Args&... args);

        template <typename Arg1, typename... Args>
        void writewt_endl(const Arg1& arg1, const Args&... args);

        template <typename Arg1, typename... Args>
        void write_endl(const Arg1& arg1, const Args&... args);

        /*
        Inserts data(const std::string&) to the string used by the output stream and resets the output stream indicator to the position given
        as the std::ios_base::openmode argument in the std::ostringstream constructor.
        Behaves exactly like this:
        { 
            std::string arg{"Emeka Daniel"};
            std::ostringstring os{}
            os.str(arg); 
        }
        [Note]: Unlike public member function StringOutputStream::write, this function is independant on the position of the output stream
        indicator. 
        Example:
        {
            using namespace Streams;

            //std::ios_base::ate for always move the indicator to the end of the stream.
            StringOutputStream sos{std::ostringstream{"Emeka Daniel", std::ios_base::ate}};
            
            sos.write(123);//function write is dependant on the indicator so sos.get_streamdata() would equal "Emeka Daniel123"

            sos.insert_str("123");//function insert_str is independant on the indicator so sos.get_streamdata() would equal "123"
        }
        */
        void insert_str(const std::string& data) noexcept;

        /*
        Inserts data(std::string&&) to the string used by the output stream and resets the output stream indicator to the position given
        as the std::ios_base::openmode argument in the std::ostringstream constructor.
        Behaves exactly like this:
        { 
            std::string arg{"Emeka Daniel"};
            std::ostringstring os{}
            os.str(arg); 
        }
        [Note]: Unlike public member function StringOutputStream::write, this function is independant on the position of the output stream
        indicator. 
        Example:
        {
            using namespace Streams;

            //std::ios_base::ate for always move the indicator to the end of the stream.
            StringOutputStream sos{std::ostringstream{"Emeka Daniel", std::ios_base::ate}};
            
            sos.write(123);//function write is dependant on the indicator so sos.get_streamdata() would equal "Emeka Daniel123"

            sos.insert_str("123");//function insert_str is independant on the indicator so sos.get_streamdata() would equal "123"
        }
        */
        void insert_str(std::string&& data) noexcept;

        /*Returns the string used by the output stream.
        Behaves exactly like this:
        {
            StringOutputStream sos{std::ostringstream{"Emeka Daniel"}};
            sos.get().str();

            Or..

            std::ostringstream os{"Emeka Daniel"};
            os.str();
        }*/
        std::string getStrUsedByStream() noexcept;

        /*Clears the string used by the output stream.
        Behaves exactly like this:
        {
            StringOutputStream sos{std::ostringstream{"Emeka Daniel"}};
            sos.insert_str("");

            Or..

            std::ostringstream os{"Emeka Daniel"};
            os.str("");
        }*/
        void emptyStrUsedByStream() noexcept;

        std::basic_ostream<CharT>& 
        moveIndicator
        (std::basic_ostringstream<CharT>::off_type number_of_bytes, std::basic_ostringstream<CharT>::seekdir offset_direction = std::ios_base::beg);        

        std::basic_ostringstream<CharT>::pos_type getIndicatorPos();

        /*Returns a reference to the string output stream object given.(Preferred)*/
        constexpr std::basic_ostringstream<CharT>& get() noexcept;

        /*Returns a reference to the string output stream object given.(Not Preferred)
        This method is not preferred because the underlying concept of deferencing a class object whoose data member is not a pointer is flawed.
        It is only defined here out of convenience - meaning it is easier to write this '*object' than use the preferred version 'object.get()'.*/
        constexpr std::basic_ostringstream<CharT>& operator*() noexcept;

        /*Returns the time, in a particular time zone, an write function finished its operation.
        The timezone_name parameter takes in the name of a time zone you wish the time to be in, default is "ETC/UTC".
        If function write has not been called prior to a function call to endtime_of_write_fcn, 
        the function returns the epoch time of std::chrono::system_clock*/
        std::chrono::zoned_time<std::chrono::nanoseconds> endtime_of_write_fcn(const char* timezone_name = "Etc/UTC") const noexcept;
    private:
        void write();
        /*An overload of writews that takes no argument.
        Does no writing but is essential to stopping the writing operation while recording the time the operation stopped.*/
        void writews();
        /*An overload of writewt that takes no argument.
        Does no writing but is essential to stopping the writing operation while recording the time the operation stopped.*/
        void writewt();
        /*An overload of writewl that takes no argument.
        Does no writing but is essential to stopping the writing operation while recording the time the operation stopped.*/
        void writewl();
        /*An overload of writews_endl that takes no argument.
        Does no writing but is essential to stopping the writing operation and calls write_endl for implementing the rest.*/
        void writews_endl();
        /*An overload of writewt_endl that takes no argument.
        Does no writing but is essential to stopping the writing operation and calls write_endl for implementing the rest.*/
        void writewt_endl();

        /*Inserts a newline character to the file output stream while implementing m_isWritten and recording the time writing operations
        stopped. Also called by both writews_endl and writewt_endl overloads that take no arguments.*/
        void write_endl();
    };

    template <typename CharT>
    constexpr StringOutputStream<CharT>::StringOutputStream(std::basic_ostringstream<CharT>&& osstream) noexcept 
    : m_osstream{ std::forward<std::basic_ostringstream<CharT>>(osstream) }
    {}

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void StringOutputStream<CharT>::write(const Arg1& arg1, const Args&... args)
    {
        m_osstream << arg1;
        write(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void StringOutputStream<CharT>::writews(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_osstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_osstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_osstream << arg1;
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets printed with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_osstream << arg1 << static_cast<CharT>(' ');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_osstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_osstream << arg1 << static_cast<CharT>(' ');
            }
        }

        writews(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void StringOutputStream<CharT>::writewt(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_osstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_osstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_osstream << arg1;
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_osstream << arg1 << static_cast<CharT>('\t');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_osstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_osstream << arg1 << static_cast<CharT>('\t');
            }
        }
        
        writewt(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void StringOutputStream<CharT>::writewl(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_osstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_osstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_osstream << arg1;
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_osstream << arg1 << static_cast<CharT>('\n');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_osstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_osstream << arg1 << static_cast<CharT>('\n');
            }
        }
        
        writewl(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void StringOutputStream<CharT>::writews_endl(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        {
            m_osstream << arg1;
            m_isWritten = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        {
            m_osstream << arg1;
            m_isWritten = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        {
            m_osstream << arg1;
            m_isWritten = false;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_osstream << arg1 << static_cast<CharT>(' ');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_osstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_osstream << arg1 << static_cast<CharT>(' ');
            }

            m_isWritten = true;
        }

        writews_endl(args...);  
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void StringOutputStream<CharT>::writewt_endl(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        {
            m_osstream << arg1;
            m_isWritten = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        {
            m_osstream << arg1;
            m_isWritten = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        {
            m_osstream << arg1;
            m_isWritten = false;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_osstream << arg1 << static_cast<CharT>('\t');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_osstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_osstream << arg1 << static_cast<CharT>('\t');
            }

            m_isWritten = true;
        }
        
        writewt_endl(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void StringOutputStream<CharT>::write_endl(const Arg1& arg1, const Args&... args)
    {
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_isWritten = false;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_isWritten = false;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_isWritten = false;
        else
        m_isWritten = true;

        m_osstream << arg1;

        write_endl(args...);
    }

    template <typename CharT>
    void StringOutputStream<CharT>::insert_str(const std::string& data) noexcept
    { m_osstream.str(data); }

    template <typename CharT>
    void StringOutputStream<CharT>::insert_str(std::string&& data) noexcept
    { m_osstream.str(data); }

    template <typename CharT>
    std::string StringOutputStream<CharT>::getStrUsedByStream() noexcept
    { return m_osstream.str(); }

        
    template <typename CharT>
    void StringOutputStream<CharT>::emptyStrUsedByStream() noexcept
    { return m_osstream.str(""); }

    
    template <typename CharT>
    std::basic_ostream<CharT>&
    StringOutputStream<CharT>::moveIndicator
    (std::basic_ostringstream<CharT>::off_type number_of_bytes, std::basic_ostringstream<CharT>::seekdir offset_direction)
    { return m_osstream.seekp(number_of_bytes, offset_direction); }  

    template <typename CharT>
    std::basic_ostringstream<CharT>::pos_type StringOutputStream<CharT>::getIndicatorPos()
    { return m_osstream.tellp(); }

    template <typename CharT>
    constexpr std::basic_ostringstream<CharT>& StringOutputStream<CharT>::get() noexcept
    { return m_osstream; }

    
    template <typename CharT>
    constexpr std::basic_ostringstream<CharT>& StringOutputStream<CharT>::operator*() noexcept
    { return m_osstream; }

    
    template <typename CharT>
    std::chrono::zoned_time<std::chrono::nanoseconds> 
    StringOutputStream<CharT>::endtime_of_write_fcn(const char* timezone_name) const noexcept
    {
        if (!m_is_write_fcn_used)
        return std::chrono::zoned_time<std::chrono::nanoseconds>{timezone_name};

        return std::chrono::zoned_time{timezone_name, m_writeEndTime};
    }

    /*--------------------------------------------------Private----------------------------------------------*/

    template <typename CharT>
    void StringOutputStream<CharT>::write()
    {
        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }

    template <typename CharT>
    void StringOutputStream<CharT>::writews()
    {
        m_isFirstArg1 = true;
        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }

    template <typename CharT>
    void StringOutputStream<CharT>::writewt()
    {
        m_isFirstArg1 = true;
        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }

    template <typename CharT>
    void StringOutputStream<CharT>::writewl()
    {
        m_isFirstArg1 = true;
        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }

    template <typename CharT>
    void StringOutputStream<CharT>::writews_endl()
    { 
        m_isFirstArg1 = true;
        write_endl();
    }

    template <typename CharT>
    void StringOutputStream<CharT>::writewt_endl()
    { 
        m_isFirstArg1 = true;
        write_endl();
    }

    template <typename CharT>
    void StringOutputStream<CharT>::write_endl()
    {
        if (m_isWritten)
        m_osstream << static_cast<CharT>('\n');

        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }
    
} // namespace Myfcn


#endif