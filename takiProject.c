#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WITHDRAW 0
#define INITIAL_CARDS_NUM 4
#define MAX_TYPE_LEN 6
#define MAX_NAME_LENGTH 20
#define FINISH_TURN 0
#define COUNTER_CLOCKWISE -1
#define CLOCKWISE 1
// CONSTANTS FOR THE COLOR OF THE CARDS
#define RED 'R'
#define YELLOW 'Y'
#define GREEN 'G'
#define BLUE 'B'
#define BLANK ' '
// CONSTANTS FOR THE TYPE OF THE CARDS
#define TAKI "TAKI"
#define STOP "STOP"
#define CHANGE_COLOR "COLOR"
#define CHANGE_DIRECTION "<->"
#define EXTRA_TURN "+"
#define NUMBER_1 "1"
#define NUMBER_2 "2"
#define NUMBER_3 "3"
#define NUMBER_4 "4"
#define NUMBER_5 "5"
#define NUMBER_6 "6"
#define NUMBER_7 "7"
#define NUMBER_8 "8"
#define NUMBER_9 "9"
// CONSTANTS FOR THE INDEXES OF THE STATISTICS ARRAY
#define NUM_OF_TYPES 14
#define NUM_1_IND 0
#define NUM_2_IND 1
#define NUM_3_IND 2
#define NUM_4_IND 3
#define NUM_5_IND 4
#define NUM_6_IND 5
#define NUM_7_IND 6
#define NUM_8_IND 7
#define NUM_9_IND 8
#define EX_TURN_IND 9
#define STOP_IND 10
#define CHANGE_DIRECTION_IND 11
#define TAKI_IND 12
#define CHANGE_COLOR_IND 13



typedef struct card  // struct for each card that holds the type of the card and its color.
{
	char type[MAX_TYPE_LEN];
	char color;
}CARD;

typedef struct player // struct for each player
{
	char name[MAX_NAME_LENGTH];  // player's name
	CARD* cardPack; // player card array ( card pack)
	int pack_PhysSize; // physical size of the player's pack
	int pack_LogSize; // logical size of the player's pack
}PLAYER;

typedef struct cardInfo // struct specially made for the statistics array.
{
	char cardType[NUM_OF_TYPES];
	int numOfAppearence;
}CARD_INFO;

// DECLARING FUNCTIONS //

//MAIN FUNCTIONS
void welcomeMessage();
PLAYER* initializePlayers(int* pnumOfPlayers);
void initializeCardStatistics(CARD_INFO cardStatistics[]);
void playGame(PLAYER players_DATA[], int numOfPlayers, CARD_INFO cardStatistics[], char winnerName[]);
void printStatistics(CARD_INFO cardStatistics[]);
void printWinner(char name[]);

//SUB FUNCTIONS
CARD* initializeCardPack();
CARD getRandomCard();
char randomColor();
void randomType(CARD* pCard);
void printCard(CARD card);
void printCardPack(CARD cardPack[], int size);
printBoard(PLAYER player, CARD upperCard);
void red();
void blue();
void green();
void yellow();
void white();
void resetPrintColor();
CARD playSpecialCard(PLAYER* pPlayer, int  gameDirection, int cardInd);
int withdrawOrPlay(PLAYER player, CARD upperCard);
bool isValidCard(CARD selectedCard, CARD upperCard);
void card_Withdraw(PLAYER* player);
CARD* cardPackRealloc(CARD* cardPack, int pack_PhysSize, int pack_LogSize);
bool isSpecialCard(CARD card);
CARD changeColor(PLAYER* pPlayer, int cardInd);
CARD takiWaterfall(PLAYER* pPlayer, CARD upperCard);
int finishTurnOrPlay(PLAYER player, CARD upperCard);
void addCardToStatistics(CARD card, CARD_INFO cardStatistics[]);
CARD_INFO* merge(CARD_INFO arr1[], int size1, CARD_INFO arr2[], int size2);
void sortStatistics(CARD_INFO cardStatistics[], int size);
void copyStatArr(CARD_INFO dest[], CARD_INFO src[], int size);
void printStatistics(CARD_INFO cardStatistics[]);

