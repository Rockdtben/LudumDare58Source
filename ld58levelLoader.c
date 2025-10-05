#include "ld58typeArray.h"
#include "ld58levelLoader.h"
#include "raylib.h"
#include <stdio.h>



char* firstname[10];
char* firstnameFemale[10];
char* lastname[10];

void ld58_Init(Arena* memArena)
{
	for (int i = 0; i < 10; i++)
	{
		firstname[i] = Arena_Alloc(memArena, sizeof(char) * 50);
		firstnameFemale[i] = Arena_Alloc(memArena, sizeof(char) * 50);
		lastname[i] = Arena_Alloc(memArena, sizeof(char) * 50);
	}

	sprintf(firstname[0], "%s", "Bob");
	sprintf(firstname[1], "%s", "Jack");
	sprintf(firstname[2], "%s", "John");
	sprintf(firstname[3], "%s", "Ben");
	sprintf(firstname[4], "%s", "Tony");
	sprintf(firstname[5], "%s", "Riker");
	sprintf(firstname[6], "%s", "Quinn");
	sprintf(firstname[7], "%s", "Roger");
	sprintf(firstname[8], "%s", "Mike");
	sprintf(firstname[9], "%s", "Peter");

	sprintf(firstnameFemale[0], "%s", "Susan");
	sprintf(firstnameFemale[1], "%s", "Jill");
	sprintf(firstnameFemale[2], "%s", "Stacy");
	sprintf(firstnameFemale[3], "%s", "Tina");
	sprintf(firstnameFemale[4], "%s", "Autumn");
	sprintf(firstnameFemale[5], "%s", "Gloria");
	sprintf(firstnameFemale[6], "%s", "Anne");
	sprintf(firstnameFemale[7], "%s", "Charlette");
	sprintf(firstnameFemale[8], "%s", "Lauren");
	sprintf(firstnameFemale[9], "%s", "Pauline");

	sprintf(lastname[0], "%s", "Smith");
	sprintf(lastname[1], "%s", "Washington");
	sprintf(lastname[2], "%s", "Shoemaker");
	sprintf(lastname[3], "%s", "Doe");
	sprintf(lastname[4], "%s", "Rogers");
	sprintf(lastname[5], "%s", "Swink");
	sprintf(lastname[6], "%s", "Collins");
	sprintf(lastname[7], "%s", "Carpen");
	sprintf(lastname[8], "%s", "Santos");
	sprintf(lastname[9], "%s", "Johnson");
}

ticket_t* GenerateTicketPrinted(ld_gamestate_t* gameStatePtr, passenger_t* passenger, flight_t* flight, bool firstClass)
{
	// What is the error on the ticket?


	ticket_t* ticket = ticket_tArray_GetNextFree(gameStatePtr->tickets);
	ticket->ticketError = TICKET_ERROR_NONE;
	ticket->ticketType = TICKETTYPE_PRINTED;
	ticket->bPrinting = true;
	ticket->Month = gameStatePtr->Month;
	ticket->Day = gameStatePtr->Day;
	ticket->Year = gameStatePtr->Year;
	ticket->pos = (Vector2){ 500 + 60 - 40,390-50 };
	ticket->size = (Vector2){ 80,50 }; //320
	ticket->bVisible = true;
	sprintf(ticket->firstname, "%s", passenger->firstname);
	sprintf(ticket->lastname, "%s", passenger->lastname);

	ticket->srcId = flight->srcId;
	ticket->destId = flight->destId;

	if (flight != NULL)
	{
		ticket->flightNum = flight->flightNumber;
		ticket->firstClass = firstClass;
		ticket->depart = flight->takeoffEst;
		ticket->arrival = flight->arrivalEst;
		printf("TICKET: FL(%d) FC(%d) %d->%d %02d-%02d-%d \n", ticket->flightNum, ticket->firstClass, ticket->srcId, ticket->destId, ticket->Month, ticket->Day, ticket->Year);
	}
	else
	{
		// Cry

		airport_t* fromAirport = airport_tArray_Get(gameStatePtr->airports, ticket->srcId);
		airport_t* toAirport = airport_tArray_Get(gameStatePtr->airports, ticket->destId);
		printf("WRONGTICKET: FL(%d) FC(%d) %d->%d %02d-%02d-%d \n", ticket->flightNum, ticket->firstClass, ticket->srcId, ticket->destId, ticket->Month, ticket->Day, ticket->Year);
		printf("WRONG: %s to %s\n", fromAirport->airportcode, toAirport->airportcode);
	}

	return ticket;
}



