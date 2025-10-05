#pragma once
#include "ggarena.h"


void ld58_Init(Arena* memArena);
void ld58_LoadLevel1(ld_gamestate_t* gameStatePtr);
void ld58_LoadLevel2(ld_gamestate_t* gameStatePtr);
void ld58_LoadLevel3(ld_gamestate_t* gameStatePtr);
void ld58_LoadLevel4(ld_gamestate_t* gameStatePtr);
void ld58_LoadLevel5(ld_gamestate_t* gameStatePtr);
ticket_t* GenerateTicketPrinted(ld_gamestate_t* gameStatePtr, passenger_t* passenger, flight_t* flight, bool firstClass);