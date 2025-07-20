#pragma once

#include <spdlog/sinks/base_sink.h>
#include "Containers/Array.h"
#include "Log/LogMessage.h"
#include "Memory/Memory.h"


namespace Lumina
{
    
    class ConsoleSink : public spdlog::sinks::base_sink<std::mutex>
    {
    public:
        ConsoleSink(TVector<FConsoleMessage>& outputMessages)
            : OutputMessages(&outputMessages) {}

    protected:
        
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            FConsoleMessage Message;

            std::time_t timestamp = std::chrono::system_clock::to_time_t(msg.time);
            char TimeBuffer[16];
            std::strftime(TimeBuffer, sizeof(TimeBuffer), "%H:%M:%S", std::localtime(&timestamp));

            Message.Time.assign(TimeBuffer);
            Message.LoggerName.assign(msg.logger_name.begin(), msg.logger_name.end());
            Message.Level = msg.level;
            Message.Message.assign(msg.payload.begin(), msg.payload.end());

            // Add new message to the output
            OutputMessages->push_back(Memory::Move(Message));

            // Check if we exceed the max number of messages, remove the oldest if needed
            if (OutputMessages->size() > 150)
            {
                OutputMessages->erase(OutputMessages->begin());  // Remove the oldest message
            }
        }

        void flush_() override {}

        
        TVector<FConsoleMessage>* OutputMessages;
    };
}

