// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GroomExporterLibrary.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGroomExporterLibrary() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_UBlueprintFunctionLibrary(ETypeConstructPhase);
HAIRSTRANDSCORE_API UClass* Z_Construct_UClass_UGroomAsset(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_UE58GroomExporter(ETypeConstructPhase);
UE58GROOMEXPORTER_API UClass* Z_Construct_UClass_UGroomExporterLibrary(ETypeConstructPhase);
UE58GROOMEXPORTER_API UClass* Z_Construct_UClass_UGroomExporterLibrary(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Class UGroomExporterLibrary Function ExportGroomToOBJ **************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGroomExporterLibrary_ExportGroomToOBJ_Statics
struct UHT_STATICS
{
	struct GroomExporterLibrary_eventExportGroomToOBJ_Parms
	{
		UGroomAsset* Groom;
		FString Filename;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Groom Export" },
		{ "ModuleRelativePath", "Public/GroomExporterLibrary.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Filename_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function ExportGroomToOBJ constinit property declarations **********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Groom;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Filename;
	static void NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GroomExporterLibrary_eventExportGroomToOBJ_Parms*)Obj)->ReturnValue = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function ExportGroomToOBJ constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function ExportGroomToOBJ Property Definitions *********************************
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_Groom = { "Groom", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, nullptr, nullptr, 1, STRUCT_OFFSET(GroomExporterLibrary_eventExportGroomToOBJ_Parms, Groom), Z_Construct_UClass_UGroomAsset, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_Filename = { "Filename", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(GroomExporterLibrary_eventExportGroomToOBJ_Parms, Filename), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Filename_MetaData), NewProp_Filename_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(GroomExporterLibrary_eventExportGroomToOBJ_Parms), &UHT_STATICS::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Groom,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Filename,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Function ExportGroomToOBJ Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGroomExporterLibrary, nullptr, "ExportGroomToOBJ", UHT_STATICS::PropPointers, UE_ARRAY_COUNT(UHT_STATICS::PropPointers), DataSizeOf<UHT_STATICS::GroomExporterLibrary_eventExportGroomToOBJ_Parms>(), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
static_assert(sizeof(UHT_STATICS::GroomExporterLibrary_eventExportGroomToOBJ_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGroomExporterLibrary_ExportGroomToOBJ(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGroomExporterLibrary::execExportGroomToOBJ)
{
	P_GET_OBJECT(UGroomAsset,Z_Param_Groom);
	P_GET_PROPERTY(FStrProperty,Z_Param_Filename);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=UGroomExporterLibrary::ExportGroomToOBJ(Z_Param_Groom,Z_Param_Filename);
	P_NATIVE_END;
}
// ********** End Class UGroomExporterLibrary Function ExportGroomToOBJ ****************************

// ********** Begin Class UGroomExporterLibrary ****************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UClass_UGroomExporterLibrary_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "IncludePath", "GroomExporterLibrary.h" },
		{ "ModuleRelativePath", "Public/GroomExporterLibrary.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UGroomExporterLibrary constinit property declarations ********************
// ********** End Class UGroomExporterLibrary constinit property declarations **********************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("ExportGroomToOBJ"), .Pointer = &UGroomExporterLibrary::execExportGroomToOBJ },
	};
	static FTypeConstructFunc* DependentSingletons[];
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGroomExporterLibrary_ExportGroomToOBJ, "ExportGroomToOBJ" }, // 21e92f995e505aba4307a1c7d82d225faee80461
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGroomExporterLibrary>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct UHT_STATICS
FTypeConstructFunc* UHT_STATICS::DependentSingletons[] = {
	(FTypeConstructFunc*)Z_Construct_UClass_UBlueprintFunctionLibrary,
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_UE58GroomExporter,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams UHT_STATICS::ClassParams = {
	&Z_Construct_UClass_UGroomExporterLibrary,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static void UGroomExporterLibrary_StaticRegisterNativesUGroomExporterLibrary()
{
	UClass* Class = UGroomExporterLibrary::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, 		MakeConstArrayView(UHT_STATICS::Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_UGroomExporterLibrary;
UClass* Z_Construct_UClass_UGroomExporterLibrary(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Inner)
	{
		using TClass = UGroomExporterLibrary;
		if (!Z_Registration_Info_UClass_UGroomExporterLibrary.InnerSingleton)
		{
			GetPrivateStaticClassBody(
				TClass::StaticPackage(),
				TEXT("GroomExporterLibrary"),
				Z_Registration_Info_UClass_UGroomExporterLibrary.InnerSingleton,
				UGroomExporterLibrary_StaticRegisterNativesUGroomExporterLibrary,
				DataSizeOf<TClass>(),
				alignof(TClass),
				TClass::StaticClassFlags,
				TClass::StaticClassCastFlags(),
				TClass::StaticConfigName(),
				(UClass::ClassConstructorType)InternalConstructor<TClass>,
				(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
				UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
				&TClass::Super::StaticClass,
				&TClass::WithinClass::StaticClass
			);
		}
		return Z_Registration_Info_UClass_UGroomExporterLibrary.InnerSingleton;
	}
	if (!Z_Registration_Info_UClass_UGroomExporterLibrary.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGroomExporterLibrary.OuterSingleton, UHT_STATICS::ClassParams);
	}
	return Z_Registration_Info_UClass_UGroomExporterLibrary.OuterSingleton;
}
#undef UHT_STATICS
UGroomExporterLibrary::UGroomExporterLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UGroomExporterLibrary);
UGroomExporterLibrary::~UGroomExporterLibrary() {}
// ********** End Class UGroomExporterLibrary ******************************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_FemaleCharacter01_Plugins_UE58GroomExporter_Source_UE58GroomExporter_Public_GroomExporterLibrary_h__Script_UE58GroomExporter_Statics
struct UHT_STATICS
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGroomExporterLibrary, TEXT("UGroomExporterLibrary"), &Z_Registration_Info_UClass_UGroomExporterLibrary, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGroomExporterLibrary), 334278528U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_FemaleCharacter01_Plugins_UE58GroomExporter_Source_UE58GroomExporter_Public_GroomExporterLibrary_h__Script_UE58GroomExporter_1401a49d407f7fc3c7de2315badd0bd9e0427616{
	TEXT("/Script/UE58GroomExporter"),
	UHT_STATICS::ClassInfo, UE_ARRAY_COUNT(UHT_STATICS::ClassInfo),
	nullptr, 0,
	nullptr, 0,
	nullptr, 0,
};
#undef UHT_STATICS
// ********** End Registration *********************************************************************
#undef UHT_STRUCT_BASE

PRAGMA_ENABLE_DEPRECATION_WARNINGS
