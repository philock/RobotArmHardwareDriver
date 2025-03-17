#include <logger.h>

void Logger::log(MessageLevel level, const char* msg) {
    // Get current slot
    StatusMessage& entry = messageBuffer[currentIndex];
    
    // Copy message with length limit
    strncpy(entry.message, msg, sizeof(entry.message) - 1);
    entry.message[sizeof(entry.message) - 1] = '\0';  // Ensure null termination
    
    // Set other fields
    entry.level = level;
    
    // Update indices
    currentIndex = (currentIndex + 1) % MAX_MESSAGES;
    if (messageCount < MAX_MESSAGES) {
        messageCount++;
    }
}

int Logger::getMessageCount() const {
    return messageCount;
}

const StatusMessage* Logger::getMessage(int index) const {
    if (index >= messageCount) {
        return nullptr;
    }
    
    // Calculate the actual index in the circular buffer
    int actualIndex = (currentIndex + MAX_MESSAGES - 1 - index) % MAX_MESSAGES;
    return &messageBuffer[actualIndex];
}

void Logger::clear() {
    messageCount = 0;
    currentIndex = 0;
}