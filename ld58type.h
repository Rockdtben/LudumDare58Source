#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "ggarena.h"
#include "raylib.h"

typedef struct RenderTexture2D_s {
	int entityId;
	bool bActive;
	int instanceId;
	RenderTexture2D renderTexture;
	struct RenderTexture2D_t* nextfree;
} RenderTexture2D_t;

typedef struct dayscorecard_s {
	int entityId;
	bool bActive;
	int instanceId;
	int incorrectFlight;
	int happyCustomer;
	int noTicketGiven;
	// lose 2 points
	int downgraded;
	// gain 1 point
	int upgraded;
	struct dayscorecard_t* nextfree;
} dayscorecard_t;


typedef struct timeofday_s {
	int HH;
	int MM;
} timeofday_t;

typedef struct flight_s {
	int entityId;
	int instanceId;
	bool bActive;
	int flightNumber;
	int srcId;
	int destId;
	int takeoffEst;
	int takeoffActual;
	int arrivalEst;
	int arrivalActual;

	int occupiedSeats;
	int maxSeats;
	int occupiedFirstClass;
	int maxFirstClass;


	struct flight_t* nextfree;
} flight_t;

typedef struct passenger_s {
	int entityId;
	int instanceId;
	bool bActive;
	int ticketId;
	int ticketLayoverId;
	// 0 male 1 female
	bool gender; 
	char* firstname;
	char* lastname;
	int srcId;
	int layoverId;
	int destId;

	bool ticketGiven;
	bool ticketReturned;

	bool printedTicketGiven;
	bool printedTicketReturned;
	int printedTicketId;

	bool idGiven;
	bool idReturned;

	bool bCompleted;
	Color shirtColor;

	struct passenger_t* nextfree;
} passenger_t;

typedef struct ticket_s {
	int entityId;
	int instanceId;
	bool bActive;
	int srcId;
	int destId;
	int flightNum;
	int depart;
	int arrival;
	bool fakeTicket;
	char* firstname;
	char* lastname;
	bool firstClass;
	int Month;
	int Day;
	int Year;
	int ticketError;

	int ticketType;

	// Ticket is on the screen somewhere.
	bool bVisible;
	bool bThrown;
	bool bHeld;
	bool bBig;
	bool bReturning;
	bool bPrinting;
	Vector2 pos;
	Vector2 size;

	Vector2 heldOffset;
	Texture2D bigTexture;



	struct ticket_t* nextfree;
} ticket_t;


typedef struct airport_s {
	int entityId;
	int instanceId;
	bool bActive;
	bool bHub;
	char* airportcode; // TPA
	char* airportname;
	char* state;
	char* city;
	struct airport_t* nextfree;
} airport_t;




// GENERATED


typedef struct passenger_tArray
{
	int32_t length;
	int32_t capacity;
	passenger_t* items;
	struct passenger_t* nextfree;
} passenger_tArray;

typedef struct ticket_tArray
{
	int32_t length;
	int32_t capacity;
	ticket_t* items;
	struct ticket_t* nextfree;
} ticket_tArray;

typedef struct airport_tArray
{
	int32_t length;
	int32_t capacity;
	airport_t* items;
	struct airport_t* nextfree;
} airport_tArray;

typedef struct flight_tArray
{
	int32_t length;
	int32_t capacity;
	flight_t* items;
	struct flight_t* nextfree;
} flight_tArray;

typedef struct RenderTexture2D_tArray
{
	int32_t length;
	int32_t capacity;
	RenderTexture2D_t* items;
	struct RenderTexture2D_t* nextfree;
} RenderTexture2D_tArray;

typedef struct dayscorecard_tArray
{
	int32_t length;
	int32_t capacity;
	dayscorecard_t* items;
	struct dayscorecard_t* nextfree;
} dayscorecard_tArray;


// END GENERATED

typedef enum ticket_error_s {
	TICKET_ERROR_NONE,
	TICKET_ERROR_WRONGDATE,
	num_of_ticketErrors
} ticket_error_s;



typedef enum comp_screen_s {
	COMPSCREEN_MAINMENU,
	COMPSCREEN_FLIGHTCHOOSER_PICKAIRPORTS,
	COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT_LOADING,
	COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT,
	COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT_PRINTING,
	COMPSCREEN_FLIGHTCHOOSER_SECURITY_LOADING,
	COMPSCREEN_FLIGHTCHOOSER_SECURITY,
	COMPSCREEN_FLIGHTCHOOSER_ENDOFSHIFT,
	COMPSCREEN_SHIFTREVIEW,
	num_of_compscreens
} comp_screen_s;

typedef enum tickettype_s {
	TICKETTYPE_ORIGINAL,
	TICKETTYPE_PRINTED,
	num_of_tickettypes
} tickettype_s;

typedef enum gamescreen_s {
	GAMESCREEN_MAINMENU,
	GAMESCREEN_GAME,
	GAMESCREEN_CREDITS,
	GAMESCREEN_GAMEOVER
} gamescreen_s;


typedef struct ld_gamestate_s{
	Arena* memArena;
	ticket_tArray* tickets;
	passenger_tArray* passengers;
	airport_tArray* airports;
	flight_tArray* flights;
	RenderTexture2D_tArray* ticketRenderTextures;
	dayscorecard_tArray* dayscorecards;

	int mainGameScreen;
	passenger_t* lastPassenger;
	passenger_t* currentPassenger;
	int currentPassengerIndex;
	
	airport_t* fromAirport;
	airport_t* toAirport;
	int compScreen;

	float chooseflightTime;
	float chooseflightTimer;
	int gameTime;

	int Month;
	int Day;
	int Year;


	Sound bellSound;
	Sound printerSound;
	Texture2D bellTexture;
	Texture2D ticketTexture;
	Texture2D bodytexture;

	bool firstClassSelected;
	int flightIdSelected;

	bool dayCompleted;
	bool bStartNextDay;
	float startNextDayTimer;
	float startNextDayTime;
	int day;

	float showBellTooltipTime;
	float showBellTooltipTimer;
	bool clickedBellOnce;
	
} ld_gamestate_t;