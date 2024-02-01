// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReflectionHelperTest.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "ReflectionHelper.h"

#if WITH_DEV_AUTOMATION_TESTS


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReflectionHelperTest, "ReflectionHelperTest",
                                 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FReflectionHelperTest::RunTest(const FString& Parameters)
{
	{
		UReflectionHelperTestObject* Target = NewObject<UReflectionHelperTestObject>();
		Target->Int32Member = 11;
		Target->Int32Member2 = 22;
		Target->Int32Member3 = 33;

		TArray<int32> Collected;

		FReflectionHelper::ForEachMember(Target, [&](int32 Each)
		{
			Collected.Add(Each);
		});

		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[0], 11);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[1], 22);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[2], 33);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected.Num(), 3);
	}

	{
		UReflectionHelperTestObject* Target = NewObject<UReflectionHelperTestObject>();
		Target->Struct.Int32Member = 42;
		Target->Struct2.Int32Member = 43;
		Target->Struct3.Int32Member = 44;
		
		TArray<int32> Collected;

		FReflectionHelper::ForEachMember(*Target, [&](const FReflectionHelperTestStruct& Each)
		{
			Collected.Add(Each.Int32Member);
		});

		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[0], 42);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[1], 43);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[2], 44);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected.Num(), 3);
	}

	{
		UReflectionHelperTestObject* Target = NewObject<UReflectionHelperTestObject>();
		Target->Object = NewObject<UReflectionHelperTestObject>();
		Target->Object->FloatMember = 100.f;
		Target->Object2 = NewObject<UReflectionHelperTestObject>();
		Target->Object2->FloatMember = 200.f;
		Target->Object3 = NewObject<UReflectionHelperTestObject>();
		Target->Object3->FloatMember = 300.f;
		
		TArray<float> Collected;

		FReflectionHelper::ForEachMember(Target, [&](UReflectionHelperTestObject* Each)
		{
			Collected.Add(Each->FloatMember);
		});

		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[0], 100.f);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[1], 200.f);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[2], 300.f);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected.Num(), 3);
	}

	{
		UReflectionHelperTestObject* Target = NewObject<UReflectionHelperTestObject>();
		Target->Int32Array.Add(0);
		Target->Int32Array.Add(1);
		Target->Int32Array.Add(2);
		Target->Int32Array2.Add(3);
		Target->Int32Array2.Add(4);
		Target->Int32Array2.Add(5);

		TArray<int32> Collected;

		FReflectionHelper::ForEachMember(Target, [&](const TArray<int32>& Each)
		{
			Collected += Each;
		});

		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[0], 0);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[1], 1);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[2], 2);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[3], 3);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[4], 4);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[5], 5);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected.Num(), 6);
	}

	{
		FReflectionHelperTestStruct Target;
		Target.BoolMember = false;
		Target.BoolMember2 = true;
		Target.BoolMember3 = false;
		
		TArray<bool> Collected;

		FReflectionHelper::ForEachMember(Target, [&](bool& Each)
		{
			Collected.Add(Each);
		});

		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[0], false);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[1], true);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected[2], false);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), Collected.Num(), 3);
	}

	{
		UReflectionHelperTestObject* Target = NewObject<UReflectionHelperTestObject>();

		int32 ObjectCount = 0;
		int32 ClassCount = 0;

		FReflectionHelper::ForEachMember(Target, [&](TSoftObjectPtr<UReflectionHelperTestObject> Each)
		{
			ObjectCount++;
		});

		FReflectionHelper::ForEachMember(Target, [&](TSoftClassPtr<UReflectionHelperTestObject> Each)
		{
			ClassCount++;
		});

		TestEqual(TEXT("새 방식으로 Reflection 테스트"), ObjectCount, 4);
		TestEqual(TEXT("새 방식으로 Reflection 테스트"), ClassCount, 1);
	}

	{
		FReflectionHelperTestStruct Target{};

		FReflectionHelper::ForEachMember(Target, [Counter = 15](int16& Each) mutable
		{
			Each = Counter++;
		});

		TestEqual(TEXT("람다 파라미터 레퍼런스로 받으면 수정 되는지 테스트"), Target.Int16Member, 15);
		TestEqual(TEXT("람다 파라미터 레퍼런스로 받으면 수정 되는지 테스트"), Target.Int16Member2, 16);
		TestEqual(TEXT("람다 파라미터 레퍼런스로 받으면 수정 되는지 테스트"), Target.Int16Member3, 17);
		TestEqual(TEXT("람다 파라미터 레퍼런스로 받으면 수정 되는지 테스트"), Target.Int32Member, 0);
		TestEqual(TEXT("람다 파라미터 레퍼런스로 받으면 수정 되는지 테스트"), Target.Int32Member2, 0);
		TestEqual(TEXT("람다 파라미터 레퍼런스로 받으면 수정 되는지 테스트"), Target.Int32Member3, 0);
	}

	{
		UReflectionHelperTestObject* Target = NewObject<UReflectionHelperTestObject>();
		Target->Object = NewObject<UReflectionHelperTestObject>();

		TestTrue(TEXT("람다 파라미터 레퍼런스로 받으면 수정 되는지 테스트"), Target->Object != nullptr);

		FReflectionHelper::ForEachMember(Target, [](UReflectionHelperTestObject*& Each)
		{
			Each = nullptr;
		});

		TestTrue(TEXT("람다 파라미터 레퍼런스로 받으면 수정 되는지 테스트"), Target->Object == nullptr);
	}

	{
		const FReflectionHelperTestStruct ConstTarget{};

		// const 없으면 컴파일 실패해야 함
		//FReflectionHelper::ForEachMember(ConstTarget, [&](bool& Each) { }); 
		FReflectionHelper::ForEachMember(ConstTarget, [&](const bool& Each) { }); 
		FReflectionHelper::ForEachMember(ConstTarget, [&](bool Each) { }); 

		// const 없으면 컴파일 실패해야 함
		//FReflectionHelper::ForEachMember(&ConstTarget, [&](bool& Each) { }); 
		FReflectionHelper::ForEachMember(&ConstTarget, [&](const bool& Each) { }); 
		FReflectionHelper::ForEachMember(&ConstTarget, [&](bool Each) { });

		const UReflectionHelperTestObject* ConstTarget2 = nullptr;

		// const 없으면 컴파일 실패해야 함
		//FReflectionHelper::ForEachMember(ConstTarget2, [&](bool& Each) { }); 
		FReflectionHelper::ForEachMember(ConstTarget2, [&](const bool& Each) { }); 
		FReflectionHelper::ForEachMember(ConstTarget2, [&](bool Each) { }); 
	}

	{
		UObject* Target = NewObject<UReflectionHelperTestObject>();

		int32 Count = 0;

		FReflectionHelper::ForEachMember(Target, [&Count](float Each)
		{
			Count++;
		});

		TestEqual(TEXT("Base Class Pointer 를 가지고 Derived Class 멤버 순회할 수 있는지 테스트"), Count, 3);
	}

	{
		UObject* Target = NewObject<UReflectionHelperTestObject>();

		int32 Count = 0;

		FReflectionHelper::ForEachMember(Target, [&Count](FSoftObjectPtr& Each)
		{
			Count++;
		});

		TestEqual(TEXT("FSoftObjectPtr 를 지정해도 TSoftObjectPtr, TSoftClassPtr 들이 순회 되는지 체크"), Count, 5);
	}

	{
		UReflectionHelperTestObject* Target = NewObject<UReflectionHelperTestObject>();

		Target->Map.FindOrAdd(0) = false;
		Target->Map.FindOrAdd(1) = true;
		Target->Map.FindOrAdd(2) = true;
		Target->Map.FindOrAdd(3) = false;

		Target->Map2.FindOrAdd(0) = 10.f;
		Target->Map2.FindOrAdd(1) = 20.f;
		Target->Map2.FindOrAdd(2) = 30.f;
		Target->Map2.FindOrAdd(3) = 40.f;

		TArray<bool> Collected;
		TArray<float> Collected2;

		FReflectionHelper::ForEachMember(Target, [&](TMap<int32, bool>& Each)
		{
			Collected.Add(Each[0]);
			Collected.Add(Each[1]);
			Collected.Add(Each[2]);
			Collected.Add(Each[3]);
		});

		FReflectionHelper::ForEachMember(Target, [&](TMap<int32, float>& Each)
		{
			Collected2.Add(Each[0]);
			Collected2.Add(Each[1]);
			Collected2.Add(Each[2]);
			Collected2.Add(Each[3]);
		});

		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected[0], false);
		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected[1], true);
		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected[2], true);
		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected[3], false);
		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected.Num(), 4);

		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected2[0], 10.f);
		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected2[1], 20.f);
		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected2[2], 30.f);
		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected2[3], 40.f);
		TestEqual(TEXT("Map 순회 되는지 테스트"), Collected.Num(), 4);
	}

	{
		UReflectionHelperTestObject* Target = NewObject<UReflectionHelperTestObject>();

		TArray<FString> Names;

		FReflectionHelper::ForEachMemberWithName(Target, [&](int32& Each, const FString& Name)
		{
			Names.Add(Name);
		});

		TestEqual(TEXT("이름이랑 같이 순회 되는지 테스트"), Names[0], TEXT("Int32Member"));
		TestEqual(TEXT("이름이랑 같이 순회 되는지 테스트"), Names[1], TEXT("Int32Member2"));
		TestEqual(TEXT("이름이랑 같이 순회 되는지 테스트"), Names[2], TEXT("Int32Member3"));
		TestEqual(TEXT("이름이랑 같이 순회 되는지 테스트"), Names.Num(), 3);
	}
	
	return true;
}

#endif
#endif
