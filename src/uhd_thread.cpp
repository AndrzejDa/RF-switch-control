#include "uhd_thread.hpp"

void UHDThread::run(){
    qDebug() << input_args.c_str();
    system(input_args.c_str());
}

void UHDThread::set_input_args(std::string args){
    input_args = args;
}