#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#pragma once

#include <istream>
#include <type_traits>
#include <chrono>
#include <string>
#include <limits>
#include <stdexcept>

#include <iostream>

namespace Streams
{
    template <typename CharT>
    class InputStream
    {
    private:
        //Holds the input stream object.
        std::basic_istream<CharT>* m_in{};

    protected:
        //A flag used to signal if an input function has been called or not.
        //Used in conjunction with m_readEndTime data member.
        bool m_isReadFcnUsed{};

        //Used to keep record of the endTime of a read/input operation
        std::chrono::sys_time<std::chrono::nanoseconds> m_readEndTime{};

        /*This constructor is meant to be used only by classes that derived from this class(InputStream<CharT>).
        Because while this class(InputStream<CharT>) models an aggerate relationship with it's input stream object data member,
        classes that derived from this class are expected to model a composite relationship with it's own input stream object
        data member, therefore instead of setting the std::basic_istream<CharT, Traits> pointer(m_out) to the memory of a predefined 
        std::basic_istream<CharT, Traits> lvalue, you set it to the derived's own input stream object, which has been
        acquired through composition*/
        constexpr InputStream() = default;

    public:
        static constexpr std::string_view failbit_m{"InputStream<CharT>::(*m_in) set failbit!"};
        static constexpr std::string_view eofbit_m{"InputStream<CharT>::(*m_in) set eofbit!"};
        static constexpr std::string_view badbit_m{"InputStream<CharT>::(*m_in) set badbit!"};
        static constexpr std::string_view failbit_eofbit_m{"InputStream<CharT>::(*m_in) set failbit and eofbit!"};
        static constexpr std::string_view failbit_badbit_m{"InputStream<CharT>::(*m_in) set failbit and badbit!"};
        static constexpr std::string_view failbit_eofbit_badbit_m{"InputStream<CharT>::(*m_in) set failbit, eofbit and badbit!"};

        static constexpr std::string_view leftoverInput_m{"Leftover inputs were found in the input stream!"};
        
        using StringType = std::basic_string<CharT>;


        /*Takes a lvalue std::basic_ostream<CharT, Traits> pointer
        This class models an aggregrate relationship through its data member(m_in)- meaning it is neither responsible for the
        creation or destruction of any input stream object given to it.*/
        constexpr InputStream(std::basic_istream<CharT>* in) noexcept;
        
        InputStream(const InputStream&) = delete;
        InputStream(InputStream&&) = delete;

        InputStream& operator=(const InputStream&) = delete;
        InputStream& operator=(InputStream&&) = delete;

        virtual ~InputStream() noexcept;

        /*
        Takes in a variable amount of arguments(as far as those arguments have overloaded the extraction operator[operator>>] to work with it)
        and attempts to read the data from the input stream to the arguments.
        Behaves exactly like this:
        {
            int a{};
            std::cin >> a;
            //Code for checking of the error states
        }
        If any of the error state bits are set during an extraction attempt, this function first clears the std::ios_base::iostate to
        std::ios_base::goodbit and throws exception std::runtime_error.

        Same with public member function InputStream<CharT>::readwLO, but considers leftover inputs invalid.
        [Note] If extraction to a std::string is needed, public member function InputStream<CharT>::read_str is better suited for the job.*/
        template <typename Arg1, typename... Args>
        void read(Arg1& arg1, Args&... args);

        /*readwLO(read with leftover).
        Takes in a variable amount of arguments(as far as those arguments have overloaded the extraction operator[operator>>] to work with it)
        and attempts to read the data from the input stream to the arguments.
        Behaves exactly like this:
        {
            int a{};
            std::cin >> a;
            //Code for checking of the error states
        }
        If any of the error states bit are set during an extraction attempt, this function first clears the std::ios_base::iostate to
        std::ios_base::goodbit and throws exception std::runtime_error.

        Same with InputStream<CharT>::read, but considers leftover inputs valid.
        [Note] If extraction to a std::string is needed, public member function read_str is better suited for the job.*/
        template <typename Arg1, typename... Args>
        void readwLO(Arg1& arg1, Args&... args);

        /*
        Takes in a lvalue reference to a std::string and boolean is_leadingWS_valid - is leading white space valid during extraction
        (set to false by default).

        If is_leadingWS_valid is set to true, std::getline(m_in, arg1) is used to read input stream data to the string parameter,
        else std::getline(m_in >> std::ws, arg1) is used.
        Because basic_istream objects or objects inherited from basic_istream ignores leading whitespace by default, 
        leaving parameter is_leadingWS_valid set to false is advisable.

        If any of the error states bit are set during an extraction attempt, this function first clears the std::ios_base::iostate to
        std::ios_base::goodbit and throws exception std::runtime_error.*/
        void read_str(StringType& arg1, bool is_leadingWS_valid = false);

