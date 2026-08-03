#pragma once

#include "Modules/ModuleManager.h"

struct FToolMenuContext;

class FUE58GroomExporterModule : public IModuleInterface
{
public:

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:

    void RegisterMenus();

    void ExportSelectedGroom(const FToolMenuContext& Context);
};