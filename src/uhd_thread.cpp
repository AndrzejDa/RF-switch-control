#include "uhd_thread.hpp"

void UHDThread::run(){
    std::string exec_cmd;
    exec_cmd += "./rx_samples_to_file";
    for (const auto& arg : input_args) {
        exec_cmd += arg;
        exec_cmd += " ";
    }
    qDebug() << "elo pa to" << exec_cmd.c_str();
    system(exec_cmd.c_str());
}

void UHDThread::set_input_args(std::vector<char*>& args){
    input_args = args;
}