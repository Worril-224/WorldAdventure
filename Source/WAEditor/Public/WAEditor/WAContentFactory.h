#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WAContentFactory.generated.h"

UCLASS()
class WAEDITOR_API UWAContentFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WorldAdventure")
	static bool CreateMap(const FString& PackagePath, bool bWorldPartition);
};
