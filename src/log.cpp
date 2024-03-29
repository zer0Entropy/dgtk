//
// Created by zeroc00l on 10/31/23.
//
#include "../include/log.hpp"

LogSystem::LogSystem(const std::filesystem::path& logDirectory):
    System(SystemID::Log) {
    std::string logPath(logDirectory);
    std::string errorPath(logDirectory);
    logPath.append("/log.txt");
    errorPath.append("/error.txt");
    primaryLogFile.open(logPath, std::ios_base::out | std::ios_base::trunc);
    errorLogFile.open(errorPath, std::ios_base::out | std::ios_base::trunc);
}

LogSystem::~LogSystem() {
    if(primaryLogFile.is_open()) {
        primaryLogFile.close();
    }
    if(errorLogFile.is_open()) {
        errorLogFile.close();
    }
}

void LogSystem::Init() {
    std::string message("Log file created.");
    PublishMessage(message);
}

void LogSystem::Update() {
    for(auto iterator = buffer.begin(); iterator != buffer.end(); ++iterator) {
        primaryLogFile << *iterator << std::endl;
    }
    buffer.clear();
    primaryLogFile.flush();
}

void LogSystem::Shutdown() {
    primaryLogFile.flush();
    errorLogFile.flush();

    if(primaryLogFile.is_open()) {
        primaryLogFile.close();
    }
    if(errorLogFile.is_open()) {
        errorLogFile.close();
    }
}

void LogSystem::PublishMessage(std::string_view message) {
    buffer.push_back((std::string)message);
}

void LogSystem::RecordError(std::string_view errorType, std::string_view errorDescription) {
    errorLogFile << "ERROR!  ErrorType = " << errorType << std::endl;
    errorLogFile << "ErrorDescription = " << errorDescription << std::endl;
    errorLogFile.flush();
}

