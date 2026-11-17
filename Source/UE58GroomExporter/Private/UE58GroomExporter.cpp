#include "UE58GroomExporter.h"

#include "Modules/ModuleManager.h"

#include "ToolMenus.h"
#include "ContentBrowserMenuContexts.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserItemPath.h"

#include "HairStrandsCore/Public/GroomAsset.h"
#include "GroomExporterLibrary.h"
#include "BlenderGroomImporter.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"

#include "Framework/Application/SlateApplication.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"

IMPLEMENT_MODULE(FUE58GroomExporterModule, UE58GroomExporter)

#define LOCTEXT_NAMESPACE "UE58GroomExporter"

void FUE58GroomExporterModule::StartupModule()
{
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(
            this,
            &FUE58GroomExporterModule::RegisterMenus));
}

void FUE58GroomExporterModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
}


//Button in right-click menue

void FUE58GroomExporterModule::RegisterMenus()
{
    FToolMenuOwnerScoped Owner(this);

    UToolMenu* Menu =
        UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(
            UGroomAsset::StaticClass());

    FToolMenuSection& Section =
        Menu->FindOrAddSection("GetAssetActions");

    Section.AddMenuEntry(
        "UE58GroomExporter.Export",

        LOCTEXT("ExportToBlender", "Export Groom to Blender"),

        LOCTEXT(
            "ExportToBlenderTooltip",
            "Export this Groom Asset as OBJ + JSON for Blender."),

        FSlateIcon(),

        FToolUIAction(

            FToolMenuExecuteAction::CreateRaw(
                this,
                &FUE58GroomExporterModule::ExportSelectedGroom)
        )
    );

    UToolMenu* FileMenu =
        UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.File");
    FToolMenuSection& ImportSection =
        FileMenu->FindOrAddSection("FileActors");
    ImportSection.AddMenuEntry(
        "UE58GroomExporter.ImportBlenderGroom",
        LOCTEXT("ImportBlenderGroom", "Import Blender Groom (.abc + .json)"),
        LOCTEXT(
            "ImportBlenderGroomTooltip",
            "Import a Blender Alembic groom and restore its group IDs from the companion JSON manifest."),
        FSlateIcon(),
        FToolUIAction(
            FToolMenuExecuteAction::CreateRaw(
                this,
                &FUE58GroomExporterModule::ImportBlenderGroom)
        )
    );
}


//export

void FUE58GroomExporterModule::ExportSelectedGroom(
    const FToolMenuContext& Context)
    {
        const UContentBrowserAssetContextMenuContext* AssetContext =
            Context.FindContext<UContentBrowserAssetContextMenuContext>();

        if (!AssetContext)
        {
            UE_LOG(LogTemp, Error, TEXT("No Asset Context."));
            return;
        }

        if (AssetContext->SelectedAssets.Num() == 0)
        {
            UE_LOG(LogTemp, Error, TEXT("No Groom selected."));
            return;
        }

        UGroomAsset* Groom =
            Cast<UGroomAsset>(
                AssetContext->SelectedAssets[0].GetAsset());

        if (!Groom)
        {
            UE_LOG(LogTemp, Error, TEXT("Selected asset is not a Groom."));
            return;
        }


        //save dialog box

        IDesktopPlatform* DesktopPlatform =
            FDesktopPlatformModule::Get();

        if (!DesktopPlatform)
        {
            UE_LOG(LogTemp, Error, TEXT("DesktopPlatform unavailable."));
            return;
        }

        const void* ParentWindowHandle = nullptr;

        if (FSlateApplication::IsInitialized())
        {
            ParentWindowHandle =
                FSlateApplication::Get()
                .FindBestParentWindowHandleForDialogs(nullptr);
        }

        TArray<FString> OutFiles;

        const bool bChosen =
            DesktopPlatform->SaveFileDialog(
                ParentWindowHandle,
                TEXT("Export Groom"),
                TEXT(""),
                Groom->GetName() + TEXT(".obj"),
                TEXT("Wavefront OBJ (*.obj)|*.obj"),
                EFileDialogFlags::None,
                OutFiles);

        if (!bChosen || OutFiles.Num() == 0)
        {
            return;
        }

        const FString OutputFile = OutFiles[0];

        if (UGroomExporterLibrary::ExportGroomToOBJ(Groom, OutputFile))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Exported to %s"),
                *OutputFile);
        }
        else
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Export failed."));
        }
    }

void FUE58GroomExporterModule::ImportBlenderGroom(
    const FToolMenuContext&)
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform)
    {
        UE_LOG(LogTemp, Error, TEXT("DesktopPlatform unavailable."));
        return;
    }

    const void* ParentWindowHandle = nullptr;
    if (FSlateApplication::IsInitialized())
    {
        ParentWindowHandle =
            FSlateApplication::Get()
            .FindBestParentWindowHandleForDialogs(nullptr);
    }

    TArray<FString> SelectedFiles;
    const bool bChosen = DesktopPlatform->OpenFileDialog(
        ParentWindowHandle,
        TEXT("Import Blender Groom"),
        FPaths::ProjectDir(),
        TEXT(""),
        TEXT("Alembic Groom (*.abc)|*.abc"),
        EFileDialogFlags::None,
        SelectedFiles);
    if (!bChosen || SelectedFiles.IsEmpty())
    {
        return;
    }

    FContentBrowserModule& ContentBrowserModule =
        FModuleManager::LoadModuleChecked<FContentBrowserModule>(
            TEXT("ContentBrowser"));
    FString PackagePath =
        ContentBrowserModule.Get().GetCurrentPath().GetInternalPathString();
    if (!FPackageName::IsValidLongPackageName(PackagePath))
    {
        PackagePath = TEXT("/Game");
    }

    const FString BaseName = FPaths::MakeValidFileName(
        FPaths::GetBaseFilename(SelectedFiles[0]));
    const FString BasePackageName =
        FString::Printf(TEXT("%s/%s"), *PackagePath, *BaseName);

    FString UniquePackageName;
    FString AssetName;
    FModuleManager::LoadModuleChecked<FAssetToolsModule>(
        TEXT("AssetTools"))
        .Get()
        .CreateUniqueAssetName(
            BasePackageName,
            TEXT(""),
            UniquePackageName,
            AssetName);

    UPackage* Package = CreatePackage(*UniquePackageName);
    if (!Package)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Could not create Groom package '%s'."),
            *UniquePackageName);
        return;
    }

    FString ImportError;
    UGroomAsset* GroomAsset = BlenderGroomImporter::ImportFromFile(
        SelectedFiles[0],
        Package,
        FName(*AssetName),
        RF_Public | RF_Standalone,
        ImportError);
    if (!GroomAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *ImportError);
        FMessageDialog::Open(
            EAppMsgType::Ok,
            FText::FromString(ImportError));
        return;
    }

    Package->MarkPackageDirty();
    FAssetRegistryModule::AssetCreated(GroomAsset);
    TArray<UObject*> ImportedAssets;
    ImportedAssets.Add(GroomAsset);
    ContentBrowserModule.Get().SyncBrowserToAssets(ImportedAssets);
    UE_LOG(
        LogTemp,
        Display,
        TEXT("Imported Blender Groom to %s."),
        *UniquePackageName);
}

#undef LOCTEXT_NAMESPACE