// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


namespace Details
{
	template <typename...>
	constexpr bool AlwaysFalse = false;


	/**
	 * Functor를 받아 해당 Functor의 operator() 함수의 첫 번째 파라미터의 타입을 반환하는 Type Function
	 * @tparam FunctorType operator()를 구현하는 클래스 타입 (즉 Functor)
	 *
	 * 예시)
	 * const auto SomeFunc = [](int32){};
	 * static_assert(std::is_same_v<TGetFirstParam<decltype(SomeFunc)>::Type, int32>);
	 */
	template <typename FunctorType>
	struct TGetFirstParam
	{
	private:
		using FuncType = std::decay_t<FunctorType>;

		template<typename F, typename Ret>
		static void GetFirstParamHelper(Ret (F::*)());

		template<typename F, typename Ret>
		static void GetFirstParamHelper(Ret (F::*)() const);

		template<typename F, typename Ret, typename A, typename... Rest>
		static A GetFirstParamHelper(Ret (F::*)(A, Rest...));

		template<typename F, typename Ret, typename A, typename... Rest>
		static A GetFirstParamHelper(Ret (F::*)(A, Rest...) const);
		
	public:
		using Type = decltype(GetFirstParamHelper(&FuncType::operator()));
	};

	
	template <typename MaybeUStructType>
	concept CUStruct = requires { MaybeUStructType::StaticStruct(); };
	
	template <typename MaybeUObjectType>
	concept CUObject = std::is_base_of_v<UObject, MaybeUObjectType>;

	template <typename MaybeContainerType>
	concept CReflectedContainer = CUStruct<MaybeContainerType> || CUObject<MaybeContainerType>;

	/**
	 * UObject를 상속하는 클래스 또는 USTRUCT()로 선언된 구조체 타입에 대해 Unreal Reflection System이 생성한 UStruct를 반환합니다.
	 * 
	 * @tparam ContainerType Deduced parameter이므로 명시적으로 넘기지 않음
	 * @tparam Decayed Deduced parameter이므로 명시적으로 넘기지 않음
	 * @param Container UObject를 상속하는 객체에 대한 레퍼런스 또는 USTRUCT()로 선언된 구조체 객체에 대한 레퍼런스
	 * @return Container의 타입에 대해 생성된 UStruct
	 */
	template <CReflectedContainer ContainerType, typename Decayed = std::remove_pointer_t<std::decay_t<ContainerType>>>
	UStruct* GetUStructOf(ContainerType& Container)
	{
		if constexpr (CUStruct<Decayed>)
		{
			return Decayed::StaticStruct();
		}
		else if constexpr (CUObject<Decayed>)
		{
			return Container.GetClass();
		}
		else
		{
			static_assert(AlwaysFalse<Decayed>);
			return nullptr;
		}
	}

	/**
	 * 특정 C++ 타입에 대응되는 Unreal FProperty 타입을 반환하는 Type Function
	 * int32 -> FIntProperty, FString -> FStrProperty 등등
	 * 
	 * @tparam CPPType FProperty가 궁금한 C++ 타입
	 */
	template <typename CPPType>
	struct TGetFPropertyTypeFromCPPType;

	template <>
	struct TGetFPropertyTypeFromCPPType<int16>
	{
		using Type = FInt16Property;
	};

	template <>
	struct TGetFPropertyTypeFromCPPType<int32>
	{
		using Type = FIntProperty;
	};

	template <>
	struct TGetFPropertyTypeFromCPPType<float>
	{
		using Type = FFloatProperty;
	};

	template <>
	struct TGetFPropertyTypeFromCPPType<bool>
	{
		using Type = FBoolProperty;
	};

	template <>
	struct TGetFPropertyTypeFromCPPType<FString>
	{
		using Type = FStrProperty;
	};

	template <CUStruct StructType>
	struct TGetFPropertyTypeFromCPPType<StructType>
	{
		using Type = FStructProperty;
	};

	template <CUObject UObjectType>
	struct TGetFPropertyTypeFromCPPType<UObjectType*>
	{
		using Type = FObjectProperty;
	};

	template <typename ElementType>
	struct TGetFPropertyTypeFromCPPType<TArray<ElementType>>
	{
		using Type = FArrayProperty;
	};

	template <typename ElementType>
	struct TGetFPropertyTypeFromCPPType<TSoftObjectPtr<ElementType>>
	{
		using Type = FSoftObjectProperty;
	};

