

#include "UI/DamageFlashWidget.h"

void UDamageFlashWidget::PlayFlash()
{
    if (Flash)
    {
        PlayAnimation(Flash);
    }
}
