#include "raylib.h"
#include "raymath.h"
#include "ld58typeArray.h"
#include <stdio.h>
#include "ld58levelLoader.h"
#include "ggarena.h"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

Vector2 mousePos;
int i = 0;
int j = 0;
int k = 0;
// Seconds in a day
int MidNightTime = 86400;

passenger_t* passenger;
airport_t* airport;
ticket_t* ticket;
flight_t* flight;

Rectangle itemDropZone = { 0, 0, 495, 300 };

void Init_First();
void Update(float deltaTime);
void Render(float deltaTime);
void GetNextPassenger();

bool ld_button(const char* text, int x, int y, int w, int h, Color color, bool disabled, bool selected);



//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
int screenWidth = 1000;
int screenHeight = 600;

Arena* memoryArena;
ld_gamestate_t* gameStatePtr;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void);     // Update and Draw one frame

//
//----------------------------------------------------------------------------------
// Program main entry point
///----------------------------------------------------------------------------------
int main()
{
	// Initialization
	//--------------------------------------------------------------------------------------
	InitWindow(screenWidth, screenHeight, "LudumDare58 - GareGamesInc");
	InitAudioDevice();


	Init_First();


	//	printf("\nDIRECTORY: %s\n\n", GetWorkingDirectory());


#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
	SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		UpdateDrawFrame();
	}
#endif

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}


int xPos = 5;
float deltaTime = 0.0f;


void ourGetFrameTime()
{
	deltaTime = GetFrameTime();
	if (deltaTime > 1.0f)
	{
		deltaTime = 1.0f / 60.0f;
	}
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void)
{
	ourGetFrameTime();


	// Update
	//----------------------------------------------------------------------------------


	Update(deltaTime);
	// Draw
	//----------------------------------------------------------------------------------
	Render(deltaTime);
	//----------------------------------------------------------------------------------
}

void UpdateGame(float deltaTime)
{
	if (gameStatePtr->clickedBellOnce)
	{
		gameStatePtr->gameTime += (deltaTime * 170);
	}	

	if (!gameStatePtr->clickedBellOnce)
	{
		gameStatePtr->showBellTooltipTimer += deltaTime;
	}

	if (gameStatePtr->currentPassenger == NULL)
	{
		for (i = 0; i < gameStatePtr->tickets->length; i++)
		{
			ticket_t* hideticket = ticket_tArray_Get(gameStatePtr->tickets, i);
			hideticket->bVisible = false;

		}
	}


	if (gameStatePtr->bStartNextDay)
	{
		gameStatePtr->clickedBellOnce = false;
		gameStatePtr->showBellTooltipTimer = 0.0f;

		if (gameStatePtr->day == 1) ld58_LoadLevel2(gameStatePtr);
		else if (gameStatePtr->day == 2) ld58_LoadLevel3(gameStatePtr);
		else if (gameStatePtr->day == 3) ld58_LoadLevel4(gameStatePtr);
		else if (gameStatePtr->day == 4) ld58_LoadLevel5(gameStatePtr);
		else if (gameStatePtr->day == 5)
		{
			gameStatePtr->mainGameScreen = GAMESCREEN_GAMEOVER;
			return;
		}

		gameStatePtr->bStartNextDay = false;
		gameStatePtr->dayCompleted = false;
		gameStatePtr->day++;
		gameStatePtr->compScreen = COMPSCREEN_MAINMENU;
	}


	if (gameStatePtr->gameTime == MidNightTime)
	{
		gameStatePtr->gameTime = 0;
	}

	if (gameStatePtr->gameTime >= ConvertTimeOfDayToInt((timeofday_t) { 18, 00 }))
	{
		gameStatePtr->dayCompleted = true;
		gameStatePtr->compScreen = COMPSCREEN_FLIGHTCHOOSER_ENDOFSHIFT;
		gameStatePtr->gameTime = ConvertTimeOfDayToInt((timeofday_t) { 18, 00 });
	}
	if (IsKeyDown(KEY_W))
	{

	}
	if (IsKeyDown(KEY_A))
	{
		xPos -= 5;
	}

	if (IsKeyDown(KEY_D))
	{
		xPos += 5;
	}

	if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT_LOADING
		|| gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_SECURITY_LOADING)
	{
		gameStatePtr->chooseflightTimer += deltaTime;

		if (gameStatePtr->chooseflightTimer > gameStatePtr->chooseflightTime)
		{
			if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_SECURITY_LOADING)
			{
				gameStatePtr->compScreen = COMPSCREEN_FLIGHTCHOOSER_SECURITY;
				gameStatePtr->chooseflightTimer = 0.0f;
			}
			if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT_LOADING)
			{
				gameStatePtr->compScreen = COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT;
				gameStatePtr->chooseflightTimer = 0.0f;
			}
		}
	}

	for (i = 0; i < gameStatePtr->tickets->length; i++)
	{
		ticket_t* ticket = ticket_tArray_Get(gameStatePtr->tickets, i);

		if (!ticket->bVisible) continue;

		if (ticket->bThrown)
		{
			ticket->pos.y += 500 * deltaTime;

			if (ticket->pos.y >= 350)
			{
				ticket->bThrown = false;
			}
		}

		if (ticket->bPrinting)
		{
			ticket->pos.y += 25 * deltaTime;

			if (ticket->pos.y >= 400)
			{
				ticket->bPrinting = false;

				gameStatePtr->compScreen = COMPSCREEN_MAINMENU;
			}
		}

		if (ticket->bReturning)
		{

			Vector2 passengerCenter = (Vector2){ 250 - 50 , 175 - 25 };
			Vector2 towardsCircle = Vector2Subtract(passengerCenter, ticket->pos);
			float len = Vector2Length(towardsCircle);
			if (len < 10)
			{
				ticket->bVisible = false;
				if (ticket->ticketType == TICKETTYPE_ORIGINAL)
				{
					gameStatePtr->currentPassenger->ticketReturned = true;
				}
				else if (ticket->ticketType == TICKETTYPE_PRINTED)
				{
					gameStatePtr->currentPassenger->printedTicketReturned = true;
					gameStatePtr->currentPassenger->printedTicketId = ticket->entityId;
				}

			}
			else
			{
				towardsCircle = Vector2Normalize(towardsCircle);
				towardsCircle = Vector2Scale(towardsCircle, 500 * deltaTime);
				ticket->pos = Vector2Add(ticket->pos, towardsCircle);
			}
		}

		if (!ticket->bThrown && !ticket->bPrinting)
		{
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, (Rectangle) { ticket->pos.x, ticket->pos.y, ticket->size.x, ticket->size.y }))
			{
				ticket->bHeld = true;
				ticket->heldOffset = Vector2Subtract(mousePos, ticket->pos);
			}

			if (ticket->bBig && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, (Rectangle) { ticket->pos.x, ticket->pos.y, ticket->size.x * 3, ticket->size.y * 3 }))
			{
				ticket->bHeld = true;
				ticket->heldOffset = Vector2Subtract(mousePos, ticket->pos);
			}

			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			{
				if (ticket->bHeld)
				{
					ticket->bHeld = false;

					if (mousePos.y <= 300)
					{
						ticket->bThrown = true;
					}

					if (CheckCollisionPointRec(mousePos, itemDropZone))
					{
						// Return item to passenger.
						ticket->bReturning = true;
						ticket->bThrown = false;
						ticket->bBig = false;
					}
				}
			}

			if (ticket->bHeld)
			{

				if (mousePos.x > 400)
				{
					if (!ticket->bBig)
					{
						ticket->bBig = true;
						ticket->heldOffset = Vector2Scale(ticket->heldOffset, 3.0f);
					}
				}
				else
				{
					if (ticket->bBig)
					{
						ticket->bBig = false;
						ticket->heldOffset = Vector2Scale(ticket->heldOffset, 1.0f / 3.0f);
					}
				}

				ticket->pos = Vector2Subtract(mousePos, ticket->heldOffset);
				if (ticket->pos.x < 0)
				{
					ticket->pos.x = 0;

				}
				if (ticket->pos.y < 0)
				{
					ticket->pos.y = 0;

				}

				// 1000, 600

				// big 240, 150
				// 80, 50

				if (ticket->bBig)
				{
					if (ticket->pos.x > 1000 - 240)
					{
						ticket->pos.x = 1000 - 240;
					}

					if (ticket->pos.y > 600 - 150)
					{
						ticket->pos.y = 600 - 150;
					}
				}
				else
				{
					if (ticket->pos.x > 1000 - 80)
					{
						ticket->pos.x = 1000 - 80;
					}

					if (ticket->pos.y > 600 - 50)
					{
						ticket->pos.y = 600 - 50;
					}
				}
			}
		}
	}
}

