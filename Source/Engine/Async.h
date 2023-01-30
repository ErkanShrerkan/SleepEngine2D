#pragma once
#pragma once
#include <ppltasks.h>
#include <functional>

template <class TResource>
class Async
{
private:
    concurrency::task<TResource*> myTask;

public:
    Async() = default;
    Async(std::function<TResource* ()>&& aMethod)
    {
        myTask = concurrency::create_task(aMethod);
    }

public:
    __inline const bool Done() const
    {
        return myTask.is_done();
    }
    __inline TResource* Raw() const
    {
        return myTask.is_done() ? myTask.get() : nullptr;
    }

public:
    __inline TResource* operator->() const
    {
        return Raw();
    }
    __inline TResource& operator*() const
    {
        return *Raw();
    }
    __inline bool operator==(TResource* aPointer)
    {
        return Raw() == aPointer;
    }
    __inline bool operator!=(TResource* aPointer)
    {
        return !(this->operator==(aPointer));
    }
    __inline explicit operator bool() const
    {
        return Raw() != nullptr;
    }
};

/// <summary>
/// Used as a quick way to create an asynchronius lambda
/// </summary>
template <>
class Async<void>
{
private:
    concurrency::task<void> myTask;

public:
    Async() = default;
    Async(std::function<void(void)>&& aMethod)
    {
        myTask = concurrency::create_task(aMethod);
    }

public:
    __inline const bool Done() const
    {
        return myTask.is_done();
    }
};
