#include "Console.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

std::mutex Console::logMutex_;
bool Console::debugMode_ = false;

void Console::setDebugMode(bool debug) {
    std::cout << "Debug mode: " << (debug ? "ON" : "OFF") << std::endl;
    debugMode_ = debug;
}

void Console::info(const std::string& message) {
    log(message, LogLevel::Info);
}

void Console::warning(const std::string& message) {
    log(message, LogLevel::Warning);
}

void Console::error(const std::string& message) {
    log(message, LogLevel::Error);
}

void Console::debug(const std::string& message) {
    if (!debugMode_) return;
    log(message, LogLevel::Debug);
}

void Console::log(const std::string& message, LogLevel level) {

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::lock_guard<std::mutex> lock(logMutex_);

    std::ostringstream prefix;
    prefix << "\033[1m[" << std::put_time(std::localtime(&t), "%T") << "] "
           << logLevelToString_(level) << "\033[0m: ";

    std::ostream& out = (level == LogLevel::Error) ? std::cerr : (level == LogLevel::Debug) ? std::clog : std::clog; //TODO: change to std::cout
    out << prefix.str() << message << "\n";
}

const std::string Console::logLevelToString_(LogLevel level) {
    switch (level) {
        case LogLevel::Info:    return "\033[32mINFO\033[0m";
        case LogLevel::Warning: return "\033[33mWARN\033[0m";
        case LogLevel::Error:   return "\033[31mERROR\033[0m";
        case LogLevel::Debug:   return "\033[36mDEBUG\033[0m";
        default:                return "\033[35mUNKNOWN\033[0m";
    }
}
