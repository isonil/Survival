/* Project name: Sandbox client
 * Author: Piotr Walczak
 *
 * Note that this is an old, abandoned project and has some design flaws,
 * like poor exceptions structure.
 *
 * The project uses C++14.
 */

#include "engine/util/Trace.hpp"
#include "app/Core.hpp"
#include "app/Global.hpp"

#include <irrlicht/irrlicht.h>

int main(int argc, char *argv[])
{
    engine::Trace::initProfiler();

    app::Global::getCore().exec(argc, argv);
    app::Global::destroyCore();

    engine::Trace::checkMemoryLeaks();
}
