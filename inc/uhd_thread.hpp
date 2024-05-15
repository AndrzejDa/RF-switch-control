#ifndef CUSTOM_THREADS_HPP
#define CUSTOM_THREADS_HPP

#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QThread>

class UHDThread : public QThread{
public:
    void run() override;
    void set_input_args(std::vector<char*>& args);
private:
    std::vector<char*> input_args;
};


#endif