#ifndef STRINGOUTPUTSTREAM_H
#define STRINGOUTPUTSTREAM_H

#pragma once

#include <sstream>
#include <utility>
#include <chrono>

#include "outputstream.h"

namespace Streams
{
    template <typename CharT>
    class StringOutputStream: public OutputStream<CharT>
    {
    private:
        //Holds string output stream object.
        std::basic_ostringstream<CharT> m_osstream;

    public:        
        StringOutputStream(std::basic_ostringstream<CharT>&& osstream) noexcept;

        //Copying not allowed.
        StringOutputStream(const StringOutputStream&) = delete;
        StringOutputStream(StringOutputStream&&) = delete;

        StringOutputStream& operator=(const StringOutputStream&) = delete;
        StringOutputStream& operator=(StringOutputStream&&) = delete;

        ~StringOutputStream() noexcept = default;

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
            std::ostringstream os{"Emeka Daniel"};
            os.str();
        }*/
        std::string get_str() noexcept;

        /*Clears the string used by the output stream.
        Behaves exactly like this:
        {
            std::ostringstream os{"Emeka Daniel"};
            os.str("");
        }*/
        void empty_str() noexcept;

        /*Returns a reference to the string output stream object given.(Preferred)*/
        constexpr std::basic_ostringstream<CharT>& get() noexcept;

        /*Returns a reference to the string input stream object given.(Not Preferred)*/
        constexpr std::basic_ostringstream<CharT>& operator*() noexcept;
    };

    template <typename CharT>
    StringOutputStream<CharT>::StringOutputStream(std::basic_ostringstream<CharT>&& osstream) noexcept 
    :OutputStream<CharT>{},
    m_osstream{ std::forward<std::basic_ostringstream<CharT>>(osstream) }
    {
        OutputStream<CharT>::set_m_out(&m_osstream);
    }

    template <typename CharT>
    void StringOutputStream<CharT>::insert_str(const std::string& data) noexcept
    { m_osstream.str(data); }

    template <typename CharT>
    void StringOutputStream<CharT>::insert_str(std::string&& data) noexcept
    { m_osstream.str(data); }

    template <typename CharT>
    std::string StringOutputStream<CharT>::get_str() noexcept
    { return m_osstream.str(); }

        
    template <typename CharT>
    void StringOutputStream<CharT>::empty_str() noexcept
    { return m_osstream.str(""); }

    template <typename CharT>
    constexpr std::basic_ostringstream<CharT>& StringOutputStream<CharT>::get() noexcept
    { return m_osstream; }
    
    template <typename CharT>
    constexpr std::basic_ostringstream<CharT>& StringOutputStream<CharT>::operator*() noexcept
    { return m_osstream; }
    
} // namespace Myfcn


#endif