ticket_t* GenerateTicket(ld_gamestate_t* gameStatePtr, passenger_t* passenger, bool bLayover, bool hasError)
{
	// What is the error on the ticket?


	ticket_t* ticket = ticket_tArray_GetNextFree(gameStatePtr->tickets);
	ticket->ticketError = TICKET_ERROR_NONE;
	ticket->ticketType = TICKETTYPE_ORIGINAL;
	ticket->bActive = false;
	ticket->instanceId++;
	ticket->nextfree = NULL;
	ticket->bVisible = false;
	ticket->bThrown = false;
	ticket->bHeld = false;
	ticket->bBig = false;
	ticket->bReturning = false;
	ticket->bPrinting = false;


	if (bLayover)
	{
		passenger->ticketLayoverId = ticket->entityId;
	}
	else
	{
		passenger->ticketId = ticket->entityId;
	}
	

	ticket->Month = gameStatePtr->Month;
	ticket->Day = gameStatePtr->Day;
	ticket->Year= gameStatePtr->Year;
	ticket->pos = (Vector2){ 250-40,250 };
	ticket->size = (Vector2){ 80,50 }; //320
	sprintf(ticket->firstname, "%s", passenger->firstname);
	sprintf(ticket->lastname, "%s", passenger->lastname);

	if (bLayover)
	{
		ticket->srcId = passenger->layoverId;
		ticket->destId = passenger->destId;
		printf("layover");
	}
	else
	{
		if (passenger->layoverId != -1)
		{
			ticket->srcId = passenger->srcId;
			ticket->destId = passenger->layoverId;
			printf("layover-flight");
		}
		else
		{
			ticket->srcId = passenger->srcId;
			ticket->destId = passenger->destId;
			printf("regular-flight");
		}
		
	}

	

	// Must use flights with 5 hrs before start of last flight of day? End of day.
	flight_t* flight = flight_tArray_GetFlightBeforeTime(gameStatePtr->flights, (timeofday_t) { 6, 0 }, ticket->srcId, ticket->destId);		
	if (flight != NULL)
	{
		ticket->flightNum = flight->flightNumber;
		int firstClass = GetRandomValue(0, 10);
		ticket->firstClass = firstClass == 10;
		ticket->depart = flight->takeoffEst;
		ticket->arrival = flight->arrivalEst;
		printf("TICKET: FL(%d) FC(%d) %d->%d %02d-%02d-%d \n", ticket->flightNum, ticket->firstClass, ticket->srcId, ticket->destId, ticket->Month, ticket->Day, ticket->Year);
	}
	else
	{
		// Cry

		airport_t* fromAirport = airport_tArray_Get(gameStatePtr->airports, ticket->srcId);
		airport_t* toAirport = airport_tArray_Get(gameStatePtr->airports, ticket->destId);
		printf("WRONGTICKET: FL(%d) FC(%d) %d->%d %02d-%02d-%d \n", ticket->flightNum, ticket->firstClass, ticket->srcId, ticket->destId, ticket->Month, ticket->Day, ticket->Year);
		printf("WRONG: %s to %s\n", fromAirport->airportcode, toAirport->airportcode);
	}

	

	  
	return ticket;
}
flight_t* GenerateFlightRebooker(ld_gamestate_t* gameStatePtr, char* sourceCode, char* targetCode, timeofday_t depart, timeofday_t arrive)
{
	flight_t* flight = flight_tArray_GetNextFree(gameStatePtr->flights);
	flight->bActive = true;
	flight->takeoffEst = ConvertTimeOfDayToInt(depart);
	flight->arrivalEst = ConvertTimeOfDayToInt(arrive);

	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 3;
	flight->occupiedSeats = 47;
	flight->maxSeats = 50;

	flight->srcId = airport_tArray_GetByCode(gameStatePtr->airports, sourceCode);
	flight->destId = airport_tArray_GetByCode(gameStatePtr->airports, targetCode);

	// Generate a random number between 10000, 50000
	// Do not Collide.
	int flightNum = GetRandomValue(10000, 50000);

	while (true)
	{
		bool unique = true;
		for (int i = 0; i < gameStatePtr->flights->length; i++)
		{
			flight_t* checkFlight = flight_tArray_Get(gameStatePtr->flights, i);

			if (checkFlight->flightNumber == flightNum)
			{
				printf("COLLISION IN FLIGHT NUMBERS\n");
				unique = false;
				break;
			}
		}

		if (unique) break;

		flightNum = GetRandomValue(10000, 50000);
	}

	flight->flightNumber = flightNum;

	printf("FLIGHT(%d): %s(%d)->%s(%d) %d -> %d\n", flight->flightNumber, sourceCode, flight->srcId, targetCode, flight->destId, flight->takeoffEst, flight->arrivalEst);

	return flight;
}
flight_t* GenerateFlight(ld_gamestate_t* gameStatePtr, char* sourceCode, char* targetCode, timeofday_t depart, timeofday_t arrive)
{
	flight_t* flight = flight_tArray_GetNextFree(gameStatePtr->flights);
	flight->bActive = true;
	flight->takeoffEst = ConvertTimeOfDayToInt(depart);
	flight->arrivalEst = ConvertTimeOfDayToInt(arrive);

	flight->srcId = airport_tArray_GetByCode(gameStatePtr->airports, sourceCode);
	flight->destId = airport_tArray_GetByCode(gameStatePtr->airports, targetCode);

	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 4;
	flight->occupiedSeats = 47;
	flight->maxSeats = 50;

	// Generate a random number between 10000, 50000
	// Do not Collide.
	int flightNum = GetRandomValue(10000, 50000);

	while (true)
	{
		bool unique = true;
		for (int i = 0; i < gameStatePtr->flights->length; i++)
		{
			flight_t* checkFlight = flight_tArray_Get(gameStatePtr->flights, i);

			if (checkFlight->flightNumber == flightNum)
			{
				printf("COLLISION IN FLIGHT NUMBERS\n");
				unique = false;
				break;
			}
		}

		if (unique) break;

		flightNum = GetRandomValue(10000, 50000);
	}

	flight->flightNumber = flightNum;

	printf("FLIGHT(%d): %s(%d)->%s(%d) %d -> %d\n", flight->flightNumber, sourceCode, flight->srcId, targetCode, flight->destId, flight->takeoffEst, flight->arrivalEst);

	return flight;
}


