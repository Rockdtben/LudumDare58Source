#include "ld58typeArray.h"
#include "raylib.h"
#include "stdio.h"
#include "string.h"



flight_t* flight_tArray_GetFlightBeforeTime(flight_tArray* array, timeofday_t timeofday, int srcId, int destId)
{
	int timeInInt = ConvertTimeOfDayToInt(timeofday);

	int totalMatches = 0;
	for (int i = 0; i < array->length; i++)
	{
		flight_t* flight = flight_tArray_Get(array, i);

		if (flight->takeoffEst < timeInInt && flight->srcId == srcId && flight->destId == destId)
		{
			totalMatches++;
		}
	}

	int itemRand = GetRandomValue(0, totalMatches - 1);


	int newMatch = 0;
	for (int i = 0; i < array->length; i++)
	{
		flight_t* flight = flight_tArray_Get(array, i);

		if (flight->takeoffEst < timeInInt && flight->srcId == srcId && flight->destId == destId)
		{
			if (itemRand == newMatch)
			{
				return flight;
			}
			newMatch++;
		}
	}

	return NULL;
}


// Instead of making these point to actual structs they just point to pointers.
flight_tArray* flight_tArray_CreateSlice(Arena* arena, int32_t capacity)
{
	flight_tArray* result = (flight_tArray*)Arena_Alloc(arena, sizeof(flight_tArray));
	result->capacity = capacity;
	result->length = 0;
	result->nextfree = NULL;
	flight_tArray_AllocArena(arena, result);
	return result;
}

int ConvertTimeOfDayToInt(timeofday_t timeofday)
{
	int res = (timeofday.HH * 60 * 60) + (timeofday.MM * 60);
	return res;
}

timeofday_t ConvertIntToTimeOfDay(int inputTime)
{
	timeofday_t timeofday;
	timeofday.HH = (int)(inputTime / 60.0f / 60.0f);
	timeofday.MM = (int)((inputTime - (timeofday.HH * 60 * 60)) / 60.0f);

	return timeofday;
}

bool util_stringMatch(char* a, char* b)
{
	int index = 0;
	while (true)
	{
		if (a[index] == '\0' && b[index] == '\0') return true;
		if (a[index] != '\0' && b[index] == '\0') return false;
		if (a[index] == '\0' && b[index] != '\0') return false;


		if (a[index] != b[index]) return false;

		index++;
	}

	return true;
}


// This will get an airport that is not TPA
airport_t* airport_tArray_GetDestinationAirport(airport_tArray* array)
{
	// Pick anything but TPA.

	int airportId = -1;
	airport_t* airport;

	while (true)
	{
		int airportId = GetRandomValue(0, array->length - 1);

		airport = airport_tArray_Get(array, airportId);

		if (!util_stringMatch(airport->airportcode, "TPA"))
		{
			return airport;
		}
		else
		{
			printf("Don't use TPA\n");
		}
	}
	return NULL;
}

// This will get the connecting airport TX -> bHub == true and state == TX
airport_t* airport_tArray_GetDestinationHub(airport_tArray* array, int airportId)
{
	int airportDestId = airport_tArray_GetByCode(array, "DFW");
	airport_t* airport = airport_tArray_Get(array, airportDestId);

	return airport;
}

int airport_tArray_GetByCode(airport_tArray* array, char* code)
{
	airport_t* airport;
	for (int i = 0; i < array->length; i++)
	{
		airport = airport_tArray_Get(array, i);
		bool match = true;
		for (int j = 0; j < 3; j++)
		{
			if (code[j] != airport->airportcode[j])
			{
				match = false;
				break;
			}
		}
		if (match)
		{
			return i;
		}

	}
	return -1;
}


// GENERATED



passenger_t* passenger_tArray_Get(passenger_tArray* array, int32_t index)
{
	if (index >= 0 && index < array->length && index < array->capacity)
	{
		return &array->items[index];
	}
	else
	{
		// Oh boy you messed up. You landed here. This means that you could be corrupting memory.
#ifdef _WIN32
		__debugbreak();
#endif
	}
	return NULL;
}

void passenger_tArray_Alloc(passenger_tArray* array)
{
	array->items = (passenger_t*)calloc(array->capacity, sizeof(passenger_t));
}

void passenger_tArray_AllocArena(Arena* arena, passenger_tArray* array)
{
	array->items = (passenger_t*)Arena_Alloc(arena, (array->capacity * sizeof(passenger_t)));
}

void passenger_tArray_ReallocArena(Arena* arena, passenger_tArray* array, int32_t newCnt)
{
	passenger_t* oldData = array->items;
	array->items = (passenger_t*)Arena_Realloc(arena, array, (array->capacity * sizeof(passenger_t)), (newCnt * sizeof(passenger_t)));
	for (int i = 0; i < array->capacity; i++)
	{
		memcpy(array->items + i, oldData + i, sizeof(passenger_t));
	}
	array->capacity = newCnt;
}

