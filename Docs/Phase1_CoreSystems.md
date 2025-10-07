# Phase 1: Core Systems

## Overview
Core Systems는 BeomsEngine의 가장 기반이 되는 시스템으로, 모든 엔진 객체의 베이스가 되는 Object System, 메모리 관리, 그리고 Reflection(RTTI) 기능을 제공합니다.

## System Architecture

```
Core Systems
├── Object System (런타임 타입 정보 및 객체 관리)
│   ├── UObject (모든 엔진 객체의 베이스 클래스)
│   ├── UClass (메타 클래스 - 런타임 타입 정보)
│   └── FName (최적화된 문자열 시스템)
├── Memory Management (메모리 및 객체 생명주기 관리)
│   ├── FMemory (메모리 통계)
│   ├── FUObjectArray (전역 객체 배열)
│   └── GC (Mark-and-Sweep)
└── Reflection System (메타프로그래밍 및 객체 생성)
    ├── ObjectMacros.h (RTTI 매크로)
    └── FObjectInitializer (객체 초기화)
```

---

## 1. Object System

### 1.1 UObject (Object.h:17)
**최상위 베이스 클래스 - 모든 엔진 객체의 기반**

#### 역할
- 모든 게임 오브젝트의 공통 인터페이스 제공
- 객체 생명주기 관리 (BeginPlay, Tick, EndPlay)
- RTTI (런타임 타입 정보) 지원
- GC를 위한 객체 추적
- 계층 구조 관리 (Outer)

#### 주요 멤버 변수
```cpp
FName ObjectName;           // 객체 이름 (최적화된 문자열)
uint64 UniqueID;            // 전역 고유 ID
UObject* Outer;             // 소유자 객체 (계층 구조)
int32 InternalIndex;        // GUObjectArray에서의 인덱스
bool bIsValid;              // 유효성 플래그
bool bPendingKill;          // GC 대상 플래그
bool bHasBegunPlay;         // BeginPlay 호출 여부
```

#### 핵심 메서드
```cpp
// 생명주기
virtual void BeginPlay();
virtual void Tick(float DeltaTime);
virtual void EndPlay();

// RTTI
virtual UClass* GetClass() const;
static UClass* GetStaticClass();
bool IsA(const UClass* SomeClass) const;
template<typename T> bool IsA() const;

// 객체 정보
FName GetName() const;
uint64 GetUniqueID() const;
UObject* GetOuter() const;

// GC
bool IsValid() const;
void MarkPendingKill();
bool IsPendingKill() const;
```

#### 상속 관계
```
UObject (Root Class)
└── [모든 엔진 클래스들이 상속]
    ├── AActor
    ├── UActorComponent
    ├── UStaticMesh
    └── ...
```

---

### 1.2 UClass (Class.h:18)
**메타 클래스 - 런타임 타입 정보를 담는 클래스**

#### 역할
- 클래스의 메타데이터 저장 (이름, 부모 클래스)
- 동적 객체 생성 (Factory Pattern)
- 상속 관계 확인 (IsChildOf)
- 전역 클래스 레지스트리 관리

#### 주요 멤버 변수
```cpp
FName ClassName;                        // 클래스 이름
UClass* SuperClass;                     // 부모 클래스
ClassConstructorType Constructor;       // 생성자 함수 포인터

// 전역 레지스트리
static TArray<UClass*> RegisteredClasses;
static TMap<FName, UClass*> ClassMap;

// 자식 클래스 캐시
mutable TArray<UClass*> ChildClasses;
mutable bool bChildClassesCached;
```

#### 핵심 메서드
```cpp
// 클래스 정보
const FName& GetName() const;
UClass* GetSuperClass() const;

// 상속 관계 확인
bool IsChildOf(const UClass* SomeBase) const;
bool IsChildOf(const FName& BaseClassName) const;

// 객체 생성
UObject* CreateDefaultObject() const;

// 전역 레지스트리
static void RegisterClass(UClass* NewClass);
static UClass* FindClass(const FName& ClassName);
static const TArray<UClass*>& GetAllClasses();

// 계층 구조
void GetAllChildClasses(TArray<UClass*>& OutChildClasses) const;
bool HasChildren() const;
```