        /*Returns a reference to the input stream object given.(Preferred)*/
        constexpr std::basic_istream<CharT>& get() noexcept;

        /*Returns a reference to the input stream object given.(Not Preferred)*/
        constexpr std::basic_istream<CharT>& operator*() noexcept;

        /*Sets additional format flags.
        Behaves exactly like this:
        {
           std::cin.setf(flag);
        }*/
        InputStream<CharT>& setFmtf(std::ios_base::fmtflags flags);
        /*Sets additional format flags that support bitgroups.
        Behaves exactly like this:
        {
           std::cin.setf(flag, flag_bitgroup);
        }*/
        InputStream<CharT>& setFmtf(std::ios_base::fmtflags flags, std::ios_base::fmtflags mask);
        /*Clears the current format flag setting and sets only the given flag.
        Behaves exactly like this:
        {
            std::cout.flag(flag);
        }
        */
        InputStream<CharT>& clearFmtf(std::ios_base::fmtflags flags);
        /*Gets the current format flag setting.
        Behaves exactly like this:
        {
            bool flagSetting = std::cin.flag();
        }
        */
        std::ios_base::fmtflags getFmtf() const;

        InputStream<CharT>& setWidth(std::streamsize new_width);
        std::streamsize getWidth() const;
        
        InputStream<CharT>& setFillc(CharT ch);
        CharT getFillc() const;

        InputStream<CharT>& dec();
        InputStream<CharT>& oct();
        InputStream<CharT>& hex();
        InputStream<CharT>& boolalpha();
        InputStream<CharT>& noboolalpha();
        InputStream<CharT>& skipws();
        InputStream<CharT>& noskipws();

        bool good() const;
        bool eof() const;
        bool fail() const;
        bool bad() const;
        bool operator!() const;
        explicit operator bool() const;
        InputStream<CharT>& setStatef(std::ios_base::iostate state);
        InputStream<CharT>& clearStatef(std::ios_base::iostate state);
        std::ios_base::iostate getStatef() const;

        InputStream<CharT>& moveIndicator
        (std::basic_ios<CharT>::off_type nOfBytes, std::ios_base::seekdir offset_direction = std::ios_base::beg);

        InputStream<CharT>& moveIndicator(std::basic_ios<CharT>::pos_type pos);

        std::basic_istream<CharT>::pos_type getIndicatorPos();

        // Calls std::cin.ignore(std::numeric<std::streamsize>::max(), '\n') to clear input stream.
        //For error/failure handling.
        void ignoreLeftOverInputs();

        std::locale imbueLoc(const std::locale& loc);
        std::locale getLoc() const;

        /*Returns the time a read function finished its operation.
        The timezone_name parameter takes in the name of a time zone you wish the time to be in, default is "ETC/UTC".
        If no read function has been called prior to a function call to public member endTime_of_readFcn,
        the function returns the epoch time of std::chrono::system_clock.*/
        std::chrono::zoned_time<std::chrono::nanoseconds> endTime_of_readFcn(const char* timezone_name = "Etc/UTC") const noexcept;
    private:
        /*An overload of read that takes no argument.
        Does no extracting/reading but is essential to stopping the extraction operation while recording the time the operation stopped.*/
        void read();
        /*An overload of read that takes no argument.
        Does no extracting/reading but is essential to stopping the extraction operation while recording the time the operation stopped.*/
        void readwLO();

        /*Checks if m_in is nullptr and throws an exception.*/
        void check_m_in();

    protected:
        /*Checks if the std::ios_base::eofbit is set, if true,
        clears the state flag to std::ios_base::goodbit and throws exception std::runtime_error{eofbitmessage.data()}.
        This function is made virtual and protected so derived classes may override it's behaviour, if the
        default behaviour does not suit the current application of the function.*/
        virtual void eofbitCheck();

        /*Checks if the std::ios_base::failbit is set, if true,
        clears the state flag to std::ios_base::goodbit, discards the initial input that made the extraction to fail
        and throws exception std::runtime_error{failbitmessage.data()}.
        This function is made virtual and protected so derived classes may override it's behaviour, if the
        default behaviour does not suit the current application of the function.*/
        virtual void failbitCheck();

        /*Checks if the std::ios_base::badbit is set, if true,
        clears the state flag to std::ios_base::goodbit, discards the initial input that made the extraction operation go bad
        and throws exception std::runtime_error{badbitmessage.data()}.
        This function is made virtual and protected so derived classes may override it's behaviour, if the
        default behaviour does not suit the current application of the function.*/
        virtual void badbitCheck();