flight_t* GenerateFlightEarly(ld_gamestate_t* gameStatePtr, char* sourceCode, char* targetCode, timeofday_t depart, timeofday_t arrive)
{
	flight_t* flight = flight_tArray_GetNextFree(gameStatePtr->flights);
	flight->bActive = true;
	flight->takeoffEst = ConvertTimeOfDayToInt(depart);
	flight->arrivalEst = ConvertTimeOfDayToInt(arrive);

	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 4;
	flight->occupiedSeats = 47;
	flight->maxSeats = 50;

	flight->srcId = airport_tArray_GetByCode(gameStatePtr->airports, sourceCode);
	flight->destId = airport_tArray_GetByCode(gameStatePtr->airports, targetCode);

	// Generate a random number between 10000, 50000
	// Do not Collide.
	int flightNum = GetRandomValue(10000, 50000);

	while (true)
	{
		bool unique = true;
		for (int i = 0; i < gameStatePtr->flights->length; i++)
		{
			flight_t* checkFlight = flight_tArray_Get(gameStatePtr->flights, i);

			if (checkFlight->flightNumber == flightNum)
			{
				printf("COLLISION IN FLIGHT NUMBERS\n");
				unique = false;
				break;
			}
		}

		if (unique) break;

		flightNum = GetRandomValue(10000, 50000);
	}

	flight->flightNumber = flightNum;

	printf("FLIGHT(%d): %s(%d)->%s(%d) %d -> %d\n", flight->flightNumber, sourceCode, flight->srcId, targetCode, flight->destId, flight->takeoffEst, flight->arrivalEst);

	return flight;
}