#### 디자인 패턴
- **Singleton Pattern**: 각 클래스마다 단일 UClass 인스턴스
- **Factory Pattern**: Constructor 함수 포인터로 동적 객체 생성
- **Registry Pattern**: 전역 클래스 등록 및 검색

---

### 1.3 FName (Name.h:166)
**최적화된 문자열 시스템 - String Interning**

#### 역할
- 문자열 중복 제거 (String Interning)
- 빠른 문자열 비교 (정수 비교)
- 메모리 효율적인 문자열 저장
- 대소문자 구분 없는 비교 지원

#### 주요 멤버 변수
```cpp
FNameEntryId ComparisonIndex;      // 전역 풀 인덱스 (비교용)
FNameDisplayIndex DisplayIndex;    // 표시 변형 인덱스 (대소문자 보존)
```

#### 핵심 메서드
```cpp
// 생성
explicit FName(const FString& InName);
explicit FName(const char* InName);

// 비교 (O(1) 정수 비교)
bool operator==(const FName& Other) const;
bool operator!=(const FName& Other) const;
bool operator<(const FName& Other) const;

// 문자열 변환
FString ToString() const;
FString ToComparisonString() const;

// 유효성
bool IsValid() const;
bool IsNone() const;

// 해시
size_t GetHash() const;
```

#### FNamePool (Name.h:24)
**전역 문자열 풀 - Singleton**

```cpp
// String Interning 구조
TArray<FNameEntry> NameEntries;              // 모든 고유 문자열
std::unordered_map<FString, FNameEntryId> StringToIdMap;  // 검색용 해시맵

// 주요 메서드
std::pair<FNameEntryId, FNameDisplayIndex> Store(const FString& InString);
const FString& Resolve(FNameEntryId Id, FNameDisplayIndex DisplayIndex) const;
```

#### 디자인 패턴
- **Flyweight Pattern**: 문자열 공유로 메모리 절약
- **Singleton Pattern**: FNamePool 전역 인스턴스

---

## 2. Memory Management

### 2.1 FMemory (Memory.h:4)
**메모리 통계 추적**

#### 역할
- 전역 메모리 할당 추적
- 메모리 사용량 통계

#### 주요 멤버 변수
```cpp
static uint32 TotalAllocationBytes;   // 총 할당 바이트
static uint32 TotalAllocationCount;   // 총 할당 횟수
```

---

### 2.2 FUObjectArray (UObjectArray.h:36)
**전역 객체 배열 - GC 관리자**

#### 역할
- 모든 UObject 인스턴스 추적
- Garbage Collection (Mark-and-Sweep)
- 객체 검색 및 순회
- 객체 인덱스 관리

#### 주요 멤버 변수
```cpp
TArray<FUObjectItem> ObjectList;           // 모든 객체
TArray<int32> ObjectAvailableList;         // 재사용 가능한 인덱스
int32 MaxObjectsEver;                      // 최대 객체 수
int32 OpenForDisregardForGarbageCollection; // GC 무시 카운터
```

#### FUObjectItem (UObjectArray.h:9)
**객체 추적 정보**

```cpp
UObject* Object;              // 실제 객체 포인터
int32 Flags;                  // 객체 상태 플래그
int32 ClusterRootIndex;       // 객체 클러스터링 (그룹 GC)
int32 SerialNumber;           // 객체 버전 관리 (약한 참조)
```

