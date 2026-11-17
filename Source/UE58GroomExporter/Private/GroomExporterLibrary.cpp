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

    if (!Groom->CanRebuildFromDescription())
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Groom asset has no source Hair Description to export."));
        return false;
    }

    const FHairDescriptionGroups& DescriptionGroups =
        Groom->GetHairDescriptionGroups();
    const int32 NumGroups = DescriptionGroups.HairGroups.Num();
    if (NumGroups == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Groom asset has no hair groups to export."));
        return false;
    }

    FString OBJ;

    FString JSON;
    JSON += TEXT("{\n");
    JSON += TEXT("    \"curves\": [\n");

    bool bFirstCurve = true;

    int32 VertexIndex = 1;
    bool bExportColors = true;

    for (int32 Group = 0; Group < NumGroups; ++Group)
    {
        const FHairDescriptionGroup& HairGroup =
            DescriptionGroups.HairGroups[Group];
        if (HairGroup.Info.GroupIndex != Group)
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Groom source group index %d does not match array index %d."),
                HairGroup.Info.GroupIndex,
                Group);
            return false;
        }

        const FHairStrandsPoints& Points =
            HairGroup.Strands.StrandsPoints;
        const int32 PointCount = Points.PointsPosition.Num();
        if (Points.PointsRadius.Num() != PointCount)
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Groom source group %d has %d points but %d radii; refusing to export altered widths."),
                Group,
                PointCount,
                Points.PointsRadius.Num());
            return false;
        }
        bExportColors &= Points.PointsBaseColor.Num() == PointCount;
    }

    if (!bExportColors)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("One or more Groom groups have no complete color data; original colors will not be exported."));
    }

    for (int32 Group = 0; Group < NumGroups; ++Group)
    {
        const FHairDescriptionGroup& HairGroup =
            DescriptionGroups.HairGroups[Group];
        const FHairStrandsCurves& Curves =
            HairGroup.Strands.StrandsCurves;
        const FHairStrandsPoints& Points =
            HairGroup.Strands.StrandsPoints;

        const bool bHasRootUV =
            Curves.CurvesRootUV.Num() == Curves.CurvesCount.Num();
        const bool bHasColors =
            bExportColors
            && Points.PointsBaseColor.Num() == Points.PointsPosition.Num();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Group %d : Curves=%d  RootUV=%s  Radius=%s  Color=%s"),
            Group,
            Curves.CurvesCount.Num(),
            bHasRootUV ? TEXT("YES") : TEXT("NO"),
            TEXT("YES (source)"),
            bHasColors ? TEXT("YES") : TEXT("NO"));

        uint32 GroupPointOffset = 0;
        for (int32 Curve = 0; Curve < Curves.CurvesCount.Num(); ++Curve)
        {
            const uint32 Offset = GroupPointOffset;
            const uint32 Count = Curves.CurvesCount[Curve];
            if (static_cast<uint64>(Offset) + Count
                > static_cast<uint64>(Points.PointsPosition.Num()))
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT("Groom source group %d curve %d has an invalid point range (offset %u, count %u, points %d)."),
                    Group,
                    Curve,
                    Offset,
                    Count,
                    Points.PointsPosition.Num());
                return false;
            }

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
            if (!FMath::IsFinite(U) || !FMath::IsFinite(V))
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT("Groom group %d curve %d has an invalid root UV."),
                    Group,
                    Curve);
                return false;
            }

            JSON += FString::Printf(
                TEXT("        {\"first\":%u,\"count\":%u,\"group\":%d,\"uv\":[%.9g,%.9g]"),
                Offset,
                Count,
                Group,
                U,
                V);
            JSON += TEXT(",\"radius\":[");
            for (uint32 Point = 0; Point < Count; ++Point)
            {
                const uint32 PointArrayIndex = Offset + Point;
                const float Radius = Points.PointsRadius[PointArrayIndex];
                if (!FMath::IsFinite(Radius) || Radius < 0.0f)
                {
                    const FVector3f& Position =
                        Points.PointsPosition[PointArrayIndex];
                    UE_LOG(
                        LogTemp,
                        Error,
                        TEXT("Invalid source Groom radius: group=%d curve=%d point=%u groupPoint=%u curveOffset=%u curveCount=%u radius=%.9g width=%.9g position=(%.9g,%.9g,%.9g)."),
                        Group,
                        Curve,
                        Point,
                        PointArrayIndex,
                        Offset,
                        Count,
                        Radius,
                        Radius * 2.0f,
                        Position.X,
                        Position.Y,
                        Position.Z);
                    return false;
                }
                if (Point > 0)
                {
                    JSON += TEXT(",");
                }
                JSON += FString::Printf(TEXT("%.9g"), Radius);
            }
            JSON += TEXT("]");
            if (bHasColors)
            {
                JSON += TEXT(",\"color\":[");
                for (uint32 Point = 0; Point < Count; ++Point)
                {
                    const FLinearColor& Color =
                        Points.PointsBaseColor[Offset + Point];
                    if (!FMath::IsFinite(Color.R)
                        || !FMath::IsFinite(Color.G)
                        || !FMath::IsFinite(Color.B)
                        || Color.R < 0.0f
                        || Color.G < 0.0f
                        || Color.B < 0.0f)
                    {
                        UE_LOG(
                            LogTemp,
                            Error,
                            TEXT("Groom group %d curve %d has an invalid color at point %u."),
                            Group,
                            Curve,
                            Point);
                        return false;
                    }
                    if (Point > 0)
                    {
                        JSON += TEXT(",");
                    }
                    JSON += FString::Printf(
                        TEXT("[%.9g,%.9g,%.9g]"),
                        Color.R,
                        Color.G,
                        Color.B);
                }
                JSON += TEXT("]");
            }
            JSON += TEXT("}");

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

            GroupPointOffset += Count;
        }

        if (GroupPointOffset
            != static_cast<uint32>(Points.PointsPosition.Num()))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Groom source group %d curve ranges cover %u of %d points."),
                Group,
                GroupPointOffset,
                Points.PointsPosition.Num());
            return false;
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