#include <iostream>
#include <algorithm>
#include <mpi.h>
#include <time.h>
#include <math.h>
#include <cstring>
#include "constants.h"
#include "utils.h"

using namespace std;


/**
 * Compares 2 cards and returns the best
 */
int card_is_better(int card, int card_compare) {  // t = O(1)
  if (card_compare < 0) return 1;
	int c_id = card / 4;
	int cc_id = card_compare / 4;
	int c_type = card % 4;
	int cc_type = card_compare % 4;

	if (c_type == cc_type) { //if same type then
		if (c_id == ACE) return 1;
		if (cc_id == ACE) return 0;
		if (c_id > cc_id) return 1;
		return 0;
	}

	if (cc_type == SPADES) return 0;

	return 1;
}


/**
 * Compares cards 2 by 2 and chooses the winner
 */
int calculate_winner(int *hand, int start) { //t = O(1)
	int winner = start;
	winner = card_is_better(hand[winner], hand[(start+1) % 4]) ? winner : (start+1) % 4;
	winner = card_is_better(hand[winner], hand[(start+2) % 4]) ? winner : (start+2) % 4;
	winner = card_is_better(hand[winner], hand[(start+3) % 4]) ? winner : (start+3) % 4;
	return winner;
}


float tricks_per_suit(int *suit, int size) {
	float tricks = 0.0;

	int miss = 0;
	if (suit[ACE]) { tricks += 1; size--;}
	else miss += 1;
	if (suit[KING]) {tricks += 1.0 - miss*0.2; size--;}
	else miss += 1;
	if (suit[QUEEN]) {tricks += 1.0 - miss*0.2; size--;}
	else miss += 1;
	if (suit[JACK]) {tricks += 1.0 - miss*0.2; size--;}
	else miss += 1;
	if (suit[9]) {tricks += 1.0 - miss*0.2; size--;}  // 9 == card 10
	else miss += 1;

	if (size > 2 && size < 4) tricks += size * 0.1;
	return tricks;
}


/**
 * Calculates possbile tricks
 * which can be won with the given cards
 */
int calculate_possible_tricks(int *deck) { // TODO improve
	int spades[PLAYER_NR_CARDS], hearts[PLAYER_NR_CARDS], clubs[PLAYER_NR_CARDS], diamonds[PLAYER_NR_CARDS];
	int is=0, ih=0, ic=0, id=0;
	for (size_t i = 0; i < PLAYER_NR_CARDS; i++) {
		spades[i] = 0;
		hearts[i] = 0;
		clubs[i] = 0;
		diamonds[i] = 0;
	}

	float tricks = 0.0;

	for (size_t i = 0; i < PLAYER_NR_CARDS; i++) {
		int card_id = deck[i] / 4;
		int card_type = deck[i] % 4;
		if (card_type == SPADES) { spades[card_id] = 1; is++; }
		if (card_type == HEARTS) { hearts[card_id] = 1; ih++; }
		if (card_type == DIAMONDS) { diamonds[card_id] = 1; id++; }
		if (card_type == CLUBS) { clubs[card_id] = 1; ic++; }
	}

	// Spades
	if (is == 6) tricks += 0.5;
	if (is > 6) tricks += (is-6);

	if (spades[ACE] && is >= 4) tricks += 1;
	if (spades[ACE] && is == 3) tricks += 0.75;
	if (spades[ACE] && is == 2) tricks += 0.25;

	int high_cards[3] = {KING, QUEEN, JACK};
	for (size_t j = 0; j < 3; j++) {
		if (!spades[high_cards[j]]) continue;
		int unowned_bigger_cards = 0;
		for (size_t i = high_cards[j]; i < PLAYER_NR_CARDS; i++) {
			if (!spades[i] && spades[i] != high_cards[j]) unowned_bigger_cards++;
		}
		if (!spades[ACE]) unowned_bigger_cards++;

		int own_lesser_cards = is - (13 - high_cards[j] - unowned_bigger_cards) - 1;
		if (own_lesser_cards - unowned_bigger_cards >= 3) tricks += 1;
		if (own_lesser_cards - unowned_bigger_cards == 2) tricks += 0.75;
		if (own_lesser_cards - unowned_bigger_cards == 1) tricks += 0.25;
		// cout << "For: " << high_cards[j] << "  Own lesser cards: " << own_lesser_cards << endl << "For: " << high_cards[j] << "  Unowned bigger cards: " << unowned_bigger_cards << endl;
	}

	// cout << "Spades tricks: " << tricks << endl << endl;

	// Hearts
	tricks += tricks_per_suit(hearts, ih);

	// Diamonds
	tricks += tricks_per_suit(diamonds, id);

	// Clubs
	tricks += tricks_per_suit(clubs, ic);

	// cout << "\tBid: " << (int)floor(tricks) << endl << endl;

	return (int)floor(tricks); // TODO improve
}


/**
 * Human input possbile tricks
 * which can be won with the given cards
 */
