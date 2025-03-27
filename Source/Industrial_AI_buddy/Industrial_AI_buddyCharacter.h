// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ApiCall.h"
#include "Industrial_AI_buddyCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAudioCapture;
class USoundWave;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AIndustrial_AI_buddyCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	// PrintMessage Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PrintMessageAction;

	// ResetGame Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ResetGameIA;

	// Language Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LanguageIA;

public:
	AIndustrial_AI_buddyCharacter();
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	// Text box events
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FString TextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FString StatusText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FString LanguageText;

	// Beep sound assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundWave* StartBeepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundWave* StopBeepSound;

	// Camera sensitivity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensitivity")
	float Sensitivity;
	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	/** Input functions */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PrintMessage(const FInputActionValue& Value);
	void LanguageSelection(const FInputActionValue& Value);

	// Function to handle the HTTP response
	void MakeApiCall(const FString& ApiUrl, const FString& Content, TFunction<void(const FString&)> OnResponse);
	FString OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	// Get Meshes function
	FString GetClosestMeshName();
	FString GetPlayerPosition();


	// Audio functions
	void StartAudioRecording();
	void StopAudioRecording();
	void PlayRecordedAudio();
	TArray<float> MergeAudioChunks();
	void PlayBase64Audio(const FString& Base64AudioData);

	// change text
	UFUNCTION(BlueprintImplementableEvent)
	void changeText();

	// set up UI
	UFUNCTION(BlueprintImplementableEvent)
	void setUpUI();

	// Reset
	UFUNCTION(BlueprintImplementableEvent)
	void ResetGame();

	// Language
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeLanguage();


private:
	UAudioCapture* AudioCaptureInstance;
	TArray<float> AudioBuffer;
	void OnAudioGenerate(const float* InAudio, int32 NumSamples);
	FCriticalSection AudioBufferLock;
	bool bIsRecording = false;

	FString ApiResponse;
	FString SecondApiResponse;
	FString SessionId;
	FVector LastKnownPosition;

};

