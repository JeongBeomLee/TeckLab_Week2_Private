#pragma once
#include "Types.h"
#include "ObjectMacros.h"
#include "Array.h"
#include <mutex>

class UObject;

struct FUObjectItem
{
    UObject* Object;
    int32 Flags;            // 객체 상태 관리
    int32 ClusterRootIndex; // 객체 클러스터링 (그룹 GC)
    int32 SerialNumber;     // 객체 버전 관리 (약한 참조?)

    FUObjectItem()
        : Object(nullptr)
        , Flags(0)
        , ClusterRootIndex(-1)
        , SerialNumber(0)
    {
    }

    explicit FUObjectItem(UObject* InObject)
        : Object(InObject)
        , Flags(0)
        , ClusterRootIndex(-1)
        , SerialNumber(0)
    {
    }

    bool IsValid() const { return Object != nullptr; }
    bool IsValidLowLevel() const { return Object != nullptr; }
};

class FUObjectArray
{
public:
    FUObjectArray();
    ~FUObjectArray();

    int32 AllocateUObjectIndex(UObject* Object, bool bMergeDuplicates = true);
    void FreeUObjectIndex(UObject* Object);
    
    UObject* GetObjectPtr(int32 Index) const;
    FUObjectItem* GetObjectItemPtr(int32 Index) const;
    
    int32 GetMaxObjectsEver() const { return MaxObjectsEver; }
    int32 GetObjectArraySize() const { return (int32)ObjectList.size(); }
    int32 GetObjectArrayMax() const { return MaxObjectsEver; }
    
    void GetAllObjects(TArray<UObject*>& OutObjects) const;
    
    template<typename T>
    void GetObjectsOfClass(TArray<T*>& OutObjects) const
    {
        OutObjects.clear();
        for (int32 i = 0; i < ObjectList.size(); ++i)
        {
            const FUObjectItem& Item = ObjectList[i];
            if (Item.IsValid())
            {
                if (Item.Object->IsA<T>())
                {
                    T* CastedObject = static_cast<T*>(Item.Object);
                    OutObjects.push_back(CastedObject);
                }
            }
        }
    }

    // Mark-and-Sweep
    void PerformGarbageCollector();
    void MarkAsGarbage(UObject* Object);

private:
    TArray<FUObjectItem> ObjectList;
    TArray<int32> ObjectAvailableList;
    
    int32 MaxObjectsEver;
    int32 OpenForDisregardForGarbageCollection; // GC 무시 카운터
    
    int32 AllocateUObjectIndexInternal(UObject* Object, bool bMergeDuplicates);
    void FreeUObjectIndexInternal(int32 Index);
};

extern FUObjectArray GUObjectArray;