int calculate_possible_tricks_human(int *own_cards){

	int c=0;
	cout << "Your cards: \n";
	print_choose_deck(own_cards, PLAYER_NR_CARDS);
	cout<<endl;
	cout << "Input your tricks number: ";
	cin >> c;
	cout << "\n\n";
	while (c < 0 || c > PLAYER_NR_CARDS) {
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Please be serious. Let's try again: ";
		cin >> c;
	}
	return c;
}


/**
 * Algorithm for picking the best card
 * for computer player
 */
void pick_best_card_ia(int *own_cards, int *hand, int rank) {
	int start = hand[(rank + 2) % 4] < 0 ? (rank + 3) % 4 : (rank + 1) % 4;
	start = hand[start] < 0 ? (start + 1) % 4 : start;
	int best_s = 0;
	int best_card = -1;
	if (start == rank){
		for (size_t i = 0; i < PLAYER_NR_CARDS; i++) {
			if (own_cards[i] != -1) {
				if (own_cards[i]%4 == SPADES) {
					best_s = card_is_better(own_cards[i], own_cards[best_s]) ? i : best_s;
				} else
					if ( own_cards[i] / 4 >= hand[rank] / 4 ) {
						hand[rank] = own_cards[i];
						best_card = i;
					}
			}
		}
		if (hand[rank] < 0) {
			hand[rank] = own_cards[best_s];
			best_card = best_s;
		}

		own_cards[best_card] = -1;
		return;
	}

	int start_type = hand[start] % 4;
	int winner = calculate_winner(hand, start);
	int worst_card = 0;
	for (size_t i = 0; i < PLAYER_NR_CARDS; i++) {
		if (own_cards[i] == -1) continue;
		if (own_cards[i]%4 == SPADES && card_is_better(own_cards[i], hand[winner]) ) {
			if ( own_cards[best_s] % 4 == SPADES && card_is_better(own_cards[best_s], hand[winner]) )
				best_s = card_is_better(own_cards[i], own_cards[best_s]) ? best_s : i;  //weakest winning spade
			else
				best_s = i;
		}

		if ((own_cards[i]/4 < own_cards[worst_card]/4 && own_cards[i]/4!=ACE) || own_cards[worst_card] == -1) worst_card = i;
		if (own_cards[i]%4 == start_type) {
			if (best_card == -1) {
				best_card = i;
				continue;
			}
			if ( !card_is_better(own_cards[i], hand[winner]) ) {
				best_card = card_is_better(own_cards[i], own_cards[best_card]) ? best_card : i;
				continue;
			}
			best_card = i;
		}
	}

	if (best_card < 0) {
		if (card_is_better(own_cards[best_s], hand[winner])) best_card = best_s;
		else best_card = worst_card;
	}

	hand[rank] = own_cards[best_card];
	own_cards[best_card] = -1;
}


/**
 * Prints player cards and
 * prompts the user to choose one to be played
 */
void pick_best_card_human(int *own_cards, int *hand, int rank) {
	int start = hand[(rank + 2) % 4] < 0 ? (rank + 3) % 4 : (rank + 1) % 4;
	start = hand[start] < 0 ? (start + 1) % 4 : start;

	int c=0;

	cout << "Your cards: \n";
	print_choose_deck(own_cards, PLAYER_NR_CARDS);
	print_hand(hand,start);

	cout<<endl;
  if (rank==start) {
    cout << "Your are first. Choose wisely: ";
  } else {
    cout << "Its your turn: ";
  }

	cin >> c;
	cout << "\n\n";
	while (own_cards[c] == -1 || c < 0 || c >= PLAYER_NR_CARDS) {
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Card unavailable, pick another: ";
		cin >> c;
	}

	hand[rank] = own_cards[c];
	own_cards[c] = -1;
  clear_screen();
}


/**
 * Picks best card to player
 * If the player is human asks which card to be played
 */
void pick_card(int *own_cards, int *hand, int rank) {
	if (rank == 0) {
		pick_best_card_human(own_cards, hand, rank);
	} else {
	  pick_best_card_ia(own_cards, hand, rank);
	}
}


/**
 *
 */
