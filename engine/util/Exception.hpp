#ifndef ENGINE_EXCEPTION_HPP
#define ENGINE_EXCEPTION_HPP

#include <exception>
#include <string>

namespace engine
{

class Exception : public std::exception
{
public:
    explicit Exception(const std::string &message);
    Exception(const std::string &message, const std::exception &innerException);

    const char *what() const noexcept override;

private:
    std::string m_message;
};

} // namespace engine

#endif // ENGINE_EXCEPTION_HPP
