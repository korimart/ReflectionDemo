// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReflectionHelperTest.generated.h"


USTRUCT()
struct FReflectionHelperTestStruct
{
	GENERATED_BODY()

	UPROPERTY()
	int16 Int16Member;

	UPROPERTY()
	int16 Int16Member2;

	UPROPERTY()
	int16 Int16Member3;
	
	UPROPERTY()
	int32 Int32Member;

	UPROPERTY()
	int32 Int32Member2;

	UPROPERTY()
	int32 Int32Member3;

	UPROPERTY()
	float FloatMember;

	UPROPERTY()
	float FloatMember2;

	UPROPERTY()
	float FloatMember3;

	UPROPERTY()
	bool BoolMember;

	UPROPERTY()
	bool BoolMember2;

	UPROPERTY()
	bool BoolMember3;
};


UCLASS()
class UReflectionHelperTestObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int16 Int16Member;

	UPROPERTY()
	int16 Int16Member2;

	UPROPERTY()
	int16 Int16Member3;
	
	UPROPERTY()
	int32 Int32Member;

	UPROPERTY()
	int32 Int32Member2;

	UPROPERTY()
	int32 Int32Member3;

	UPROPERTY()
	float FloatMember;

	UPROPERTY()
	float FloatMember2;

	UPROPERTY()
	float FloatMember3;

	UPROPERTY()
	bool BoolMember;

	UPROPERTY()
	bool BoolMember2;

	UPROPERTY()
	bool BoolMember3;

	UPROPERTY()
	FReflectionHelperTestStruct Struct;

	UPROPERTY()
	FReflectionHelperTestStruct Struct2;

	UPROPERTY()
	FReflectionHelperTestStruct Struct3;

	UPROPERTY()
	UReflectionHelperTestObject* Object;

	UPROPERTY()
	UReflectionHelperTestObject* Object2;

	UPROPERTY()
	UReflectionHelperTestObject* Object3;

	UPROPERTY()
	TArray<int32> Int32Array;

	UPROPERTY()
	TArray<int32> Int32Array2;

	UPROPERTY()
	TArray<float> FloatArray;

	UPROPERTY()
	TSoftObjectPtr<UReflectionHelperTestObject> SoftObjectPtr;

	UPROPERTY()
	TSoftObjectPtr<UReflectionHelperTestObject> SoftObjectPtr2;

	UPROPERTY()
	TSoftObjectPtr<UReflectionHelperTestObject> SoftObjectPtr3;

	UPROPERTY()
	TSoftObjectPtr<UReflectionHelperTestObject> SoftObjectPtr4;

	UPROPERTY()
	TSoftClassPtr<UReflectionHelperTestObject> SoftClassPtr;

	UPROPERTY()
	TMap<int32, bool> Map;

	UPROPERTY()
	TMap<int32, float> Map2;
};
