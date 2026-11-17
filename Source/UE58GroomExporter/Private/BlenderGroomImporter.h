#pragma once

#include "CoreMinimal.h"

class UGroomAsset;

namespace BlenderGroomImporter
{
	UGroomAsset* ImportFromFile(
		const FString& Filename,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		FString& OutError);
}
