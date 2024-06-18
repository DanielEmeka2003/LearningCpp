#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#pragma once

#include <ostream>
#include <type_traits>
#include <chrono>
#include <stdexcept>
#include <iomanip>

namespace Streams
{
    template <typename CharT>
    class OutputStream
    {
    private:
        /*Ouput manipulators implemted as function pointers*/

        using ios_base_fcn = std::ios_base& (std::ios_base&);
        using basic_ios_fcn = std::basic_ios<CharT, std::char_traits<CharT>>& (std::basic_ios<CharT, std::char_traits<CharT>>&);
        using basic_ostream_fcn = std::basic_ostream<CharT, std::char_traits<CharT>>& (std::basic_ostream<CharT, std::char_traits<CharT>>&);

        /*Output manipulators implemented as functions
        Since manipulators defined in the <iomanip> header are compiler specific, using decltype to obtain the return type of
        each of the manipulators is needed.
        [Note] The arguments are meaningless.*/

        using setw_fcn_rT = decltype(std::setw(0));
        using setprecision_fcn_rT = decltype(std::setprecision(0));
        using setfill_fcn_rT = decltype(std::setfill<CharT>(CharT{}));
        using setbase_fcn_rT = decltype(std::setbase(0));
        using setiosflags_fcn_rT = decltype(std::setiosflags(std::ios_base::boolalpha));
        using resetiosflags_fcn_rT = decltype(std::resetiosflags(std::ios_base::boolalpha));

        //Holds the output stream object.
        std::basic_ostream<CharT>* m_out{};

        /*An exculsive flag used to signal if an argument of an output function was written out.
        The condition for an agrument to be consider writeable:
        1.The argument must not be an I/O manipulator function, i.e: std::boolalpha, std::fixed e.t.c
        It is dubbed exculsive because only a handful of output funtions make use of the flag.*/
        bool m_isWritten{};

        /*Used in formatting functions to keep track of the first arg1 argument given*/
        bool m_isFirstArg1{true};

    protected:
        //A flag used to signal if an output function has been called or not.
        //Used in conjunction with m_writeEndTime data member.
        bool m_isWriteFcnUsed{};
    	
        //Used to keep record of the endTime of a write/output operation
        std::chrono::sys_time<std::chrono::nanoseconds> m_writeEndTime{};

        /*This constructor is meant to be used only by classes that derived from this class(OutputStream<CharT>).
        Because while this class(OutputStream<CharT>) models an aggerate relationship with it's output stream object data member,
        classes that derived from this class are expected to model a composite relationship with it's own output stream object
        data member, therefore instead of setting the std::basic_ostream<CharT, Traits> pointer(m_out) to the memory of a predefined 
        std::basic_ostream<CharT, Traits> lvalue, you set it to the derived's own output stream object, which has been
        acquired through composition*/
        constexpr OutputStream() = default;

    public:
        static constexpr std::string_view failbit_m{"OutputStream<CharT>::(*m_out) set failbit!"};
        static constexpr std::string_view badbit_m{"OutputStream<CharT>::(*m_out) set badbit!"};
        static constexpr std::string_view failbit_badbit_m{"OutputStream<CharT>::(*m_out) set failbit and badbit!"};

        /*Takes a lvalue std::basic_ostream<CharT, Traits> pointer
        This class models an aggregrate relationship through its data member(m_out)- meaning it is neither responsible for the
        creation or destruction of any output stream object given to it.*/
        constexpr OutputStream(std::basic_ostream<CharT>* out) noexcept;

        OutputStream(const OutputStream&) = delete;
        OutputStream(OutputStream&&) = delete;

        OutputStream& operator=(const OutputStream&) = delete;
        OutputStream& operator=(OutputStream&&) = delete;

        virtual ~OutputStream() noexcept;

