#pragma once
#include "Archiver.h"

namespace Lumina
{
    class FProxyArchive : public FArchive
    {
    public:

        FProxyArchive(FArchive& InInnerAr)
            :InnerArchive(InInnerAr)
        {}

        // Non-copyable
        FProxyArchive(FProxyArchive&&) = delete;
        FProxyArchive(const FProxyArchive&) = delete;
        FProxyArchive& operator=(FProxyArchive&&) = delete;
        FProxyArchive& operator=(const FProxyArchive&) = delete;
        
        // Forward all standard types to the inner archive
        FArchive& operator<<(uint8& Value)  override { return InnerArchive << Value; }
        FArchive& operator<<(int8& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(uint16& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(int16& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(uint32& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(int32& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(uint64& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(int64& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(bool& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(float& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(double& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(Lumina::FString& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(FName& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(FField*& Value) override { return InnerArchive << Value; }
        FArchive& operator<<(FObjectHandle& Value) override { return InnerArchive << Value; }
        
        template<typename ValueType>
        FArchive& operator<<(TVector<ValueType>& Array)
        {
            return InnerArchive << Array;
        }
    
    
    protected:
        
        FArchive& InnerArchive;
        
    };
}
