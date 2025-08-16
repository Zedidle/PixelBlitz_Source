// Fill out your copyright notice in the Description page of Project Settings.


#include "Basic/PXGameInstance.h"

#include "Kismet/KismetStringLibrary.h"

FName UPXGameInstance::GetCurLevelName(bool Continue)
{
	if (Continue)
	{
		return Main_CurLevelName;
	}
	else
	{
		if (Main_SurLevels.Num() == 0) return "";
		
		if (Main_SurLevels.Num() == 1)
		{
			Main_CurLevelName = Main_SurLevels[0];
		}
		else
		{
			const FString s0 = UKismetStringLibrary::GetSubstring(Main_SurLevels[0].ToString(), 1, 1);
			float v0 = FCString::Atod(*s0);

			const FString s1 = UKismetStringLibrary::GetSubstring(Main_SurLevels[1].ToString(), 1, 1);
			float v1 = FCString::Atod(*s1);

			float r = v0 / (v0 + v1);
			if (FMath::FRand() > r)
			{
				Main_CurLevelName = Main_SurLevels[0];
			}
			else
			{
				Main_CurLevelName = Main_SurLevels[1];
			}
		}

		Main_SurLevels.Remove(Main_CurLevelName);
		return Main_CurLevelName;
	}
}

FName UPXGameInstance::GetCurLevelName_Simple(bool Next)
{
	if (Main_SurLevels.Num() == 0) return "";
	
	if (Next)
	{
		Main_CurLevelName = Main_SurLevels[0];
		Main_SurLevels.Remove(Main_CurLevelName);
		return Main_CurLevelName;
	}
	
	if (Main_CurLevelName == FName("None"))
	{
		Main_CurLevelName = Main_SurLevels[0];
	}
	
	return Main_CurLevelName;	
}

void UPXGameInstance::GetTotalUseTime(float& usetime, bool& newrecord)
{
	float Time = 0;
	for (float r : Main_Results)
	{
		Time += r;
	}

	newrecord = Achieve_TotalUseTime > Time;
	if (Achieve_TotalUseTime > Time)
	{
		Achieve_TotalUseTime = Time;
	}
	usetime = Time;
}

void UPXGameInstance::OnPlayerDead_Implementation(bool& End)
{
	Main_LostLife++;
	Main_SupLife--;
	
	End = Main_SupLife == -1;
}
