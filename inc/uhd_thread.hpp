#ifndef UHD_THREAD_HPP
#define UHD_THREAD_HPP

#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QThread>

class UHDThread : public QThread{
public:
    void run() override;
    void set_input_args(std::string args);
private:
    std::string input_args;
};


#endif