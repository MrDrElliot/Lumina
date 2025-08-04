#ifdef LE_PLATFORM_WINDOWS

#include "Dialogs.h"
#include <windows.h>

namespace Lumina::Dialogs
{
    EResult ShowInternal(ESeverity Severity, EType Type, const FString& Title, const FString& Message)
    {
        UINT style = MB_TOPMOST | MB_SYSTEMMODAL;

        switch (Severity)
        {
            case ESeverity::Info:
            {
                if (Type == EType::Ok)
                {
                    style |= MB_ICONINFORMATION;
                }
                else
                {
                    style |= MB_ICONQUESTION;
                }
            }
            break;

            case ESeverity::Warning:
            {
                style |= MB_ICONWARNING;
            }
            break;

            case ESeverity::Error:
            case ESeverity::FatalError:
            {
                style |= MB_ICONERROR;
            }
            break;
        }

        //-------------------------------------------------------------------------

        switch (Type)
        {
            case EType::Ok: style |= MB_OK; break;
            case EType::OkCancel: style |= MB_OKCANCEL; break;
            case EType::YesNo: style |= MB_YESNO; break;
            case EType::YesNoCancel: style |= MB_YESNOCANCEL; break;
            case EType::RetryCancel: style |= MB_RETRYCANCEL; break;
            case EType::AbortRetryIgnore: style |= MB_ABORTRETRYIGNORE; break;
            case EType::CancelTryContinue: style |= MB_CANCELTRYCONTINUE; break;
        }

        //-------------------------------------------------------------------------

        EResult result = EResult::Yes;
        MessageBeep(0x00000030L);
        int32_t resultCode = MessageBoxA(nullptr, Message.c_str(), Title.c_str(), style);
        switch( resultCode )
        {
            case IDOK:
            case IDYES:
            {
                result = EResult::Yes;
            }
            break;

            case IDNO:
            {
                result = EResult::No;
            }
            break;

            case IDCANCEL:
            case IDABORT:
            {
                result = EResult::Cancel;
            }
            break;

            case IDRETRY:
            case IDTRYAGAIN:
            {
                result = EResult::Retry;
            }
            break;

            case IDIGNORE:
            case IDCONTINUE:
            {
                result = EResult::Continue;
            }
            break;

            case IDCLOSE:
            {
                result = EResult::Cancel;
            }
            break;

            default:
            {
                result = EResult::Yes;
            }
            break;
        }

        return result;
    }
}

#endif