void Update(float deltaTime)
{
#ifdef _WIN32
	if (IsKeyPressed(KEY_F1))
	{
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

		gameStatePtr->mainGameScreen = GAMESCREEN_GAME;
		ld58_LoadLevel1(gameStatePtr);
	}

	if (IsKeyPressed(KEY_F2))
	{
		gameStatePtr->day = 2;

		for (int i = 0; i < gameStatePtr->dayscorecards->length; i++)
		{
			dayscorecard_t* scorecard = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, i);
			scorecard->downgraded = 0;
			scorecard->upgraded = 0;
			scorecard->incorrectFlight = 0;
			scorecard->happyCustomer = 0;
			scorecard->noTicketGiven = 0;
		}

		gameStatePtr->mainGameScreen = GAMESCREEN_GAME;
		ld58_LoadLevel2(gameStatePtr);
	}

	if (IsKeyPressed(KEY_F3))
	{
		gameStatePtr->day = 3;

		for (int i = 0; i < gameStatePtr->dayscorecards->length; i++)
		{
			dayscorecard_t* scorecard = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, i);
			scorecard->downgraded = 0;
			scorecard->upgraded = 0;
			scorecard->incorrectFlight = 0;
			scorecard->happyCustomer = 0;
			scorecard->noTicketGiven = 0;
		}

		gameStatePtr->mainGameScreen = GAMESCREEN_GAME;
		ld58_LoadLevel3(gameStatePtr);
	}

	if (IsKeyPressed(KEY_F4))
	{
		gameStatePtr->day = 4;

		for (int i = 0; i < gameStatePtr->dayscorecards->length; i++)
		{
			dayscorecard_t* scorecard = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, i);
			scorecard->downgraded = 0;
			scorecard->upgraded = 0;
			scorecard->incorrectFlight = 0;
			scorecard->happyCustomer = 0;
			scorecard->noTicketGiven = 0;
		}

		gameStatePtr->mainGameScreen = GAMESCREEN_GAME;
		ld58_LoadLevel4(gameStatePtr);
	}

	if (IsKeyPressed(KEY_F5))
	{
		gameStatePtr->day = 5;

		for (int i = 0; i < gameStatePtr->dayscorecards->length; i++)
		{
			dayscorecard_t* scorecard = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, i);
			scorecard->downgraded = 0;
			scorecard->upgraded = 0;
			scorecard->incorrectFlight = 0;
			scorecard->happyCustomer = 0;
			scorecard->noTicketGiven = 0;
		}

		gameStatePtr->mainGameScreen = GAMESCREEN_GAME;
		ld58_LoadLevel5(gameStatePtr);
	}


	if (IsKeyPressed(KEY_F6))
	{
		dayscorecard_t* day1 = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, 0);
		dayscorecard_t* day2 = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, 1);
		dayscorecard_t* day3 = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, 2);
		dayscorecard_t* day4 = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, 3);
		dayscorecard_t* day5 = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, 4);

		day1->happyCustomer		= 10;
		day1->upgraded			= 2;
		day1->downgraded		= 1;		
		day1->noTicketGiven		= 5;
		day1->incorrectFlight	= 3;

		day2->happyCustomer = 1;
		day2->upgraded = 20;
		day2->downgraded = 11;
		day2->noTicketGiven = 3;
		day2->incorrectFlight = 1;

		day3->happyCustomer = 2;
		day3->upgraded = 7;
		day3->downgraded = 3;
		day3->noTicketGiven = 1;
		day3->incorrectFlight = 3;

		day4->happyCustomer = 15;
		day4->upgraded = 11;
		day4->downgraded = 12;
		day4->noTicketGiven = 51;
		day4->incorrectFlight = 13;

		day5->happyCustomer = 110;
		day5->upgraded = 21;
		day5->downgraded = 21;
		day5->noTicketGiven = 35;
		day5->incorrectFlight = 33;

		gameStatePtr->day = 5;
		

		gameStatePtr->mainGameScreen = GAMESCREEN_GAMEOVER;
		return;
	}
#endif

	mousePos = GetMousePosition();

	if (gameStatePtr->mainGameScreen == GAMESCREEN_GAME)
	{
		UpdateGame(deltaTime);
	}	
}


