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
    static constexpr int MAX_MESSAGES = 20;  // Store last 20 messages
    std::array<StatusMessage, MAX_MESSAGES> messageBuffer;

    int currentIndex = 0;
    int messageCount = 0;
    bool newMessagesFlag = false;
    
public:
    Logger() {}
    
    // Log a new message
    void log(MessageLevel level, const char* msg);
    
    // Helper methods for common log levels
    void info(const char* msg) { log(MessageLevel::INFO, msg); }
    void warning(const char* msg) { log(MessageLevel::WARNING, msg); }
    void error(const char* msg) { log(MessageLevel::ERROR, msg); }
    
    // Get number of messages stored
    int getMessageCount() const;

    // Returns true if messages have been added. Reset when calling getMessage.
    bool newMessagesAvailable() const;
    
    // Get message from circular buffer, starting from the most recent. Reset new messages flag.
    const StatusMessage* getMessage(int index);
    
    // Clear all messages
    void clear();
};

// Single global instance
extern Logger logger;

#endif 