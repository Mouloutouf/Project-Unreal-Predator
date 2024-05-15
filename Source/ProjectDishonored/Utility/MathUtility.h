#pragma once

#include "Kismet/KismetMathLibrary.h"

struct MathUtility
{
	// TODO Review this method
	static float AngleBetweenVectors(FVector _FirstVector, FVector _SecondVector)
	{
		_FirstVector = _FirstVector.GetSafeNormal();
		_SecondVector = _SecondVector.GetSafeNormal();

		FVector Cross = FVector::CrossProduct(_FirstVector, _SecondVector);
		float Dot = FVector::DotProduct(_FirstVector, _SecondVector);

		float Angle = UKismetMathLibrary::DegAcos(Dot);

		return Cross.Z > 0 ? Angle : -Angle;
	}

	static FVector GetPositionAtDistance(FVector _ClosestPosition, FVector _FurthestPosition, float _Distance)
	{
		FVector Direction = (_FurthestPosition - _ClosestPosition).GetSafeNormal();
		return _ClosestPosition + (Direction * _Distance);
	}
};