void main() // OMER MIGDAL 208202473
{
	srand(time(NULL));
	char winnerName[MAX_NAME_LENGTH];  // variable that will hold the winner's name
	CARD_INFO cardStatistics[NUM_OF_TYPES]; // array for the statistics
	PLAYER* players_DATA; // array for the player in the game
	int numOfPlayers; // holds the number of players
	welcomeMessage();
	players_DATA = initializePlayers(&numOfPlayers); // inititalizing the players array.
	initializeCardStatistics(cardStatistics); // initializing the card statistics array.
	playGame(players_DATA, numOfPlayers, cardStatistics, winnerName); //playing game
	printWinner(winnerName); // announcing winner
	sortStatistics(cardStatistics, NUM_OF_TYPES); // sorting the game statistics
	printStatistics(cardStatistics); // priniting the game statistics
	free(players_DATA); // freeing the allocated memory
}

//IMPLEMENTING FUNCTIONS
// 
//the function prints a welcome message to the user
void welcomeMessage()
{
	printf("************  Welcome to TAKI game !!! ***********\n");
}



// the function recieves the players arry, the number of players, the card statistics array and an empty string for the winner's name
// the function is responsible for handling the game - each player plays his turn until there is a winner.
// the function returns the name of the winner.
void playGame(PLAYER players_DATA[], int numOfPlayers, CARD_INFO cardStatistics[], char winnerName[])
{
	bool winner = false;
	int gameDirection = CLOCKWISE; // default direction is clockwise
	int i = 0; // variable for the turns
	int playerMove, cardInd;
	CARD withdrawnCard; // holds the withdrawn card
	CARD playedCard; // hold the card the the player has played
	CARD lastCard_inPack;  /// holds the ast card in the pack of a certain player.

	CARD upperCard = getRandomCard();  // getting the starting card
	while (isSpecialCard(upperCard) == true) // making sure the starting card is not special.
		upperCard = getRandomCard();
	while (winner == false) //  a loop that runs while there is not winner in the game...
	{
		if (i > numOfPlayers) // making sure not overflowing after "stop" card.
			i = numOfPlayers - i;
		if (i < 0)
			i = numOfPlayers + i;
		if (i < 0 && gameDirection == COUNTER_CLOCKWISE) // making sure index is not negative and reseting the loop.
			i = numOfPlayers - 1;
		else if (i == numOfPlayers && gameDirection == CLOCKWISE) // finished one loop of turns-reseting the loop.
			i = 0;

		printBoard(players_DATA[i], upperCard); // printing the game board.
		playerMove = withdrawOrPlay(players_DATA[i], upperCard); // the player is choosing his move

		if (playerMove == WITHDRAW) // player wants to withdraw
		{
			card_Withdraw(&players_DATA[i]);     // sending the function the player that wants to withdraw .
			withdrawnCard = players_DATA[i].cardPack[(players_DATA[i].pack_LogSize) - 1];   // withdrawn card is the last card in the cardpack
			addCardToStatistics(withdrawnCard, cardStatistics);   // add the card that has been withdrwan to the statistics.
		}
		else // the player has a card to play with.
		{
			cardInd = playerMove - 1;// the card index in the array is the pick-1.
			playedCard = players_DATA[i].cardPack[cardInd];
			lastCard_inPack = players_DATA[i].cardPack[players_DATA[i].pack_LogSize - 1];
			if (isSpecialCard(playedCard) == true) // if the user is playing a special card
			{
				if (strcmp(playedCard.type, TAKI) == 0) // Checking if the user is playng taki waterfall
				{
					players_DATA[i].cardPack[cardInd] = lastCard_inPack;
					players_DATA[i].pack_LogSize--;
					upperCard = takiWaterfall(&players_DATA[i], playedCard);
					if (isSpecialCard(upperCard) == true && strcmp(upperCard.type, TAKI) != 0) // checking if the last card that the player has put in his taki waterfall is special
						playSpecialCard(&players_DATA[i], gameDirection, cardInd);
				}
				else
					upperCard = playSpecialCard(&players_DATA[i], gameDirection, cardInd);
			}

			else // player is playing a regular card
			{
				upperCard = playedCard;
				players_DATA[i].cardPack[cardInd] = lastCard_inPack; // "removing" the played card from the deck.
				players_DATA[i].pack_LogSize--;

			}
		}

		// CHECKING LAST CARD //
		if (players_DATA[i].pack_LogSize == 0 && strcmp(upperCard.type, EXTRA_TURN) == 0) // if the player played extra turn as last card.
		{
			// giving him a random card.
			players_DATA[i].cardPack[0] = getRandomCard();
			players_DATA[i].pack_LogSize = 1;
		}
		if (players_DATA[i].pack_LogSize == 0 && strcmp(upperCard.type, STOP) == 0 && numOfPlayers == 2)// if the player played stop as random card and there are 2 players
		{
			// gvinig random card.
			players_DATA[i].cardPack[0] = getRandomCard();
			players_DATA[i].pack_LogSize = 1;

		}
		// checking if the player has finished his card pack therfore he is the winner
		if (players_DATA[i].pack_LogSize == 0)
		{
			winner = true; // breaking the loop if there is a winner.
			strcpy(winnerName, players_DATA[i].name); // returning the winner's name.
		}
		if (gameDirection == CLOCKWISE) // moving the turn index in the direction of the game.
		{
			i++;
			// making sure to operate the loop  by the played card
			if (strcmp(upperCard.type, STOP) == 0 && playerMove != WITHDRAW)
				i++;
			if (strcmp(upperCard.type, EXTRA_TURN) == 0 && playerMove != WITHDRAW)
				i--;
		}
		else // game direction is counterclockwise.
		{
			i--;
			// making sure to operate the loop  by the played card
			if (strcmp(upperCard.type, STOP) == 0 && playerMove != WITHDRAW)
				i--;
			if (strcmp(upperCard.type, EXTRA_TURN) == 0 && playerMove != WITHDRAW)
				i++;
		}
	}
}


