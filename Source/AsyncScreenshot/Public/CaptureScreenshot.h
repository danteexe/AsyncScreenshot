#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CaptureScreenshot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenshotCompleted, const FString&, FullFilePath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenshotFailed);

UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncScreenshot"))
class ASYNCSCREENSHOT_API UCaptureScreenshot final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnScreenshotCompleted Completed;

	UPROPERTY(BlueprintAssignable)
	FOnScreenshotFailed Failed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName = "Capture Screenshot", WorldContext = "WorldContextObject"))
	static UCaptureScreenshot* CaptureScreenshot(UObject* WorldContextObject, const FString& FileName, const FString& SaveLocation, bool bShowUI = false, bool bAddFilenameSuffix = false);

	virtual void Activate() override;

private:
	UPROPERTY()
	UObject* WorldContextObject = nullptr;

	FString TargetFilePath;
	bool bShowUI = false;
	bool bAddFilenameSuffix = false;

	// Kesin tanımlı methodlar
	void HandleScreenshotProcessed();
	void BroadcastCompletedNextFrame();
	void BroadcastFailedNextFrame();
	void Cleanup();
};
