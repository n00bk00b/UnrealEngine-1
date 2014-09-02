// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayAbilityTargetActor_Trace.h"
#include "GameplayAbilityWorldReticle.h"
#include "GameplayAbilityTargetActor_GroundTrace.generated.h"

UCLASS(Blueprintable)
class GAMEPLAYABILITIES_API AGameplayAbilityTargetActor_GroundTrace : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_UCLASS_BODY()

protected:
	virtual FHitResult PerformTrace(AActor* InSourceActor) const override;
};