// this function gets a pointer to the playing player, the game direction, pointer to the turn index and the index of the card the player is playing
// the function is checking which of the special card is played and acts according to it
// the function returns a card variable - that will be the upper card,
CARD playSpecialCard(PLAYER* pPlayer, int  gameDirection, int cardInd)
{
	CARD returnedUpperCard; // declaring local variables for the function 
	PLAYER playingPlayer = *pPlayer;
	CARD playedCard = returnedUpperCard = playingPlayer.cardPack[cardInd];
	CARD lastCard_inPack = playingPlayer.cardPack[playingPlayer.pack_LogSize - 1];
	if (strcmp(playedCard.type, STOP) == 0)// the player is playing STOP card
	{
		playingPlayer.cardPack[cardInd] = lastCard_inPack; // "removing" the played card from the deck.
		playingPlayer.pack_LogSize--;
		returnedUpperCard = playedCard;
	}
	else if (strcmp(playedCard.type, CHANGE_COLOR) == 0)// the player is playing COLOR card
	{
		returnedUpperCard = changeColor(&playingPlayer, cardInd);
	}
	else if (strcmp(playedCard.type, EXTRA_TURN) == 0)// the player is playing EXTRA TURN card
	{
		playingPlayer.cardPack[cardInd] = lastCard_inPack; // "removing" the played card from the deck.
		playingPlayer.pack_LogSize--;
		returnedUpperCard = playedCard;

	}
	else if (strcmp(playedCard.type, CHANGE_DIRECTION) == 0)// the player is playing CHANGE DIRECTION card
	{
		playingPlayer.cardPack[cardInd] = lastCard_inPack; // "removing" the played card from the deck.
		playingPlayer.pack_LogSize--;
		returnedUpperCard = playedCard;
		if (gameDirection == CLOCKWISE) // switching clockwise to counterclockwise.
			gameDirection = COUNTER_CLOCKWISE;
		else // switching counterclockwise to clockwise.
			gameDirection = CLOCKWISE;
	}
	*pPlayer = playingPlayer;
	return returnedUpperCard;
}


