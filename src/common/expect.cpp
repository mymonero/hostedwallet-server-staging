#include "expect.h"

#include <easylogging++.h>
#include <string>

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

    void expect::throw_(std::error_code ec, const char* msg, const char* file, unsigned line)
    {
        if (msg || file)
            throw std::system_error{ec, generate_error(msg, file, line)};
        throw std::system_error{ec};
    }
} // detail