Color GenerateShirtColor()
{
	int rand = GetRandomValue(0, 16);
	if (rand == 0) return YELLOW;
	else if (rand == 1) return GOLD;
	else if (rand == 2) return ORANGE;
	else if (rand == 3) return RED;
	else if (rand == 4) return MAROON;
	else if (rand == 5) return GREEN;
	else if (rand == 6) return LIME;
	else if (rand == 7) return DARKGREEN;
	else if (rand == 8) return MAGENTA;
	else if (rand == 9) return DARKBLUE;
	else if (rand == 10) return PURPLE;
	else if (rand == 11) return VIOLET;
	else if (rand == 12) return DARKPURPLE;
	else if (rand == 13) return BEIGE;
	else if (rand == 14) return BROWN;
	else if (rand == 15) return DARKBROWN;
	else if (rand == 16) return BLACK;
	
	return BLACK;
}

passenger_t* GeneratePassenger(ld_gamestate_t* gameStatePtr)
{
	passenger_t* passenger = passenger_tArray_GetNextFree(gameStatePtr->passengers);
	passenger->bActive = true;
	passenger->gender = GetRandomValue(0, 1);
	passenger->layoverId = -1;
	passenger->ticketLayoverId = -1;
	passenger->bActive = false;
	passenger->instanceId++;
	passenger->nextfree = NULL;

	passenger->ticketId = -1;
	passenger->ticketGiven = false;
	passenger->ticketReturned = false;

	passenger->printedTicketGiven = false;
	passenger->printedTicketReturned = false;
	passenger->printedTicketId = -1;

	passenger->idGiven = false;
	passenger->idReturned = false;

	passenger->bCompleted = false;
	
	passenger->shirtColor = GenerateShirtColor();
	int first = GetRandomValue(0, 9);
	if (passenger->gender)
	{
		passenger->firstname = firstnameFemale[first];
	}
	else
	{
		passenger->firstname = firstname[first];
	}
	
	passenger->lastname = lastname[GetRandomValue(0, 9)];	

	// Where are they heading?
	passenger->srcId = airport_tArray_GetByCode(gameStatePtr->airports, "TPA");
	airport_t* srcAirport = airport_tArray_Get(gameStatePtr->airports, passenger->srcId);
	airport_t* destAirport = airport_tArray_GetDestinationAirport(gameStatePtr->airports);
	airport_t* layoverAirport;
	passenger->destId = destAirport->entityId;

	if (destAirport->state[0] != 'F' && !util_stringMatch(destAirport->airportcode, "DFW"))
	{

		layoverAirport = airport_tArray_GetDestinationHub(gameStatePtr->airports, passenger->destId);
		passenger->layoverId = layoverAirport->entityId;
	}
	else
	{
		passenger->layoverId = -1; 
	}

	char* test = "N/A";
	if (passenger->layoverId != -1)
	{
		test = layoverAirport->airportcode;
	}

	printf("Passenger: %s %s %d %d %s %d %s %d %s %s\n", passenger->firstname, passenger->lastname, passenger->gender, passenger->srcId, srcAirport->airportcode, passenger->layoverId , test, passenger->destId, destAirport->airportcode, destAirport->state);

	return passenger;
}

void ld58_ClearLevel(ld_gamestate_t* gameStatePtr)
{
	gameStatePtr->currentPassengerIndex = 0;
	gameStatePtr->currentPassenger = NULL;
	gameStatePtr->passengers->length = 0;
	gameStatePtr->flights->length = 0;
	gameStatePtr->tickets->length = 0;
	
}