// this function recieves a pointer to a player and a card ( upper card)
// the fnction returns the last card that thet player has put in the end of his taki waterfall.
CARD takiWaterfall(PLAYER* pPlayer, CARD upperCard)
{
	int playerMove;
	int cardInd;
	char takiColor = upperCard.color;
	PLAYER player = *pPlayer;
	CARD lastCard_inPack;
	CARD lastPlayedCard = upperCard;
	while (player.pack_LogSize != 0)
	{
		lastCard_inPack = player.cardPack[player.pack_LogSize - 1];
		printBoard(player, lastPlayedCard);
		playerMove = finishTurnOrPlay(player, upperCard);
		if (playerMove == FINISH_TURN)
		{
			*pPlayer = player;
			return lastPlayedCard;
		}
		else
		{
			cardInd = playerMove - 1;
			lastPlayedCard = upperCard = player.cardPack[cardInd];
			player.cardPack[cardInd] = lastCard_inPack;
			player.pack_LogSize--;
			if (strcmp(lastPlayedCard.type, CHANGE_COLOR) == 0) // color card in the middle of taki waterfall breaks the taki waterfall
			{
				lastPlayedCard = upperCard = changeColor(&player, cardInd);
				break;
			}

		}
	}
	*pPlayer = player;
	return lastPlayedCard;
}





// the function receives a pointer to a player and a card index
// the function gets the player's choice of color that he wants to change the upper card to
// the function returns the upper card as a return value
CARD changeColor(PLAYER* pPlayer, int cardInd)
{
	int num;
	CARD playedCard;
	PLAYER player = *pPlayer;
	CARD lastCard_inPack = player.cardPack[player.pack_LogSize - 1];
	strcpy(playedCard.type, CHANGE_COLOR);
	printf("Please enter your color choice:\n");
	printf("1 - Yellow\n");
	printf("2 - Red\n");
	printf("3 - Blue\n");
	printf("4 - Green\n");
	scanf("%d", &num);
	switch (num)
	{
	case 1:
		playedCard.color = YELLOW;
		break;
	case 2:
		playedCard.color = RED;
		break;
	case 3:
		playedCard.color = BLUE;
		break;
	case 4:
		playedCard.color = GREEN;
		break;
	}
	player.cardPack[cardInd] = lastCard_inPack; // "removing" the played card from the deck.
	player.pack_LogSize--;
	*pPlayer = player;
	return playedCard;
}

// the function recieves an int pointer as an output parameter
// the function initialzies the players array.
// the function returns number of players which are playing the game inside the return paramater
// the function returns a PLAYER type pointer to the beggining of the players array.
// Complexity O(numofplayers).
PLAYER* initializePlayers(int* pnumOfPlayers)
{
	int numOfPlayers, playerIndex;
	printf("Please enter the number of players:\n");
	scanf("%d", &numOfPlayers);
	PLAYER* players_DATA = (PLAYER*)malloc(numOfPlayers * sizeof(PLAYER)); // memory allocating- the size will be number of players
	if (players_DATA == NULL)
		exit(1); // memory allocation failed.
	for (playerIndex = 0; playerIndex < numOfPlayers; playerIndex++)
	{
		printf("Please enter the first name of player #%d:\n", playerIndex + 1);
		scanf("%s", players_DATA[playerIndex].name);
		players_DATA[playerIndex].cardPack = initializeCardPack();
		players_DATA[playerIndex].pack_PhysSize = INITIAL_CARDS_NUM; // setting the pack sizes to 4 cards
		players_DATA[playerIndex].pack_LogSize = INITIAL_CARDS_NUM;
	}
	printf("\n");
	*pnumOfPlayers = numOfPlayers; // return value the number of players.
	return players_DATA;
}

// the function returns a pointer to the begginig of a CARD array (card pack).
// the size of the initialized card pack is 4 cards.
CARD* initializeCardPack()
{
	int cardIndex;
	CARD* cardPack = (CARD*)malloc(INITIAL_CARDS_NUM * sizeof(CARD)); // memory allocating
	if (cardPack == NULL)
		exit(1); // memory allocation failed.
	for (cardIndex = 0; cardIndex < INITIAL_CARDS_NUM; cardIndex++)
		cardPack[cardIndex] = getRandomCard();
	return cardPack;
}

// the function returns a random CARD
CARD getRandomCard()
{
	CARD card;
	randomType(&card);
	if (strcmp(card.type, CHANGE_COLOR) == 0)// CHANGE COLOR type has no color.
		card.color = BLANK;
	else
	{
		card.color = randomColor();
	}
	return card;
}