#### 핵심 메서드
```cpp
// 객체 인덱스 관리
int32 AllocateUObjectIndex(UObject* Object, bool bMergeDuplicates = true);
void FreeUObjectIndex(UObject* Object);

// 객체 접근
UObject* GetObjectPtr(int32 Index) const;
UObject* operator[](int32 Index) const;

// 객체 검색
void GetAllObjects(TArray<UObject*>& OutObjects) const;
template<typename T>
void GetObjectsOfClass(TArray<T*>& OutObjects) const;

// Garbage Collection
void PerformGarbageCollector();
void MarkAsGarbage(UObject* Object);
```

#### GC 알고리즘
**Mark-and-Sweep 방식**
1. **Mark Phase**: 루트 객체부터 시작해 도달 가능한 모든 객체 마킹
2. **Sweep Phase**: 마킹되지 않은 객체 제거

#### 전역 인스턴스
```cpp
extern FUObjectArray GUObjectArray;  // 전역 싱글톤
```

---

## 3. Reflection System

### 3.1 ObjectMacros.h
**RTTI 매크로 시스템 - 메타프로그래밍**

#### 핵심 매크로

##### UCLASS() (ObjectMacros.h:8)
클래스에 RTTI 기능 추가
```cpp
#define UCLASS()
    public:
        static UClass* GetStaticClass();
        virtual UClass* GetClass() const override;
    private:
```

##### GENERATED_BODY(ClassName, SuperClassName) (ObjectMacros.h:15)
클래스 내부에서 타입 정의 및 생성 함수 추가
```cpp
#define GENERATED_BODY(ClassName, SuperClassName)
    public:
        typedef ClassName ThisClass;
        typedef SuperClassName Super;
        static UObject* CreateInstance() { return new ClassName(); }
    private:
```

##### IMPLEMENT_CLASS(ClassName, SuperClassName) (ObjectMacros.h:23)
cpp 파일에서 UClass 등록 구현
```cpp
#define IMPLEMENT_CLASS(ClassName, SuperClassName)
    UClass* ClassName::GetStaticClass()
    {
        static UClass* StaticClass = nullptr;
        if (!StaticClass)
        {
            StaticClass = new UClass(
                FName(#ClassName),
                SuperClassName::GetStaticClass(),
                &ClassName::CreateInstance
            );
            UClass::RegisterClass(StaticClass);
        }
        return StaticClass;
    }
    UClass* ClassName::GetClass() const
    {
        return ClassName::GetStaticClass();
    }
```

##### IMPLEMENT_ROOT_CLASS(ClassName) (ObjectMacros.h:44)
최상위 클래스용 (SuperClass가 없는 경우)

#### 타입 캐스팅 매크로

```cpp
// 동적 캐스팅
#define CAST(TargetType, SourceObject)
    Cast<TargetType>(SourceObject)

// 안전한 캐스팅 (nullptr 체크)
#define SAFE_CAST(TargetType, SourceObject)
    (SourceObject ? Cast<TargetType>(SourceObject) : nullptr)

// 타입 체크
#define IS_A(Object, TargetClass)
    (Object && Object->IsA(TargetClass::GetStaticClass()))

// 유효성 체크
#define IS_VALID(Object)
    (Object && Object->IsValid())
```

#### 템플릿 캐스트 함수 (ObjectMacros.h:82)
```cpp
template<typename T>
T* Cast(UObject* Object)
{
    if (!Object)
        return nullptr;

    if (Object->IsA(T::GetStaticClass()))
        return static_cast<T*>(Object);

    return nullptr;
}
```

---

### 3.2 FObjectInitializer (ObjectInitializer.h:11)
**객체 초기화 헬퍼 - Builder Pattern**

#### 역할
- 객체 생성 시 초기화 정보 전달
- Fluent Interface로 설정 (Method Chaining)
- Outer, Name, Class 정보 관리

#### 주요 멤버 변수
```cpp
UObject* Outer;          // 소유자 객체
FName ObjectName;        // 객체 이름
UClass* ObjectClass;     // 객체 클래스 타입
```