void ld58_LoadLevel1(ld_gamestate_t* gameStatePtr)
{
	ld58_ClearLevel(gameStatePtr);

	gameStatePtr->Day = 10;


	gameStatePtr->day = 1;

	for (int i = 0; i < gameStatePtr->dayscorecards->length; i++)
	{
		dayscorecard_t* scorecard = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, i);
		scorecard->downgraded = 0;
		scorecard->upgraded = 0;
		scorecard->incorrectFlight = 0;
		scorecard->happyCustomer = 0;
		scorecard->noTicketGiven = 0;
	}

	gameStatePtr->gameTime = (60 * 60 * 6) + (60 * 15);

	flight_t* flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 12, 30 }, (timeofday_t) { 15, 30 });

	// EARLY FLIGHTS
	flight = GenerateFlightEarly(gameStatePtr, "TPA", "FLL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight = GenerateFlightEarly(gameStatePtr, "TPA", "EYW", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight = GenerateFlightEarly(gameStatePtr, "TPA", "GNV", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight = GenerateFlightEarly(gameStatePtr, "DFW", "GLS", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight = GenerateFlightEarly(gameStatePtr, "DFW", "GRK", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight = GenerateFlightEarly(gameStatePtr, "DFW", "HRL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });

	int startingPoint = gameStatePtr->flights->length;


	/*for (int i = 0; i < startingPoint; i++)
	{
		flight_t* sourceFlight = gameStatePtr->fligth
	}*/

	flight = GenerateFlightRebooker(gameStatePtr, "TPA", "DFW", (timeofday_t) { 9, 00 }, (timeofday_t) { 12, 00 });
	flight = GenerateFlightRebooker(gameStatePtr, "TPA", "DFW", (timeofday_t) { 9, 00 }, (timeofday_t) { 12, 00 });
	flight = GenerateFlightRebooker(gameStatePtr, "TPA", "DFW", (timeofday_t) { 16, 00 }, (timeofday_t) { 19, 00 });
	flight = GenerateFlightRebooker(gameStatePtr, "TPA", "FLL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight = GenerateFlightRebooker(gameStatePtr, "TPA", "EYW", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight = GenerateFlightRebooker(gameStatePtr, "TPA", "GNV", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight = GenerateFlightRebooker(gameStatePtr, "DFW", "GLS", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight = GenerateFlightRebooker(gameStatePtr, "DFW", "GRK", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight = GenerateFlightRebooker(gameStatePtr, "DFW", "HRL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });

		


	for (int i = 0; i < 6; i++)
	{
		passenger_t* passenger = GeneratePassenger(gameStatePtr);
		GenerateTicket(gameStatePtr, passenger, false, false);
		if (passenger->layoverId != -1) GenerateTicket(gameStatePtr, passenger, true, false);
	}
}