	template <typename ElementType>
	struct TGetFPropertyTypeFromCPPType<TSoftClassPtr<ElementType>>
	{
		using Type = FSoftClassProperty;
	};

	template <>
	struct TGetFPropertyTypeFromCPPType<FSoftObjectPtr>
	{
		using Type = FSoftObjectProperty;
	};

	template <typename KeyType, typename ValueType>
	struct TGetFPropertyTypeFromCPPType<TMap<KeyType, ValueType>>
	{
		using Type = FMapProperty;
	};
	// ~TGetFPropertyTypeFromCPPType


	/**
	 * C++ 타입과 FProperty 타입이 서로 일치하는지 검사합니다.
	 * 예를 들어서 int32와 일치하는 FProperty의 구현은 FIntProperty이고
	 * TArray<int32>와 일치하는 FProperty의 구현은 FArrayProperty::Inner의 타입이 FIntProperty인 FArrayProperty입니다.
	 * 
	 * @tparam TargetCPPType 검사할 C++ 타입
	 */
	template <typename TargetCPPType>
	struct TIsPropertyExactMatch
	{
		template <typename PropertyType>
		static bool Check(PropertyType* Property)
		{
			static_assert(std::is_same_v<typename TGetFPropertyTypeFromCPPType<TargetCPPType>::Type, PropertyType>);
			return true;
		}
	};

	template <CUStruct UStructType>
	struct TIsPropertyExactMatch<UStructType>
	{
		static bool Check(FStructProperty* Property)
		{
			return Property->Struct == UStructType::StaticStruct();
		}
	};

	template <CUObject UObjectType>
	struct TIsPropertyExactMatch<UObjectType*>
	{
		static bool Check(FObjectProperty* Property)
		{
			return Property->PropertyClass == UObjectType::StaticClass();
		}
	};

	template <typename ElementType>
	struct TIsPropertyExactMatch<TArray<ElementType>>
	{
		static bool Check(FArrayProperty* Property)
		{
			using ElementFPropertyType = typename TGetFPropertyTypeFromCPPType<ElementType>::Type;
			ElementFPropertyType* ElementProperty = CastField<ElementFPropertyType>(Property->Inner);
			return ElementProperty && TIsPropertyExactMatch<ElementType>::Check(ElementProperty);
		}
	};

	template <CUObject UObjectType>
	struct TIsPropertyExactMatch<TSoftObjectPtr<UObjectType>>
	{
		static bool Check(FSoftObjectProperty* Property)
		{
			return Property->PropertyClass == UObjectType::StaticClass();
		}
	};

	template <CUObject UObjectType>
	struct TIsPropertyExactMatch<TSoftClassPtr<UObjectType>>
	{
		static bool Check(FSoftClassProperty* Property)
		{
			return Property->MetaClass == UObjectType::StaticClass();
		}
	};

	template <typename KeyType, typename ValueType>
	struct TIsPropertyExactMatch<TMap<KeyType, ValueType>>
	{
		static bool Check(FMapProperty* Property)
		{
			using KeyFPropertyType = typename TGetFPropertyTypeFromCPPType<KeyType>::Type;
			using ValueFPropertyType = typename TGetFPropertyTypeFromCPPType<ValueType>::Type;

			KeyFPropertyType* KeyProperty = CastField<KeyFPropertyType>(Property->KeyProp);
			ValueFPropertyType* ValueProperty = CastField<ValueFPropertyType>(Property->ValueProp);

			return KeyProperty
				&& ValueProperty
				&& TIsPropertyExactMatch<KeyType>::Check(KeyProperty)
				&& TIsPropertyExactMatch<ValueType>::Check(ValueProperty);
		}
	};
	// ~TIsPropertyExactMatch


	template <typename TargetCPPType>
	struct TFieldIterationHelper
	{
		using TargetFPropertyType = typename TGetFPropertyTypeFromCPPType<TargetCPPType>::Type;

		template <typename ContainerType, typename FuncType>
		static void ForEach(ContainerType& DerefedContainer, FuncType&& Func)
		{
			ForEachExactMatchProperty(DerefedContainer, [&](TargetFPropertyType* Each)
			{
				// ContainerType 의 const 여부에 따라 const pointer 일 수도 있기 때문에 auto 로 받아야 함
				// (ContainerPtrToValuePtr의 구현이 Container의 const 여부에 따라 리턴 타입이 다르게 되어 있음)
				auto ValuePtr = Each->template ContainerPtrToValuePtr<TargetCPPType>(&DerefedContainer);
				return Func(*ValuePtr);
			});
		}

