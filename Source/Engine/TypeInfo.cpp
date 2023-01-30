#include "pch.h"
#include "TypeInfo.h"

namespace TypeInfo
{
    Number locGlobalID = 0;
    Number TypeInfo::GetAndIncrementGlobalID()
    {
        return locGlobalID++;
    }
}
