#pragma once
#include "ld58type.h"
#include "ggarena.h"

//ConvertTimeOfDayToInt((timeofday){10,30});
bool util_stringMatch(char* a, char* b);

flight_t* flight_tArray_GetFlightBeforeTime(flight_tArray* array, timeofday_t timeofday, int srcId, int destId);

int ConvertTimeOfDayToInt(timeofday_t timeofday);
timeofday_t ConvertIntToTimeOfDay(int inputTime);

// This will get an airport that is not TPA
airport_t* airport_tArray_GetDestinationAirport(airport_tArray* array);

// This will get the connecting airpot TX -> bHub == true and state == TX
airport_t* airport_tArray_GetDestinationHub(airport_tArray* array, int airportId); 

int airport_tArray_GetByCode(airport_tArray* array, char* code);

//Generated

void passenger_tArray_Alloc(passenger_tArray* array);
void passenger_tArray_AllocArena(Arena* arena, passenger_tArray* array);
void passenger_tArray_ReallocArena(Arena* arena, passenger_tArray* array, int32_t newCnt);
passenger_t* passenger_tArray_Get(passenger_tArray* array, int32_t index);
passenger_tArray* passenger_tArray_CreateArena(Arena* arena, int32_t capacity);
void passenger_tArray_FreeItem(passenger_tArray* array, passenger_t* item);
passenger_t* passenger_tArray_GetNextFree(passenger_tArray* array);
void ticket_tArray_Alloc(ticket_tArray* array);
void ticket_tArray_AllocArena(Arena* arena, ticket_tArray* array);
void ticket_tArray_ReallocArena(Arena* arena, ticket_tArray* array, int32_t newCnt);
ticket_t* ticket_tArray_Get(ticket_tArray* array, int32_t index);
ticket_tArray* ticket_tArray_CreateArena(Arena* arena, int32_t capacity);
void ticket_tArray_FreeItem(ticket_tArray* array, ticket_t* item);
ticket_t* ticket_tArray_GetNextFree(ticket_tArray* array);
void airport_tArray_Alloc(airport_tArray* array);
void airport_tArray_AllocArena(Arena* arena, airport_tArray* array);
void airport_tArray_ReallocArena(Arena* arena, airport_tArray* array, int32_t newCnt);
airport_t* airport_tArray_Get(airport_tArray* array, int32_t index);
airport_tArray* airport_tArray_CreateArena(Arena* arena, int32_t capacity);
void airport_tArray_FreeItem(airport_tArray* array, airport_t* item);
airport_t* airport_tArray_GetNextFree(airport_tArray* array);
void flight_tArray_Alloc(flight_tArray* array);
void flight_tArray_AllocArena(Arena* arena, flight_tArray* array);
void flight_tArray_ReallocArena(Arena* arena, flight_tArray* array, int32_t newCnt);
flight_t* flight_tArray_Get(flight_tArray* array, int32_t index);
flight_tArray* flight_tArray_CreateArena(Arena* arena, int32_t capacity);
void flight_tArray_FreeItem(flight_tArray* array, flight_t* item);
flight_t* flight_tArray_GetNextFree(flight_tArray* array);
void RenderTexture2D_tArray_Alloc(RenderTexture2D_tArray* array);
void RenderTexture2D_tArray_AllocArena(Arena* arena, RenderTexture2D_tArray* array);
void RenderTexture2D_tArray_ReallocArena(Arena* arena, RenderTexture2D_tArray* array, int32_t newCnt);
RenderTexture2D_t* RenderTexture2D_tArray_Get(RenderTexture2D_tArray* array, int32_t index);
RenderTexture2D_tArray* RenderTexture2D_tArray_CreateArena(Arena* arena, int32_t capacity);
void RenderTexture2D_tArray_FreeItem(RenderTexture2D_tArray* array, RenderTexture2D_t* item);
RenderTexture2D_t* RenderTexture2D_tArray_GetNextFree(RenderTexture2D_tArray* array);
void dayscorecard_tArray_Alloc(dayscorecard_tArray* array);
void dayscorecard_tArray_AllocArena(Arena* arena, dayscorecard_tArray* array);
void dayscorecard_tArray_ReallocArena(Arena* arena, dayscorecard_tArray* array, int32_t newCnt);
dayscorecard_t* dayscorecard_tArray_Get(dayscorecard_tArray* array, int32_t index);
dayscorecard_tArray* dayscorecard_tArray_CreateArena(Arena* arena, int32_t capacity);
void dayscorecard_tArray_FreeItem(dayscorecard_tArray* array, dayscorecard_t* item);
dayscorecard_t* dayscorecard_tArray_GetNextFree(dayscorecard_tArray* array);

//END GENERATED