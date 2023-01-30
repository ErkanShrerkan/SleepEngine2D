#pragma once

namespace TypeInfo
{
    using Number = unsigned;

    Number GetAndIncrementGlobalID();

    template <class T>
    Number TypeID()
    {
        static Number number = GetAndIncrementGlobalID();
        return number;
    }
}
