#ifndef FILEOUTPUTSTREAM
#define FILEOUTPUTSTREAM

#pragma once

#include <fstream>
#include <chrono>
#include <utility>
#include <type_traits>
#include <filesystem>

namespace Streams
{
    template <typename CharT>
    class FileOutputStream
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
        
        //Holds the file output stream object.
        std::basic_ofstream<CharT>&& m_ofstream;

        /*An exculsive flag used to signal if an argument of a write function was writeed out.
        The condition for an agrument to be consider writeable:
        1.The argument must not be an I/O manipulator function, i.e: std::boolalpha, std::fixed e.t.c
        It is dubbed exculsive because only a handful of write funtions make use of the flag.*/
        bool m_isWritten{};

        //A flag used to signal if a write function has been called or not.
        //Used in conjunction with m_writeEndTime data member.
        bool m_is_write_fcn_used{};

        //Used to keep record of the endTime of a write operation
        std::chrono::sys_time<std::chrono::nanoseconds> m_writeEndTime{};

        /*Used in formatting functions to keep track of the first argument given*/
        bool m_isFirstArg1{true};
    public:
        /*Uses std::forward from the utility library to forward rvalue reference ofstream to data member m_ofstream as a rvalue.
        [Miscellany] It would have been just fine to use std::move instead, but the semantics behind what i am trying to do favours forwarding.
        Tips:
        If you wish to write to a file but not overwrite the file's contents completely, the std::ios_base::openmode should be 
        std::ios_base::in(the open mode used for reading). If std::ios_base::out is used instead, the contents of the file would be
        completely overwritten.*/
        constexpr FileOutputStream(std::basic_ofstream<CharT>&& ofstream) noexcept;

        FileOutputStream(const FileOutputStream&) = delete;
        FileOutputStream(FileOutputStream&&) = default;

        FileOutputStream& operator=(const FileOutputStream&) = delete;
        FileOutputStream& operator=(FileOutputStream&&) = default;

        constexpr ~FileOutputStream() = default;

        /*Takes in a variable amount of arguments and writes them to a file as they were(as far as those arguments have overloaded the
        insertion operator[operator<<] to work with it).
        
        [Note]: This function writes data to the output stream and moves the output stream indicator, 
        which means it is dependant on the position of the output stream indicator.

        Throws no exception, but the file output stream object my throw when there is a formatting error during write operations.*/
        template <typename Arg1, typename... Args>
        void write(const Arg1& arg1, const Args&... args);

        /*writews - write with spacing.
        Takes in a variable amount of arguments and writes each individual argument with the space character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it) to a file.
        
        [Note]: This function writes data to the output stream and moves the output stream indicator, 
        which means it is dependant on the position of the output stream indicator.
        
        Throws no exception, but the file output stream object my throw when there is a formatting error during write operations.
        */
        template <typename Arg1, typename... Args>
        void writews(const Arg1& arg1, const Args&... args);

        /*writewt - write with tabing.
        Takes in a variable amount of arguments and writes each individual argument with the tab character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it) to a file.
        
        [Note]: This function writes data to the output stream and moves the output stream indicator, 
        which means it is dependant on the position of the output stream indicator.
        
        Throws no exception, but the file output stream object my throw when there is a formatting error during write operations.*/
        template <typename Arg1, typename... Args>
        void writewt(const Arg1& arg1, const Args&... args);

        /*writewl - write with new line.
        Takes in a variable amount of arguments and writes each individual argument with the new line character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it) to a file.
        
        [Note]: This function writes data to the output stream and moves the output stream indicator, 
        which means it is dependant on the position of the output stream indicator.
        
        Throws no exception, but the file output stream object my throw when there is a formatting error during write operations.*/
        template <typename Arg1, typename... Args>
        void writewl(const Arg1& arg1, const Args&... args);

        /*writews_endl - write with spacing and end in a new line.
        Takes in a variable amount of arguments and writes each individual argument with the space character appended after it,
        expect the last argument in a *variable list* of arguments,
        and ends the write with the new line character(as far as those arguments have overloaded the insertion operator[operator<<]
        to work with it).

        [Note]: This function writes data to the output stream and moves the output stream indicator, 
        which means it is dependant on the position of the output stream indicator.
        
        Throws no exception, but the file output stream object my throw when there is a formatting error during write operations.
        [Miscellany] :  sets m_isWritten.*/
        template <typename Arg1, typename... Args>
        void writews_endl(const Arg1& arg1, const Args&... args);