int calcScoreFromScorecard(dayscorecard_t* scorecard)
{
	return scorecard->happyCustomer - scorecard->incorrectFlight - scorecard->noTicketGiven - (scorecard->downgraded * 2) + (scorecard->upgraded * 2);
}

void RenderGameOver(float deltaTime)
{
	BeginDrawing();
	ClearBackground(SKYBLUE);

	int totalScore = 0;
	for (i = 0; i < gameStatePtr->day; i++)
	{
		dayscorecard_t* scorecardByDay = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, i);

		int score = calcScoreFromScorecard(scorecardByDay);
		totalScore += score;
		DrawText(TextFormat("Day %d: %d", i + 1, score), 300, 90 + (i * 15), 10, RED);
	}
	DrawText(TextFormat("Total Score:"), 510, 180, 10, RED);
	DrawText(TextFormat("%d", totalScore), 610, 180, 10, RED);

	DrawText("GAME OVER", 350, 50, 20, RED);



	if (ld_button("Play Again", 450, 265, 100, 30, RED, false, false))
	{
		gameStatePtr->mainGameScreen = GAMESCREEN_GAME;
		ld58_LoadLevel1(gameStatePtr);
	}

	if (ld_button("Main Menu", 450, 300, 100, 30, RED, false, false))
	{
		gameStatePtr->mainGameScreen = GAMESCREEN_MAINMENU;
	}

	EndDrawing();
}

void RenderCredits(float deltaTime)
{
	BeginDrawing();
	ClearBackground(SKYBLUE);

	DrawText("Producer: Gare Games Inc.",			450, 10, 12, RED);
	DrawText("Sound Art: Gare Games Inc.",			450, 120, 12, RED);
	DrawText("Graphic Art: Gare Games Inc.",		450, 140, 12, RED);
	DrawText("Gameplay Engineer: Gare Games Inc.",	450, 160, 12, RED);
	DrawText("Developer: Gare Games Inc.",			450, 180, 12, RED);
	DrawText("Publisher: Gare Games Inc.",			450, 200, 12, RED);

	DrawText("CREDITS", 450, 20, 20, RED);

	if (ld_button("Back", 450, 300, 100, 30, RED, false, false))
	{
		gameStatePtr->mainGameScreen = GAMESCREEN_MAINMENU;
	}

	EndDrawing();
}

void RenderMainMenu(float deltaTime)
{
	BeginDrawing();
	ClearBackground(SKYBLUE);

	DrawText("Flight Game Thingy", 450, 0, 20, RED);

	DrawText("MAIN MENU", 450, 100, 20, RED);

	if (ld_button("Start Game", 450, 265, 100, 30, RED, false, false))
	{
		gameStatePtr->mainGameScreen = GAMESCREEN_GAME;
		ld58_LoadLevel1(gameStatePtr);
	}


	if (ld_button("Credits", 450, 300, 100, 30, RED, false, false))
	{
		gameStatePtr->mainGameScreen = GAMESCREEN_CREDITS;
	}

	EndDrawing();
}

