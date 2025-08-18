// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"
PIXEL2DKIT_API DECLARE_LOG_CATEGORY_EXTERN(LogHall, Log, All)
PIXEL2DKIT_API DECLARE_LOG_CATEGORY_EXTERN(LogGame, Log, All)
PIXEL2DKIT_API DECLARE_LOG_CATEGORY_EXTERN(LogBuild, Log, All)
PIXEL2DKIT_API DECLARE_LOG_CATEGORY_EXTERN(LogRGAI, Log, All)
PIXEL2DKIT_API DECLARE_LOG_CATEGORY_EXTERN(LogRGProtocol, Log, All)
DECLARE_STATS_GROUP(TEXT("RAGame"), STATGROUP_RAGame, STATCAT_Advanced);
#define CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Pointer) if(nullptr==(Pointer)){return;}
#define CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Pointer,Val) if(nullptr==(Pointer)){return (Val);}
//Judge is server
#define IS_SERVER_OR_STANDALONE (NM_Client != GetNetMode())
//Judge is client
#define IS_CLIENT (NM_Client == GetNetMode())
//Judge is dedicated server
#define IS_DEDICATED_SERVER (NM_DedicatedServer == GetNetMode())
//Judge is listen server
#define IS_LISTEN_SERVER (NM_ListenServer == GetNetMode())
//Judge is server
#define IS_SERVER (IS_DEDICATED_SERVER || IS_LISTEN_SERVER)
//Judge is standalone
#define IS_STAND_ALONE (NM_Standalone == GetNetMode())
//Judge is server or standalone
#define IS_SERVER_OR_STAND_ALONE (IS_SERVER||IS_STAND_ALONE)
//Judge is server or standalone
#define IS_CLIENT_OR_STAND_ALONE (IS_CLIENT||IS_STAND_ALONE)
//Judge is AUTONOMOUS client
#define IS_AUTONOMOUS_CLIENT (ROLE_AutonomousProxy == Role)
//Judge is SIMULATE client
#define IS_SIMULATED_CLIENT (ROLE_SimulatedProxy == Role)
#define IS_SINGLE_PLAYER_FROM_PAWN(Pawn) (ROLE_Authority == Role&&Pawn->IsLocallyControlled())
#define IS_SINGLE_PLAYERF_ROM_CONTROLLER(Controller) (ROLE_Authority == Role&&Controller->IsLocalPlayerController())




