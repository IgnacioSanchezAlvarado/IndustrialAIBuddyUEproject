#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "ApiCall.generated.h"

UCLASS()
class INDUSTRIAL_AI_BUDDY_API AApiCall : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AApiCall();

    using FResponseCallback = TFunction<void(const FString&)>;

    void SendApiRequest(const FResponseCallback& Callback);

private:
    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FResponseCallback Callback);
};