#### 핵심 메서드
```cpp
// 생성자
explicit FObjectInitializer(UObject* InOuter, const FName& InName, UClass* InClass);

// Fluent Interface (Method Chaining)
FObjectInitializer& SetOuter(UObject* InOuter);
FObjectInitializer& SetName(const FName& InName);
FObjectInitializer& SetClass(UClass* InClass);

// 객체 생성
UObject* CreateObject() const;

// 유효성 검사
bool IsValid() const;
```

#### 전역 헬퍼 함수

##### NewObject (ObjectInitializer.h:74)
일반 객체 생성
```cpp
template<typename T>
T* NewObject(UObject* Outer, const FName& Name)
{
    FObjectInitializer ObjectInitializer(Outer, Name, T::GetStaticClass());
    UObject* NewObj = ObjectInitializer.CreateObject();
    return static_cast<T*>(NewObj);
}
```

##### CreateDefaultSubobject (ObjectInitializer.h:104)
서브 오브젝트(컴포넌트) 생성
```cpp
template<typename T>
T* CreateDefaultSubobject(UObject* Outer, const FName& SubobjectName)
{
    FObjectInitializer SubInitializer;
    SubInitializer.SetOuter(Outer)
                  .SetName(SubobjectName)
                  .SetClass(T::GetStaticClass());

    T* NewSubobject = static_cast<T*>(SubInitializer.CreateObject());
    return NewSubobject;
}
```

#### 디자인 패턴
- **Builder Pattern**: FObjectInitializer
- **Fluent Interface**: Method Chaining (SetOuter().SetName().SetClass())
- **Factory Pattern**: NewObject/CreateDefaultSubobject

---

## Key Flows

### 객체 생성 흐름

```
1. NewObject<T>(Outer, Name) 호출
   ↓
2. FObjectInitializer 생성 (Outer, Name, T::GetStaticClass())
   ↓
3. FObjectInitializer::CreateObject()
   ↓
4. UClass->Constructor() 호출 (new T())
   ↓
5. UObject 생성자
   ├─ UniqueID 생성
   ├─ Outer 설정
   ├─ ObjectName 설정
   └─ GUObjectArray::AllocateUObjectIndex() 등록
   ↓
6. T* 반환
```

### RTTI (IsA) 확인 흐름

```
1. object->IsA<AActor>() 호출
   ↓
2. object->IsA(AActor::GetStaticClass()) 호출
   ↓
3. object->GetClass() 획득 (런타임 타입)
   ↓
4. UClass::IsChildOf() 확인
   ├─ 현재 클래스와 비교
   ├─ 같으면 true 반환
   └─ 다르면 SuperClass로 재귀 확인
   ↓
5. bool 반환
```

### Garbage Collection 흐름

```
1. GUObjectArray::PerformGarbageCollector() 호출
   ↓
2. Mark Phase
   ├─ 모든 객체의 마킹 플래그 초기화
   ├─ 루트 객체부터 시작
   └─ 참조 그래프 순회하며 마킹
   ↓
3. Sweep Phase
   ├─ ObjectList 순회
   ├─ 마킹되지 않은 객체 찾기
   ├─ delete 호출
   └─ FreeUObjectIndex() 호출
   ↓
4. ObjectAvailableList에 인덱스 반환
```

### FName String Interning 흐름

```
1. FName name("MyObject") 생성
   ↓
2. FNamePool::Store("MyObject") 호출
   ↓
3. 소문자로 변환: "myobject"
   ↓
4. StringToIdMap에서 검색
   ├─ 존재하면: 기존 ID 반환
   └─ 없으면: 새 엔트리 생성
       ├─ NameEntries.push_back(FNameEntry)
       ├─ StringToIdMap["myobject"] = NewId
       └─ NewId 반환
   ↓
5. FName{ ComparisonIndex, DisplayIndex } 생성
```

---

## Design Patterns Summary

### 1. Singleton Pattern
- **FNamePool**: 전역 문자열 풀
- **FUObjectArray (GUObjectArray)**: 전역 객체 배열
- **UClass::RegisteredClasses**: 전역 클래스 레지스트리

