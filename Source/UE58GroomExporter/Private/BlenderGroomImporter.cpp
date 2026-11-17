#include "BlenderGroomImporter.h"

#include "EditorFramework/AssetImportData.h"
#include "GroomAsset.h"
#include "GroomAssetImportData.h"
#include "GroomBuilder.h"
#include "GroomImportOptions.h"
#include "HairDescription.h"
#include "HairStrandsEditor.h"
#include "HairStrandsImporter.h"
#include "HairStrandsTranslator.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	constexpr TCHAR BlenderGroomManifestFormat[] = TEXT("UE Groom Alembic");
	constexpr int32 BlenderGroomManifestVersion = 1;

	struct FBlenderGroomCurve
	{
		int32 First = 0;
		int32 Count = 0;
		int32 GroupID = 0;
		FVector2f RootUV = FVector2f::ZeroVector;
		TArray<float> Radii;
		TArray<FVector3f> Colors;
		bool bHasRadii = false;
		bool bHasColors = false;
	};

	bool TryConvertJsonInteger(double Number, int32& OutValue)
	{
		if (!FMath::IsFinite(Number) || Number < 0.0 || Number > MAX_int32)
		{
			return false;
		}

		const int32 IntegerValue = static_cast<int32>(Number);
		if (static_cast<double>(IntegerValue) != Number)
		{
			return false;
		}

		OutValue = IntegerValue;
		return true;
	}

	bool TryGetIntegerField(
		const FJsonObject& Object,
		const TCHAR* FieldName,
		int32& OutValue)
	{
		double Number = 0.0;
		return Object.TryGetNumberField(FieldName, Number)
			&& TryConvertJsonInteger(Number, OutValue);
	}

	bool TryGetFiniteFloat(
		const TSharedPtr<FJsonValue>& Value,
		float& OutValue,
		const bool bRequireNonNegative)
	{
		double Number = 0.0;
		if (!Value.IsValid()
			|| !Value->TryGetNumber(Number)
			|| !FMath::IsFinite(Number)
			|| Number < -MAX_flt
			|| Number > MAX_flt
			|| (bRequireNonNegative && Number < 0.0))
		{
			return false;
		}

		OutValue = static_cast<float>(Number);
		return FMath::IsFinite(OutValue);
	}

	bool ReadBlenderGroomManifest(
		const FString& AlembicFilename,
		TArray<FBlenderGroomCurve>& OutCurves,
		TArray<int32>& OutGroupIDs,
		FString& OutError)
	{
		const FString ManifestFilename =
			FPaths::ChangeExtension(AlembicFilename, TEXT("json"));
		FString ManifestText;
		if (!FFileHelper::LoadFileToString(ManifestText, *ManifestFilename))
		{
			OutError = FString::Printf(
				TEXT("Could not read Blender Groom manifest: %s"),
				*ManifestFilename);
			return false;
		}

		TSharedPtr<FJsonObject> Manifest;
		const TSharedRef<TJsonReader<>> Reader =
			TJsonReaderFactory<>::Create(ManifestText);
		if (!FJsonSerializer::Deserialize(Reader, Manifest) || !Manifest.IsValid())
		{
			OutError = TEXT("The Blender Groom JSON manifest is invalid.");
			return false;
		}

		FString Format;
		int32 Version = 0;
		if (!Manifest->TryGetStringField(TEXT("format"), Format)
			|| Format != BlenderGroomManifestFormat
			|| !TryGetIntegerField(*Manifest, TEXT("version"), Version)
			|| Version != BlenderGroomManifestVersion)
		{
			OutError = TEXT("The Blender Groom JSON manifest format/version is unsupported.");
			return false;
		}

		const TArray<TSharedPtr<FJsonValue>>* GroupIDValues = nullptr;
		if (!Manifest->TryGetArrayField(TEXT("group_ids"), GroupIDValues)
			|| GroupIDValues == nullptr
			|| GroupIDValues->IsEmpty())
		{
			OutError = TEXT("The Blender Groom manifest has no group IDs.");
			return false;
		}

		TSet<int32> DeclaredGroupIDs;
		for (const TSharedPtr<FJsonValue>& Value : *GroupIDValues)
		{
			double Number = 0.0;
			int32 GroupID = 0;
			if (!Value.IsValid()
				|| !Value->TryGetNumber(Number)
				|| !TryConvertJsonInteger(Number, GroupID)
				|| DeclaredGroupIDs.Contains(GroupID))
			{
				OutError = TEXT("The Blender Groom manifest contains an invalid or duplicate group ID.");
				return false;
			}
			DeclaredGroupIDs.Add(GroupID);
			OutGroupIDs.Add(GroupID);
		}

		TArray<int32> SortedGroupIDs = OutGroupIDs;
		SortedGroupIDs.Sort();
		for (int32 Index = 0; Index < SortedGroupIDs.Num(); ++Index)
		{
			if (SortedGroupIDs[Index] != Index)
			{
				OutError = TEXT("Blender Groom group IDs must be zero-based and contiguous.");
				return false;
			}
		}

		const TArray<TSharedPtr<FJsonValue>>* CurveValues = nullptr;
		if (!Manifest->TryGetArrayField(TEXT("curves"), CurveValues)
			|| CurveValues == nullptr
			|| CurveValues->IsEmpty())
		{
			OutError = TEXT("The Blender Groom manifest has no curves.");
			return false;
		}

		TSet<int32> UsedGroupIDs;
		OutCurves.Reserve(CurveValues->Num());
		for (int32 CurveIndex = 0; CurveIndex < CurveValues->Num(); ++CurveIndex)
		{
			const TSharedPtr<FJsonValue>& Value = (*CurveValues)[CurveIndex];
			const TSharedPtr<FJsonObject>* CurveObject = nullptr;
			if (!Value.IsValid()
				|| !Value->TryGetObject(CurveObject)
				|| CurveObject == nullptr
				|| !CurveObject->IsValid())
			{
				OutError = FString::Printf(
					TEXT("Blender Groom manifest curve %d is not an object."),
					CurveIndex);
				return false;
			}

			FBlenderGroomCurve Curve;
			if (!TryGetIntegerField(**CurveObject, TEXT("first"), Curve.First)
				|| !TryGetIntegerField(**CurveObject, TEXT("count"), Curve.Count)
				|| !TryGetIntegerField(**CurveObject, TEXT("group"), Curve.GroupID)
				|| Curve.Count < 2
				|| !DeclaredGroupIDs.Contains(Curve.GroupID))
			{
				OutError = FString::Printf(
					TEXT("Blender Groom manifest curve %d has invalid point or group data."),
					CurveIndex);
				return false;
			}

			const TArray<TSharedPtr<FJsonValue>>* UVValues = nullptr;
			if (!(*CurveObject)->TryGetArrayField(TEXT("uv"), UVValues)
				|| UVValues == nullptr
				|| UVValues->Num() != 2
				|| !TryGetFiniteFloat((*UVValues)[0], Curve.RootUV.X, false)
				|| !TryGetFiniteFloat((*UVValues)[1], Curve.RootUV.Y, false))
			{
				OutError = FString::Printf(
					TEXT("Blender Groom manifest curve %d has invalid root UV data."),
					CurveIndex);
				return false;
			}

			const TArray<TSharedPtr<FJsonValue>>* RadiusValues = nullptr;
			if ((*CurveObject)->TryGetArrayField(
				TEXT("radius"),
				RadiusValues))
			{
				if (RadiusValues == nullptr
					|| RadiusValues->Num() != Curve.Count)
				{
					OutError = FString::Printf(
						TEXT("Blender Groom manifest curve %d has an invalid radius count."),
						CurveIndex);
					return false;
				}

				Curve.bHasRadii = true;
				Curve.Radii.Reserve(Curve.Count);
				for (const TSharedPtr<FJsonValue>& RadiusValue : *RadiusValues)
				{
					float Radius = 0.0f;
					if (!TryGetFiniteFloat(RadiusValue, Radius, true))
					{
						OutError = FString::Printf(
							TEXT("Blender Groom manifest curve %d has an invalid radius."),
							CurveIndex);
						return false;
					}
					Curve.Radii.Add(Radius);
				}
			}
			else if ((*CurveObject)->HasField(TEXT("radius")))
			{
				OutError = FString::Printf(
					TEXT("Blender Groom manifest curve %d has an invalid radius array."),
					CurveIndex);
				return false;
			}

			const TArray<TSharedPtr<FJsonValue>>* ColorValues = nullptr;
			if ((*CurveObject)->TryGetArrayField(TEXT("color"), ColorValues))
			{
				if (ColorValues == nullptr
					|| ColorValues->Num() != Curve.Count)
				{
					OutError = FString::Printf(
						TEXT("Blender Groom manifest curve %d has an invalid color count."),
						CurveIndex);
					return false;
				}

				Curve.bHasColors = true;
				Curve.Colors.Reserve(Curve.Count);
				for (const TSharedPtr<FJsonValue>& ColorValue : *ColorValues)
				{
					const TArray<TSharedPtr<FJsonValue>>* RGBValues = nullptr;
					FVector3f Color = FVector3f::ZeroVector;
					if (!ColorValue.IsValid()
						|| !ColorValue->TryGetArray(RGBValues)
						|| RGBValues == nullptr
						|| RGBValues->Num() != 3
						|| !TryGetFiniteFloat((*RGBValues)[0], Color.X, true)
						|| !TryGetFiniteFloat((*RGBValues)[1], Color.Y, true)
						|| !TryGetFiniteFloat((*RGBValues)[2], Color.Z, true))
					{
						OutError = FString::Printf(
							TEXT("Blender Groom manifest curve %d has an invalid color."),
							CurveIndex);
						return false;
					}
					Curve.Colors.Add(Color);
				}
			}
			else if ((*CurveObject)->HasField(TEXT("color")))
			{
				OutError = FString::Printf(
					TEXT("Blender Groom manifest curve %d has an invalid color array."),
					CurveIndex);
				return false;
			}

			UsedGroupIDs.Add(Curve.GroupID);
			OutCurves.Add(Curve);
		}

		if (UsedGroupIDs.Num() != DeclaredGroupIDs.Num())
		{
			OutError = TEXT("The Blender Groom manifest group list does not match its curves.");
			return false;
		}

		return true;
	}

	TSharedPtr<IGroomTranslator> FindAlembicHairTranslator(
		const FString& Filename)
	{
		const FString Extension = FPaths::GetExtension(Filename);
		for (const TSharedPtr<IGroomTranslator>& Translator :
			FGroomEditor::Get().GetHairTranslators())
		{
			if (Translator.IsValid()
				&& Translator->IsFileExtensionSupported(Extension)
				&& Translator->CanTranslate(Filename))
			{
				return Translator;
			}
		}
		return nullptr;
	}
}

