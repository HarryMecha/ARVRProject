// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"

void UHealthBarWidget::CreateHealthBar(int32 maxHealth)
{

    healthBar->ClearChildren();
    heartWidgets.Empty();

    for (int32 i = 0; i < maxHealth; ++i)
    {
        UHeartWidget* newHeart = CreateWidget<UHeartWidget>(this, heartWidgetClass);
        newHeart->getHeartImage()->SetBrushSize(FVector2D(50, 50));
        healthBar->AddChild(newHeart);
        heartWidgets.Add(newHeart);
    }
}

void UHealthBarWidget::updateHearts(float currentHealth)
{
    float currentHealthAmount = currentHealth;

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("Enemy Current Health: %f"), currentHealthAmount));


    for (UHeartWidget* heart : heartWidgets)
    {
        heart->setHeartState(EHeartFillState::EMPTY);
    }

    for (int32 i = 0; i < heartWidgets.Num(); ++i)
    {
        if (currentHealthAmount >= 1.0f)
        {
            heartWidgets[i]->setHeartState(EHeartFillState::FULL);
            currentHealthAmount -= 1.0f;
        }
        else
        {
            if (currentHealthAmount == 0.25f)
            {
                heartWidgets[i]->setHeartState(EHeartFillState::QUARTER);
                currentHealthAmount = 0.0f;
            }
            else if (currentHealthAmount == 0.5f)
            {
                heartWidgets[i]->setHeartState(EHeartFillState::HALF);
                currentHealthAmount = 0.0f;
            }
            else if (currentHealthAmount == 0.75f)
            {
                heartWidgets[i]->setHeartState(EHeartFillState::THREEQUARTER);
                currentHealthAmount = 0.0f;
            }
            else
            {
                heartWidgets[i]->setHeartState(EHeartFillState::EMPTY);
            }
        }
    }
}

void UHealthBarWidget::setHealthBarHeaderText(FString text)
{
    healthBarheader->SetText(FText::FromString(text));
}