		template <typename ContainerType, typename FuncType>
		static void ForEachWithName(ContainerType& DerefedContainer, const FuncType& Func)
		{
			ForEachExactMatchProperty(DerefedContainer, [&](TargetFPropertyType* Each)
			{
				// ContainerType 의 const 여부에 따라 const pointer 일 수도 있기 때문에 auto 로 받아야 함
				// (ContainerPtrToValuePtr의 구현이 Container의 const 여부에 따라 리턴 타입이 다르게 되어 있음)
				auto ValuePtr = Each->template ContainerPtrToValuePtr<TargetCPPType>(&DerefedContainer);
				return Func(*ValuePtr, Each->GetName());
			});
		}

	private:
		template <typename ContainerType, typename FuncType>
		static void ForEachExactMatchProperty(ContainerType& DerefedContainer, FuncType&& Func)
		{
			for (TFieldIterator<TargetFPropertyType> It{ GetUStructOf(DerefedContainer) }; It; ++It)
			{
				if (TIsPropertyExactMatch<TargetCPPType>::Check(*It))
				{
					Func(*It);
				}
			}
		}
	};


	template <typename T>
	bool IsValidPointer(const T* Pointer)
	{
		if constexpr (CUObject<T>)
		{
			return IsValid(Pointer);
		}
		return Pointer != nullptr;
	}


	template <typename MaybePointerType>
	void DerefIfPointer(MaybePointerType&& MaybePointer, const auto& Func)
	{
		if constexpr (std::is_pointer_v<std::decay_t<MaybePointerType>>)
		{
			if (IsValidPointer(MaybePointer))
			{
				Func(*MaybePointer);
			}
		}
		else
		{
			Func(MaybePointer);
		}
	}
}


class FReflectionHelper
{
public:
	/**
	 * UObject를 상속하는 객체 또는 USTRUCT()로 선언된 구조체 객체의 멤버들 중 특정 타입인 것들을 순회합니다.
	 * 
	 * @tparam ContainerType Deduced Parameter이므로 명시적으로 넘기지 않음
	 * @tparam FuncType Deduced Parameter이므로 명시적으로 넘기지 않음
	 * @param Container UObject를 상속하는 객체 또는 USTRUCT()로 선언된 구조체 객체를 레퍼런스 또는 포인터로 넘김
	 * @param Func 방문되는 멤버가 유일한 파라미터인 Unary Function
	 */
	template <typename ContainerType, typename FuncType>
	static void ForEachMember(ContainerType&& Container, FuncType&& Func)
	{
		using TypeToIterate = std::decay_t<typename Details::TGetFirstParam<FuncType>::Type>;
		
		Details::DerefIfPointer(Container, [&](auto& Derefed)
		{
			Details::TFieldIterationHelper<TypeToIterate>::ForEach(Derefed, Func);
		});
	}

	/**
	 * UObject를 상속하는 객체 또는 USTRUCT()로 선언된 구조체 객체의 멤버들 중 특정 타입인 것들을 순회합니다.
	 * 위에 것과 다르게 Func의 두 번째 파라미터로 방문되는 멤버의 C++ 이름이 주어집니다.
	 * 
	 * @tparam ContainerType Deduced Parameter이므로 명시적으로 넘기지 않음
	 * @tparam FuncType Deduced Parameter이므로 명시적으로 넘기지 않음
	 * @param Container UObject를 상속하는 객체 또는 USTRUCT()로 선언된 구조체 객체를 레퍼런스 또는 포인터로 넘김
	 * @param Func 방문되는 멤버가 첫 번째 파라미터, 해당 멤버의 이름(FString)이 두 번째 파라미터인 Binary Function
	 */
	template <typename ContainerType, typename FuncType>
	static void ForEachMemberWithName(ContainerType&& Container, FuncType&& Func)
	{
		using TypeToIterate = std::decay_t<typename Details::TGetFirstParam<FuncType>::Type>;
		
		Details::DerefIfPointer(Container, [&](auto& Derefed)
		{
			Details::TFieldIterationHelper<TypeToIterate>::ForEachWithName(Derefed, Func);
		});
	}
};
