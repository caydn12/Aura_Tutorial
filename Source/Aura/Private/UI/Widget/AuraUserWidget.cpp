// Copyright Kickback Studio


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWdigetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}