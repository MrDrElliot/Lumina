#pragma once
#include "Containers/String.h"
#include "Module/API.h"

namespace Lumina::Dialogs
{
    enum class LUMINA_API EType
    {
        Ok = 0,
        OkCancel,
        YesNo,
        YesNoCancel,
        RetryCancel,
        AbortRetryIgnore,
        CancelTryContinue,
    };

    enum class LUMINA_API EResult
    {
        No,
        Cancel,
        Yes,
        Retry,
        Continue,
    };

    enum class LUMINA_API ESeverity
    {
        Info = 0,
        Warning,
        Error,
        FatalError,
    };
    
    EResult ShowInternal(ESeverity Severity, EType Type, const FString& Title, const FString& Message);
    
    LUMINA_API inline void Info(const FString& Title, const char* MessageFormat, ...)
    {
        va_list Args;
        va_start(Args, MessageFormat);
        FString Message;
        Message.sprintf_va_list(MessageFormat, Args);
        va_end(Args);

        ShowInternal(ESeverity::Info, EType::Ok, Title, Message);
    }

    LUMINA_API inline void Warning(const FString& Title, const char* MessageFormat, ...)
    {
        va_list Args;
        va_start(Args, MessageFormat);
        FString Message;
        Message.sprintf_va_list(MessageFormat, Args);
        va_end(Args);

        ShowInternal(ESeverity::Warning, EType::Ok, Title, Message);
    }
    
    LUMINA_API inline void Error(const FString& Title, const char* MessageFormat, ...)
    {
        va_list Args;
        va_start(Args, MessageFormat);
        FString Message;
        Message.sprintf_va_list(MessageFormat, Args);
        va_end(Args);

        ShowInternal(ESeverity::Error, EType::Ok, Title, Message);
    }
    
    LUMINA_API inline bool Confirmation(const FString& Title, const char* MessageFormat, ...)
    {
        va_list Args;
        va_start(Args, MessageFormat);
        FString Message;
        Message.sprintf_va_list(MessageFormat, Args);
        va_end(Args);

        return ShowInternal(ESeverity::Info, EType::YesNo, Title, Message) == EResult::Yes;
    }

    
}
