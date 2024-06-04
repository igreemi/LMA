// ---------------LMA--------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

class UUserWidget;

UCLASS()
class LEAVEMEALONE_API ABaseHUD : public AHUD
{
	GENERATED_BODY()
	
	public:

		UPROPERTY(BlueprintReadWrite)
		TArray<UUserWidget*> WidgetsContainer;
};
