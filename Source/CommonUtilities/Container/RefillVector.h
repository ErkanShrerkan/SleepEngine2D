#pragma once
#include <vector>

namespace CommonUtilities
{
    template <class T>
    class RefillVector
    {
    public:
        using Iterator = typename std::vector<T>::iterator;
        using ConstIterator = typename std::vector<T>::const_iterator;
        using SizeType = typename std::vector<T>::size_type;

    private:
        std::vector<T> myVector;
        SizeType mySize;
        SizeType myIndex;

    public:
        inline Iterator begin()
        {
            return myVector.begin();
        }
        inline ConstIterator begin() const
        {
            return myVector.begin();
        }
        inline Iterator end()
        {
            return myVector.begin() + mySize;
        }
        inline ConstIterator end() const
        {
            return myVector.begin() + mySize;
        }
        inline bool empty() const
        {
            return myVector.empty();
        }

    public:
        inline SizeType size() const
        {
            return mySize;
        }
        inline void clear()
        {
            myVector.clear();
            mySize = 0;
        }
        inline void flush()
        {
            mySize = 0;
        }
        inline std::vector<T>& vector()
        {
            return myVector;
        }
        inline const std::vector<T>& vector() const
        {
            return myVector;
        }

    public:
        void push_back(T&& aValue);
        void push_back(const T& aValue);
        void push_back(const std::vector<T>& aVector);
        T& operator[](const SizeType anIndex);
        const T& operator[](const SizeType anIndex) const;
    };

    template<class T>
    inline void RefillVector<T>::push_back(T&& aValue)
    {
        if (mySize == myVector.size())
        {
            myVector.push_back(aValue);
        }
        else
        {
            myVector[mySize] = aValue;
        }
        mySize++;
    }

    template<class T>
    inline void RefillVector<T>::push_back(const T& aValue)
    {
        if (mySize == myVector.size())
        {
            myVector.push_back(aValue);
        }
        else
        {
            myVector[mySize] = aValue;
        }
        mySize++;
    }

    template<class T>
    inline void RefillVector<T>::push_back(const std::vector<T>& aVector)
    {
        if (aVector.empty())
        {
            return;
        }

        uint size = (uint)myVector.size();
        uint vectorSize = (uint)aVector.size();
        if (mySize + vectorSize > size)
        {
            myVector.resize(mySize + vectorSize);
        }
        memcpy(&myVector[mySize], &aVector[0], sizeof(T) * vectorSize);
        mySize += vectorSize;
    }

    template<class T>
    inline T& RefillVector<T>::operator[](const SizeType anIndex)
    {
        return myVector.operator[](anIndex);
    }
    template<class T>
    inline const T& RefillVector<T>::operator[](const SizeType anIndex) const
    {
        return myVector.operator[](anIndex);
    }
}
