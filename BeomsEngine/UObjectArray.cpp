#include "pch.h"
#include "UObjectArray.h"
#include "Object.h"

FUObjectArray GUObjectArray;

FUObjectArray::FUObjectArray()
    : MaxObjectsEver(0)
    , OpenForDisregardForGarbageCollaction(0)
{
    ObjectList.reserve(1024);
    ObjectAvailableList.reserve(256);
}

FUObjectArray::~FUObjectArray()
{
    std::lock_guard<std::mutex> Lock(UObjectArrayMutex);
    
    for (int32 i = 0; i < ObjectList.size(); ++i)
    {
        FUObjectItem& Item = ObjectList[i];
        if (Item.IsValid())
        {
            Item.Object = nullptr;
        }
    }
    
    ObjectList.clear();
    ObjectAvailableList.clear();
}

int32 FUObjectArray::AllocateUObjectIndex(UObject* Object, bool bMergeDuplicates)
{
    std::lock_guard<std::mutex> Lock(UObjectArrayMutex);
    return AllocateUObjectIndexInternal(Object, bMergeDuplicates);
}

int32 FUObjectArray::AllocateUObjectIndexInternal(UObject* Object, bool bMergeDuplicates)
{
    if (!Object)
        return -1;
        
    int32 Index = -1;
    
    if (bMergeDuplicates)
    {
        for (int32 i = 0; i < ObjectList.size(); ++i)
        {
            if (ObjectList[i].Object == Object)
            {
                return i;
            }
        }
    }
    
    if (ObjectAvailableList.size() > 0)
    {
        Index = ObjectAvailableList.back();
        ObjectAvailableList.pop_back();
        ObjectList[Index] = FUObjectItem(Object);
    }
    else
    {
        Index = (int32)ObjectList.size();
        ObjectList.push_back(FUObjectItem(Object));
        MaxObjectsEver = std::max(MaxObjectsEver, (int32)ObjectList.size());
    }
    
    return Index;
}

void FUObjectArray::FreeUObjectIndex(UObject* Object)
{
    std::lock_guard<std::mutex> Lock(UObjectArrayMutex);
    
    for (int32 i = 0; i < ObjectList.size(); ++i)
    {
        if (ObjectList[i].Object == Object)
        {
            FreeUObjectIndexInternal(i);
            break;
        }
    }
}

void FUObjectArray::FreeUObjectIndexInternal(int32 Index)
{
    if (Index >= 0 && Index < ObjectList.size())
    {
        FUObjectItem& Item = ObjectList[Index];
        Item.Object = nullptr;
        Item.Flags = 0;
        Item.ClusterRootIndex = -1;
        Item.SerialNumber = 0;
        
        ObjectAvailableList.push_back(Index);
    }
}

UObject* FUObjectArray::GetObjectPtr(int32 Index) const
{
    std::lock_guard<std::mutex> Lock(UObjectArrayMutex);
    
    if (Index >= 0 && Index < ObjectList.size())
    {
        const FUObjectItem& Item = ObjectList[Index];
        return Item.IsValid() ? Item.Object : nullptr;
    }
    return nullptr;
}

FUObjectItem* FUObjectArray::GetObjectItemPtr(int32 Index) const
{
    std::lock_guard<std::mutex> Lock(UObjectArrayMutex);
    
    if (Index >= 0 && Index < ObjectList.size())
    {
        return const_cast<FUObjectItem*>(&ObjectList[Index]);
    }
    return nullptr;
}

void FUObjectArray::GetAllObjects(TArray<UObject*>& OutObjects) const
{
    std::lock_guard<std::mutex> Lock(UObjectArrayMutex);
    
    OutObjects.clear();
    OutObjects.reserve(ObjectList.size());
    
    for (int32 i = 0; i < ObjectList.size(); ++i)
    {
        const FUObjectItem& Item = ObjectList[i];
        if (Item.IsValid())
        {
            OutObjects.push_back(Item.Object);
        }
    }
}

void FUObjectArray::PerformGarbageCollection()
{
    std::lock_guard<std::mutex> Lock(UObjectArrayMutex);
    
    TArray<int32> ObjectsToDelete;
    
    for (int32 i = 0; i < ObjectList.size(); ++i)
    {
        FUObjectItem& Item = ObjectList[i];
        if (Item.IsValid() && Item.Object->IsPendingKill())
        {
            ObjectsToDelete.push_back(i);
        }
    }
    
    for (int32 Index : ObjectsToDelete)
    {
        FUObjectItem& Item = ObjectList[Index];
        if (Item.IsValid())
        {
            UObject* ObjectToDelete = Item.Object;
            FreeUObjectIndexInternal(Index);
            
            if (ObjectToDelete)
            {
                delete ObjectToDelete;
            }
        }
    }
}

void FUObjectArray::MarkAsGarbage(UObject* Object)
{
    if (Object)
    {
        Object->MarkPendingKill();
    }
}