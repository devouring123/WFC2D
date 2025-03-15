// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC2DModel.h"
#include "Math/IntVector.h"
#include "Math/RandomStream.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "WFC2DActor.generated.h"


// Cell Struct holds RemainingOptions, IsCollapsed, Location
USTRUCT(BlueprintType)
struct WFC2D_API FWFC2DCell
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	TArray<FWFC2DOption> RemainingOptions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	bool IsCollapsed = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	int32 Location;
	
	FWFC2DCell() = default;

	FWFC2DCell(const TArray<FWFC2DOption>& Options, const bool IsCollapsed, const int32 Location)
		: RemainingOptions(Options), IsCollapsed(IsCollapsed), Location(Location)
	{
		
	}
};

/**
 * A Actor of WFC2D
 * Actor contains WFC2DModel, RandomSeed, Instanced Static Mesh Component, Material
 */
UCLASS(Blueprintable, BlueprintType)
class WFC2D_API AWFC2DActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWFC2DActor();
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	UWFC2DModel* WFC2DModel;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	bool bInitializeInstancedMeshes = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	bool bCreateWFCModelFromInformation = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	bool bCreateInstancedStaticMeshFromConstraints = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	bool bCreateRandomSeed;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	int32 RandomSeed = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	float TileSize = 100;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	FIntVector Dimension = FIntVector(1,1,0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	int32 TryCount = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	TArray<FWFC2DCell> FinalGrid;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere)
	TArray<UInstancedStaticMeshComponent*> InstancedMeshes;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	UStaticMesh* StaticMeshReference;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	TArray<UMaterialInterface*> MaterialInstances;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC2D")
	int32 TypeOfTiles = 10;
	
	// Collapse All Cells
	UFUNCTION(BlueprintCallable, Category = "WFC2D")
	void Collapse();

private:
	// Direction Array for Grid
	static const TArray<int32> Dx;
	static const TArray<int32> Dy;
	
	FRandomStream RandomStream;


	// Initialize Grid
	void InitGrid(TArray<FWFC2DCell>& Grid) const;
	
	// Check Valid Options in Current Options
	void FilterValidOptions(TArray<FWFC2DOption>& CurrentOptions, TArray<FWFC2DOption>& ValidOptions) const;

	// Check Location is In Grid
	bool IsInGrid(int32 LocationX, int32 LocationY) const;

	// Initialize Static Mesh Reference and Materials To Instanced Meshes
	void InitInstancedMeshes();
	
	// Create Instanced Mesh
	void CreateInstancedMeshes();
};

