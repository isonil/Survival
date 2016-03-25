#include "Exception.hpp"

#include "Trace.hpp"

namespace engine
{

Exception::Exception(const std::string &message)
    : m_message{message + "\nat:\n" + Trace::getTrace()}
{
}

Exception::Exception(const std::string &message, const std::exception &innerException)
    : m_message{message + "\nat:\n" + Trace::getTrace() + "\n--->\n" + innerException.what()}
{
}

const char *Exception::what() const noexcept
{
    return m_message.c_str();
}

} // namespace engine
