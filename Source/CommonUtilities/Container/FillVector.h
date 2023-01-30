#pragma once
#include <vector>

namespace CommonUtilities
{
    template <class T>
    class FillVector
    {
    public:
        using Iterator = typename std::vector<T>::iterator;
        using ConstIterator = typename std::vector<T>::const_iterator;
        using SizeType = typename std::vector<T>::size_type;

    private:
        std::vector<T> myVector;

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
            return myVector.end();
        }
        inline ConstIterator end() const
        {
            return myVector.end();
        }
        inline bool empty() const
        {
            return myVector.empty();
        }
        inline size_t size() const
        {
            return myVector.size();
        }
        inline void clear()
        {
            myVector.clear();
        }
        inline void push_back(T&& aValue)
        {
            myVector.push_back(aValue);
        }
        inline void push_back(const T& aValue)
        {
            myVector.push_back(aValue);
        }

    public:
        inline std::vector<T>& vector()
        {
            return myVector;
        }
        inline const std::vector<T>& vector() const
        {
            return myVector;
        }
        
    public:
        T& operator[](const SizeType anIndex);
        const T& operator[](const SizeType anIndex) const;
    };

    template<class T>
    inline T& FillVector<T>::operator[](const SizeType anIndex)
    {
        while(!(anIndex < myVector.size()))
        {
            myVector.emplace_back();
        }
        return myVector.operator[](anIndex);
    }
    template<class T>
    inline const T& FillVector<T>::operator[](const SizeType anIndex) const
    {
        while (!(anIndex < myVector.size()))
        {
            myVector.emplace_back();
        }
        return myVector.operator[](anIndex);
    }
}
