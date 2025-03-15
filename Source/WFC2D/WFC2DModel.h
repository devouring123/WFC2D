// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WFC2DModel.generated.h"


// Option Struct contains the Texture2d and Its Index, Rotation Multiplier
USTRUCT(BlueprintType)
struct WFC2D_API FWFC2DOption
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	UTexture2D* TileTexture2D;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	int32 TileTextureIndex;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	uint8 RotationMultiplierBy90Degree = 0;

	FWFC2DOption() = default;

	FWFC2DOption(UTexture2D* TileTexture2D, int32 TileTextureIndex, uint8 RotationMultiply90Degree)
		: TileTexture2D(TileTexture2D), TileTextureIndex(TileTextureIndex),RotationMultiplierBy90Degree(RotationMultiply90Degree)
	{
		
	}

	bool operator==(const FWFC2DOption& Rhs) const
	{
		return TileTexture2D == Rhs.TileTexture2D && TileTextureIndex == Rhs.TileTextureIndex && RotationMultiplierBy90Degree == Rhs.RotationMultiplierBy90Degree;
	}

	bool operator!=(const FWFC2DOption& Rhs) const
	{
		return TileTexture2D != Rhs.TileTexture2D || TileTextureIndex != Rhs.TileTextureIndex != RotationMultiplierBy90Degree != Rhs.RotationMultiplierBy90Degree;
	}

	friend uint32 GetTypeHash(const FWFC2DOption& Output)
	{
		uint32 OutputHash = HashCombine(GetTypeHash(Output.TileTexture2D), GetTypeHash(Output.RotationMultiplierBy90Degree));
		OutputHash = HashCombine(OutputHash, GetTypeHash(Output.TileTextureIndex));
		return OutputHash;
	}
	
};

// Struct for Array of Options
USTRUCT(BlueprintType)
struct WFC2D_API FWFC2DOptions
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	TArray<FWFC2DOption> Options;
	
	friend uint32 GetTypeHash(const FWFC2DOptions& Output)
	{
		uint32 OutputHash = 0;
		for (auto Option : Output.Options)
		{
			OutputHash = HashCombine(OutputHash, GetTypeHash(Option));
		}
		return OutputHash;
	}
	
};

// Struct for Constraint
USTRUCT(BlueprintType)
struct WFC2D_API FWFC2DConstraint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	TArray<FWFC2DOptions> Array;

	friend uint32 GetTypeHash(const FWFC2DConstraint& Output)
	{
		uint32 OutputHash = 0;
		for (auto Options : Output.Array)
		{
			OutputHash = HashCombine(OutputHash, GetTypeHash(Options));
		}
		return OutputHash;
	}
};

// Tile Class holds Option, Edges, Constraint
USTRUCT(BlueprintType)
struct WFC2D_API FWFC2DTile
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	FWFC2DOption Option;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D", DisplayName= "Edges (Up, Right, Down, Left)")
	TArray<FString> Edges;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	FWFC2DConstraint Constraint;
	
	FWFC2DTile() = default;
	
	FWFC2DTile(FWFC2DOption* Option, const TArray<FString>& Edges)
		: Option(*Option), Edges(Edges)
	{
		
	}
	
	bool operator==(const FWFC2DTile& Rhs) const
	{
		return Option.TileTexture2D == Rhs.Option.TileTexture2D && Edges == Rhs.Edges;
	}

	bool operator!=(const FWFC2DTile& Rhs) const
	{
		return Option.TileTexture2D != Rhs.Option.TileTexture2D || Edges != Rhs.Edges;
	}
	
	friend uint32 GetTypeHash(const FWFC2DTile& Output)
	{
		uint32 OutputHash = GetTypeHash(Output.Option);
		for (auto EdgeString : Output.Edges)
		{
			OutputHash = HashCombine(OutputHash, GetTypeHash(EdgeString));
		}
		OutputHash = HashCombine(OutputHash,GetTypeHash(Output.Constraint));
		return OutputHash;	
	}
	
};

/**
* A Model of WFC2D Constraints.
* Data asset contain Tiles, Constraints
*/

UCLASS(Blueprintable, BlueprintType)
class WFC2D_API UWFC2DModel : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	TArray<FWFC2DTile> TileInformation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	TMap<FWFC2DOption, FWFC2DConstraint> Constraints;

	// Rotate Tile 90 Degree by Multiplier
	UFUNCTION(BlueprintCallable, Category = "WFC2D")
	static FWFC2DTile RotateTileClockwise(FWFC2DTile& Tile, int32 RotationMultiplierBy90Degree);

	// Create Tile Set
	UFUNCTION(BlueprintCallable, Category = "WFC2D")
	TArray<FWFC2DTile> CreateTileSet();
	
	// Calculate Tile Constraint
	UFUNCTION(BlueprintCallable, Category = "WFC2D")
	static void CalculateTileConstraints(FWFC2DTile& BaseTile, TArray<FWFC2DTile> TileSet);

	// Generate Constraints
	UFUNCTION(BlueprintCallable, Category = "WFC2D")
	void GenerateConstraints();
};