// the function recieves a CARD pointer as an output paramter.
// the function returns initializes a random type inside the card.
void randomType(CARD* pCard)
{
	CARD card;
	int num = 1 + rand() % 14; // random number between 1 to 14.
	switch (num)
	{
	case 1:
		strcpy(card.type, NUMBER_1);
		break;
	case 2:
		strcpy(card.type, NUMBER_2);
		break;
	case 3:
		strcpy(card.type, NUMBER_3);
		break;
	case 4:
		strcpy(card.type, NUMBER_4);
		break;
	case 5:
		strcpy(card.type, NUMBER_5);
		break;
	case 6:
		strcpy(card.type, NUMBER_6);
		break;
	case 7:
		strcpy(card.type, NUMBER_7);
		break;
	case 8:
		strcpy(card.type, NUMBER_8);
		break;
	case 9:
		strcpy(card.type, NUMBER_9);
		break;
	case 10:
		strcpy(card.type, EXTRA_TURN);
		break;
	case 11:
		strcpy(card.type, STOP);
		break;
	case 12:
		strcpy(card.type, CHANGE_DIRECTION);
		break;
	case 13:
		strcpy(card.type, TAKI);
		break;
	case 14:
		strcpy(card.type, CHANGE_COLOR);
		break;
	}
	*pCard = card;
}

// this function returns a random char which represents a color of a card in the game (R/G/B/Y).
char randomColor()
{
	char color;
	int num = rand() % 4; // random number between 0 to 3
	switch (num)
	{
	case 0:
		color = RED;
		break;
	case 1:
		color = BLUE;
		break;
	case 2:
		color = GREEN;
		break;
	case 3:
		color = YELLOW;
		break;
	}
	return color;
}

// returns if the card is special or not
bool isSpecialCard(CARD card)
{
	if (strcmp(card.type, EXTRA_TURN) == 0 || strcmp(card.type, STOP) == 0 || strcmp(card.type, CHANGE_DIRECTION) == 0 || strcmp(card.type, TAKI) == 0 || strcmp(card.type, CHANGE_COLOR) == 0)
		return true;
	else
		return false;
}

void red() //prints in red
{
	printf("\033[1;31m");
}
void blue() //prints in blue
{
	printf("\033[0;34m");
}
void green() //prints in green
{
	printf("\033[0;32m");
}
void yellow() //prints in yellow
{
	printf("\033[1;33m");
}
void white() //prints in white
{
	printf("\033[0; 37m");
}
void resetPrintColor()// default color
{
	printf("\033[0m");
}


// the functions recieves a card.
// the function prints the card in the matching card color.
void printCard(CARD card)
{
	if (card.color == RED)
		red();
	else if (card.color == BLUE)
		blue();
	else if (card.color == GREEN)
		green();
	else if (card.color == YELLOW)
		yellow();
	else
		white();

	printf("*********\n");
	printf("*       *\n");
	if (isSpecialCard(card) == false)
		printf("*   %s   *\n", card.type);
	else
	{
		if (strcmp(card.type, TAKI) == 0)
			printf("*  TAKI *\n");
		else if (strcmp(card.type, CHANGE_COLOR) == 0)
			printf("* COLOR *\n");
		else if (strcmp(card.type, CHANGE_DIRECTION) == 0)
			printf("*  <->  *\n");
		else if (strcmp(card.type, STOP) == 0)
			printf("*  STOP *\n");
		else if (strcmp(card.type, EXTRA_TURN) == 0)
			printf("*   +   *\n");
	}
	if (card.color == BLANK)
		printf("*       *\n");
	else
		printf("*   %c   *\n", card.color);
	printf("*       *\n");
	printf("*********\n");
	printf("\n");
	resetPrintColor();
}


// functin recieves a card pack and size.
// function prints the card pack
// complexity O(packSize);
void printCardPack(CARD cardPack[], int packSize)
{
	for (int cardIndex = 0; cardIndex < packSize; cardIndex++)
	{
		printf("Card #%d:\n", cardIndex + 1);
		printCard(cardPack[cardIndex]);
	}
}


//the function recieves a player variable and card
// the function prints the game board according to the player and upper card.
printBoard(PLAYER player, CARD upperCard)
{
	printf("\nUpper Card:\n");
	printCard(upperCard);
	printf("%s's turn\n\n", player.name);
	printCardPack(player.cardPack, player.pack_LogSize);
}



