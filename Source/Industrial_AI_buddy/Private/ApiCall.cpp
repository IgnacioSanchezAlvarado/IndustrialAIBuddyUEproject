#include "ApiCall.h"
#include "Engine/Engine.h"
#include "Logging/LogMacros.h"

// Sets default values
AApiCall::AApiCall()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
}

void AApiCall::SendApiRequest(const FResponseCallback& Callback)
{
    UE_LOG(LogTemp, Warning, TEXT("SendApiRequest called"));

    // Create the HTTP request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AApiCall::OnResponseReceived, Callback);
    Request->SetURL(TEXT("https://z4l7mvuhi4.execute-api.us-east-1.amazonaws.com/default/Unreal_engine_function1")); // Replace with your API endpoint
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Set the request content
    FString RequestContent = TEXT("{\"message\":\"Hello, this is a default string!\"}");
    Request->SetContentAsString(RequestContent);

    // Send the request
    bool bRequestSent = Request->ProcessRequest();
    if (bRequestSent)
    {
        UE_LOG(LogTemp, Warning, TEXT("HTTP request sent successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to send HTTP request"));
    }
}

void AApiCall::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FResponseCallback Callback)
{
    UE_LOG(LogTemp, Warning, TEXT("OnResponseReceived called"));

    FString ResponseContent;
    if (bWasSuccessful && Response.IsValid())
    {
        ResponseContent = Response->GetContentAsString();
        UE_LOG(LogTemp, Warning, TEXT("API Response: %s"), *ResponseContent);
    }
    else
    {
        ResponseContent = TEXT("Failed to get a valid response from the API");
        UE_LOG(LogTemp, Error, TEXT("Failed to get a valid response from the API"));
    }

    // Call the callback function with the response content
    Callback(ResponseContent);
}


