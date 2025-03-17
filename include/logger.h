#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <array>
#include <cstring>

// Message severity levels
enum class MessageLevel {
    INFO,
    WARNING,
    ERROR
};

// Message structure
struct StatusMessage {
    MessageLevel level;
    char message[64];  // 64 character fixed size message buffer 
    
    // Constructor
    StatusMessage() : level(MessageLevel::INFO) {
        message[0] = '\0';
    }
};

class Logger {
private:
    static constexpr size_t MAX_MESSAGES = 20;  // Store last 20 messages
    std::array<StatusMessage, MAX_MESSAGES> messageBuffer;

    int currentIndex = 0;
    int messageCount = 0;
    
public:
    Logger() {}
    
    // Singleton pattern for global access
    static Logger& getInstance(){
        static Logger instance;
        return instance;
    }
    
    // Log a new message
    void log(MessageLevel level, const char* msg);
    
    // Helper methods for common log levels
    void info(const char* msg) { log(MessageLevel::INFO, msg); }
    void warning(const char* msg) { log(MessageLevel::WARNING, msg); }
    void error(const char* msg) { log(MessageLevel::ERROR, msg); }
    
    // Get number of messages stored
    int getMessageCount() const;
    
    // Get message from circular buffer, starting from the most recent
    const StatusMessage* getMessage(int index) const;
    
    // Clear all messages
    void clear();
};

// Global accessor function
inline Logger& getLogger() {
    return Logger::getInstance();
}

#endif 