#include "UE58GroomExporter.h"

#include "Modules/ModuleManager.h"

#include "ToolMenus.h"
#include "ContentBrowserMenuContexts.h"

#include "HairStrandsCore/Public/GroomAsset.h"
#include "GroomExporterLibrary.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"

#include "Framework/Application/SlateApplication.h"

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
#undef LOCTEXT_NAMESPACE