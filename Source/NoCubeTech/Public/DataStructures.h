// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataStructures.generated.h"


USTRUCT()
struct NOCUBETECH_API FIntegerPair {
	GENERATED_BODY()
	UPROPERTY()
	int32 x;
	UPROPERTY()
	int32 y;
public:
	FIntegerPair() : x(0), y(0) {

	}

	FIntegerPair(int32 x, int32 y) : x(x), y(y) {

	}

	friend uint32 GetTypeHash(const FIntegerPair& struc) {
		return ((uint32)(struc.x)) ^ ((uint32)(struc.y));
	}

	bool operator==(const FIntegerPair& Other) const
	{
		return x == Other.x && y == Other.y;
	}

};

USTRUCT()
struct NOCUBETECH_API FActorGrid
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FIntegerPair, TWeakObjectPtr<AActor>> data;

public:

	FActorGrid() {
		data = TMap<FIntegerPair, TWeakObjectPtr<AActor>>();
	};

	void Put(int32 x, int32 y, AActor* actor) {
		if (actor != nullptr) {
			data.Add(FIntegerPair(x, y), TWeakObjectPtr<AActor>(actor));
		}
	}

	TWeakObjectPtr<AActor> Get(int32 x, int32 y) {
		TWeakObjectPtr<AActor>* result = data.Find(FIntegerPair(x, y));
		if (result == nullptr) {
			return nullptr;
		}
		else {
			return *result;
		}
	}

	bool Contains(int32 x, int32 y) {
		FIntegerPair key = FIntegerPair(x, y);
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
		data.Remove(FIntegerPair(x, y));
	}
};


USTRUCT()
struct NOCUBETECH_API FBoolGrid
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FIntegerPair, bool> data;

public:

	FBoolGrid() {

		data = TMap<FIntegerPair, bool>();
	};

	void Put(int32 x, int32 y, bool value) {
		data.Add(FIntegerPair(x, y), value);
	}

	bool Get(int32 x, int32 y) {
		return data[FIntegerPair(x, y)];
	}

	bool Contains(int32 x, int32 y) {
		FIntegerPair key = FIntegerPair(x, y);
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
	TMap<FIntegerPair, FActorList> data;

public:
	FActorListGrid() {
		data = TMap<FIntegerPair, FActorList>();
	}

	void Add(int32 x, int32 y, AActor* actor) {
		FIntegerPair key = FIntegerPair(x, y);
		if (!data.Contains(key)) {
			data.Emplace(key);
		}
		data[key].lst.Add(TWeakObjectPtr<AActor>(actor));
	}

	bool Contains(int32 x, int32 y, AActor* actor) {
		FIntegerPair key = FIntegerPair(x, y);
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

USTRUCT()
struct NOCUBETECH_API FObjectGrid
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FIntegerPair, UObject*> data;

public:

	FObjectGrid() {

		data = TMap<FIntegerPair, UObject*>();
	};

	void Put(int32 x, int32 y, UObject* obj) {
		if (obj == nullptr) {
			data.Remove(FIntegerPair(x, y));
		}
		else {
			data.Add(FIntegerPair(x, y), obj);
		}
	}

	UObject* Get(int32 x, int32 y) {
		UObject** result = data.Find(FIntegerPair(x, y));
		if (result == nullptr) {
			return nullptr;
		}
		else {
			return *result;
		}
	}

	bool ContainsNonNull(int32 x, int32 y) {
		FIntegerPair key = FIntegerPair(x, y);
		UObject** result = data.Find(key);
		if (result == nullptr) {
			return false;
		}
		if (*result == nullptr) {
			return false;
		}
		return true;
	};

	void CheckAndRemove(int32 x, int32 y) {
		data.Remove(FIntegerPair(x, y));
	}

	void CheckAndRemove(const FIntegerPair& xyPair) {
		data.Remove(xyPair);
	}

	auto CreateConstIterator() {
		return data.CreateConstIterator();
	}

};

USTRUCT()
struct NOCUBETECH_API FInt64BoolMap {
	GENERATED_BODY();
private:
	UPROPERTY()
	TMap<int64, bool> data;
public:

	FInt64BoolMap() {
		data = TMap<int64, bool>();
	};

	void Put(int64 key, bool value) {
		bool* ptr = data.Find(key);
		bool nul = ptr == nullptr;
		if (value) {
			if (nul) {
				data.Add(key, true);
			}
			else {
				*ptr = value;
			}
		}
		else {
			if (nul) {
				// nothing
			}
			else {
				data.Remove(key);
			}
		}
	}

	bool Get(int64 key) {
		bool* result = data.Find(key);
		if (result == nullptr) {
			return false;
		}
		else {
			return *result;
		}
	}

};