// this function recieves a PLAYER varible and the upper card of the game
// the function gets the user choice- play or withdraw from the deck
// the function returns the user's choice.
int withdrawOrPlay(PLAYER player, CARD upperCard)
{
	int playerMove;
	int pack_LogSize = player.pack_LogSize;
	printf("Please enter 0 if you want to take a card from the deck\n");
	printf("or 1-%d if you want to put one of your cards in the middle:\n", pack_LogSize);
	scanf("%d", &playerMove);
	// checking if the user inserted valid input.
	if (playerMove == 0)
		return WITHDRAW;
	if ((0 < playerMove < pack_LogSize) && isValidCard(player.cardPack[playerMove - 1], upperCard))
		return playerMove;

	else // the user has put invalid input.
		while ((playerMove < 0 || playerMove > pack_LogSize) || isValidCard(player.cardPack[playerMove - 1], upperCard) == false)
		{
			if ((playerMove > pack_LogSize) || (playerMove < 0)) // invalid card number- selected bigger number or negative. 
			{
				printf("Invalid choice! Try again.\n");
				printf("Please enter 0 if you want to take a card from the deck\n");
				printf("or 1-%d if you want to put one of your cards in the middle:\n", pack_LogSize);
				scanf("%d", &playerMove);
				if (playerMove == 0)
					return WITHDRAW;
			}
			else if (isValidCard(player.cardPack[playerMove - 1], upperCard) == false) // the color of the selected card not matching to top card.
			{
				printf("Invalid card! Try again.\n");
				printf("Please enter 0 if you want to take a card from the deck\n");
				printf("or 1-%d if you want to put one of your cards in the middle:\n", pack_LogSize);
				scanf("%d", &playerMove);
				if (playerMove == 0)
					return WITHDRAW;
			}
		}
	return playerMove;
}


// this funcction recieves a CARD array, its logic size and physical size
// the function allocataes a new CARD array twice the size of the recieved one
// the function copies the information from the received CARD array to the new CARD array.
// the function returns the new CARD array.
// complexity O(pack_logSize).
CARD* cardPackRealloc(CARD* cardPack, int pack_PhysSize, int pack_LogSize)
{
	int newSize = pack_LogSize * 2;
	CARD* newCardPack = (CARD*)malloc(newSize * sizeof(CARD));
	//checking if memoey allocation succeded
	if (newCardPack == NULL)
		exit(1); // memory allocation failed.

	 // copying the old pack to the new one
	for (int i = 0; i < pack_LogSize; i++)
		newCardPack[i] = cardPack[i];

	free(cardPack); // free the old card array

	return newCardPack;
}

//this function recieves a player.
 //the function add a card to the player.
 //the user is responsible to free the newCard array memory if allocated
void card_Withdraw(PLAYER* player)
{
	int pack_LogSize = player->pack_LogSize;
	int pack_PhysSize = player->pack_PhysSize;

	CARD cardFromDeck = getRandomCard();    // getting a random card.
	// add card to statistics
	if (pack_LogSize == pack_PhysSize) // there is not enough space in the card array for an extra card.
	{
		player->cardPack = cardPackRealloc(player->cardPack, pack_PhysSize, pack_LogSize); // new array twice the size of the old one
		player->cardPack[pack_LogSize] = cardFromDeck; // adding the withdrawn card to the pack.
		player->pack_LogSize++; // the log size s one card more
		player->pack_PhysSize *= 2; // physical size is twice.
	}
	else // there is enough space in the array.
	{
		player->cardPack[pack_LogSize] = cardFromDeck; // putting the extra card in the end of the array.
		player->pack_LogSize++;
	}
}

// checks if the player choice of card is valid.
bool isValidCard(CARD selectedCard, CARD upperCard)
{
	if ((selectedCard.color == upperCard.color) || (selectedCard.color == BLANK) || strcmp(selectedCard.type, upperCard.type) == 0)
		return true;
	else
		return false;
}