        /*Takes in a variable amount of arguments and writes them as they were(as far as those arguments have overloaded the
        insertion operator[operator<<] to work with it).

        If either the error state flag std::ios_base::failbit or std::ios_base::badbit are set during an insertion attempt, 
        this function first clears the std::ios_base::iostate to std::ios_base::goodbit and throws exception std::runtime_error.*/
        template <typename Arg1, typename... Args>
        void write(const Arg1& arg1, const Args&... args);

        /*writews - write with spacing.
        Takes in a variable amount of arguments and writes each individual argument with the space character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it).

        If either the error state flag std::ios_base::failbit or std::ios_base::badbit are set during an insertion attempt, 
        this function first clears the std::ios_base::iostate to std::ios_base::goodbit and throws exception std::runtime_error.*/
        template <typename Arg1, typename... Args>
        void writews(const Arg1& arg1, const Args&... args);

        /*writewt - write with tabing.
        Takes in a variable amount of arguments and writes each individual argument with the tab character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it).

        If either the error state flag std::ios_base::failbit or std::ios_base::badbit are set during an insertion attempt, 
        this function first clears the std::ios_base::iostate to std::ios_base::goodbit and throws exception std::runtime_error.*/
        template <typename Arg1, typename... Args>
        void writewt(const Arg1& arg1, const Args&... args);

        /*writewl - write with newline.
        Takes in a variable amount of arguments and writes each individual argument with the new line character appended after it,
        expect the last argument in a *variable list* of arguments,
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it).

        If either the error state flag std::ios_base::failbit or std::ios_base::badbit are set during an insertion attempt, 
        this function first clears the std::ios_base::iostate to std::ios_base::goodbit and throws exception std::runtime_error.*/
        template <typename Arg1, typename... Args>
        void writewl(const Arg1& arg1, const Args&... args);

        /*writews_nl - write with spacing and end in a newline.
        Takes in a variable amount of arguments and outputs each individual argument with the space character appended after it,
        expect the last argument in a *variable list* of arguments,
        and ends the write with the new line character (as far as those arguments have overloaded the insertion operator[operator<<]
        to work with it).

        If either the error state flag std::ios_base::failbit or std::ios_base::badbit are set during an insertion attempt, 
        this function first clears the std::ios_base::iostate to std::ios_base::goodbit and throws exception std::runtime_error.
        [Miscellany] :  sets m_isWritten.*/
        template <typename Arg1, typename... Args>
        void writews_nl(const Arg1& arg1, const Args&... args);

        /*writewt_nl - write with tabing and end in a newline.
        Takes in a variable amount of arguments and writes each individual argument with the tab character appended after it,
        expect the last argument in a *variable list* of arguments,
        and ends the write with the new line character (as far as those arguments have overloaded the insertion operator[operator<<]
        to work with it).

        If either the error state flag std::ios_base::failbit or std::ios_base::badbit are set during an insertion attempt, 
        this function first clears the std::ios_base::iostate to std::ios_base::goodbit and throws exception std::runtime_error.
        [Miscellany] :  sets m_isWritten.*/
        template <typename Arg1, typename... Args>
        void writewt_nl(const Arg1& arg1, const Args&... args);

        /*write_nl - write and end in a newline.
        Takes in a variable amount of arguments and writes them as they were, then ends the write in a newline
        (as far as those arguments have overloaded the insertion operator[operator<<] to work with it).

        If either the error state flag std::ios_base::failbit or std::ios_base::badbit are set during an insertion attempt, 
        this function first clears the std::ios_base::iostate to std::ios_base::goodbit and throws exception std::runtime_error.
        [Miscellany] :  sets m_isWritten.*/
        template <typename Arg1, typename... Args>
        void write_nl(const Arg1& arg1, const Args&... args);

        /*Returns a reference to the output stream object given.(Preferred)*/
        constexpr std::basic_ostream<CharT>& get() noexcept;

        /*Returns a reference to the output stream object given.(Not Preferred)*/
        constexpr std::basic_ostream<CharT>& operator*() noexcept;

