#pragma once

#include <spdlog/sinks/base_sink.h>

#include "Containers/String.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{

    enum class EConsoleLogLevel : uint8
    {
        Trace = SPDLOG_LEVEL_TRACE,
        Debug = SPDLOG_LEVEL_DEBUG,
        Info = SPDLOG_LEVEL_INFO,
        Warn = SPDLOG_LEVEL_WARN,
        Error = SPDLOG_LEVEL_ERROR,
        Critical = SPDLOG_LEVEL_CRITICAL,
        Off = SPDLOG_LEVEL_OFF,
        n_levels
    };
    
    struct ConsoleMessage
    {
        FString Time;               // Time of the log message
        FString LoggerName;         // Name of the logger
        EConsoleLogLevel Level;     // Log level as a string (e.g., "INFO", "WARN")
        FString Message;            // The log message content
    };

    class ConsoleSink : public spdlog::sinks::base_sink<std::mutex>
    {
    public:
        ConsoleSink(std::vector<ConsoleMessage>& outputMessages)
            : OutputMessages(outputMessages) {}

    protected:
        // Override the `_sink_it_` method to handle log messages
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            // Format the log message
            FString log_message(msg.payload.begin(), msg.payload.end());

            // Convert the timestamp to string
            std::time_t timestamp = std::chrono::system_clock::to_time_t(msg.time);
            char time_buffer[100];
            std::strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", std::localtime(&timestamp));
            std::string log_time(time_buffer);

            // Create a ConsoleMessage to store the full details
            ConsoleMessage consoleMessage;
            consoleMessage.Time = log_time.c_str();
            consoleMessage.LoggerName = msg.logger_name.data();
            consoleMessage.Level = static_cast<EConsoleLogLevel>(msg.level);
            consoleMessage.Message = log_message;

            // Store the log message
            OutputMessages.push_back(consoleMessage);
        }

        // Override the `flush` method to flush any pending messages (if needed)
        void flush_() override {}

        
        std::vector<ConsoleMessage>& OutputMessages;  // Reference to the output messages vector
    };
}

