#include "pch.h"
#include "UObjectArray.h"
#include "Object.h"

FUObjectArray GUObjectArray;

FUObjectArray::FUObjectArray()
    : MaxObjectsEver(0)
    , OpenForDisregardForGarbageCollection(0)
{
    ObjectList.reserve(1024);
    ObjectAvailableList.reserve(256);
}

FUObjectArray::~FUObjectArray()
{
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
    return AllocateUObjectIndexInternal(Object, bMergeDuplicates);
}

int32 FUObjectArray::AllocateUObjectIndexInternal(UObject* Object, bool bMergeDuplicates)
{
    if (!Object)
        return -1;
        
    int32 Index = -1;
    
    // 중복 체크 옵션
    if (bMergeDuplicates)
    {
        for (int32 i = 0; i < ObjectList.size(); ++i)
        {
            if (ObjectList[i].Object == Object)
            {
                return i;   // 이미 존재하면 기존 인덱스 반환
            }
        }
    }
    
    // 재사용 가능 체크
    if (ObjectAvailableList.size() > 0)
    {
        Index = ObjectAvailableList.back();
        ObjectAvailableList.pop_back();
        ObjectList[Index] = FUObjectItem(Object);   // 슬롯 재사용
    }
    else
    {
        // 안되면 새 슬롯 생성
        Index = (int32)ObjectList.size();
        ObjectList.push_back(FUObjectItem(Object));
        MaxObjectsEver = std::max(MaxObjectsEver, (int32)ObjectList.size());
    }
    
    return Index;
}

void FUObjectArray::FreeUObjectIndex(UObject* Object)
{
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
    if (Index >= 0 && Index < ObjectList.size())
    {
        const FUObjectItem& Item = ObjectList[Index];
        return Item.IsValid() ? Item.Object : nullptr;
    }
    return nullptr;
}

FUObjectItem* FUObjectArray::GetObjectItemPtr(int32 Index) const
{
    if (Index >= 0 && Index < ObjectList.size())
    {
        return const_cast<FUObjectItem*>(&ObjectList[Index]);
    }
    return nullptr;
}

void FUObjectArray::GetAllObjects(TArray<UObject*>& OutObjects) const
{
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

void FUObjectArray::PerformGarbageCollector()
{
    // GC 비활성화 상태 체크
    if (OpenForDisregardForGarbageCollection > 0) {
        return;  // GC 실행하지 않고 종료
    }

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