// the functions receives a card and a CARD_INFO arry of statistics
// the function add's the card to the statistics.
void addCardToStatistics(CARD card, CARD_INFO cardStatistics[])
{
	if (strcmp(card.type, NUMBER_1) == 0)
		cardStatistics[NUM_1_IND].numOfAppearence++;
	else if (strcmp(card.type, NUMBER_2) == 0)
		cardStatistics[NUM_2_IND].numOfAppearence++;
	else if (strcmp(card.type, NUMBER_3) == 0)
		cardStatistics[NUM_3_IND].numOfAppearence++;
	else if (strcmp(card.type, NUMBER_4) == 0)
		cardStatistics[NUM_4_IND].numOfAppearence++;
	else if (strcmp(card.type, NUMBER_5) == 0)
		cardStatistics[NUM_5_IND].numOfAppearence++;
	else if (strcmp(card.type, NUMBER_6) == 0)
		cardStatistics[NUM_6_IND].numOfAppearence++;
	else if (strcmp(card.type, NUMBER_7) == 0)
		cardStatistics[NUM_7_IND].numOfAppearence++;
	else if (strcmp(card.type, NUMBER_8) == 0)
		cardStatistics[NUM_8_IND].numOfAppearence++;
	else if (strcmp(card.type, NUMBER_9) == 0)
		cardStatistics[NUM_9_IND].numOfAppearence++;
	else if (strcmp(card.type, EXTRA_TURN) == 0)
		cardStatistics[EX_TURN_IND].numOfAppearence++;
	else if (strcmp(card.type, STOP) == 0)
		cardStatistics[STOP_IND].numOfAppearence++;
	else if (strcmp(card.type, CHANGE_DIRECTION) == 0)
		cardStatistics[CHANGE_DIRECTION_IND].numOfAppearence++;
	else if (strcmp(card.type, TAKI) == 0)
		cardStatistics[TAKI_IND].numOfAppearence++;
	else if (strcmp(card.type, CHANGE_COLOR) == 0)
		cardStatistics[CHANGE_COLOR_IND].numOfAppearence++;
}

// the function initializes the statistics array.
void initializeCardStatistics(CARD_INFO cardStatistics[])
{
	for (int i = 0; i < NUM_OF_TYPES; i++)
		cardStatistics[i].numOfAppearence = 0; // initializing number of appearences
	strcpy(cardStatistics[NUM_1_IND].cardType, NUMBER_1);
	strcpy(cardStatistics[NUM_2_IND].cardType, NUMBER_2);
	strcpy(cardStatistics[NUM_3_IND].cardType, NUMBER_3);
	strcpy(cardStatistics[NUM_4_IND].cardType, NUMBER_4);
	strcpy(cardStatistics[NUM_5_IND].cardType, NUMBER_5);
	strcpy(cardStatistics[NUM_6_IND].cardType, NUMBER_6);
	strcpy(cardStatistics[NUM_7_IND].cardType, NUMBER_7);
	strcpy(cardStatistics[NUM_8_IND].cardType, NUMBER_8);
	strcpy(cardStatistics[NUM_9_IND].cardType, NUMBER_9);
	strcpy(cardStatistics[EX_TURN_IND].cardType, EXTRA_TURN);
	strcpy(cardStatistics[STOP_IND].cardType, STOP);
	strcpy(cardStatistics[CHANGE_DIRECTION_IND].cardType, CHANGE_DIRECTION);
	strcpy(cardStatistics[TAKI_IND].cardType, TAKI);
	strcpy(cardStatistics[CHANGE_COLOR_IND].cardType, CHANGE_COLOR);
}

// the function recieves two CARD_INFO arrays and their sizes
// the function returns a sroted array of CARD_INFO. sorted by number of appearences.
// complexity O(size)
CARD_INFO* merge(CARD_INFO arr1[], int size1, CARD_INFO arr2[], int size2)
{
	int ind1, ind2, resInd;
	CARD_INFO* res = (CARD_INFO*)malloc((size1 + size2) * sizeof(CARD_INFO));
	if (res) // memory allocation success
	{
		resInd = ind1 = ind2 = 0;
		while ((ind1 < size1) && (ind2 < size2))
		{
			if (arr1[ind1].numOfAppearence >= arr2[ind2].numOfAppearence)
			{
				res[resInd] = arr1[ind1];
				ind1++;
			}
			else
			{
				res[resInd] = arr2[ind2];
				ind2++;
			}
			resInd++;
		}
		while (ind1 < size1)
		{
			res[resInd] = arr1[ind1];
			resInd++;
			ind1++;
		}
		while (ind2 < size2)
		{
			res[resInd] = arr2[ind2];
			resInd++;
			ind2++;
		}
	}
	return res;
}


