//
// Created by zeroc00l on 10/31/23.
//

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "system.hpp"

#ifndef DGTKPROJECT_LOG_HPP
#define DGTKPROJECT_LOG_HPP

class LogSystem: public System {
public:
    explicit LogSystem(const std::filesystem::path& logDirectory);
    LogSystem(const LogSystem& copy) = delete;
    ~LogSystem();

    void            Init();
    void            Update();
    void            Shutdown();

    void            PublishMessage(std::string_view message);
    void            RecordError(std::string_view errorType, std::string_view errorDescription);
private:
    std::ofstream   primaryLogFile;
    std::ofstream   errorLogFile;

    std::vector<std::string>    buffer;
};

#endif //DGTKPROJECT_LOG_HPP