### 2. Factory Pattern
- **UClass::CreateDefaultObject()**: 동적 객체 생성
- **ClassConstructorType**: 함수 포인터로 생성 추상화

### 3. Flyweight Pattern
- **FName / FNamePool**: 문자열 중복 제거 및 공유

### 4. Builder Pattern
- **FObjectInitializer**: 객체 생성 설정 관리

### 5. Registry Pattern
- **UClass::RegisterClass()**: 클래스 등록 및 검색

### 6. Template Method Pattern
- **UObject 생명주기**: BeginPlay() → Tick() → EndPlay()

---

## Class Hierarchy Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                      Core Systems                            │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐    ┌──────────────┐    ┌──────────────────┐
│ Object System │    │   Memory     │    │   Reflection     │
└───────────────┘    └──────────────┘    └──────────────────┘
        │                     │                     │
        │                     │                     │
   ┌────┴────┐          ┌─────┴─────┐         ┌────┴─────┐
   │         │          │           │         │          │
   ▼         ▼          ▼           ▼         ▼          ▼
UObject   UClass    FMemory   FUObjectArray  Macros  FObjectInit
   │         │          │           │           │          │
   │         │          │           │           │          │
   ▼         ▼          ▼           ▼           ▼          ▼
 FName   Registry   Stats    GC(Mark&Sweep)  RTTI     Builder
```

### UObject 상속 계층 (간략화)

```
UObject (Root)
├── AActor
│   ├── AStaticMeshActor
│   └── [기타 Actor 클래스들]
├── UActorComponent
│   ├── USceneComponent
│   │   ├── UPrimitiveComponent
│   │   │   └── UMeshComponent
│   │   │       └── UStaticMeshComponent
│   │   └── [기타 Scene Component들]
│   └── [기타 Actor Component들]
├── UStaticMesh
├── UMaterialInterface
├── UWorld
├── ULevel
└── [기타 UObject 파생 클래스들]
```

---

## 다른 시스템과의 관계

### Core Systems → Rendering Pipeline
- UObject를 상속받아 렌더링 가능한 객체 생성
- FUObjectArray를 통해 렌더 대상 검색
- RTTI로 동적 타입 체크

### Core Systems → Engine Core
- Actor/Component 시스템의 기반
- World/Level 관리 객체
- Asset 클래스들의 베이스

### Core Systems → Editor
- Editor UI 객체들의 베이스
- 에디터 도구들의 리플렉션 활용
- 런타임 객체 검색 및 조작

---

## 참고: 클래스 매핑

| 필터 경로 | 클래스/파일 | 주요 역할 |
|---------|----------|---------|
| Engine/Core/Object | UObject.h | 최상위 베이스 클래스 |
| Engine/Core/Object | UClass.h | 메타 클래스 (RTTI) |
| Engine/Core/Object | ObjectMacros.h | RTTI 매크로 시스템 |
| Engine/Core/Object | FName.h | 최적화된 문자열 |
| Engine/Core/Object | UObjectArray.h | 전역 객체 배열 (GC) |
| Engine/Core/Object | ObjectInitializer.h | 객체 생성 헬퍼 |
| Engine/Core/Memory | Memory.h | 메모리 통계 |

---

## 학습 체크리스트

- [ ] UObject가 제공하는 기본 인터페이스 이해
- [ ] UClass의 역할과 RTTI 동작 원리 이해
- [ ] FName의 String Interning 최적화 이해
- [ ] GUObjectArray의 GC (Mark-and-Sweep) 이해
- [ ] RTTI 매크로 시스템 사용법 숙지
- [ ] FObjectInitializer의 Builder Pattern 이해
- [ ] NewObject vs CreateDefaultSubobject 차이 이해
- [ ] 각 시스템에서 사용된 디자인 패턴 파악

---

**다음 단계**: Phase 2 - Rendering Pipeline 분석