UGroomAsset* BlenderGroomImporter::ImportFromFile(
	const FString& Filename,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	FString& OutError)
{
	OutError.Reset();

	if (!FPaths::GetExtension(Filename).Equals(
		TEXT("abc"),
		ESearchCase::IgnoreCase))
	{
		OutError = TEXT("Select a Blender Groom Alembic (.abc) file.");
		return nullptr;
	}

	TArray<FBlenderGroomCurve> ManifestCurves;
	TArray<int32> ManifestGroupIDs;
	if (!ReadBlenderGroomManifest(
		Filename,
		ManifestCurves,
		ManifestGroupIDs,
		OutError))
	{
		return nullptr;
	}

	const TSharedPtr<IGroomTranslator> Translator =
		FindAlembicHairTranslator(Filename);
	if (!Translator.IsValid())
	{
		OutError = FString::Printf(
			TEXT("No Unreal Groom Alembic translator can read '%s'. Enable the Alembic Groom Importer plugin."),
			*Filename);
		return nullptr;
	}

	UGroomImportOptions* ImportOptions = NewObject<UGroomImportOptions>();
	ImportOptions->ConversionSettings.Scale = FVector::OneVector;
	ImportOptions->ConversionSettings.Rotation = FVector::ZeroVector;
	FHairDescription HairDescription;
	if (!Translator->Translate(
		Filename,
		HairDescription,
		ImportOptions->ConversionSettings))
	{
		OutError = FString::Printf(
			TEXT("The Unreal Groom Alembic translator failed to read '%s'."),
			*Filename);
		return nullptr;
	}

	if (HairDescription.GetNumStrands() != ManifestCurves.Num())
	{
		OutError = FString::Printf(
			TEXT("Blender manifest curve count (%d) does not match Alembic curve count (%d)."),
			ManifestCurves.Num(),
			HairDescription.GetNumStrands());
		return nullptr;
	}

	const TStrandAttributesConstRef<int> PointCounts =
		HairDescription.StrandAttributes().GetAttributesRef<int>(
			HairAttribute::Strand::VertexCount);
	if (!PointCounts.IsValid())
	{
		OutError = TEXT("The Alembic translator did not provide strand point counts.");
		return nullptr;
	}

	int64 ExpectedFirstPoint = 0;
	for (int32 CurveIndex = 0; CurveIndex < ManifestCurves.Num(); ++CurveIndex)
	{
		const FBlenderGroomCurve& Curve = ManifestCurves[CurveIndex];
		if (Curve.First != ExpectedFirstPoint
			|| Curve.Count != PointCounts[FStrandID(CurveIndex)])
		{
			OutError = FString::Printf(
				TEXT("Blender manifest point range for curve %d does not match the Alembic data."),
				CurveIndex);
			return nullptr;
		}

		SetHairStrandAttribute(
			HairDescription,
			FStrandID(CurveIndex),
			HairAttribute::Strand::GroupID,
			Curve.GroupID);
		SetHairStrandAttribute(
			HairDescription,
			FStrandID(CurveIndex),
			HairAttribute::Strand::RootUV,
			Curve.RootUV);
		for (int32 PointIndex = 0; PointIndex < Curve.Count; ++PointIndex)
		{
			const FVertexID VertexID = FVertexID(
				static_cast<int32>(ExpectedFirstPoint + PointIndex));
			if (Curve.bHasRadii)
			{
				SetHairVertexAttribute(
					HairDescription,
					VertexID,
					HairAttribute::Vertex::Width,
					Curve.Radii[PointIndex] * 2.0f);
			}
			if (Curve.bHasColors)
			{
				SetHairVertexAttribute(
					HairDescription,
					VertexID,
					HairAttribute::Vertex::Color,
					Curve.Colors[PointIndex]);
			}
		}
		ExpectedFirstPoint += Curve.Count;
	}

	if (ExpectedFirstPoint != HairDescription.GetNumVertices())
	{
		OutError = FString::Printf(
			TEXT("Blender manifest point count (%lld) does not match Alembic point count (%d)."),
			ExpectedFirstPoint,
			HairDescription.GetNumVertices());
		return nullptr;
	}

	FHairDescriptionGroups GroupDescriptions;
	if (!FGroomBuilder::BuildHairDescriptionGroups(
		HairDescription,
		GroupDescriptions))
	{
		OutError = TEXT("Unreal could not build Groom groups from the Blender manifest.");
		return nullptr;
	}

	if (GroupDescriptions.HairGroups.Num() != ManifestGroupIDs.Num())
	{
		OutError = FString::Printf(
			TEXT("Blender manifest group count (%d) does not match the imported Groom group count (%d)."),
			ManifestGroupIDs.Num(),
			GroupDescriptions.HairGroups.Num());
		return nullptr;
	}

	ImportOptions->InterpolationSettings.Init(
		FHairGroupsInterpolation(),
		GroupDescriptions.HairGroups.Num());
	FHairImportContext ImportContext(
		ImportOptions,
		InParent,
		UGroomAsset::StaticClass(),
		InName,
		Flags);
	UGroomAsset* GroomAsset =
		FHairStrandsImporter::ImportHair(ImportContext, HairDescription);
	if (!GroomAsset)
	{
		OutError = FString::Printf(
			TEXT("Unreal failed to create a Groom asset from '%s'."),
			*Filename);
		return nullptr;
	}

	UGroomAssetImportData* ImportData =
		Cast<UGroomAssetImportData>(GroomAsset->AssetImportData);
	if (!ImportData)
	{
		ImportData = NewObject<UGroomAssetImportData>(GroomAsset);
		GroomAsset->AssetImportData = ImportData;
	}
	ImportData->Update(Filename);
	ImportData->ImportOptions =
		DuplicateObject<UGroomImportOptions>(ImportOptions, ImportData);

	return GroomAsset;
}