        /*Sets additional format flags.
        Behaves exactly like this:
        {
           std::cout.setf(flag);
        }*/
        OutputStream<CharT>& setFmtf(std::ios_base::fmtflags flags);
        /*Sets additional format flags that support bitgroups.
        Behaves exactly like this:
        {
           std::cout.setf(flag, flag_bitgroup);
        }*/
        OutputStream<CharT>& setFmtf(std::ios_base::fmtflags flags, std::ios_base::fmtflags mask);
        /*clears the current format flag setting and sets only the given flag.
        Behaves exactly like this:
        {
            std::cout.flag(flag);
        }
        */
        OutputStream<CharT>& clearFmtf(std::ios_base::fmtflags flags);
        /*Gets the current format flag setting.
        Behaves exactly like this:
        {
            bool flagSetting = std::cout.flag();
        }
        */
        std::ios_base::fmtflags getFmtf() const;

        OutputStream<CharT>& setWidth(std::streamsize new_width);
        std::streamsize getWidth() const;

        OutputStream<CharT>& setPrecision(std::streamsize new_precision);
        std::streamsize getPrecision() const ;

        OutputStream<CharT>& setFillc(CharT ch);
        CharT getFillc() const;

        OutputStream<CharT>& flush();

        OutputStream<CharT>& left();
        OutputStream<CharT>& right();
        OutputStream<CharT>& internal();
        OutputStream<CharT>& dec();
        OutputStream<CharT>& oct();
        OutputStream<CharT>& hex();
        OutputStream<CharT>& scientific();
        OutputStream<CharT>& fixed();
        OutputStream<CharT>& boolalpha();
        OutputStream<CharT>& noboolalpha();
        OutputStream<CharT>& showpos();
        OutputStream<CharT>& noshowpos();
        OutputStream<CharT>& showpoint();
        OutputStream<CharT>& noshowpoint();
        OutputStream<CharT>& showbase();
        OutputStream<CharT>& noshowbase();
        OutputStream<CharT>& unitbuf();
        OutputStream<CharT>& nounitbuf();
        OutputStream<CharT>& uppercase();
        OutputStream<CharT>& nouppercase();

        bool good() const;
        bool fail() const;
        bool bad() const;
        bool operator!() const;
        explicit operator bool() const;
        OutputStream<CharT>& setStatef(std::ios_base::iostate state);
        OutputStream<CharT>& clearStatef(std::ios_base::iostate state);
        std::ios_base::iostate getStatef() const;

        OutputStream<CharT>& moveIndicator
        (std::basic_ios<CharT>::off_type nOfBytes, std::ios_base::seekdir offset_direction = std::ios_base::beg);

        OutputStream<CharT>& moveIndicator(std::basic_ios<CharT>::pos_type pos);

        std::basic_ios<CharT>::pos_type getIndicatorPos();

        std::locale imbueLoc(const std::locale& loc);
        std::locale getLoc() const;

        /*Returns the time a write funct;n finished its operation.
        The timezone_name parameter takes in the name of a timezone you wish the time to be in, default is "ETC/UTC".
        If no output function has been called prior to a function call to public member function endTime_of_writeFcn,
        the function returns the epoch time of std::chrono::system_clock.*/
        std::chrono::zoned_time<std::chrono::nanoseconds> endTime_of_writeFcn(const char* timezone_name = "Etc/UTC") const noexcept;

    private:
        /*Checks if Arg is an ouput manipulator*/
        template <typename Arg>
        bool isOManipulator();

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
        /*An overload of writews_nl that takes no argument.
        Does no writing but is essential to stopping the writing operation and calls write_nl for implementing the rest.*/
        void writews_nl();
        /*An overload of writewt_nl that takes no argument.
        Does no writing but is essential to stopping the writing operation and calls write_nl for implementing the rest.*/
        void writewt_nl();
        /*Inserts a newline character to the output stream while implementing m_isWritten and recording the time writing operations
        stopped. Also called by both writews_nl and writewt_nl overloads that take no arguments.*/
        void write_nl();
        /*Checks if m_out is nullptr and throws an exception.*/
        void check_m_out();