// this function recieves two CARD_INFO type arrays source and destanation.
// the function is copying  src to dest.
//complexity O(size).
void copyStatArr(CARD_INFO dest[], CARD_INFO src[], int size)
{
	for (int i = 0; i < size; i++)
		dest[i] = src[i];
}


// recursive function.
 // the function recieves a CARD_INFO array and its size
 // the function sorts the array - biggest number of appearences first.
 // Complexity sizeLog(Size).
void sortStatistics(CARD_INFO cardStatistics[], int size)
{
	if (size == 1)
		return;
	else
	{
		sortStatistics(cardStatistics, size / 2);
		sortStatistics(cardStatistics + (size / 2), size - (size / 2));
		CARD_INFO* tmpStat = merge(cardStatistics, size / 2, cardStatistics + (size / 2), size - (size / 2));
		if (tmpStat == NULL)
			exit(1); // memory allocation failed.
		else
		{
			copyStatArr(cardStatistics, tmpStat, size);
			free(tmpStat);
		}
	}
}

// this function prints the statistics of the cards that has been withdrawn.
// Complexity O(1)
void printStatistics(CARD_INFO cardStatistics[])
{
	printf("************ Game Statistics ************\n");
	printf("Card # | Frequency\n");
	printf("______________\n");
	for (int i = 0; i < NUM_OF_TYPES; i++)
	{
		if (strcmp(cardStatistics[i].cardType, TAKI) == 0 || strcmp(cardStatistics[i].cardType, STOP) == 0)
			printf(" %s  |  %d\n", cardStatistics[i].cardType, cardStatistics[i].numOfAppearence);
		else if (strcmp(cardStatistics[i].cardType, CHANGE_COLOR) == 0)
			printf(" %s |  %d\n", cardStatistics[i].cardType, cardStatistics[i].numOfAppearence);
		else if (strcmp(cardStatistics[i].cardType, CHANGE_DIRECTION) == 0)
			printf("  %s  |  %d\n", cardStatistics[i].cardType, cardStatistics[i].numOfAppearence);
		else
			printf("   %s   |  %d\n", cardStatistics[i].cardType, cardStatistics[i].numOfAppearence);
	}
}


//this is a function specially for the taki waterfall
//the function recieves the player playing taki waterfall and the upper card
// the function return the player's choice.
int finishTurnOrPlay(PLAYER player, CARD upperCard)
{
	int playerMove;
	int pack_LogSize = player.pack_LogSize;
	printf("Please enter 0 if you want to finish your turn\n");
	printf("or 1-%d if you want to put one of your cards in the middle:\n", pack_LogSize);
	scanf("%d", &playerMove);
	// checking if the user inserted valid input.
	if (playerMove == 0)
		return FINISH_TURN;
	if ((0 < playerMove < pack_LogSize) && isValidCard(player.cardPack[playerMove - 1], upperCard))
		return playerMove;

	else // the user has put invalid input.
		while ((playerMove < 0 || playerMove > pack_LogSize) || isValidCard(player.cardPack[playerMove - 1], upperCard) == false)
		{
			if ((playerMove > pack_LogSize) || (playerMove < 0)) // invalid card number - selected bigger number than the deck size or negative number. 
			{
				printf("Invalid choice! Try again.\n");
				printf("Please enter 0 if you want to take a card from the deck\n");
				printf("or 1-%d if you want to put one of your cards in the middle:\n", pack_LogSize);
				scanf("%d", &playerMove);
				if (playerMove == 0)
					return FINISH_TURN;
			}
			else if (isValidCard(player.cardPack[playerMove - 1], upperCard) == false) // the color of the selected card not matching to top card.
			{
				printf("Invalid card! Try again.\n");
				printf("Please enter 0 if you want to finish your turn\n");
				printf("or 1-%d if you want to put one of your cards in the middle:\n", pack_LogSize);
				scanf("%d", &playerMove);
				if (playerMove == 0)
					return FINISH_TURN;
			}
		}
	return playerMove;
}

// functions prints the winner's name
void printWinner(char winnerName[])
{
	printf("The winner is...%s! Congratulations! \n", winnerName);
}