void start_play(int rank, int numtasks, int *own_cards, int *score) {
	int prev, next;

	int hand[PLAYER_NR];
	for (size_t i = 0; i < PLAYER_NR; i++) { hand[i] = -1; }

	MPI_Status status;
	int winner;
	int j=0;

	while (j < PLAYER_NR_CARDS) {

		// Determine next player
		next = (rank+1) % 4;

		// Determin previous player
		prev = (rank+3) % 4;

		// First hand played by the dealer
		if (j == 0 && rank == 0) {
			pick_card(own_cards, hand, rank);
			MPI_Send(hand, PLAYER_NR, MPI_INT, next, TAG_PLAY_HAND, MPI_COMM_WORLD);
			j++;
		}


		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);


    if (status.MPI_TAG == TAG_UPDATE_WINNER && rank == 0) { // Update score
			MPI_Recv(&winner, 1, MPI_INT, MPI_ANY_SOURCE, TAG_UPDATE_WINNER, MPI_COMM_WORLD, &status);
			score[winner]++;
    }


		if (status.MPI_TAG == TAG_START_HAND) { // The winner has to start the new hand
			MPI_Recv(hand, PLAYER_NR, MPI_INT, MPI_ANY_SOURCE, TAG_START_HAND, MPI_COMM_WORLD, &status);

			// Reset hand vector
			for (size_t i = 0; i < PLAYER_NR; i++) { hand[i] = -1; }

			pick_card(own_cards, hand, rank);
			MPI_Send(hand, PLAYER_NR, MPI_INT, next, TAG_PLAY_HAND, MPI_COMM_WORLD);
			j++;
		} else

		if (status.MPI_TAG == TAG_PLAY_HAND) {
			MPI_Recv(hand, PLAYER_NR, MPI_INT, prev, TAG_PLAY_HAND, MPI_COMM_WORLD, &status);

			// Every player has played his hand
			// It's time to evaluate the winner
			if (hand[0] != -1 && hand[1] != -1 && hand[2] != -1 && hand[3] != -1) {
				winner = calculate_winner(hand, rank);

				// Send to dealer who the winner is
				MPI_Send(&winner, 1, MPI_INT, DEALER, TAG_UPDATE_WINNER, MPI_COMM_WORLD);

        // if (j != 13) clear_screen();

				cout << "================================\n";
				print_hand(hand, rank);
				cout << "WINNER of hand " << j << " is player: " << winner << endl;
				cout << "================================ \n\n\n";

				// Send message to the winner to start the new hand
				MPI_Send(hand, PLAYER_NR, MPI_INT, winner, TAG_START_HAND, MPI_COMM_WORLD);
			} else {
				j++;
				pick_card(own_cards, hand, rank);
				MPI_Send(hand, PLAYER_NR, MPI_INT, next, TAG_PLAY_HAND, MPI_COMM_WORLD);
			}
		}
	}
}


/**
 * Send card to all player from process with rank 0
 */
void distribute_cards(int numtasks, int *deck, int *own_cards) {
	int i=0, k=0;
	while (i < DECK_SIZE) {
		for (size_t j = 0; j < numtasks; j++) {
			if (j == 0) {
				own_cards[k++] = deck[i];
			} else {
				MPI_Send(&deck[i], 1, MPI_INT, j, TAG_DISTRIBUTE_CARD, MPI_COMM_WORLD);
			}
			i++;
		}
	}
}


int main(int argc, char *argv[]) {

	int rc, numtasks, rank;
  clear_screen();
	MPI_Init(&argc, &argv);
	if (rc != MPI_SUCCESS) {
		cout << "Error starting MPI program. Terminating.\n";
		MPI_Abort(MPI_COMM_WORLD, rc);
	}


	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	if (numtasks != 4) {
		cout << "Need 4 players";
		MPI_Finalize();
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int *own_cards = new int[PLAYER_NR_CARDS];
	MPI_Status status;

  int *bids = new int[PLAYER_NR];
  int *score = new int[PLAYER_NR];
  for (size_t i = 0; i < PLAYER_NR; i++) { score[i] = 0; }


	/**
	 * Human player and dealer
	 */
	if (rank == 0) {

		int *deck = new int[DECK_SIZE];

		init_deck(deck);
    if (DEBUG) {
  		cout << "Deck: \n";
  		print_deck(deck, DECK_SIZE);
  		cout << "\n\n";
    }

		// Send cards
		distribute_cards(numtasks, deck, own_cards);

		// Calculate bid
		bids[DEALER] = calculate_possible_tricks_human(own_cards);

		// Receive bids from other players
		for (size_t i = 1; i < numtasks; i++) {
			int bid=-1;
			MPI_Recv(&bid, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			bids[status.MPI_SOURCE] = bid;
		}
    clear_screen();

		cout << "Bids: ";
		print_vector(bids, numtasks);
		cout << "\n\n\n";

		cout << "GAME BEGINS !!! \n\n\n";
	}


	/**
	 * Computer players
	 */
	if (rank != 0) {
		// Receive cards from DEALER
		for (size_t i = 0; i < PLAYER_NR_CARDS; i++) {
			int card;
			MPI_Recv(&card, 1, MPI_INT, DEALER, TAG_DISTRIBUTE_CARD, MPI_COMM_WORLD, &status);
			own_cards[i] = card;
		}

		// Calculate and send bid to DEALER
		int tricks = calculate_possible_tricks(own_cards);
		MPI_Send(&tricks, 1, MPI_INT, DEALER, 0, MPI_COMM_WORLD);
	}

	// DEBUG
	if (DEBUG) {
		cout << "Player: " << rank << "  \n\tCards: ";
		print_deck(own_cards, PLAYER_NR_CARDS);
	}

	// Start playing
	start_play(rank, numtasks, own_cards, score);

  if (rank == DEALER){
    print_points(bids, score);
  }

	MPI_Finalize();
	return 0;
}
