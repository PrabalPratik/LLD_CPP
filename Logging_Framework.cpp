#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <format>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>

using namespace std;

/*

Designing a Logging Framework

Requirements

The logging framework should support different log levels, such as DEBUG, INFO, WARNING, ERROR, and FATAL.
It should allow logging messages with a timestamp, log level, and message content.
The framework should support multiple output destinations, such as console, file, and database.
It should provide a configuration mechanism to set the log level and output destination.
The logging framework should be thread-safe to handle concurrent logging from multiple threads.
It should be extensible to accommodate new log levels and output destinations in the future.

*/

/*

SOLUTION : 

Functional Requirements : 
- Support different log levels
- Log a message
- Have timestamp
- Have message content
- Configure minimum log level.
- Configure output dest.
- Support multiple output Destinations

Non Functional Req : 
- Thread-safe(Will be implemented at last as my primary focus for now is building the system)
- Extensible.
- Maintainable.
- Easy to add new outputs.

WorkFlow : 

User
   │
   ▼
Requests to log a message
(Level + Message Content)
   │
   ▼
Current timestamp is generated
   │
   ▼
Check if the log level satisfies the configured minimum log level
   │
   ├── No → Ignore the log
   │
   └── Yes
         │
         ▼
Create a complete log message
(Timestamp + Level + Message)
         │
         ▼
Forward the log to all configured output destinations
         │
         ▼
Each output destination writes the log
(Console / File / Database / ...)

*/


enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

string getCurrentTimestamp() {

    auto now = chrono::system_clock::now();

    time_t currentTime = chrono::system_clock::to_time_t(now);

    string timestamp = ctime(&currentTime);

    // Remove the trailing '\n'
    timestamp.pop_back();

    return timestamp;
}

class LogMessage {
private:
    int messageId;
    string timestamp;
    string message;
    LogLevel level;

public:
    LogMessage(int mID, string t , string msg ,LogLevel lvl){
        messageId = mID;
        timestamp = t;
        message = msg;
        level = lvl;
    }

    int getMessageId()const {
        return messageId;
    }
    string getTimestamp() const {
        return timestamp;
    }
    string getMessage() const {
        return message;
    }
    LogLevel getLogLevel() const {
        return level;
    }
};

// Base class for all output destinations
class Output {
public:
    virtual void write(LogMessage& msg) = 0;
};

// Console Output
class ConsoleOutput : public Output {
public:
    void write(LogMessage& msg) override {
        cout << "[" << msg.getTimestamp() << "] ";
        cout << "[" << static_cast<int>(msg.getLogLevel()) << "] ";
        cout << msg.getMessage() << endl;
    }
};

// File Output
class FileOutput : public Output {
public:
    void write(LogMessage& msg) override {
        cout << "Writing to File : " << msg.getMessage() << endl;
    }
};

// Database Output
class DatabaseOutput : public Output {
public:
    void write(LogMessage& msg) override {
        cout << "Writing to Database : " << msg.getMessage() << endl;
    }
};

class Logger {
private:
    LogLevel currentLogLevel;
    vector<Output*> outputs;
    vector<LogMessage> logs;   // Optional: storing logs for demonstration
    mutex mtx;

public:
    Logger() {
        currentLogLevel = LogLevel::INFO;
    }

    void log(LogLevel level, string message) {

        lock_guard<mutex> lock(mtx);
        if (level < currentLogLevel)
            return;

        string timestamp = getCurrentTimestamp();   // Replace later
        int messageId = logs.size() + 1;

        LogMessage msg(messageId, timestamp, message, level);

        logs.push_back(msg);

        for (auto output : outputs) {
            output->write(msg);
        }
}

    void setLogLevel(LogLevel level) {
        lock_guard<mutex> lock(mtx);
        currentLogLevel = level;
    }

    void addOutput(Output* output) {
        lock_guard<mutex> lock(mtx);
        outputs.push_back(output);
    }
};

int main() {

    Logger logger;

    ConsoleOutput console;
    FileOutput file;
    DatabaseOutput database;

    // Add output destinations
    logger.addOutput(&console);
    logger.addOutput(&file);
    logger.addOutput(&database);

    // Set minimum log level
    logger.setLogLevel(LogLevel::INFO);

    // These logs will be processed
    logger.log(LogLevel::INFO, "Application Started");
    logger.log(LogLevel::WARNING, "Low Memory Warning");
    logger.log(LogLevel::ERROR, "Database Connection Failed");
    logger.log(LogLevel::FATAL, "System Crashed");

    // This log will be ignored (DEBUG < INFO)
    logger.log(LogLevel::DEBUG, "Debugging Message");

    return 0;
}