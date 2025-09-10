#include "pch.h"
#include "Object.h"
#include "USphere.h"
#include "ObjectMacros.h"
#include <iostream>

void TestRTTISystem()
{
    // 오브젝트 생성
    UObject* pObject = new UObject();
    USphere* pSphere = new USphere();
    
    std::cout << "=== RTTI System Test ===" << std::endl;
    
    // 1. 기본 UObject 테스트
    std::cout << "pObject class name: " << pObject->GetClassName().c_str() << std::endl;
    std::cout << "pSphere class name: " << pSphere->GetClassName().c_str() << std::endl;
    
    // 2. IsA 테스트 - 요청된 예제
    if (pSphere->IsA(USphere::GetStaticClass()))
    {
        std::cout << "HI" << std::endl;
    }
    
    // 3. 추가 IsA 테스트
    std::cout << "pObject->IsA(UObject): " << (pObject->IsA(UObject::GetStaticClass()) ? "true" : "false") << std::endl;
    std::cout << "pSphere->IsA(UObject): " << (pSphere->IsA(UObject::GetStaticClass()) ? "true" : "false") << std::endl;
    std::cout << "pSphere->IsA(USphere): " << (pSphere->IsA(USphere::GetStaticClass()) ? "true" : "false") << std::endl;
    std::cout << "pObject->IsA(USphere): " << (pObject->IsA(USphere::GetStaticClass()) ? "true" : "false") << std::endl;
    
    // 4. 문자열 기반 IsA 테스트
    std::cout << "pSphere->IsA(\"USphere\"): " << (pSphere->IsA(TEXT("USphere")) ? "true" : "false") << std::endl;
    std::cout << "pSphere->IsA(\"UObject\"): " << (pSphere->IsA(TEXT("UObject")) ? "true" : "false") << std::endl;
    
    // 5. Cast 테스트
    USphere* CastedSphere = Cast<USphere>(pSphere);
    USphere* FailedCast = Cast<USphere>(pObject);
    
    std::cout << "Cast<USphere>(pSphere): " << (CastedSphere ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Cast<USphere>(pObject): " << (FailedCast ? "SUCCESS" : "FAILED") << std::endl;
    
    // 6. 동적 속성 테스트
    if (CastedSphere)
    {
        CastedSphere->SetRadius(5.0f);
        std::cout << "Sphere radius: " << CastedSphere->GetRadius() << std::endl;
    }
    
    // 메모리 정리
    delete pObject;
    delete pSphere;
    
    std::cout << "=== Test Complete ===" << std::endl;
}