// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataStructures.generated.h"

USTRUCT()
struct NOCUBETECH_API FActorGrid
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<int64, TWeakObjectPtr<AActor>> data;

	inline int64 getKey(int32 x, int32 y)
	{
		return static_cast<int64>(x + 1000) << 32 | static_cast<int64>(y + 1000);
		return static_cast<int64>(static_cast<uint64>(x) << 32 | static_cast<uint64>(y) & 0x00000000FFFFFFFF);
	}

public:

	FActorGrid() {

		data = TMap<int64, TWeakObjectPtr<AActor>>();
	};
	
	void Put(int32 x, int32 y, AActor* actor) {
		if (actor != nullptr) {
			data.Add(getKey(x, y), TWeakObjectPtr<AActor>(actor));
		}
	}

	TWeakObjectPtr<AActor> Get(int32 x, int32 y) {
		return data[getKey(x, y)];
	}

	bool Contains(int32 x, int32 y) {
		int64 key = getKey(x, y);
		TWeakObjectPtr<AActor>* result = data.Find(key);
		if (result == nullptr) {
			return false;
		}
		else {
			if (result->IsValid()) {
				return true;
			}
			else {
				data.Remove(key);
				return false;
			}
		}
	};

	void CheckAndRemove(int32 x, int32 y) {
		data.Remove(getKey(x, y));
	}
};


USTRUCT()
struct NOCUBETECH_API FBoolGrid
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<int64, bool> data;

	inline int64 getKey(int32 x, int32 y)
	{
		return static_cast<int64>(x + 1000) << 32 | static_cast<int64>(y + 1000);
		return static_cast<int64>(static_cast<uint64>(x) << 32 | static_cast<uint64>(y) & 0x00000000FFFFFFFF);
	}

public:

	FBoolGrid() {

		data = TMap<int64, bool>();
	};
	
	void Put(int32 x, int32 y, bool value) {
		data.Add(getKey(x, y), value);
	}

	bool Get(int32 x, int32 y) {
		return data[getKey(x, y)];
	}

	bool Contains(int32 x, int32 y) {
		int64 key = getKey(x, y);
		bool* result = data.Find(key);
		if (result == nullptr) {
			return false;
		}
		else {
			return *result;
		}
	};
};

USTRUCT()
struct NOCUBETECH_API FActorList {
	GENERATED_BODY()
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> lst;
	FActorList() {
		lst = TArray<TWeakObjectPtr<AActor>>();
	}
};

USTRUCT()
struct NOCUBETECH_API FActorListGrid {
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<int64, FActorList> data;

	inline int64 getKey(int32 x, int32 y)
	{
		return static_cast<int64>(x + 1000) << 32 | static_cast<int64>(y + 1000);
		return static_cast<int64>(static_cast<uint64>(x) << 32 | static_cast<uint64>(y) & 0x00000000FFFFFFFF);
	}

public:
	FActorListGrid() {
		data = TMap<int64, FActorList>();
	}

	void Add(int32 x, int32 y, AActor* actor) {
		int64 key = getKey(x, y);
		if (!data.Contains(key)) {
			data.Emplace(key);
		}
		data[key].lst.Add(TWeakObjectPtr<AActor>(actor));
	}

	bool Contains(int32 x, int32 y, AActor* actor) {
		int64 key = getKey(x, y);
		if (!data.Contains(key)) {
			return false;
		}
		for (const TWeakObjectPtr<AActor>& ptr : data[key].lst) {
			if (ptr.IsValid() && ptr.Get() == actor) {
				return true;
			}
		}
		return false;
	}

};