        /*writewt_endl - write with tabing and end in a new line.
        Takes in a variable amount of arguments and writes each individual argument with the tab character appended after it,
        expect the last argument in a *variable list* of arguments,
        and ends the write with the new line character(as far as those arguments have overloaded the insertion operator[operator<<]
        to work with it).

        [Note]: This function writes data to the output stream and moves the output stream indicator, 
        which means it is dependant on the position of the output stream indicator.

        Throws no exception, but the file output stream object my throw when there is a formatting error during write operations.
        [Miscellany] :  sets m_isWritten.*/
        template <typename Arg1, typename... Args>
        void writewt_endl(const Arg1& arg1, const Args&... args);

        /*write_endl - write and end in a newline.
        Takes in a variable amount of arguments and writes them as they were, then ends the output in a newline
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it).

        [Note]: This function writes data to the output stream and moves the output stream indicator, 
        which means it is dependant on the position of the output stream indicator.

        Throws no exception, but the output stream object my throw when there is a formatting error during output operations.
        [Miscellany] :  sets m_isPrinted.*/
        template <typename Arg1, typename... Args>
        void write_endl(const Arg1& arg1, const Args&... args);

        std::basic_ostream<CharT>& 
        moveIndicator
        (std::basic_ofstream<CharT>::off_type number_of_bytes, std::basic_ofstream<CharT>::seekdir offset_direction = std::ios_base::beg);        

        std::basic_ofstream<CharT>::pos_type indicatorPosition();

        /*Returns a reference to the file output stream object given.(Preferred)*/
        constexpr std::basic_ofstream<CharT>& get() noexcept;

        /*Returns a reference to the file output stream object given.(Not Preferred)
        This method is not preferred because the underlying concept of deferencing a class object whoose data member is not a pointer is flawed.
        It is only defined here out of convenience - meaning it is easier to write this '*object' than use the preferred version 'object.get()'.*/
        constexpr std::basic_ofstream<CharT>& operator*() noexcept;

