#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GroomAsset.h"

#include "GroomExporterLibrary.generated.h"

UCLASS()
class UE58GROOMEXPORTER_API UGroomExporterLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Groom Export")
    static bool ExportGroomToOBJ(
        UGroomAsset* Groom,
        const FString& Filename);
};