void RenderGame(float deltaTime)
{
	BeginDrawing();

	for (i = 0; i < gameStatePtr->tickets->length; i++)
	{
		ticket_t* ticket = ticket_tArray_Get(gameStatePtr->tickets, i);
		if (ticket->bVisible && ticket->bBig)
		{
			// 240, 150
			RenderTexture2D_t* renderT = RenderTexture2D_tArray_GetNextFree(gameStatePtr->ticketRenderTextures);
			BeginTextureMode(renderT->renderTexture);
			ClearBackground(WHITE);
			DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 80, 0, 240, 150 }, (Rectangle) { 0, 0, 240, 150 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
			airport = airport_tArray_Get(gameStatePtr->airports, ticket->destId);
			airport_t* srcAirport = airport_tArray_Get(gameStatePtr->airports, ticket->srcId);


			timeofday_t depart = ConvertIntToTimeOfDay(ticket->depart);
			timeofday_t arrive = ConvertIntToTimeOfDay(ticket->arrival);


			DrawText(TextFormat("~TPA~"), 170, 0, 20, BLACK);
			DrawText(TextFormat("%s %s", ticket->firstname, ticket->lastname), 50, 50, 14, BLACK);

			DrawText(TextFormat("%02d:%02d - %02d:%02d", depart.HH, depart.MM, arrive.HH, arrive.MM), 50, 70, 14, BLACK);

			DrawText(TextFormat("%s -> %s", srcAirport->airportcode, airport->airportcode), 110, 130, 20, BLACK);

			// Rebooked
			if (ticket->ticketType == TICKETTYPE_PRINTED)
			{
				DrawLine(75, 12, 120, 12, RED);
				DrawLine(75, 16, 120, 12, RED);
				DrawText(TextFormat("REBOOKED"), 80, 10, 21, RED);
			}
			// FC
			if (ticket->firstClass)
			{
				DrawText(TextFormat("FC"), 5, 130, 20, GREEN);
			}
			EndTextureMode();
			ticket->bigTexture = renderT->renderTexture.texture;
		}
	}



	ClearBackground(SKYBLUE);

	if (gameStatePtr->currentPassenger != NULL)
	{
		if (gameStatePtr->currentPassenger->gender)
		{
			DrawTexture(gameStatePtr->bodytexture, 150, 110, gameStatePtr->currentPassenger->shirtColor);
			DrawCircle(250, 175, 50, PINK);

		}
		else
		{
			DrawTexture(gameStatePtr->bodytexture, 150, 110, gameStatePtr->currentPassenger->shirtColor);
			DrawCircle(250, 175, 50, BLUE);

		}

	}


	// TableTop/Desk
	DrawRectangle(0, 300, 1000, 400, GRAY);



	DrawRectangle(398, 300, 4, 400, DARKBROWN);




	DrawTexturePro(gameStatePtr->bellTexture, (Rectangle) { 0, 0, 88, 64 }, (Rectangle) { 400, 300, 88, 64 }, (Vector2) { 0, 0 }, 0.0f, WHITE);

	Rectangle bellCol1 = { 410, 319, 68, 45 };
	Rectangle bellCol2 = { 427, 302, 30, 60 };
	/*DrawRectangleLinesEx(bellCol1, 1, RED);
	DrawRectangleLinesEx(bellCol2, 1, RED);*/

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		if (CheckCollisionPointRec(mousePos, bellCol1)
			|| CheckCollisionPointRec(mousePos, bellCol2))
		{
			PlaySound(gameStatePtr->bellSound);

			gameStatePtr->clickedBellOnce = true;
			GetNextPassenger();
		}
	}
	//Computer Monitor Border
	DrawRectangle(1000 - 500 - 5, 50, 500, 350, LIGHTGRAY);



	// Computer Screen
	DrawRectangle(1000 - 500 + 5, 60, 480, 250, DARKBLUE);


	// Current Passenger
	if (gameStatePtr->currentPassenger != NULL)
	{
		DrawText(gameStatePtr->currentPassenger->firstname, 0, 0, 5, RED);
		DrawText(gameStatePtr->currentPassenger->lastname, 0, 10, 5, RED);


		// Print Ticket Debug Data
		if (gameStatePtr->currentPassenger->ticketId != -1)
		{
			ticket = ticket_tArray_Get(gameStatePtr->tickets, gameStatePtr->currentPassenger->ticketId);
			ticket_t* layoverTicket = NULL;
			if (gameStatePtr->currentPassenger->layoverId != -1)
			{
				layoverTicket = ticket_tArray_Get(gameStatePtr->tickets, gameStatePtr->currentPassenger->ticketLayoverId);
			}

			DrawText(ticket->firstname, 60, 0, 5, BLUE);
			DrawText(ticket->lastname, 60, 10, 5, BLUE);

			if (ticket->srcId != -1)
			{
				airport = airport_tArray_Get(gameStatePtr->airports, ticket->srcId);
				DrawText(airport->airportname, 120, 0, 5, BLUE);
				DrawText(airport->airportcode, 120, 10, 5, BLUE);
			}


			if (layoverTicket != NULL)
			{
				airport = airport_tArray_Get(gameStatePtr->airports, layoverTicket->destId);
				DrawText(airport->airportname, 120, 20, 5, GREEN);
				DrawText(airport->airportcode, 120, 30, 5, GREEN);

				airport = airport_tArray_Get(gameStatePtr->airports, ticket->destId);
				DrawText(airport->airportname, 120, 40, 5, RED);
				DrawText(airport->airportcode, 120, 50, 5, RED);



			}
			else if (ticket->destId != -1)
			{
				airport = airport_tArray_Get(gameStatePtr->airports, ticket->destId);
				DrawText(airport->airportname, 120, 20, 5, RED);
				DrawText(airport->airportcode, 120, 30, 5, RED);
			}
		}
	}



	if (gameStatePtr->compScreen == COMPSCREEN_MAINMENU)
	{
		//DrawRectangle(1000 - 500 + 5, 60, 480, 250, DARKBLUE);

		if (ld_button("Flight Chooser", 510, 75, 85, 25, RED, false, false))
		{
			gameStatePtr->compScreen = COMPSCREEN_FLIGHTCHOOSER_PICKAIRPORTS;
		}

		//if (ld_button("Security", 610, 75, 85, 25, RED, false, false))
		//{
		//	gameStatePtr->compScreen = COMPSCREEN_FLIGHTCHOOSER_SECURITY_LOADING;
		//}
	}
	else if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_ENDOFSHIFT)
	{
		if (ld_button("End Shift", 510, 75, 85, 25, RED, false, false))
		{
			gameStatePtr->compScreen = COMPSCREEN_SHIFTREVIEW;
		}
	}
	else if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT_PRINTING)
	{
		gameStatePtr->toAirport = NULL;
		gameStatePtr->fromAirport = NULL;
		DrawText("PRINTING", 510, 250, 20, RED);
	}
	else if (gameStatePtr->compScreen == COMPSCREEN_SHIFTREVIEW)
	{
		DrawText("Daily Review", 610, 70, 20, RED);


		dayscorecard_t* scorecard = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, gameStatePtr->day - 1);

		DrawText(TextFormat("Happy Customers:"), 510, 90, 10, RED);
		DrawText(TextFormat("Incorrect Flight:"), 510, 105, 10, RED);
		DrawText(TextFormat("No Ticket Given: "), 510, 120, 10, RED);
		DrawText(TextFormat("Upgraded:        "), 510, 135, 10, RED);
		DrawText(TextFormat("Downgraded:      "), 510, 150, 10, RED);


		int startX = 620;
		DrawText(TextFormat("%d (+%d)", scorecard->happyCustomer, scorecard->happyCustomer), startX, 90, 10, RED);
		DrawText(TextFormat("%d (-%d)", scorecard->incorrectFlight, scorecard->incorrectFlight), startX, 105, 10, RED);
		DrawText(TextFormat("%d (-%d)", scorecard->noTicketGiven, scorecard->noTicketGiven), startX, 120, 10, RED);
		DrawText(TextFormat("%d (+%d)", scorecard->upgraded, scorecard->upgraded * 2), startX, 135, 10, RED);
		DrawText(TextFormat("%d (-%d)", scorecard->downgraded, scorecard->downgraded * 2), startX, 150, 10, RED);

		int scoreDay = calcScoreFromScorecard(scorecard);

		DrawText(TextFormat("Daily Score:"), 510, 165, 10, RED);
		DrawText(TextFormat("%d", scoreDay), startX, 165, 10, RED);

		int totalScore = 0;
		for (i = 0; i < gameStatePtr->day; i++)
		{
			dayscorecard_t* scorecardByDay = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, i);

			int score = calcScoreFromScorecard(scorecardByDay);
			totalScore += score;
			DrawText(TextFormat("Day %d: %d", i + 1, score), 710, 90 + (i * 15), 10, RED);
		}
		DrawText(TextFormat("Total Score:"), 510, 180, 10, RED);
		DrawText(TextFormat("%d", totalScore), startX, 180, 10, RED);

		if (ld_button("Next Day", 510, 275, 85, 25, RED, false, false))
		{
			gameStatePtr->bStartNextDay = true;
		}
	}
	else if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_PICKAIRPORTS)
	{
		j = 0;
		k = 0;
		for (i = 0; i < gameStatePtr->airports->length; i++)
		{

			airport = airport_tArray_Get(gameStatePtr->airports, i);
			bool disabled = false;

			if (gameStatePtr->fromAirport != NULL && airport->entityId == gameStatePtr->fromAirport->entityId)
			{
				disabled = true;
			}
			else if (gameStatePtr->toAirport != NULL && airport->entityId == gameStatePtr->toAirport->entityId)
			{
				disabled = true;
			}

			if (ld_button(airport->airportcode, 510 + (k * 40), 75 + (j * 30), 35, 25, RED, disabled, false))
			{
				printf("%s\n", airport->airportcode);

				if (gameStatePtr->fromAirport == NULL)
				{
					gameStatePtr->fromAirport = airport;
				}
				else if (gameStatePtr->toAirport == NULL)
				{
					gameStatePtr->toAirport = airport;
				}
			}


			k++;
			if ((i % 4) == 0 && i > 0)
			{
				j++;
				k = 0;
			}
		}

		if (gameStatePtr->fromAirport != NULL)
		{
			DrawText(gameStatePtr->fromAirport->airportcode, 510, 10, 5, BLUE);
		}

		if (gameStatePtr->toAirport != NULL)
		{
			DrawText(gameStatePtr->toAirport->airportcode, 510, 20, 5, BLUE);
		}

		if (ld_button("  <", 720, 75, 35, 25, RED, false, false))
		{
			if (gameStatePtr->toAirport != NULL)
			{
				gameStatePtr->toAirport = NULL;
			}
			else if (gameStatePtr->fromAirport != NULL)
			{
				gameStatePtr->fromAirport = NULL;
			}
			else
			{
				gameStatePtr->compScreen = COMPSCREEN_MAINMENU;
			}
		}

		bool confirmDisabled = true;
		if (gameStatePtr->toAirport != NULL && gameStatePtr->fromAirport != NULL)
		{
			confirmDisabled = false;
		}

		if (ld_button("Confirm", 510, 150, 50, 25, RED, confirmDisabled, false))
		{
			gameStatePtr->compScreen = COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT_LOADING;
		}

		if (ld_button("Cancel", 570, 150, 50, 25, RED, false, false))
		{
			gameStatePtr->toAirport = NULL;
			gameStatePtr->fromAirport = NULL;
			gameStatePtr->compScreen = COMPSCREEN_MAINMENU;
		}
	}
	else if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT_LOADING)
	{
		DrawText("LOADING", 510, 250, 20, RED);
		// Speed this up by getting upgrades.
	}
	else if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_SECURITY_LOADING)
	{
		DrawText("LOADING", 510, 250, 20, RED);
		// Speed this up by getting upgrades.
	}
	else if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_SECURITY)
	{
		if (ld_button("Proceed", 510, 275, 80, 25, RED, false, false))
		{

		}
		if (ld_button("Cancel", 600, 275, 50, 25, RED, false, false))
		{
			gameStatePtr->compScreen = COMPSCREEN_MAINMENU;
		}
	}
	else if (gameStatePtr->compScreen == COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT)
	{


		int actualI = 0;
		for (i = 0; i < gameStatePtr->flights->length; i++)
		{
			flight = flight_tArray_Get(gameStatePtr->flights, i);

			bool firstClassFull = flight->maxFirstClass == flight->occupiedFirstClass;
			bool coachFull = flight->maxSeats == flight->occupiedSeats;

			if (flight->destId == gameStatePtr->toAirport->entityId
				&& flight->srcId == gameStatePtr->fromAirport->entityId
				&& gameStatePtr->gameTime < (flight->takeoffEst - (60 * 15))
				&& (!firstClassFull || !coachFull)
				)
			{
				timeofday_t arrivalEst = ConvertIntToTimeOfDay(flight->arrivalEst);
				timeofday_t departEst = ConvertIntToTimeOfDay(flight->takeoffEst);

				DrawText(TextFormat("FL(%d) %02d:%02d TO %02d:%02d",
					flight->flightNumber,
					departEst.HH, departEst.MM,
					arrivalEst.HH, arrivalEst.MM
				),
					510 + 5, 75 + (actualI * 25) + 5, 5, WHITE);

				bool firstClassSelected = gameStatePtr->flightIdSelected == flight->entityId && gameStatePtr->firstClassSelected;
				bool coachSelected = gameStatePtr->flightIdSelected == flight->entityId && !gameStatePtr->firstClassSelected;



				if (ld_button(TextFormat("FC: %d/%d",
					flight->occupiedFirstClass, flight->maxFirstClass
				),
					650, 75 + (actualI * 25), 60, 20, RED, firstClassFull, firstClassSelected))
				{
					gameStatePtr->firstClassSelected = true;
					gameStatePtr->flightIdSelected = flight->entityId;
				}

				if (ld_button(TextFormat("St: %d/%d",
					flight->occupiedSeats, flight->maxSeats
				),
					715, 75 + (actualI * 25), 60, 20, RED, coachFull, coachSelected))
				{
					gameStatePtr->firstClassSelected = false;
					gameStatePtr->flightIdSelected = flight->entityId;
				}

				actualI++;
			}
			else
			{
				if (gameStatePtr->flightIdSelected == flight->entityId)
				{
					gameStatePtr->flightIdSelected = -1;
					gameStatePtr->firstClassSelected = false;
				}
			}
		}







		if (ld_button("Print Ticket", 510, 275, 80, 25, RED, gameStatePtr->flightIdSelected == -1 || gameStatePtr->currentPassenger == NULL, false))
		{
			gameStatePtr->compScreen = COMPSCREEN_FLIGHTCHOOSER_CHOOSEFLIGHT_PRINTING;
			flight = flight_tArray_Get(gameStatePtr->flights, gameStatePtr->flightIdSelected);
			GenerateTicketPrinted(gameStatePtr, gameStatePtr->currentPassenger, flight, gameStatePtr->firstClassSelected);
			PlaySound(gameStatePtr->printerSound);

			if (gameStatePtr->firstClassSelected)
			{
				flight->occupiedFirstClass++;
			}
			else
			{
				flight->occupiedSeats++;
			}


			gameStatePtr->currentPassenger->printedTicketGiven = true;
			gameStatePtr->flightIdSelected = -1;
			gameStatePtr->firstClassSelected = false;
		}
		if (ld_button("Cancel", 600, 275, 50, 25, RED, false, false))
		{
			gameStatePtr->compScreen = COMPSCREEN_FLIGHTCHOOSER_PICKAIRPORTS;
			gameStatePtr->flightIdSelected = -1;
			gameStatePtr->firstClassSelected = false;
		}
	}

	if (gameStatePtr->dayCompleted && gameStatePtr->compScreen != COMPSCREEN_FLIGHTCHOOSER_ENDOFSHIFT && gameStatePtr->compScreen != COMPSCREEN_SHIFTREVIEW)
	{
		if (ld_button("End Shift", 710, 275, 85, 25, RED, false, false))
		{
			gameStatePtr->compScreen = COMPSCREEN_SHIFTREVIEW;
		}
	}

	// Draw Clock
	int hours = (int)(gameStatePtr->gameTime / 60.0f / 60.0f);
	int minutes = (int)(((gameStatePtr->gameTime - (hours * 60 * 60)) / 60.0f));
	int seconds = (int)(((gameStatePtr->gameTime - (hours * 60 * 60) - (minutes * 60))));


	DrawText(TextFormat("%02d/%02d/%04d - ", gameStatePtr->Month, gameStatePtr->Day, gameStatePtr->Year), 830, 292, 16, BLACK);
	DrawText(TextFormat("%02d:%02d", hours, minutes), 940, 292, 16, BLACK);

	// Printer Hardware
	DrawRectangle(500, 385, 120, 10, DARKGRAY);
	DrawRectangle(505, 390, 110, 2, BLACK);





	for (i = 0; i < gameStatePtr->tickets->length; i++)
	{
		ticket_t* ticket = ticket_tArray_Get(gameStatePtr->tickets, i);
		if (!ticket->bVisible) continue;
		if (!ticket->bPrinting) continue;

		DrawRectangle(ticket->pos.x, ticket->pos.y, ticket->size.x, ticket->size.y, RED);
		if (ticket->firstClass && ticket->ticketType == TICKETTYPE_PRINTED)
		{
			DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 0, 50, 80, 50 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 80, 50 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
		}
		else if (ticket->firstClass)
		{
			DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 0, 100, 80, 50 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 80, 50 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
		}
		else if (ticket->ticketType == TICKETTYPE_PRINTED)
		{
			DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 0, 150, 80, 50 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 80, 50 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
		}
		else
		{
			DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 0, 0, 80, 50 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 80, 50 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
		}
		//DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 0, 0, 80, 50 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 80, 50 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
	}
	DrawRectangle(500, 325, 120, 60, LIGHTGRAY);
	DrawRectangle(500, 385, 120, 5, DARKGRAY);
	DrawRectangle(505, 390, 110, 2, BLACK);


	for (i = 0; i < gameStatePtr->tickets->length; i++)
	{
		ticket_t* ticket = ticket_tArray_Get(gameStatePtr->tickets, i);
		if (!ticket->bVisible) continue;
		if (ticket->bPrinting) continue;

		if (ticket->bBig)
		{
			DrawRectangle(ticket->pos.x, ticket->pos.y, ticket->size.x * 3, ticket->size.y * 3, RED);
			DrawTexturePro(ticket->bigTexture, (Rectangle) { 0, 0, 240, -150 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 240, 150 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
		}
		else
		{
			DrawRectangle(ticket->pos.x, ticket->pos.y, ticket->size.x, ticket->size.y, RED);
			if (ticket->firstClass && ticket->ticketType == TICKETTYPE_PRINTED)
			{
				DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 0, 50, 80, 50 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 80, 50 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
			}
			else if (ticket->firstClass)
			{
				DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 0, 100, 80, 50 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 80, 50 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
			}
			else if (ticket->ticketType == TICKETTYPE_PRINTED)
			{
				DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 0, 150, 80, 50 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 80, 50 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
			}
			else
			{
				DrawTexturePro(gameStatePtr->ticketTexture, (Rectangle) { 0, 0, 80, 50 }, (Rectangle) { (int)ticket->pos.x, (int)ticket->pos.y, 80, 50 }, (Vector2) { 0, 0 }, 0.0f, WHITE);
			}

		}
	}


	if (!gameStatePtr->clickedBellOnce && gameStatePtr->showBellTooltipTimer > gameStatePtr->showBellTooltipTime)
	{
		DrawText("Please Click the Bell", 10, 10, 24, RED);
	}



	EndDrawing();

	// TODO - Create a flight dataset
	// TODO - Show on Screen
	// TODO - Let player pick a flight to replace flight.
	// TODO - Print the flight
	// TODO - Generate random people based on existing flights at start of game?
	// TODO - Have 5 Days for Ludum Dare.
	// TODO - Have items that you can drag around screen.
	// TODO - Give Pplayer some 1st class vouchers for mad customers.
	// TODO - Lose Conditions would be getting fired. Get fired if you get 3 bad reviews in a day.
	// TODO - Allow for fake tickets. You lose if you get 3 scams happening.
	// TODO - Passengers walking into queue.
	// TODO - Bell button to queue next passenger.
	// TODO - Drag and drop stuff from left side to right side.



	for (i = 0; i < gameStatePtr->ticketRenderTextures->capacity; i++)
	{
		RenderTexture2D_t* renderTex = gameStatePtr->ticketRenderTextures->items + i;
		renderTex->bActive = false;
		renderTex->nextfree = NULL;
	}
	gameStatePtr->ticketRenderTextures->length = 0;
}

void Render(float deltaTime)
{
	if (gameStatePtr->mainGameScreen == GAMESCREEN_GAME)
	{
		RenderGame(deltaTime);
	}
	else if (gameStatePtr->mainGameScreen == GAMESCREEN_MAINMENU)
	{
		RenderMainMenu(deltaTime);
	}
	else if (gameStatePtr->mainGameScreen == GAMESCREEN_CREDITS)
	{
		RenderCredits(deltaTime);
	}
	else if (gameStatePtr->mainGameScreen == GAMESCREEN_GAMEOVER)
	{
		RenderGameOver(deltaTime);
	}
}


#define airportAmt 20
#define passengerAmt 50
#define ticketAmt 100
#define flightAmt 50



void Init_First()
{
	MidNightTime = 60 * 60 * 24;

	memoryArena = Arena_CreateArena(500000);

	gameStatePtr = Arena_Alloc(memoryArena, sizeof(ld_gamestate_t)); 

#if defined(PLATFORM_WEB)
	gameStatePtr->bellSound = LoadSound("resources/sounds/gamebell.wav");
	gameStatePtr->printerSound = LoadSound("resources/sounds/printer3.wav");
	gameStatePtr->bellTexture = LoadTexture("resources/bell.png");
	gameStatePtr->ticketTexture = LoadTexture("resources/ticket.png");
	gameStatePtr->bodytexture = LoadTexture("/resources/body.png");
#else
	gameStatePtr->bellSound = LoadSound("../resources/sounds/gamebell.wav");
	gameStatePtr->printerSound = LoadSound("../resources/sounds/printer3.wav");
	gameStatePtr->bellTexture = LoadTexture("../resources/bell.png");
	gameStatePtr->ticketTexture = LoadTexture("../resources/ticket.png");
	gameStatePtr->bodytexture = LoadTexture("../resources/body.png");
#endif

	gameStatePtr->mainGameScreen = GAMESCREEN_MAINMENU;
	gameStatePtr->Month = 9;
	gameStatePtr->Day = 10;
	gameStatePtr->Year = 1990;
	gameStatePtr->clickedBellOnce = false;
	gameStatePtr->showBellTooltipTimer = 0.0f;
	gameStatePtr->showBellTooltipTime = 1.5f;

	gameStatePtr->firstClassSelected = false;
	gameStatePtr->flightIdSelected = -1;
	gameStatePtr->dayCompleted = false;	
	gameStatePtr->bStartNextDay = false;

	gameStatePtr->startNextDayTime = 2.0f;
	gameStatePtr->startNextDayTimer = 0.0f;
	gameStatePtr->day = 1;

	gameStatePtr->memArena = memoryArena;
	gameStatePtr->airports = airport_tArray_CreateArena(memoryArena, airportAmt);
	gameStatePtr->passengers = passenger_tArray_CreateArena(memoryArena, passengerAmt);
	gameStatePtr->tickets = ticket_tArray_CreateArena(memoryArena, ticketAmt);
	gameStatePtr->flights = flight_tArray_CreateArena(memoryArena, flightAmt);
	gameStatePtr->ticketRenderTextures = RenderTexture2D_tArray_CreateArena(memoryArena, 10);
	gameStatePtr->dayscorecards = dayscorecard_tArray_CreateArena(memoryArena, 5);
	gameStatePtr->dayscorecards->length = 5;
	gameStatePtr->compScreen = COMPSCREEN_MAINMENU;

	gameStatePtr->chooseflightTime = 1.00f;
	gameStatePtr->chooseflightTimer = 0.0f;
	gameStatePtr->gameTime = 0;


	for (i = 0; i < gameStatePtr->dayscorecards->capacity; i++)
	{
		dayscorecard_t* scorecard = gameStatePtr->dayscorecards->items + i;
		scorecard->entityId = i;
		scorecard->bActive = false;		
	}

	for (i = 0; i < gameStatePtr->ticketRenderTextures->capacity; i++)
	{
		RenderTexture2D_t* renderTexture = gameStatePtr->ticketRenderTextures->items + i;
		renderTexture->renderTexture = LoadRenderTexture(240, 150);
		renderTexture->entityId = i;
		renderTexture->bActive = false;
	}
	

	for (i = 0; i < gameStatePtr->flights->capacity; i++)
	{
		flight = gameStatePtr->flights->items + i;
		flight->bActive = false;
		flight->entityId = i;
		flight->instanceId = 0;
		flight->nextfree = NULL;
	}

	for (i = 0; i < gameStatePtr->tickets->capacity; i++)
	{
		ticket = gameStatePtr->tickets->items + i;
		ticket->bActive = false;
		ticket->entityId = i;
		ticket->instanceId = 0;
		ticket->nextfree = NULL;
		ticket->firstname = Arena_Alloc(memoryArena, sizeof(char) * 50);
		ticket->lastname = Arena_Alloc(memoryArena, sizeof(char) * 50);
		ticket->bVisible = false;
		ticket->bThrown = false;
		ticket->bHeld = false;
		ticket->bBig = false;
		ticket->bReturning = false;
		ticket->bPrinting = false;
		
	}

	for (i = 0; i < gameStatePtr->airports->capacity; i++)
	{
		airport = gameStatePtr->airports->items + i;

		airport->airportcode = (char*)Arena_Alloc(memoryArena, sizeof(char) * 4);
		airport->airportname = (char*)Arena_Alloc(memoryArena, sizeof(char) * 50);
		airport->city = (char*)Arena_Alloc(memoryArena, sizeof(char) * 50);
		airport->state = (char*)Arena_Alloc(memoryArena, sizeof(char) * 3);
		airport->bActive = false;
		airport->entityId = i;
		airport->instanceId = 0;
		airport->nextfree = NULL;
	}


	airport = airport_tArray_GetNextFree(gameStatePtr->airports);
	airport->bActive = true;
	airport->bHub = true;

	sprintf(airport->airportcode, "TPA");
	sprintf(airport->airportname, "Tampa International Airport");
	sprintf(airport->city, "Tampa");
	sprintf(airport->state, "FL");

	airport = airport_tArray_GetNextFree(gameStatePtr->airports);
	airport->bActive = true;
	airport->bHub = true;
	sprintf(airport->airportcode, "DFW");
	sprintf(airport->airportname, "Dallas/Fort Worth International Airport");
	sprintf(airport->city, "Dallas");
	sprintf(airport->state, "TX");

	airport = airport_tArray_GetNextFree(gameStatePtr->airports);
	airport->bActive = true;
	sprintf(airport->airportcode, "EYW");
	sprintf(airport->airportname, "Key West International Airport");
	sprintf(airport->city, "Key West");
	sprintf(airport->state, "FL");

	airport = airport_tArray_GetNextFree(gameStatePtr->airports);
	airport->bActive = true;
	sprintf(airport->airportcode, "FLL");
	sprintf(airport->airportname, "Fort Lauderdale Hollywood International Airport");
	sprintf(airport->city, "Fort Lauderdale");
	sprintf(airport->state, "FL");

	airport = airport_tArray_GetNextFree(gameStatePtr->airports);
	airport->bActive = true;
	sprintf(airport->airportcode, "GNV");
	sprintf(airport->airportname, "Gainesville Regional Airport");
	sprintf(airport->city, "Gainesville");
	sprintf(airport->state, "FL");


	airport = airport_tArray_GetNextFree(gameStatePtr->airports);
	airport->bActive = true;
	sprintf(airport->airportcode, "GLS");
	sprintf(airport->airportname, "Galveston/Scholes International Airport");
	sprintf(airport->city, "Galveston");
	sprintf(airport->state, "TX");


	airport = airport_tArray_GetNextFree(gameStatePtr->airports);
	airport->bActive = true;
	sprintf(airport->airportcode, "GRK");
	sprintf(airport->airportname, "Killeen-Fort Hood Regional Airport");
	sprintf(airport->city, "Fort Hood / Killeen");
	sprintf(airport->state, "TX");

	airport = airport_tArray_GetNextFree(gameStatePtr->airports);
	airport->bActive = true;
	sprintf(airport->airportcode, "HRL");
	sprintf(airport->airportname, "Valley International Airport");
	sprintf(airport->city, "Harlingen");
	sprintf(airport->state, "TX");

	for (i = 0; i < gameStatePtr->passengers->capacity; i++)
	{
		passenger = gameStatePtr->passengers->items + i;
		passenger->firstname = Arena_Alloc(memoryArena, sizeof(char) * 50);
		passenger->lastname = Arena_Alloc(memoryArena, sizeof(char) * 50);
		passenger->bActive = false;
		passenger->entityId = i;
		passenger->instanceId = 0;
		passenger->nextfree = NULL;
		passenger->ticketId = -1;
		passenger->ticketGiven = false;
		passenger->ticketReturned = false;
		passenger->printedTicketGiven = false;
		passenger->printedTicketReturned = false;
		passenger->bCompleted = false;
		passenger->idGiven = false;
		passenger->idReturned = false;
		passenger->printedTicketId = -1;
	}

	ld58_Init(gameStatePtr->memArena);
}


bool ld_button(const char* text, int x, int y, int w, int h, Color color, bool disabled, bool selected)
{
	Color drawColor = color;
	bool collides = false;
	bool clicked = false;
	if (CheckCollisionPointRec(mousePos, (Rectangle) { x, y, w, h }))
	{
		drawColor = BLUE;
		collides = true;
	}

	if (collides && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		clicked = true;
		drawColor = DARKGREEN;
	}

	if (disabled)
	{
		drawColor = GRAY;
		clicked = false;
		collides = false;
	}

	if (selected)
	{
		drawColor = GREEN;
	}

	DrawRectangle(x, y, w, h, drawColor);
	DrawText(text, x + 5, y + 5, 5, WHITE);

	return collides & clicked;
}

void GetNextPassenger()
{
	if (gameStatePtr->currentPassenger != NULL)
	{
		printf("GetNextPassenger()");	
	}


	bool allDone = true;

	// VALIDATE CUSTOMER
	if (gameStatePtr->currentPassenger != NULL)
	{
		if (gameStatePtr->currentPassenger->idGiven && !gameStatePtr->currentPassenger->idReturned)
		{

			// COMPLAIN DIALOG
			return;
		}

		if (gameStatePtr->currentPassenger->ticketGiven && !gameStatePtr->currentPassenger->ticketReturned && !gameStatePtr->currentPassenger->printedTicketGiven)
		{
			// COMPLAIN DIALOG
			return;
		}

		if (gameStatePtr->currentPassenger->printedTicketGiven && !gameStatePtr->currentPassenger->printedTicketReturned)
		{
			// COMPLAIN DIALOG
			return;
		}
	}

	

	if (gameStatePtr->currentPassenger != NULL)
	{
		gameStatePtr->currentPassenger->bCompleted = true;

		for (i = 0; i < gameStatePtr->tickets->length; i++)
		{
			ticket_t* ticketCleanup = ticket_tArray_Get(gameStatePtr->tickets, i);
			ticketCleanup->bVisible = false;
			ticketCleanup->bReturning = false;
		}


		dayscorecard_t* scorecard = dayscorecard_tArray_Get(gameStatePtr->dayscorecards, gameStatePtr->day - 1);
		if (!gameStatePtr->currentPassenger->printedTicketReturned)
		{
			scorecard->noTicketGiven++;
		}
		else
		{
			// Check validity of ticket
			ticket_t* printedTicket = ticket_tArray_Get(gameStatePtr->tickets, gameStatePtr->currentPassenger->printedTicketId);
			ticket_t* originalTicket = ticket_tArray_Get(gameStatePtr->tickets, gameStatePtr->currentPassenger->ticketId);

			bool happyCustomer = false;

			if (printedTicket->srcId == gameStatePtr->currentPassenger->srcId && printedTicket->destId == gameStatePtr->currentPassenger->destId)
			{
				happyCustomer = true;
			}
			else if (gameStatePtr->currentPassenger->layoverId != -1 && printedTicket->srcId == gameStatePtr->currentPassenger->layoverId && printedTicket->destId == gameStatePtr->currentPassenger->destId)
			{
				happyCustomer = true;
			}
			else if (gameStatePtr->currentPassenger->layoverId != -1 && printedTicket->srcId == gameStatePtr->currentPassenger->srcId && printedTicket->destId == gameStatePtr->currentPassenger->layoverId)
			{
				happyCustomer = true;
			}
			else
			{
				scorecard->incorrectFlight++;
			}

			if (happyCustomer)
			{
				scorecard->happyCustomer++;

				if (originalTicket->firstClass && originalTicket->firstClass != printedTicket->firstClass)
				{
					scorecard->happyCustomer--;
					scorecard->downgraded++;
				}


				if (!originalTicket->firstClass && originalTicket->firstClass != printedTicket->firstClass)
				{
					scorecard->upgraded++;
				}
			}		
		}	

		gameStatePtr->lastPassenger = gameStatePtr->currentPassenger;
		gameStatePtr->currentPassenger = NULL;
	}



	int passengerIndex = -1;
	int passengerLowestTime = 99999999;

	for (i = 0; i < gameStatePtr->passengers->length; i++)
	{
		passenger_t* passengerInternal = passenger_tArray_Get(gameStatePtr->passengers, i);
		ticket = ticket_tArray_Get(gameStatePtr->tickets, passengerInternal->ticketId);

		if (passengerInternal->bCompleted)
		{
			continue;
		}

		allDone = false;

		if (gameStatePtr->gameTime > ticket->depart)
		{
			if (passengerLowestTime > ticket->depart)
			{
				passengerLowestTime = ticket->depart;
				passengerIndex = i;
			}
		}
	}

	if (allDone)
	{
		gameStatePtr->dayCompleted = true;
	}
	if (passengerIndex == -1) return;
	
	passenger_t* passenger = passenger_tArray_Get(gameStatePtr->passengers, passengerIndex);
	// add logic here to see if the passenger should actually be available or not.

	gameStatePtr->currentPassenger = passenger;


	int rand = GetRandomValue(0, 100);

	if (passenger->ticketLayoverId != -1)
	{
		if (rand > 50)
		{
			ticket_t* ticket = ticket_tArray_Get(gameStatePtr->tickets, passenger->ticketLayoverId);
			ticket->bVisible = true;
			ticket->bThrown = true;
			printf("LAYOVER");
		}
		else
		{
			ticket_t* ticket = ticket_tArray_Get(gameStatePtr->tickets, passenger->ticketId);
			ticket->bThrown = true;
			ticket->bVisible = true;
			printf("LAYOVER-REGULAR");
		}
	}
	else
	{
		ticket_t* ticket = ticket_tArray_Get(gameStatePtr->tickets, passenger->ticketId);
		ticket->bVisible = true;
		ticket->bThrown = true;
		printf("REGULAR");
	}

	gameStatePtr->currentPassenger->ticketGiven = true;
	gameStatePtr->currentPassengerIndex++;
}