passenger_tArray* passenger_tArray_CreateArena(Arena* arena, int32_t capacity)
{
	passenger_tArray* result = (passenger_tArray*)Arena_Alloc(arena, sizeof(passenger_tArray));
	result->capacity = capacity;
	result->length = 0;
	result->nextfree = NULL;
	passenger_tArray_AllocArena(arena, result);
	return result;
}

void passenger_tArray_FreeItem(passenger_tArray* array, passenger_t* item)
{
	item->bActive = false;
	item->nextfree = array->nextfree;
	array->nextfree = (struct passenger_t*)item;
}

passenger_t* passenger_tArray_GetNextFree(passenger_tArray* array)
{
	passenger_t* result = (passenger_t*)array->nextfree;

	if (result == NULL)
	{
		int index = array->length;
		array->length++;
		result = passenger_tArray_Get(array, index);
		result->nextfree = NULL;
		result->instanceId++;
	}
	else
	{
		array->nextfree = result->nextfree;
		result->nextfree = NULL;
		result->instanceId++;
		if (result->entityId >= array->length)
		{
			// Typically wont' happen, but if it does we wannt know.
			array->length = result->entityId;
#ifdef _WIN32
			__debugbreak();
#endif
		}
	}
	return result;
}


ticket_t* ticket_tArray_Get(ticket_tArray* array, int32_t index)
{
	if (index >= 0 && index < array->length && index < array->capacity)
	{
		return &array->items[index];
	}
	else
	{
		// Oh boy you messed up. You landed here. This means that you could be corrupting memory.
#ifdef _WIN32
		__debugbreak();
#endif
	}
	return NULL;
}

void ticket_tArray_Alloc(ticket_tArray* array)
{
	array->items = (ticket_t*)calloc(array->capacity, sizeof(ticket_t));
}

void ticket_tArray_AllocArena(Arena* arena, ticket_tArray* array)
{
	array->items = (ticket_t*)Arena_Alloc(arena, (array->capacity * sizeof(ticket_t)));
}

void ticket_tArray_ReallocArena(Arena* arena, ticket_tArray* array, int32_t newCnt)
{
	ticket_t* oldData = array->items;
	array->items = (ticket_t*)Arena_Realloc(arena, array, (array->capacity * sizeof(ticket_t)), (newCnt * sizeof(ticket_t)));
	for (int i = 0; i < array->capacity; i++)
	{
		memcpy(array->items + i, oldData + i, sizeof(ticket_t));
	}
	array->capacity = newCnt;
}

ticket_tArray* ticket_tArray_CreateArena(Arena* arena, int32_t capacity)
{
	ticket_tArray* result = (ticket_tArray*)Arena_Alloc(arena, sizeof(ticket_tArray));
	result->capacity = capacity;
	result->length = 0;
	result->nextfree = NULL;
	ticket_tArray_AllocArena(arena, result);
	return result;
}

void ticket_tArray_FreeItem(ticket_tArray* array, ticket_t* item)
{
	item->bActive = false;
	item->nextfree = array->nextfree;
	array->nextfree = (struct ticket_t*)item;
}

ticket_t* ticket_tArray_GetNextFree(ticket_tArray* array)
{
	ticket_t* result = (ticket_t*)array->nextfree;

	if (result == NULL)
	{
		int index = array->length;
		array->length++;
		result = ticket_tArray_Get(array, index);
		result->nextfree = NULL;
		result->instanceId++;
	}
	else
	{
		array->nextfree = result->nextfree;
		result->nextfree = NULL;
		result->instanceId++;
		if (result->entityId >= array->length)
		{
			// Typically wont' happen, but if it does we wannt know.
			array->length = result->entityId;
#ifdef _WIN32
			__debugbreak();
#endif
		}
	}
	return result;
}


airport_t* airport_tArray_Get(airport_tArray* array, int32_t index)
{
	if (index >= 0 && index < array->length && index < array->capacity)
	{
		return &array->items[index];
	}
	else
	{
		// Oh boy you messed up. You landed here. This means that you could be corrupting memory.
#ifdef _WIN32
		__debugbreak();
#endif
	}
	return NULL;
}

void airport_tArray_Alloc(airport_tArray* array)
{
	array->items = (airport_t*)calloc(array->capacity, sizeof(airport_t));
}

void airport_tArray_AllocArena(Arena* arena, airport_tArray* array)
{
	array->items = (airport_t*)Arena_Alloc(arena, (array->capacity * sizeof(airport_t)));
}

