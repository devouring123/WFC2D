// Fill out your copyright notice in the Description page of Project Settings.


#include "WFC2DModel.h"

FWFC2DTile UWFC2DModel::RotateTileClockwise(FWFC2DTile& Tile, int32 RotationMultiplierBy90Degree)
{
	TArray<FString> RotatedEdge;
	for(int32 i = 0; i < 4; i++)
	{
		RotatedEdge.Add(Tile.Edges[(i - RotationMultiplierBy90Degree + 4) % 4]);
	}
	Tile.Option.RotationMultiplierBy90Degree = RotationMultiplierBy90Degree;
	Tile.Edges = RotatedEdge;
	return Tile;
}

TArray<FWFC2DTile> UWFC2DModel::CreateTileSet()
{
	TArray<FWFC2DTile> TileSet;
	for (const auto &Tile : TileInformation)
	{
		TileSet.Add(Tile);
		for(int32 i = 1; i < 4; i++)
		{
			FWFC2DTile CopyTile(Tile);
			TileSet.AddUnique(RotateTileClockwise(CopyTile,i));
		}
	}
	return TileSet;
}

void UWFC2DModel::CalculateTileConstraints(FWFC2DTile& BaseTile, TArray<FWFC2DTile> TileSet)
{
	BaseTile.Constraint.Array.SetNum(4);
	for(int32 i = 0; i < TileSet.Num(); i++)
	{
		FWFC2DTile& Tile = TileSet[i];
		for (int32 j = 0; j < 4; j++)
		{
			if(BaseTile.Edges[j].Equals(Tile.Edges[(j + 2) % 4].Reverse()))
			{
				BaseTile.Constraint.Array[j].Options.Add(Tile.Option);
			}
		}
	}
}

void UWFC2DModel::GenerateConstraints()
{
	TArray<FWFC2DTile> TileSet = CreateTileSet();
	Constraints.Empty();
	for (auto &Tile : TileSet)
	{
		CalculateTileConstraints(Tile, TileSet);
		Constraints.Emplace(Tile.Option, Tile.Constraint);
	}
	UE_LOG(LogTemp, Display, TEXT("Generate Constraints"));
}


