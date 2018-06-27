#include "expect.h"

#include <easylogging++.h>
#include <string>
/*
#if _POSIX_C_SOURCE >= 200112L
 #include <unistd.h>
#endif


void safe_abort() {
#ifdef NDEBUG
 #if _POSIX_C_SOURCE >= 200112L
    _exit(1);
 #elif _ISOC99_SOURCE
    _Exit(1);
 #endif
#endif
    abort();
}*/

namespace detail
{
    namespace
    {
        std::string generate_error(const char* msg, const char* file, unsigned line)
        {
            std::string error_msg{};
            if (msg)
            {
                error_msg.append(msg);
                if (file)
                    error_msg.append(" (");
            }
            if (file)
            {
                error_msg.append("thrown at ");
                char buff[256] = {0};
                el::base::utils::File::buildBaseFilename(file, buff, sizeof(buff) - 1);
                error_msg.append(buff);
                error_msg.push_back(':');
                error_msg.append(std::to_string(line));
            }
            if (msg && file)
                error_msg.push_back(')');
            return error_msg;
        }
    }

    void expect::log(std::error_code ec, const char* msg, const char* file, unsigned short line)
    { /*
        if (!file)
            file = "";
        if (!msg)
            msg = "";
        el::base::Writer(ERROR, file, line, el::base::DispatchAction::NormalLog, category) << msg; */
    }

    void expect::throw_(std::error_code ec, const char* msg, const char* file, unsigned line)
    {
        if (msg || file)
            throw std::system_error{ec, generate_error(msg, file, line)};
        throw std::system_error{ec};
    }

    void expect::abort_(std::error_code ec, const char* msg, const char* file, unsigned short line) noexcept
    {
        /*try
        {
            log(ec, msg, file, line);
        } 
        catch (...)
        {}
        safe_abort(); */
    }
} // detail