        /*Returns the time a write function finished its operation.
        The timezone_name parameter takes in the name of a timezone you wish the time to be in, default is "ETC/UTC".
        If no write function has been called prior to a function call to public member function endtime_of_write_fcn,
        the function returns the epoch time of std::chrono::system_clock.*/
        std::chrono::zoned_time<std::chrono::nanoseconds> endtime_of_write_fcn(const char* timezone_name = "Etc/UTC") const noexcept;

        
        void openRegularFile(const std::filesystem::path& file);
    private:
        /*An overload of write that takes no argument.
        Does no writing but is essential to stopping the writing operation while recording the time the operation stopped.*/
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
    constexpr FileOutputStream<CharT>::FileOutputStream(std::basic_ofstream<CharT>&& ofstream) noexcept
    :m_ofstream{ std::forward<std::basic_ofstream<CharT>>(ofstream) }
    {}

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void FileOutputStream<CharT>::write(const Arg1& arg1, const Args&... args)
    {   
        m_ofstream << arg1;
        write(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void FileOutputStream<CharT>::writews(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_ofstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_ofstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_ofstream << arg1;
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_ofstream << arg1 << static_cast<CharT>(' ');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_ofstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_ofstream << arg1 << static_cast<CharT>(' ');
            }
        }

        writews(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void FileOutputStream<CharT>::writewt(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_ofstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_ofstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_ofstream << arg1;
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_ofstream << arg1 << static_cast<CharT>('\t');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_ofstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_ofstream << arg1 << static_cast<CharT>('\t');
            }
        }
        
        writewt(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void FileOutputStream<CharT>::writewl(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_ofstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_ofstream << arg1;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_ofstream << arg1;
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_ofstream << arg1 << static_cast<CharT>('\n');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_ofstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_ofstream << arg1 << static_cast<CharT>('\n');
            }
        }
        
        writewl(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void FileOutputStream<CharT>::writews_endl(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        {
            m_ofstream << arg1;
            m_isWritten = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        {
            m_ofstream << arg1;
            m_isWritten = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        {
            m_ofstream << arg1;
            m_isWritten = false;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_ofstream << arg1 << static_cast<CharT>(' ');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_ofstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_ofstream << arg1 << static_cast<CharT>(' ');
            }

            m_isWritten = true;
        }

        writews_endl(args...);  
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void FileOutputStream<CharT>::writewt_endl(const Arg1& arg1, const Args&... args)
    {
        /*If any of the argument is an i/o manipulator implemented that has been implemented as a function pointer,
        these conditions make sure not to print the i/o manipulator with the format sepicification*/
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        {
            m_ofstream << arg1;
            m_isWritten = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        {
            m_ofstream << arg1;
            m_isWritten = false;
        }
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        {
            m_ofstream << arg1;
            m_isWritten = false;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                m_ofstream << arg1 << static_cast<CharT>('\t');
                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if (sizeof...(Args) == 0)
                {
                    m_ofstream << arg1;
                    m_isFirstArg1 = true;
                }
                else
                m_ofstream << arg1 << static_cast<CharT>('\t');
            }

            m_isWritten = true;
        }
        
        writewt_endl(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void FileOutputStream<CharT>::write_endl(const Arg1& arg1, const Args&... args)
    {
        if constexpr (std::is_same_v<Arg1, ios_base_fcn>)
        m_isWritten = false;
        else if constexpr (std::is_same_v<Arg1, basic_ios_fcn>)
        m_isWritten = false;
        else if constexpr (std::is_same_v<Arg1, basic_ostream_fcn>)
        m_isWritten = false;
        else
        m_isWritten = true;

        m_ofstream << arg1;

        write_endl(args...);
    }

    template <typename CharT>
    std::basic_ostream<CharT>&
    FileOutputStream<CharT>::moveIndicator(std::basic_ofstream<CharT>::off_type number_of_bytes, std::basic_ofstream<CharT>::seekdir offset_direction)
    { return m_ofstream.seekp(number_of_bytes, offset_direction); }   

    template <typename CharT>
    std::basic_ofstream<CharT>::pos_type FileOutputStream<CharT>::indicatorPosition()
    { return m_ofstream.tellp(); }

    template <typename CharT>
    constexpr std::basic_ofstream<CharT>& FileOutputStream<CharT>::get() noexcept
    { return m_ofstream; }

    template <typename CharT>
    constexpr std::basic_ofstream<CharT>& FileOutputStream<CharT>::operator*() noexcept
    { return m_ofstream; }

    template <typename CharT>
    std::chrono::zoned_time<std::chrono::nanoseconds> 
    FileOutputStream<CharT>::endtime_of_write_fcn(const char* timezone_name) const noexcept
    {
        if (!m_is_write_fcn_used)
        return std::chrono::zoned_time<std::chrono::nanoseconds>{timezone_name};

        return std::chrono::zoned_time{timezone_name, m_writeEndTime};
    }

    template <typename CharT>
    void FileOutputStream<CharT>::openRegularFile(const std::filesystem::path& file)
    { m_ofstream.open(file); }

    /*--------------------------------------------------Private------------------------------------------------------------*/

    template <typename CharT>
    void FileOutputStream<CharT>::write()
    {
        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }

    template <typename CharT>
    void FileOutputStream<CharT>::writews()
    {
        m_isFirstArg1 = true;
        /*Setting m_isFirst to true here is needed to make sure that even if only one argument is given to the overload that takes in
        a variable amount of arguments, m_isFisrtArg1 is still set to true.
        Example(where # is any format specification):
        write#(3); would equal 3#,

        The print# would set m_isFirstArg1 to false, and because the argument is only one, m_isFirstArg1 is never set to back to true,
        so using the same write#(or another format function since they all use the same boolean m_isFirstArg1) would have m_isFirstArg1
        set to false and therefore write the argument without any format specification.
        
        So calling this again:
        write#(3); would equal 3.

        [Note] Check the implementation of the overload that takes in a variable amount of arguments to understand more clearly.
        Appicable to any overload of a format function that takes no arguments, expect write_endl*/
        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }

    template <typename CharT>
    void FileOutputStream<CharT>::writewt()
    {
        m_isFirstArg1 = true;
        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }

    template <typename CharT>
    void FileOutputStream<CharT>::writewl()
    {
        m_isFirstArg1 = true;
        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }

    template <typename CharT>
    void FileOutputStream<CharT>::writews_endl()
    { 
        m_isFirstArg1 = true;
        write_endl();
    }

    template <typename CharT>
    void FileOutputStream<CharT>::writewt_endl()
    { 
        m_isFirstArg1 = true;
        write_endl();
    }

    template <typename CharT>
    void FileOutputStream<CharT>::write_endl()
    {
        if (m_isWritten)
        m_ofstream << static_cast<CharT>('\n');

        m_writeEndTime = std::chrono::system_clock::now();
        m_is_write_fcn_used = true;
    }

} // namespace Streams


#endif