        /*Checks if the std::ios_base::failbit and std::ios_base::eofbit are set,
        if true, clears the state flag to std::ios_base::goodbit and throws exception std::runtime_error{badbitmessage.data()}.
        This function is made virtual and protected so derived classes may override it's behaviour, if the
        default behaviour does not suit the current application of the function.*/
        virtual void failbit_eofbit_check();

        virtual void failbit_badbit_check();

        virtual void failbit_eofbit_badbit_check();

        /*Checks if leftover inputs were in the input stream if true,
        throws exception std::runtime_error{leftOverInputsMessage.data()}.
        This function is made virtual and protected so derived classes may override it's behaviour, if the
        default behaviour does not suit the current application of the function.*/
        virtual void leftoverInputsCheck();

        /*sets the data member m_in*/
        void set_m_in(std::basic_istream<CharT>* derived_in);
    };

    template <typename CharT>
    constexpr InputStream<CharT>::InputStream(std::basic_istream<CharT>* in) noexcept
    : m_in{ in }
    {}
    
    template <typename CharT>
    InputStream<CharT>::~InputStream() noexcept
    { m_in = nullptr; }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void InputStream<CharT>::read(Arg1& arg1, Args&... args)
    {
        (*m_in) >> arg1;

        if (!m_in->good())
        {
            failbit_eofbit_check(); //do not move
            eofbitCheck();
            failbitCheck();
            badbitCheck();
        }
        
        ignoreLeftOverInputs();
        leftoverInputsCheck();

        read(args...);
    }

    template <typename CharT>
    template <typename Arg1, typename... Args>
    void InputStream<CharT>::readwLO(Arg1& arg1, Args&... args)
    {
        (*m_in) >> arg1;

        if (!m_in->good())
        {
            failbit_eofbit_check(); //do not move
            eofbitCheck();
            failbitCheck();
            badbitCheck();
        }

        readwLO(args...);
    }

    template <typename CharT>
    void InputStream<CharT>::read_str(StringType& arg1, bool is_leadingWS_valid)
    {
        if (is_leadingWS_valid)
        std::getline((*m_in), arg1);
        else //std::ws to ignore leading whitespace.
        std::getline((*m_in) >> std::ws, arg1); 

        if (!m_in->good())
        {
            failbit_eofbit_check(); //do not move
            eofbitCheck();
            failbitCheck();
            badbitCheck();
        }

        m_readEndTime = std::chrono::system_clock::now();
        m_isReadFcnUsed = true;
    }

    template <typename CharT>
    constexpr std::basic_istream<CharT>& InputStream<CharT>::get() noexcept
    { return (*m_in); }

