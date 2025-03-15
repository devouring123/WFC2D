// Fill out your copyright notice in the Description page of Project Settings.

#include "WFC2DActor.h"

const TArray<int32> AWFC2DActor::Dx = {1, 0, -1, 0};
const TArray<int32> AWFC2DActor::Dy = {0, 1, 0, -1};

AWFC2DActor::AWFC2DActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RandomStream.Initialize(0);
	
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene"));
	SetRootComponent(RootSceneComponent);
	
	InstancedMeshes.SetNum(TypeOfTiles);
	for(int32 i = TypeOfTiles - 1; i >= 0; i--)
	{
		FString InstancedMeshName = FString("Instanced Static Mesh");
		InstancedMeshName.Append(FString::FromInt(i));
		InstancedMeshes[i] = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName(*InstancedMeshName));
		InstancedMeshes[i]->SetupAttachment(RootSceneComponent);
		InstancedMeshes[i]->AttachToComponent(RootSceneComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		InstancedMeshes[i]->SetMobility(EComponentMobility::Static);
		InstancedMeshes[i]->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
		InstancedMeshes[i]->NumCustomDataFloats = 1;
		InstancedMeshes[i]->SetCustomPrimitiveDataFloat(0,1);

	}
}

void AWFC2DActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Initialize Instanced Meshes
	if(bInitializeInstancedMeshes)
	{
		InitInstancedMeshes();
		bInitializeInstancedMeshes = false;
	}
	
	// Create WFC Model
	if(bCreateWFCModelFromInformation)
	{
		WFC2DModel->GenerateConstraints();
		bCreateWFCModelFromInformation = false;
	}

	// Create Instanced Meshes
	if(bCreateInstancedStaticMeshFromConstraints)
	{
		Collapse();
		bCreateInstancedStaticMeshFromConstraints = false;
	}

	// Create Random Seed
	if(bCreateRandomSeed)
	{
		RandomSeed = RandomStream.RandRange(0,MAX_int32 - 1);
		RandomStream.Initialize(RandomSeed);
		UE_LOG(LogTemp, Display, TEXT("RandomSeed = %d"), RandomSeed);
		bCreateRandomSeed = false;
	}
}

void AWFC2DActor::Collapse()
{
	UE_LOG(LogTemp, Display, TEXT("--------STRAT--------"));

	// If RandomSeed is Zero, Apply Random Seed Temporary
	if(RandomSeed == 0)
	{
		const int32 TempRandomSeed = RandomStream.RandRange(0,MAX_int32 - 1);
		RandomStream.Initialize(TempRandomSeed);
		UE_LOG(LogTemp, Display, TEXT("RandomSeed = %d"), TempRandomSeed);
	}
	else
	{
		RandomStream.Initialize(RandomSeed);
		UE_LOG(LogTemp, Display, TEXT("RandomSeed = %d"), RandomSeed);
	}
	
	bool CreateSucceed = false;
	int32 CurrentTryCount = TryCount;
	TArray<FWFC2DCell> Grid, SelectedCells;

	// Try Collapse
	while(!CreateSucceed && CurrentTryCount > 0)
	{
		UE_LOG(LogTemp, Display, TEXT("TryCount: %d"), CurrentTryCount);
		CurrentTryCount--;
		int32 WFC2DIterationCount = 0;
		
		// Initialize Grid
		Grid.Empty();
		Grid.SetNum(Dimension.X * Dimension.Y);
		InitGrid(Grid);
		FinalGrid.Empty();

		
		// Wave Function Collapse Single Iteration
		while (WFC2DIterationCount != Dimension.X * Dimension.Y)
		{
			WFC2DIterationCount++;

			// Filter GridCopy Not Collapsed
			TArray<FWFC2DCell> GridCopy = Grid.FilterByPredicate([](const FWFC2DCell& Cell)
			{
				return !Cell.IsCollapsed;
			});

			// If GridCopy Has No Tile, Try New Collapse
			if(GridCopy.Num() == 0)
				break;

			// Sort GridCopy by RemainingOptionsLength
			GridCopy.Sort([](const FWFC2DCell& A, const FWFC2DCell& B)
			{
				return A.RemainingOptions.Num() < B.RemainingOptions.Num();
			});

			// Create SubArray From Min Length Of Options
			const int32 MinLengthOfOptions = GridCopy[0].RemainingOptions.Num();
			int32 GridCopyCutIndex = 0;
			for(int32 i = 1; i < GridCopy.Num(); i++)
			{
				if(GridCopy[i].RemainingOptions.Num() > MinLengthOfOptions)
				{
					GridCopyCutIndex = i;
					break;
				}
			}
			if(GridCopyCutIndex > 0) GridCopy.SetNum(GridCopyCutIndex);
			
			// Choose Random Cell From SubArray and Collapse Cell
			const int32 SelectedIndex = RandomStream.RandRange(0, GridCopy.Num() - 1);
			FWFC2DCell& SelectedCell = GridCopy[SelectedIndex];
			if(SelectedCell.RemainingOptions.Num() > 0)
			{
				Grid[SelectedCell.Location].IsCollapsed = true;
				Grid[SelectedCell.Location].RemainingOptions.Empty();
				Grid[SelectedCell.Location].RemainingOptions.Add(SelectedCell.RemainingOptions[RandomStream.RandRange(0, SelectedCell.RemainingOptions.Num() - 1)]);
				FinalGrid.Add(Grid[SelectedCell.Location]);
			}
			else
				break;
			
			// Filter Collapsed Cell
			TArray<FWFC2DCell> GridNext = Grid;
			TArray<FWFC2DCell> CollapsedGrid = Grid.FilterByPredicate([](const FWFC2DCell& Cell)
			{
				return Cell.IsCollapsed;
			});
			
			// Create Indices Of Cells To Visit 
			TArray<int32> IndicesOfCellsToVisit;
			for (const auto Cell : CollapsedGrid)
			{
				for(int32 dir = 0; dir < 4; dir++)
				{
					const int32 LocationX = Cell.Location / Dimension.Y + Dx[dir];
					const int32 LocationY = Cell.Location % Dimension.Y + Dy[dir];
					const int32 IndexOfDirCell = LocationX * Dimension.Y + LocationY;
					if(IsInGrid(LocationX,LocationY) && !GridNext[IndexOfDirCell].IsCollapsed && !IndicesOfCellsToVisit.Contains(IndexOfDirCell))
					{
						IndicesOfCellsToVisit.Add(IndexOfDirCell);
					}
				}
			}
			
			// Check Valid Options to All Remaining Cells 
			for (const auto IndexOfCellToVisit : IndicesOfCellsToVisit)
			{
				FWFC2DCell& CellToVisit = GridNext[IndexOfCellToVisit];
				
				TArray<FWFC2DOption> FilteredCellOptions;
				WFC2DModel->Constraints.GenerateKeyArray(FilteredCellOptions);

				// Filter Cell Options which is Not Valid
				for(int32 dir = 0; dir < 4; dir++)
				{
					const int32 CellToVisitLocationX = CellToVisit.Location / Dimension.Y + Dx[dir];
					const int32 CellToVisitLocationY = CellToVisit.Location % Dimension.Y + Dy[dir];
					const int32 CellToVisitIndex = CellToVisitLocationX * Dimension.Y + CellToVisitLocationY;
					if(IsInGrid(CellToVisitLocationX, CellToVisitLocationY) && GridNext[CellToVisitIndex].IsCollapsed)
					{
						TArray<FWFC2DOption> CollapsedCellOptions = GridNext[CellToVisitIndex].RemainingOptions;
						TArray<FWFC2DOption> AllValidOptions;
						for (auto Option : CollapsedCellOptions)
						{
							FWFC2DOptions ValidOptions = WFC2DModel->Constraints.Find(Option)->Array[(dir + 2) % 4];
							for (auto ValidOption : ValidOptions.Options)
							{
								AllValidOptions.AddUnique(ValidOption);
							}
						}
						FilterValidOptions(FilteredCellOptions, AllValidOptions);
					}
				}
				CellToVisit.RemainingOptions = FilteredCellOptions;
			}
			// Update Grid
			Grid = GridNext;
			
			// Judge Success
			if(WFC2DIterationCount == Dimension.X * Dimension.Y)
				CreateSucceed = true;
			
		}
	}
	// If All Cell are Collapsed Create Instanced Static Mesh
	if(CreateSucceed)
	{
		UE_LOG(LogTemp, Display, TEXT("CREATE SUCCEED"))
		FinalGrid.Sort([](const FWFC2DCell& A, const FWFC2DCell& B)
		{
			return A.Location < B.Location;
		});
		if(InstancedMeshes[0]->GetStaticMesh() == nullptr)
			InitInstancedMeshes();
		CreateInstancedMeshes();
	}
	// Else Log Failed
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Create Failed"));
	}
}

