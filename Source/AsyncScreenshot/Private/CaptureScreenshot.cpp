#include "CaptureScreenshot.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"    // FTSTicker
#include "UnrealClient.h"         // FScreenshotRequest

UCaptureScreenshot* UCaptureScreenshot::CaptureScreenshot(UObject* InWorldContextObject,
                                                           const FString& FileName,
                                                           const FString& SaveLocation,
                                                           bool bInShowUI,
                                                           bool bInAddFilenameSuffix)
{
    UCaptureScreenshot* Node = NewObject<UCaptureScreenshot>();
    Node->WorldContextObject = InWorldContextObject;
    Node->bShowUI = bInShowUI;
    Node->bAddFilenameSuffix = bInAddFilenameSuffix;

    FString Dir = FPaths::ConvertRelativePathToFull(SaveLocation);
    IFileManager::Get().MakeDirectory(*Dir, true);

    FString Name = FileName;
    if (FPaths::GetExtension(Name).IsEmpty())
        Name += TEXT(".png");

    Node->TargetFilePath = FPaths::Combine(Dir, Name);
    return Node;
}

void UCaptureScreenshot::Activate()
{
    if (TargetFilePath.IsEmpty())
    {
        BroadcastFailedNextFrame();
        return;
    }

    FScreenshotRequest::OnScreenshotRequestProcessed().RemoveAll(this);
    FScreenshotRequest::OnScreenshotRequestProcessed().AddUObject(this, &UCaptureScreenshot::HandleScreenshotProcessed);

    IFileManager::Get().MakeDirectory(*FPaths::GetPath(TargetFilePath), true);
    FScreenshotRequest::RequestScreenshot(TargetFilePath, bShowUI, bAddFilenameSuffix, false);
}

void UCaptureScreenshot::HandleScreenshotProcessed()
{
    FScreenshotRequest::OnScreenshotRequestProcessed().RemoveAll(this);
    BroadcastCompletedNextFrame();
}

void UCaptureScreenshot::BroadcastCompletedNextFrame()
{
    FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateWeakLambda(this, [this](float)
        {
            Completed.Broadcast(TargetFilePath);
            Cleanup();
            return false;
        }),
        0.0f
    );
}

void UCaptureScreenshot::BroadcastFailedNextFrame()
{
    FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateWeakLambda(this, [this](float)
        {
            Failed.Broadcast();
            Cleanup();
            return false;
        }),
        0.0f
    );
}

void UCaptureScreenshot::Cleanup()
{
    FScreenshotRequest::OnScreenshotRequestProcessed().RemoveAll(this);
    SetReadyToDestroy();
}