    template <typename CharT>
    constexpr std::basic_istream<CharT>& InputStream<CharT>::operator*() noexcept
    { return (*m_in); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::setFmtf(std::ios_base::fmtflags flags)
    { return (m_in->setf(flags), *this); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::setFmtf(std::ios_base::fmtflags flags, std::ios_base::fmtflags mask)
    { return (m_in->setf(flags, mask), *this); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::clearFmtf(std::ios_base::fmtflags flags)
    { return (m_in->flags(flags), *this); }

    template <typename CharT>
    std::ios_base::fmtflags InputStream<CharT>::getFmtf() const
    { return m_in->flags(); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::setWidth(std::streamsize new_width)
    { return (m_in->width(new_width), *this); }

    template <typename CharT>
    std::streamsize InputStream<CharT>::getWidth() const
    { return m_in->width(); }
    
    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::setFillc(CharT ch)
    { return (m_in->fill(ch), *this); }

    template <typename CharT>
    CharT InputStream<CharT>::getFillc() const
    { return m_in->fill(); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::dec()
    { return (m_in->setf(std::ios_base::dec, std::ios_base::basefield), *this); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::oct()
    { return (m_in->setf(std::ios_base::oct, std::ios_base::basefield), *this); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::hex()
    { return (m_in->setf(std::ios_base::hex, std::ios_base::basefield), *this); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::boolalpha()
    { return (m_in->setf(std::ios_base::boolalpha), *this); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::noboolalpha()
    { return (m_in->unsetf(std::ios_base::boolalpha), *this); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::skipws()
    { return (m_in->setf(std::ios_base::skipws), *this); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::noskipws()
    { return (m_in->unsetf(std::ios_base::skipws), *this); }

    template <typename CharT>
    bool InputStream<CharT>::good() const
    { return m_in->good(); }

    template <typename CharT>
    bool InputStream<CharT>::eof() const
    { return m_in->eof(); }

    template <typename CharT>
    bool InputStream<CharT>::fail() const
    { return m_in->fail(); }

    template <typename CharT>
    bool InputStream<CharT>::bad() const
    { return m_in->bad(); }

    template <typename CharT>
    bool InputStream<CharT>::operator!() const
    { return m_in->operator!(); }

    template <typename CharT>
    InputStream<CharT>::operator bool() const
    { return static_cast<bool>(*m_in); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::setStatef(std::ios_base::iostate state)
    { return (m_in->setstate(state), *this); }

    template <typename CharT>
    InputStream<CharT>& InputStream<CharT>::clearStatef(std::ios_base::iostate state)
    { return (m_in->clear(state), *this); }

    template <typename CharT>
    std::ios_base::iostate InputStream<CharT>::getStatef() const
    { return m_in->rdstate(); }

    template <typename CharT>
    InputStream<CharT>&
    InputStream<CharT>::moveIndicator(std::basic_ios<CharT>::off_type nOfBytes, std::ios_base::seekdir offset_direction)
    { return (m_in->seekg(nOfBytes, offset_direction), *this); }

    template <typename CharT>
    InputStream<CharT>& 
    InputStream<CharT>::moveIndicator (std::basic_ios<CharT>::pos_type pos)
    { return (m_in->seekg(pos), *this); }

    template <typename CharT>
    std::basic_istream<CharT>::pos_type InputStream<CharT>::getIndicatorPos()
    { return m_in->tellg(); }

    template <typename CharT>
    void InputStream<CharT>::ignoreLeftOverInputs()
    {
        //Ignore all data still left in the input stream.
        m_in->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    template <typename CharT>
    std::locale InputStream<CharT>::imbueLoc(const std::locale& loc)
    { return m_in->imbue(loc); }

    template <typename CharT>
    std::locale InputStream<CharT>::getLoc() const
    { return m_in->getloc();  }


    template <typename CharT>
    std::chrono::zoned_time<std::chrono::nanoseconds> 
    InputStream<CharT>::endTime_of_readFcn(const char* timezone_name) const noexcept
    {
        if (!m_isReadFcnUsed)
        return std::chrono::zoned_time<std::chrono::nanoseconds>{timezone_name};

        return std::chrono::zoned_time{timezone_name, m_readEndTime};
    }


/*--------------------------------------------------Private------------------------------------------------------------*/

    template <typename CharT>
    void InputStream<CharT>::read()
    {
        m_readEndTime = std::chrono::system_clock::now();
        m_isReadFcnUsed = true;
    }

    template <typename CharT>
    void InputStream<CharT>::readwLO()
    {
        m_readEndTime = std::chrono::system_clock::now();
        m_isReadFcnUsed = true;
    }

    template <typename CharT>
    void InputStream<CharT>::check_m_in()
    {
        if (!m_in)
        throw std::runtime_error{"In class InputStream<CharT>, input stream object data member m_in is nullptr"};
    }

    /*------------------------------------------------Protected-----------------------------------------------------*/

    template <typename CharT>
    void InputStream<CharT>::eofbitCheck()
    {
        if (m_in->eof())
        {
            m_in->clear();
            throw std::runtime_error{eofbit_m.data()};
        }
    }

    template <typename CharT>
    void InputStream<CharT>::failbitCheck()
    {
        if (m_in->fail())
        {
            m_in->clear();
            throw std::runtime_error{failbit_m.data()};
        }
    }

    template <typename CharT>
    void InputStream<CharT>::badbitCheck()
    {
        if (m_in->fail())
        {
            m_in->clear();
            throw std::runtime_error{badbit_m.data()};
        }
    }

    template <typename CharT>
    void InputStream<CharT>::failbit_eofbit_check()
    {
        if (m_in->fail() and m_in->eof())
        {
            m_in->clear();
            throw std::runtime_error{failbit_eofbit_m.data()};
        }
    }

    template <typename CharT>
    void InputStream<CharT>::failbit_badbit_check()
    {
        if (m_in->fail() and m_in->bad())
        {
            m_in->clear();
            throw std::runtime_error{failbit_badbit_m.data()};
        }
    }

    template <typename CharT>
    void InputStream<CharT>::failbit_eofbit_badbit_check()
    {
        if (m_in->fail() and m_in->eof() and m_in->bad())
        {
            m_in->clear();
            throw std::runtime_error{failbit_eofbit_badbit_m.data()};
        }
    }

    template <typename CharT>
    void InputStream<CharT>::leftoverInputsCheck()
    { 
        if (m_in->gcount() > 1)
        {
            throw std::runtime_error{leftoverInput_m.data()};
        }
    }

    template <typename CharT>
    void InputStream<CharT>::set_m_in(std::basic_istream<CharT>* derived_in)
    { m_in = derived_in; }
    
} // namespace Myfcn::System



#endif