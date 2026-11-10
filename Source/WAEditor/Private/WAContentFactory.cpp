#include "WAEditor/WAContentFactory.h"

#include "AssetToolsModule.h"
#include "Factories/WorldFactory.h"
#include "Misc/PackageName.h"

bool UWAContentFactory::CreateMap(const FString& PackagePath, bool bWorldPartition)
{
	UWorldFactory* Factory = NewObject<UWorldFactory>(GetTransientPackage());
	Factory->bCreateWorldPartition = bWorldPartition;
	Factory->bEnableWorldPartitionStreaming = bWorldPartition;

	const FString PackageDirectory = FPackageName::GetLongPackagePath(PackagePath);
	const FString AssetName = FPackageName::GetShortName(PackagePath);
	UObject* World = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get().CreateAsset(
		AssetName,
		PackageDirectory,
		UWorld::StaticClass(),
		Factory);
	return World != nullptr;
}