void airport_tArray_ReallocArena(Arena* arena, airport_tArray* array, int32_t newCnt)
{
	airport_t* oldData = array->items;
	array->items = (airport_t*)Arena_Realloc(arena, array, (array->capacity * sizeof(airport_t)), (newCnt * sizeof(airport_t)));
	for (int i = 0; i < array->capacity; i++)
	{
		memcpy(array->items + i, oldData + i, sizeof(airport_t));
	}
	array->capacity = newCnt;
}

airport_tArray* airport_tArray_CreateArena(Arena* arena, int32_t capacity)
{
	airport_tArray* result = (airport_tArray*)Arena_Alloc(arena, sizeof(airport_tArray));
	result->capacity = capacity;
	result->length = 0;
	result->nextfree = NULL;
	airport_tArray_AllocArena(arena, result);
	return result;
}

void airport_tArray_FreeItem(airport_tArray* array, airport_t* item)
{
	item->bActive = false;
	item->nextfree = array->nextfree;
	array->nextfree = (struct airport_t*)item;
}

airport_t* airport_tArray_GetNextFree(airport_tArray* array)
{
	airport_t* result = (airport_t*)array->nextfree;

	if (result == NULL)
	{
		int index = array->length;
		array->length++;
		result = airport_tArray_Get(array, index);
		result->nextfree = NULL;
		result->instanceId++;
	}
	else
	{
		array->nextfree = result->nextfree;
		result->nextfree = NULL;
		result->instanceId++;
		if (result->entityId >= array->length)
		{
			// Typically wont' happen, but if it does we wannt know.
			array->length = result->entityId;
#ifdef _WIN32
			__debugbreak();
#endif
		}
	}
	return result;
}


flight_t* flight_tArray_Get(flight_tArray* array, int32_t index)
{
	if (index >= 0 && index < array->length && index < array->capacity)
	{
		return &array->items[index];
	}
	else
	{
		// Oh boy you messed up. You landed here. This means that you could be corrupting memory.
#ifdef _WIN32
		__debugbreak();
#endif
	}
	return NULL;
}

void flight_tArray_Alloc(flight_tArray* array)
{
	array->items = (flight_t*)calloc(array->capacity, sizeof(flight_t));
}

void flight_tArray_AllocArena(Arena* arena, flight_tArray* array)
{
	array->items = (flight_t*)Arena_Alloc(arena, (array->capacity * sizeof(flight_t)));
}

void flight_tArray_ReallocArena(Arena* arena, flight_tArray* array, int32_t newCnt)
{
	flight_t* oldData = array->items;
	array->items = (flight_t*)Arena_Realloc(arena, array, (array->capacity * sizeof(flight_t)), (newCnt * sizeof(flight_t)));
	for (int i = 0; i < array->capacity; i++)
	{
		memcpy(array->items + i, oldData + i, sizeof(flight_t));
	}
	array->capacity = newCnt;
}

flight_tArray* flight_tArray_CreateArena(Arena* arena, int32_t capacity)
{
	flight_tArray* result = (flight_tArray*)Arena_Alloc(arena, sizeof(flight_tArray));
	result->capacity = capacity;
	result->length = 0;
	result->nextfree = NULL;
	flight_tArray_AllocArena(arena, result);
	return result;
}

void flight_tArray_FreeItem(flight_tArray* array, flight_t* item)
{
	item->bActive = false;
	item->nextfree = array->nextfree;
	array->nextfree = (struct flight_t*)item;
}

flight_t* flight_tArray_GetNextFree(flight_tArray* array)
{
	flight_t* result = (flight_t*)array->nextfree;

	if (result == NULL)
	{
		int index = array->length;
		array->length++;
		result = flight_tArray_Get(array, index);
		result->nextfree = NULL;
		result->instanceId++;
	}
	else
	{
		array->nextfree = result->nextfree;
		result->nextfree = NULL;
		result->instanceId++;
		if (result->entityId >= array->length)
		{
			// Typically wont' happen, but if it does we wannt know.
			array->length = result->entityId;
#ifdef _WIN32
			__debugbreak();
#endif
		}
	}
	return result;
}


RenderTexture2D_t* RenderTexture2D_tArray_Get(RenderTexture2D_tArray* array, int32_t index)
{
	if (index >= 0 && index < array->length && index < array->capacity)
	{
		return &array->items[index];
	}
	else
	{
		// Oh boy you messed up. You landed here. This means that you could be corrupting memory.
#ifdef _WIN32
		__debugbreak();
#endif
	}
	return NULL;
}

void RenderTexture2D_tArray_Alloc(RenderTexture2D_tArray* array)
{
	array->items = (RenderTexture2D_t*)calloc(array->capacity, sizeof(RenderTexture2D_t));
}

void RenderTexture2D_tArray_AllocArena(Arena* arena, RenderTexture2D_tArray* array)
{
	array->items = (RenderTexture2D_t*)Arena_Alloc(arena, (array->capacity * sizeof(RenderTexture2D_t)));
}

