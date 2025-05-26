#include "app/app.hpp"
#include "cfg.hpp"
#include "lib/logger.hpp"

using namespace app;

int main(int argc, char** argv) {
    config::Config config(argc, argv);
    logger::Log::SetLogLevel(config.getLogLevel());

    App app(config);
    app.start();
}
