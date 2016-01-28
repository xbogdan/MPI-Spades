#include <iostream>
#include <time.h>

using namespace std;

void print_vector(int *vector, int size) {
	for (size_t i = 0; i < size; i++) {
		cout << vector[i] << " ";
	}
	cout << endl;
}


void print_card(int card) {
	int card_id = card / 4 + 1;
	int card_type = card % 4;
	string card_symbol = to_string(card_id);

	if (card_id == 11) card_symbol = 'J';
	if (card_id == 12) card_symbol = 'Q';
	if (card_id == 13) card_symbol = 'K';
	if (card_id == 1) card_symbol = 'A';
	if (card_type == SPADES) cout << "\u2660" << card_symbol;
	if (card_type == HEARTS) cout << "\u2665" << card_symbol;
	if (card_type == DIAMONDS) cout << "\u2666" << card_symbol;
	if (card_type == CLUBS) cout << "\u2663" << card_symbol;
}


void print_points(int *tricks, int *score) {
	int sc;
	for (size_t i = 0; i < PLAYER_NR; i++) {
		sc = 0;
		if (tricks[i]>score[i]) {
			sc = -tricks[i]*10 ;
		} else {
			sc = tricks[i]*10 + ( score[i] - tricks[i] );
			if (score[i] - tricks[i] >= TRICKS_LIMIT) sc -= 100;
		}
		if (sc == 0) sc = 100;
		cout<<" Player "<< i <<" scored: "<< sc << "\n";
	}
	cout<<endl;
}


int myrandom (int i) { srand(time(NULL)); return rand()%i; }


void init_deck(int *deck) {
	for (size_t i = 0; i < DECK_SIZE; i++) {
		deck[i] = i;
	}
	random_shuffle(&deck[0], &deck[51], myrandom);
}


void print_deck(int *deck, int size) {
	for (size_t i = 0; i < size; i++) {
		print_card(deck[i]);
		cout << " ";
	}
	cout << endl;
}


void print_hand(int *hand, int start) {
	if (hand[start] != -1) {
		cout << "Hand: ";
		print_card(hand[start]);
		cout << " ";
	}
	start = (start+1) % 4;
	if(hand[start] != -1) print_card(hand[start]);
	cout << " ";
	start = (start+1) % 4;
	if(hand[start] != -1) print_card(hand[start]);
	cout << " ";
	start = (start+1) % 4;
	if(hand[start] != -1) print_card(hand[start]);
	cout << " ";

	cout << endl;
}


void print_choose_deck(int *deck, int size) {
	for (size_t i = 0; i < size; i++) {
		if (deck[i] == -1) continue;
		print_card(deck[i]);
		cout << "\t";
	}
	cout << endl;
	for (size_t i = 0; i < size; i++) {
		if (deck[i] == -1) continue;
		cout << i << "\t";
	}
	cout << endl;
}


void clear_screen() {
	if (!DEBUG)	system("clear");
}

/*
float tricks_per_suit(int *suit, int size) {
	float tricks = 0.0;

	if (suit[ACE]) {
		if (size < 7) tricks += 1;
		if (size >= 7) tricks += 0.5;
	}

	if (suit[KING]) {
		if (size == 1) tricks += 0.25;
		if (size < 5 && size > 1) tricks += 0.75;
		if (size >= 5) tricks += 0.25;
		if (size < 6 && (suit[ACE] || suit[QUEEN] || suit[JACK])) tricks += 0.25;
	}

	if (suit[QUEEN]) {
		if (size <= 5 && !suit[ACE] && !suit[KING]) tricks += 0.25;
		if (suit[ACE] && suit[KING]) tricks += 0.5;
	}

	return tricks;
}
*/