void ld58_LoadLevel2(ld_gamestate_t* gameStatePtr)
{
	ld58_ClearLevel(gameStatePtr);

	gameStatePtr->Day = 11;

	gameStatePtr->gameTime = (60 * 60 * 6) + (60 * 15);

	flight_t* flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 12, 30 }, (timeofday_t) { 15, 30 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 50;


	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 4, 30 }, (timeofday_t) { 7, 30 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 50;

	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 9, 00 }, (timeofday_t) { 12, 00 });
	flight->occupiedFirstClass = 4;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 50;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 16, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "FLL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	flight = GenerateFlight(gameStatePtr, "TPA", "EYW", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "GNV", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "DFW", "GLS", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "DFW", "GRK", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;




	flight = GenerateFlight(gameStatePtr, "DFW", "HRL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	// EARLY FLIGHTS

	flight = GenerateFlight(gameStatePtr, "TPA", "FLL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	flight = GenerateFlight(gameStatePtr, "TPA", "EYW", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "GNV", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "DFW", "GLS", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "DFW", "GRK", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;




	flight = GenerateFlight(gameStatePtr, "DFW", "HRL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;




	for (int i = 0; i < 10; i++)
	{
		passenger_t* passenger = GeneratePassenger(gameStatePtr);
		GenerateTicket(gameStatePtr, passenger, false, false);
		if (passenger->layoverId != -1) GenerateTicket(gameStatePtr, passenger, true, false);
	}
}


void ld58_LoadLevel3(ld_gamestate_t* gameStatePtr)
{
	ld58_ClearLevel(gameStatePtr);

	gameStatePtr->Day = 12;
	gameStatePtr->gameTime = (60 * 60 * 6) + (60 * 15);

	flight_t* flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 12, 30 }, (timeofday_t) { 15, 30 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 50;


	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 4, 30 }, (timeofday_t) { 7, 30 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 50;

	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 9, 00 }, (timeofday_t) { 12, 00 });
	flight->occupiedFirstClass = 4;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 50;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 16, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "FLL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	flight = GenerateFlight(gameStatePtr, "TPA", "EYW", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "GNV", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "DFW", "GLS", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "DFW", "GRK", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;




	flight = GenerateFlight(gameStatePtr, "DFW", "HRL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	// EARLY FLIGHTS

	flight = GenerateFlight(gameStatePtr, "TPA", "FLL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	flight = GenerateFlight(gameStatePtr, "TPA", "EYW", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "GNV", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "DFW", "GLS", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "DFW", "GRK", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;




	flight = GenerateFlight(gameStatePtr, "DFW", "HRL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;




	for (int i = 0; i < 15; i++)
	{
		passenger_t* passenger = GeneratePassenger(gameStatePtr);
		GenerateTicket(gameStatePtr, passenger, false, false);
		if (passenger->layoverId != -1) GenerateTicket(gameStatePtr, passenger, true, false);
	}
}
void ld58_LoadLevel4(ld_gamestate_t* gameStatePtr)
{
	ld58_ClearLevel(gameStatePtr);

	gameStatePtr->Day = 13;
	gameStatePtr->gameTime = (60 * 60 * 6) + (60 * 15);

	flight_t* flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 12, 30 }, (timeofday_t) { 15, 30 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 50;


	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 4, 30 }, (timeofday_t) { 7, 30 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 50;

	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 9, 00 }, (timeofday_t) { 12, 00 });
	flight->occupiedFirstClass = 4;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 50;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 16, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "FLL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	flight = GenerateFlight(gameStatePtr, "TPA", "EYW", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "GNV", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "DFW", "GLS", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "DFW", "GRK", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;




	flight = GenerateFlight(gameStatePtr, "DFW", "HRL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	// EARLY FLIGHTS

	flight = GenerateFlight(gameStatePtr, "TPA", "FLL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	flight = GenerateFlight(gameStatePtr, "TPA", "EYW", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "GNV", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "DFW", "GLS", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "DFW", "GRK", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;




	flight = GenerateFlight(gameStatePtr, "DFW", "HRL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;




	for (int i = 0; i < 15; i++)
	{
		passenger_t* passenger = GeneratePassenger(gameStatePtr);
		GenerateTicket(gameStatePtr, passenger, false, false);
		if (passenger->layoverId != -1) GenerateTicket(gameStatePtr, passenger, true, false);
	}
}
void ld58_LoadLevel5(ld_gamestate_t* gameStatePtr){
	ld58_ClearLevel(gameStatePtr);

	gameStatePtr->Day = 14;
	gameStatePtr->gameTime = (60 * 60 * 6) + (60 * 15);

	flight_t* flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 12, 30 }, (timeofday_t) { 15, 30 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 50;


	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 4, 30 }, (timeofday_t) { 7, 30 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 50;

	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 9, 00 }, (timeofday_t) { 12, 00 });
	flight->occupiedFirstClass = 4;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 50;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "TPA", "DFW", (timeofday_t) { 16, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "FLL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	flight = GenerateFlight(gameStatePtr, "TPA", "EYW", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "GNV", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "DFW", "GLS", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "DFW", "GRK", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;




	flight = GenerateFlight(gameStatePtr, "DFW", "HRL", (timeofday_t) { 12, 00 }, (timeofday_t) { 19, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	// EARLY FLIGHTS

	flight = GenerateFlight(gameStatePtr, "TPA", "FLL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;


	flight = GenerateFlight(gameStatePtr, "TPA", "EYW", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "TPA", "GNV", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	flight = GenerateFlight(gameStatePtr, "DFW", "GLS", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;


	flight = GenerateFlight(gameStatePtr, "DFW", "GRK", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->occupiedFirstClass = 5;
	flight->maxFirstClass = 5;
	flight->occupiedSeats = 48;
	flight->maxSeats = 50;




	flight = GenerateFlight(gameStatePtr, "DFW", "HRL", (timeofday_t) { 4, 00 }, (timeofday_t) { 7, 00 });
	flight->maxFirstClass = 5;
	flight->occupiedFirstClass = 5;
	flight->maxSeats = 50;
	flight->occupiedSeats = 48;

	for (int i = 0; i < 20; i++)
	{
		passenger_t* passenger = GeneratePassenger(gameStatePtr);
		GenerateTicket(gameStatePtr, passenger, false, false);
		if (passenger->layoverId != -1) GenerateTicket(gameStatePtr, passenger, true, false);
	}
}