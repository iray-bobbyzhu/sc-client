#include "socketcan.h"

#include <iostream>
#include <signal.h>
#include <thread>

bool stop_running = false;

void CtrlC_Handler(int p_sig)
{
    if (p_sig == SIGINT)
    {
        SocketCan::StopServer();

        stop_running = true;
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "usage: " << argv[0] << " <canX>" << std::endl;
        return EXIT_FAILURE;
    }

    signal(SIGINT, CtrlC_Handler);

    SocketCan::StartServer(argv[1]);

    struct can_frame raw_frame = {.can_id = 1 | CAN_EFF_FLAG, .can_dlc = 8};
    raw_frame.data[0] = 0;
    raw_frame.data[1] = 0;
    raw_frame.data[2] = 0;
    raw_frame.data[3] = 0;
    raw_frame.data[4] = 0;
    raw_frame.data[5] = 0;
    raw_frame.data[6] = 0;
    raw_frame.data[7] = 0;

    int index = 0;
    while (!stop_running)
    {
        raw_frame.data[index] = 0;
        index = (index + 1) % 8;
        raw_frame.data[index] = 1;
        SocketCan::EnqueueOutgoing(raw_frame);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
