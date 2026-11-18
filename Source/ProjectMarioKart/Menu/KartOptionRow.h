#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KartOptionRow.generated.h"

class UTextBlock;

UCLASS(Abstract)
class PROJECTMARIOKART_API UKartOptionRow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Label;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Kart|Option")
	FText LabelText;
	
};