    protected:
        /*Checks if the std::ios_base::failbit is set, if true,
        clears the state flag to std::ios_base::goodbit and throws exception std::runtime_error{failbit_m.data()}.
        This function is made virtual and protected so derived classes may override it's behaviour, if the
        default behaviour does not suit the current application of the function.*/
        virtual void failbitCheck();

        /*Checks if the std::ios_base::badbit is set, if true,
        clears the state flag to std::ios_base::goodbit and throws exception std::runtime_error{badbit_m.data()}.
        This function is made virtual and protected so derived classes may override it's behaviour, if the
        default behaviour does not suit the current application of the function.*/
        virtual void badbitCheck();

        virtual void failbit_badbit_check();

        /*sets the data member m_out*/
        void set_m_out(std::basic_ostream<CharT>* derived_out);
    };

    template <typename CharT>
    constexpr OutputStream<CharT>::OutputStream(std::basic_ostream<CharT>* out) noexcept
    : m_out{out}
    {}

    template <typename CharT>
    OutputStream<CharT>::~OutputStream() noexcept
    { m_out = nullptr; }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::write(const Arg1& arg1, const Args&... args)
    {
        check_m_out();

        (*m_out) << arg1;

        if (!m_out->good())
        {
            failbit_badbit_check();
            failbitCheck();
            badbitCheck();
        }

        write(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::writews(const Arg1& arg1, const Args&... args)
    {
        check_m_out();

        if (isOManipulator<Arg1>())
        {
            (*m_out) << arg1;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                (*m_out) << arg1;
                m_out->put(m_out->widen(' '));

                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if constexpr (sizeof...(Args) == 0)
                {
                    (*m_out) << arg1;
                    m_isFirstArg1 = true;
                }
                else
                {
                    (*m_out) << arg1; 
                    m_out->put(m_out->widen(' '));
                }
            }
            
        }

        if (!m_out->good())
        {
            failbit_badbit_check();
            failbitCheck();
            badbitCheck();
        }
                
        writews(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::writewt(const Arg1& arg1, const Args&... args)
    {
        check_m_out();

        if (isOManipulator<Arg1>())
        {
            (*m_out) << arg1;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                (*m_out) << arg1;
                m_out->put(m_out->widen('\t'));

                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if constexpr (sizeof...(Args) == 0)
                {
                    (*m_out) << arg1;
                    m_isFirstArg1 = true;
                }
                else
                {
                    (*m_out) << arg1;
                    m_out->put(m_out->widen('\t'));
                }
            }
        }

        if (!m_out->good())
        {
            failbit_badbit_check();
            failbitCheck();
            badbitCheck();
        }
        
        writewt(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::writewl(const Arg1& arg1, const Args&... args)
    {
        check_m_out();

        if (isOManipulator<Arg1>())
        {
            (*m_out) << arg1;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                (*m_out) << arg1;
                m_out->put(m_out->widen('\n'));

                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if constexpr (sizeof...(Args) == 0)
                {
                    (*m_out) << arg1;
                    m_isFirstArg1 = true;
                }
                else
                {
                    (*m_out) << arg1;
                    m_out->put(m_out->widen('\n'));
                }
            }
        }

        if (!m_out->good())
        {
            failbit_badbit_check();
            failbitCheck();
            badbitCheck();
        }
        
        writewl(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::writews_nl(const Arg1& arg1, const Args&... args)
    {
        check_m_out();

        if (isOManipulator<Arg1>())
        {
            (*m_out) << arg1;
            m_isWritten = false;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/   
            if (m_isFirstArg1)
            {
                (*m_out) << arg1;
                m_out->put(m_out->widen(' '));

                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if constexpr (sizeof...(Args) == 0)
                {
                    (*m_out) << arg1;
                    m_isFirstArg1 = true;
                }
                else
                {
                    (*m_out) << arg1;
                    m_out->put(m_out->widen(' '));
                }
            }

            m_isWritten = true;
        }

        if (!m_out->good())
        {
            failbit_badbit_check();
            failbitCheck();
            badbitCheck();
        }

        writews_nl(args...);  
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::writewt_nl(const Arg1& arg1, const Args&... args)
    {
        check_m_out();

        if (isOManipulator<Arg1>())
        {
            (*m_out) << arg1;
            m_isWritten = false;
        }
        else
        {
            /*Checks if the argument is the first in a list of arguments to ensure that the first argument
            gets written with the format sepicification*/
            if (m_isFirstArg1)
            {
                (*m_out) << arg1;
                m_out->put(m_out->widen('\t'));

                m_isFirstArg1 = false;
            }
            else
            {
                /*If the argument is the last argument in a list of arguments, it is written without the format specification,
                So this: writew#(1, 2, 3, 4) would equal this: 1#2#3#4, where # is any format specification*/
                if constexpr (sizeof...(Args) == 0)
                {
                    (*m_out) << arg1;
                    m_isFirstArg1 = true;
                }
                else
                {
                    (*m_out) << arg1;
                    m_out->put(m_out->widen('\t'));
                }
            }

            m_isWritten = true;
        }

        if (!m_out->good())
        {
            failbit_badbit_check();
            failbitCheck();
            badbitCheck();
        }
        
        writewt_nl(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void OutputStream<CharT>::write_nl(const Arg1& arg1, const Args&... args)
    {
        check_m_out();

        if (isOManipulator<Arg1>())
        {   
            m_isWritten = false;
        }
        else
        {
            m_isWritten = true;
        }

        (*m_out) << arg1;

        if (!m_out->good())
        {
            failbit_badbit_check();
            failbitCheck();
            badbitCheck();
        }

        write_nl(args...);
    }

    template <typename CharT>
    constexpr std::basic_ostream<CharT>& OutputStream<CharT>::get() noexcept
    { return (*m_out); }

    template <typename CharT>
    constexpr std::basic_ostream<CharT>& OutputStream<CharT>::operator*() noexcept
    { return (*m_out); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::setFmtf(std::ios_base::fmtflags flags)
    { return (m_out->setf(flags), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::setFmtf(std::ios_base::fmtflags flags, std::ios_base::fmtflags mask)
    { return (m_out->setf(flags, mask), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::clearFmtf(std::ios_base::fmtflags flags)
    { return (m_out->flags(flags), *this); }

    template <typename CharT>
    std::ios_base::fmtflags OutputStream<CharT>::getFmtf() const
    { return m_out->flags(); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::setWidth(std::streamsize new_width)
    { return (m_out->width(new_width), *this); }

    template <typename CharT>
    std::streamsize OutputStream<CharT>::getWidth() const
    { return m_out->width(); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::setPrecision(std::streamsize new_precision)
    { return (m_out->precision(new_precision), *this); }

    template <typename CharT>
    std::streamsize OutputStream<CharT>::getPrecision() const
    { return m_out->width(); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::setFillc(CharT ch)
    { return (m_out->fill(ch), *this); }

    template <typename CharT>
    CharT OutputStream<CharT>::getFillc() const
    { return m_out->fill(); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::flush()
    { return (m_out->flush(), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::left()
    { return (m_out->setf(std::ios_base::left, std::ios_base::adjustfield), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::right()
    { return (m_out->setf(std::ios_base::right, std::ios_base::adjustfield), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::internal()
    { return (m_out->setf(std::ios_base::internal, std::ios_base::adjustfield), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::dec()
    { return (m_out->setf(std::ios_base::dec, std::ios_base::basefield), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::oct()
    { return (m_out->setf(std::ios_base::oct, std::ios_base::basefield), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::hex()
    { return (m_out->setf(std::ios_base::hex, std::ios_base::basefield), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::scientific()
    { return (m_out->setf(std::ios_base::scientific, std::ios_base::floatfield), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::fixed()
    { return (m_out->setf(std::ios_base::fixed, std::ios_base::basefield), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::boolalpha()
    { return (m_out->setf(std::ios_base::boolalpha), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::noboolalpha()
    { return (m_out->unsetf(std::ios_base::boolalpha), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::showpos()
    { return (m_out->setf(std::ios_base::showpos), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::noshowpos()
    { return (m_out->unsetf(std::ios_base::showpos), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::showpoint()
    { return (m_out->setf(std::ios_base::showpoint), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::noshowpoint()
    { return (m_out->unsetf(std::ios_base::showpoint), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::showbase()
    { return (m_out->setf(std::ios_base::showbase), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::noshowbase()
    { return (m_out->unsetf(std::ios_base::showbase), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::unitbuf()
    { return (m_out->setf(std::ios_base::unitbuf), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::nounitbuf()
    { return (m_out->unsetf(std::ios_base::unitbuf), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::uppercase()
    { return (m_out->setf(std::ios_base::uppercase), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::nouppercase()
    { return (m_out->unsetf(std::ios_base::uppercase), *this); }
    
    template <typename CharT>
    bool OutputStream<CharT>::good() const
    { return m_out->good(); }

    template <typename CharT>
    bool OutputStream<CharT>::fail() const
    { return m_out->fail(); }

    template <typename CharT>
    bool OutputStream<CharT>::bad() const
    { return m_out->bad(); }

    template <typename CharT>
    bool OutputStream<CharT>::operator!() const
    { return m_out->operator!(); }

    template <typename CharT>
    OutputStream<CharT>::operator bool() const
    { return static_cast<bool>(*m_out); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::setStatef(std::ios_base::iostate state)
    { return (m_out->setstate(state), *this); }

    template <typename CharT>
    OutputStream<CharT>& OutputStream<CharT>::clearStatef(std::ios_base::iostate state)
    { return (m_out->clear(state), *this); }

    template <typename CharT>
    std::ios_base::iostate OutputStream<CharT>::getStatef() const
    { return m_out->rdstate(); }

    template <typename CharT>
    OutputStream<CharT>&
    OutputStream<CharT>::moveIndicator (std::basic_ios<CharT>::off_type nOfBytes, std::ios_base::seekdir offset_direction)
    { return (m_out->seekp(nOfBytes, offset_direction), *this); }

    template <typename CharT>
    OutputStream<CharT>&
    OutputStream<CharT>::moveIndicator (std::basic_ios<CharT>::pos_type pos)
    { return (m_out->seekp(pos), *this); }

    template <typename CharT>
    std::basic_ios<CharT>::pos_type OutputStream<CharT>::getIndicatorPos()
    { return m_out->tellp(); }

    template <typename CharT>
    std::locale OutputStream<CharT>::imbueLoc(const std::locale& loc)
    { return m_out->imbue(loc); }

    template <typename CharT>
    std::locale OutputStream<CharT>::getLoc() const
    { return m_out->getloc();  }

    template <typename CharT>
    std::chrono::zoned_time<std::chrono::nanoseconds>
    OutputStream<CharT>::endTime_of_writeFcn(const char* timezone_name) const noexcept
    {
        if (!m_isWriteFcnUsed)
        return std::chrono::zoned_time<std::chrono::nanoseconds>{timezone_name};

        return std::chrono::zoned_time{timezone_name, m_writeEndTime};
    }

    /*--------------------------------------------------Private------------------------------------------------------------*/

    template <typename CharT>
    template <typename Arg>
    bool OutputStream<CharT>::isOManipulator()
    {
        if constexpr
        (
            std::is_same_v<Arg, ios_base_fcn>
            or
            std::is_same_v<Arg, basic_ios_fcn>
            or
            std::is_same_v<Arg, basic_ostream_fcn>
            or
            std::is_same_v<Arg, setw_fcn_rT>
            or
            std::is_same_v<Arg, setprecision_fcn_rT>
            or
            std::is_same_v<Arg, setfill_fcn_rT>
            or
            std::is_same_v<Arg, setbase_fcn_rT>
            or
            std::is_same_v<Arg, setiosflags_fcn_rT> 
            or
            std::is_same_v<Arg, resetiosflags_fcn_rT>
        )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    template <typename CharT>
    void OutputStream<CharT>::write()
    {
        m_writeEndTime = std::chrono::system_clock::now();
        m_isWriteFcnUsed = true;
    }

    template <typename CharT>
    void OutputStream<CharT>::writews()
    {
        m_isFirstArg1 = true;
        /*Setting m_isFirstArg1 to true here is needed to make sure that even if only one argument is given to the overload that takes in
        a variable amount of arguments, m_isFisrtArg1 is still set to true.
        Example(where # is any format specification):
        write#(3); would equal 3#,

        The write# would set m_isFirstArg1 to false, and because the argument is only one, m_isFirstArg1 is never set to back to true,
        so using the same write#(or another format function since they all use the same boolean m_isFirstArg1) would have m_isFirstArg1
        set to false and therefore write the argument without any format specification(#).
        
        So calling this again:
        write#(3); would equal 3.

        [Note] Check the implementation of the overload that takes in a variable amount of arguments to understand more clearly.
        Appicable to any overload of a format function that takes no arguments, expect write_nl.*/
        m_writeEndTime = std::chrono::system_clock::now();
        m_isWriteFcnUsed = true;
    }

    template <typename CharT>
    void OutputStream<CharT>::writewt()
    {
        m_isFirstArg1 = true;
        m_writeEndTime = std::chrono::system_clock::now();
        m_isWriteFcnUsed = true;
    }

    template <typename CharT>
    void OutputStream<CharT>::writewl()
    {
        m_isFirstArg1 = true;
        m_writeEndTime = std::chrono::system_clock::now();
        m_isWriteFcnUsed = true;
    }

    template <typename CharT>
    void OutputStream<CharT>::writews_nl()
    { 
        m_isFirstArg1 = true;
        write_nl();
    }

    template <typename CharT>
    void OutputStream<CharT>::writewt_nl()
    { 
        m_isFirstArg1 = true;
        write_nl();
    }

    template <typename CharT>
    void OutputStream<CharT>::write_nl()
    {
        if (m_isWritten)
        m_out->put(m_out->widen('\n'));

        m_writeEndTime = std::chrono::system_clock::now();
        m_isWriteFcnUsed = true;
    }

    template <typename CharT>
    void OutputStream<CharT>::check_m_out()
    {
        if (!m_out)
        throw std::runtime_error{"In class OutputStream<CharT>, output stream object data member m_out is nullptr"};
    }

    /*------------------------------------------------Protected-----------------------------------------------------*/

    template <typename CharT>
    void OutputStream<CharT>::failbitCheck()
    {
        if (m_out->fail())
        {
            m_out->clear();
            throw std::runtime_error{failbit_m.data()};
        }
    }

    template <typename CharT>
    void OutputStream<CharT>::badbitCheck()
    {
        if (m_out->fail())
        {
            m_out->clear();
            throw std::runtime_error{badbit_m.data()};
        }
    }

    template <typename CharT>
    void OutputStream<CharT>::failbit_badbit_check()
    {
        if (m_out->fail() and m_out->bad())
        {
            m_out->clear();
            throw std::runtime_error{failbit_badbit_m.data()};
        }
    }

    template <typename CharT>
    void OutputStream<CharT>::set_m_out(std::basic_ostream<CharT>* derived_out)
    { m_out = derived_out; }
    
} // namespace Myfcn::System


#endif