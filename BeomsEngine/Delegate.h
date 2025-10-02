#pragma once
#include <functional>
#include <vector>

template<typename... Args>
class TDelegate
{
private:
    std::function<void(Args...)> Function;

public:
    TDelegate() = default;

    template<typename T>
    void BindLambda(T&& Lambda)
    {
        Function = std::forward<T>(Lambda);
    }

    template<typename T, typename Method>
    void BindRaw(T* Object, Method&& method)
    {
        Function = [Object, method](Args... args) {
            (Object->*method)(args...);
        };
    }

    template<typename T, typename Method>
    void BindUObject(T* Object, Method&& method)
    {
        Function = [Object, method](Args... args) {
            if (Object && Object->IsValid())
            {
                (Object->*method)(args...);
            }
        };
    }

    void Execute(Args... args)
    {
        if (Function)
        {
            Function(args...);
        }
    }

    void ExecuteIfBound(Args... args)
    {
        Execute(args...);
    }

    bool IsBound() const
    {
        return static_cast<bool>(Function);
    }

    void Unbind()
    {
        Function = nullptr;
    }
};

template<typename... Args>
class TMulticastDelegate
{
private:
    std::vector<std::function<void(Args...)>> Functions;

public:
    TMulticastDelegate() = default;

    template<typename T>
    void AddLambda(T&& Lambda)
    {
        Functions.emplace_back(std::forward<T>(Lambda));
    }

    template<typename T, typename Method>
    void AddRaw(T* Object, Method&& method)
    {
        Functions.emplace_back([Object, method](Args... args) {
            (Object->*method)(args...);
        });
    }

    template<typename T, typename Method>
    void AddUObject(T* Object, Method&& method)
    {
        Functions.emplace_back([Object, method](Args... args) {
            if (Object && Object->IsValid())
            {
                (Object->*method)(args...);
            }
        });
    }

    void Broadcast(Args... args)
    {
        for (auto& Function : Functions)
        {
            if (Function)
            {
                Function(args...);
            }
        }
    }

    void Clear()
    {
        Functions.clear();
    }

    bool IsBound() const
    {
        return !Functions.empty();
    }

    size_t GetBoundFunctionCount() const
    {
        return Functions.size();
    }
};

// 매크로 정의
#define DECLARE_DELEGATE(DelegateName) \
    using DelegateName = TDelegate<>;

#define DECLARE_DELEGATE_OneParam(DelegateName, Param1Type) \
    using DelegateName = TDelegate<Param1Type>;

#define DECLARE_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type) \
    using DelegateName = TDelegate<Param1Type, Param2Type>;

#define DECLARE_DELEGATE_ThreeParams(DelegateName, Param1Type, Param2Type, Param3Type) \
    using DelegateName = TDelegate<Param1Type, Param2Type, Param3Type>;

#define DECLARE_MULTICAST_DELEGATE(DelegateName) \
    using DelegateName = TMulticastDelegate<>;

#define DECLARE_MULTICAST_DELEGATE_OneParam(DelegateName, Param1Type) \
    using DelegateName = TMulticastDelegate<Param1Type>;

#define DECLARE_MULTICAST_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type) \
    using DelegateName = TMulticastDelegate<Param1Type, Param2Type>;

#define DECLARE_MULTICAST_DELEGATE_ThreeParams(DelegateName, Param1Type, Param2Type, Param3Type) \
    using DelegateName = TMulticastDelegate<Param1Type, Param2Type, Param3Type>;