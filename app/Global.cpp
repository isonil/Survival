#include "Global.hpp"

#include "Core.hpp"

namespace app
{

Core &Global::getCore()
{
    if(!m_core)
        m_core = std::unique_ptr <Core, void(*)(Core*)> {new Core{}, deleter};

    return *m_core;
}

void Global::destroyCore()
{
    m_core.reset();
}

bool Global::isNowDestroying()
{
    return m_isNowDestroying;
}

void Global::deleter(Core *core)
{
    m_isNowDestroying = true;
    delete core;
    m_isNowDestroying = false;
}

std::unique_ptr <Core, void(*)(Core*)> Global::m_core{nullptr, deleter};
bool Global::m_isNowDestroying{};

} // namespace app