void RenderTexture2D_tArray_ReallocArena(Arena* arena, RenderTexture2D_tArray* array, int32_t newCnt)
{
	RenderTexture2D_t* oldData = array->items;
	array->items = (RenderTexture2D_t*)Arena_Realloc(arena, array, (array->capacity * sizeof(RenderTexture2D_t)), (newCnt * sizeof(RenderTexture2D_t)));
	for (int i = 0; i < array->capacity; i++)
	{
		memcpy(array->items + i, oldData + i, sizeof(RenderTexture2D_t));
	}
	array->capacity = newCnt;
}

RenderTexture2D_tArray* RenderTexture2D_tArray_CreateArena(Arena* arena, int32_t capacity)
{
	RenderTexture2D_tArray* result = (RenderTexture2D_tArray*)Arena_Alloc(arena, sizeof(RenderTexture2D_tArray));
	result->capacity = capacity;
	result->length = 0;
	result->nextfree = NULL;
	RenderTexture2D_tArray_AllocArena(arena, result);
	return result;
}

void RenderTexture2D_tArray_FreeItem(RenderTexture2D_tArray* array, RenderTexture2D_t* item)
{
	item->bActive = false;
	item->nextfree = array->nextfree;
	array->nextfree = (struct RenderTexture2D_t*)item;
}

RenderTexture2D_t* RenderTexture2D_tArray_GetNextFree(RenderTexture2D_tArray* array)
{
	RenderTexture2D_t* result = (RenderTexture2D_t*)array->nextfree;

	if (result == NULL)
	{
		int index = array->length;
		array->length++;
		result = RenderTexture2D_tArray_Get(array, index);
		result->nextfree = NULL;
		result->instanceId++;
	}
	else
	{
		array->nextfree = result->nextfree;
		result->nextfree = NULL;
		result->instanceId++;
		if (result->entityId >= array->length)
		{
			// Typically wont' happen, but if it does we wannt know.
			array->length = result->entityId;
#ifdef _WIN32
			__debugbreak();
#endif
		}
	}
	return result;
}


dayscorecard_t* dayscorecard_tArray_Get(dayscorecard_tArray* array, int32_t index)
{
	if (index >= 0 && index < array->length && index < array->capacity)
	{
		return &array->items[index];
	}
	else
	{
		// Oh boy you messed up. You landed here. This means that you could be corrupting memory.
#ifdef _WIN32
		__debugbreak();
#endif
	}
	return NULL;
}

void dayscorecard_tArray_Alloc(dayscorecard_tArray* array)
{
	array->items = (dayscorecard_t*)calloc(array->capacity, sizeof(dayscorecard_t));
}

void dayscorecard_tArray_AllocArena(Arena* arena, dayscorecard_tArray* array)
{
	array->items = (dayscorecard_t*)Arena_Alloc(arena, (array->capacity * sizeof(dayscorecard_t)));
}

void dayscorecard_tArray_ReallocArena(Arena* arena, dayscorecard_tArray* array, int32_t newCnt)
{
	dayscorecard_t* oldData = array->items;
	array->items = (dayscorecard_t*)Arena_Realloc(arena, array, (array->capacity * sizeof(dayscorecard_t)), (newCnt * sizeof(dayscorecard_t)));
	for (int i = 0; i < array->capacity; i++)
	{
		memcpy(array->items + i, oldData + i, sizeof(dayscorecard_t));
	}
	array->capacity = newCnt;
}

dayscorecard_tArray* dayscorecard_tArray_CreateArena(Arena* arena, int32_t capacity)
{
	dayscorecard_tArray* result = (dayscorecard_tArray*)Arena_Alloc(arena, sizeof(dayscorecard_tArray));
	result->capacity = capacity;
	result->length = 0;
	result->nextfree = NULL;
	dayscorecard_tArray_AllocArena(arena, result);
	return result;
}

void dayscorecard_tArray_FreeItem(dayscorecard_tArray* array, dayscorecard_t* item)
{
	item->bActive = false;
	item->nextfree = array->nextfree;
	array->nextfree = (struct dayscorecard_t*)item;
}

dayscorecard_t* dayscorecard_tArray_GetNextFree(dayscorecard_tArray* array)
{
	dayscorecard_t* result = (dayscorecard_t*)array->nextfree;

	if (result == NULL)
	{
		int index = array->length;
		array->length++;
		result = dayscorecard_tArray_Get(array, index);
		result->nextfree = NULL;
		result->instanceId++;
	}
	else
	{
		array->nextfree = result->nextfree;
		result->nextfree = NULL;
		result->instanceId++;
		if (result->entityId >= array->length)
		{
			// Typically wont' happen, but if it does we wannt know.
			array->length = result->entityId;
#ifdef _WIN32
			__debugbreak();
#endif
		}
	}
	return result;
}

