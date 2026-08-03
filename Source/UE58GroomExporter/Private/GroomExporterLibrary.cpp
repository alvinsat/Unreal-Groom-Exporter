#include "GroomExporterLibrary.h"

#include "HairStrandsDatas.h"

#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool UGroomExporterLibrary::ExportGroomToOBJ(
    UGroomAsset* Groom,
    const FString& Filename)
{
    if (!Groom)
    {
        UE_LOG(LogTemp, Error, TEXT("No Groom Asset."));
        return false;
    }

    FString OBJ;

    FString JSON;
    JSON += TEXT("{\n");
    JSON += TEXT("    \"curves\": [\n");

    bool bFirstCurve = true;

    int32 VertexIndex = 1;
    int32 GlobalCurveID = 0;

    const int32 NumGroups = Groom->GetNumHairGroups();

    for (int32 Group = 0; Group < NumGroups; ++Group)
    {
        FHairStrandsDatas Strands;
        FHairStrandsDatas Guides;

        if (!Groom->GetHairStrandsDatas(Group, Strands, Guides))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to read Groom Group %d"), Group);
            continue;
        }

        const auto& Curves = Strands.StrandsCurves;
        const auto& Points = Strands.StrandsPoints;

        const bool bHasRootUV =
            Curves.CurvesRootUV.Num() == Curves.CurvesCount.Num();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Group %d : Curves=%d  RootUV=%s"),
            Group,
            Curves.CurvesCount.Num(),
            bHasRootUV ? TEXT("YES") : TEXT("NO"));

        for (int32 Curve = 0; Curve < Curves.CurvesCount.Num(); ++Curve)
        {
            const uint32 Offset = Curves.CurvesOffset[Curve];
            const uint32 Count = Curves.CurvesCount[Curve];

            //
            // JSON
            //
            if (!bFirstCurve)
            {
                JSON += TEXT(",\n");
            }

            bFirstCurve = false;

            float U = 0.0f;
            float V = 0.0f;

            if (bHasRootUV)
            {
                U = Curves.CurvesRootUV[Curve].X;
                V = Curves.CurvesRootUV[Curve].Y;
            }

            JSON += FString::Printf(
                TEXT("        {\"first\":%u,\"count\":%u,\"group\":%d,\"uv\":[%.9f, %.9f]}"),
                Offset,
                Count,
                Group,
                U,
                V);

            //
            // OBJ vertices
            //
            for (uint32 Point = 0; Point < Count; ++Point)
            {
                const FVector3f& P = Points.PointsPosition[Offset + Point];

                OBJ += FString::Printf(
                    TEXT("v %.6f %.6f %.6f\n"),
                    P.X,
                    P.Y,
                    P.Z);
            }

            //
            // OBJ polyline
            //
            OBJ += TEXT("l");

            for (uint32 Point = 0; Point < Count; ++Point)
            {
                OBJ += FString::Printf(
                    TEXT(" %d"),
                    VertexIndex++);
            }

            OBJ += TEXT("\n");

            ++GlobalCurveID;
        }
    }

    JSON += TEXT("\n    ]\n");
    JSON += TEXT("}\n");

    const FString JsonFilename =
        FPaths::ChangeExtension(Filename, TEXT("json"));

    const bool bOBJSaved =
        FFileHelper::SaveStringToFile(OBJ, *Filename);

    const bool bJSONSaved =
        FFileHelper::SaveStringToFile(JSON, *JsonFilename);

    if (!bOBJSaved || !bJSONSaved)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save exported files."));
        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("OBJ exported to: %s"), *Filename);
    UE_LOG(LogTemp, Warning, TEXT("JSON exported to: %s"), *JsonFilename);

    return true;
}