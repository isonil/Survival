#ifndef APP_GLOBAL_HPP
#define APP_GLOBAL_HPP

#include <memory>

namespace app
{

class Core;

class Global
{
public:
    static Core &getCore();
    static void destroyCore();
    static bool isNowDestroying();

private:
    static void deleter(Core *core);

    static std::unique_ptr <Core, void(*)(Core*)> m_core;
    static bool m_isNowDestroying;
};

} // namespace app

#endif // APP_GLOBAL_HPP
