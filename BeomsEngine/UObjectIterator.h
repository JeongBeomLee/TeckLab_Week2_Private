#pragma once

template <typename T>
class TObjectIterator
{
public:
	using ElementType = T;

	TObjectIterator()
		: Index(0)
		, EndIndex(GUObjectArray.GetObjectArrayNum())
	{
		Advance();
	}

	static TObjectIterator End()
	{
		TObjectIterator It;
		It.Index = It.EndIndex;
		It.CurrentObject = nullptr;
		return It;
	}

	// 역참조
	T* operator*() const { return CurrentObject; }
	T* operator->() const { return CurrentObject; }

	TObjectIterator& operator++()
	{
		++Index;
		Advance();
		return *this;
	}

	// Bool context (for while/for conditions)
	explicit operator bool() const { return CurrentObject != nullptr; }

	// Range-for compatibility
	bool operator!=(const TObjectIterator& InOther) const { return Index != InOther.Index; }

private:
	void Advance()
	{
		CurrentObject = nullptr;
		for (size_t i = Index; i < EndIndex; ++i)
		{
			UObject* Object = GUObjectArray.GetObjectPtr(static_cast<int32>(i));
			if (Object && IsValidObject(Object))
			{
				auto* CastedObject = Cast<T>(Object);
				if (CastedObject)
				{
					CurrentObject = CastedObject;
					Index = i;
					return;
				}
			}
		}
	}

	bool IsValidObject(UObject* Object) const
	{
		if (!Object)
		{
			return false;
		}

		// 가비지 컬렉션 대상 객체 제외
		if (Object->IsPendingKill())
		{
			return false;
		}

		return true;
	}

	size_t Index;
	size_t EndIndex;
	T* CurrentObject = nullptr;
};

template <typename T>
struct TObjectRange
{
	TObjectIterator<T> begin() const { return TObjectIterator<T>(); }
	TObjectIterator<T> end() const { return TObjectIterator<T>::End(); }
};

template <typename T>
inline TObjectRange<T> MakeObjectRange()
{
	return TObjectRange<T>();
}
