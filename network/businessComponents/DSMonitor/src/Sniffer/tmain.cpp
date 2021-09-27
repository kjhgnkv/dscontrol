#include <csignal>

#include "utils/utils.h"
#include "core/core.h"
#include "config.h"

int main(int argc, char** argv)
{
    config config = utils::cli_parser(argc, argv);

    utils::VecString vecString = core::device_list();
    core::console_device_list(vecString);

    // std::cout << ">> device: ";
    // std::cin >> config.device;
    config.device = "enp34s0";

    struct sigaction sigIntHandler = {};

    sigIntHandler.sa_handler = core::control_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    core::sniffer* sniffer = core::sniffer::GetInstance(config);

    sniffer->start_listen();

    return 0;
}