void AWFC2DActor::InitGrid(TArray<FWFC2DCell>& Grid) const
{
	// Initialize Grid Options to All Tile Option
	for (int32 i = 0; i < Grid.Num(); i++)
	{
		WFC2DModel->Constraints.GenerateKeyArray(Grid[i].RemainingOptions);
		Grid[i].Location = i;
	}
}

void AWFC2DActor::FilterValidOptions(TArray<FWFC2DOption>& CurrentOptions, TArray<FWFC2DOption>& ValidOptions) const
{
	// Delete CurrentOptions Option which Not Included ValidOptions
	for(int32 i = CurrentOptions.Num() - 1; i >= 0; i--)
	{
		FWFC2DOption CheckOption = CurrentOptions[i];
		if(!ValidOptions.Contains(CheckOption))
		{
			CurrentOptions.Remove(CheckOption);
		}
	}
}

bool AWFC2DActor::IsInGrid(int32 LocationX, int32 LocationY) const
{
	return (LocationX >= 0 && LocationX < Dimension.X && LocationY >= 0 && LocationY < Dimension.Y);
}

void AWFC2DActor::InitInstancedMeshes()
{
	UE_LOG(LogTemp, Display, TEXT("Initialize Instanced Meshes"));
	for(int32 i = 0; i < MaterialInstances.Num(); i++)
	{
		InstancedMeshes[i]->SetStaticMesh(StaticMeshReference);
		InstancedMeshes[i]->SetMaterial(0,MaterialInstances[i]);
	}
}

void AWFC2DActor::CreateInstancedMeshes()
{
	// Delete Instances
	for (const auto InstancedMesh : InstancedMeshes)
	{
		InstancedMesh->ClearInstances();
	}
	
	// Create Instances
	for(int32 i = 0; i < FinalGrid.Num(); i++)
	{
		const int32 TextureNumber = FinalGrid[i].RemainingOptions[0].TileTextureIndex;
		InstancedMeshes[TextureNumber]->AddInstance(FTransform(
			FQuat::Identity,
			FVector(TileSize *  (i / Dimension.Y + 0.5), TileSize * (i % Dimension.Y + 0.5), 0),
			FVector(TileSize / 100,TileSize / 100,TileSize / 100)
			));
		InstancedMeshes[TextureNumber]->SetCustomDataValue(
			InstancedMeshes[TextureNumber]->GetInstanceCount() - 1, 0, FinalGrid[i].RemainingOptions[0].RotationMultiplierBy90Degree);
	}
}