
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>

using namespace std;

const char hard_total[18][10] = {       // A   2   3   4   5   6   7   8   9  10
                                          'H','H','H','H','H','H','H','H','H','H', // 4
                                          'H','H','H','H','H','H','H','H','H','H', // 5
                                          'H','H','H','H','H','H','H','H','H','H', // 6
                                          'H','H','H','H','H','H','H','H','H','H', // 7
                                          'H','H','H','H','H','H','H','H','H','H', // 8
                                          'H','H','D','D','D','D','H','H','H','H', // 9
                                          'H','D','D','D','D','D','D','D','D','H', // 10
                                          'H','D','D','D','D','D','D','D','D','D', // 11
                                          'H','H','H','S','S','S','H','H','H','H', // 12
                                          'H','S','S','S','S','S','H','H','H','H', // 13
                                          'H','S','S','S','S','S','H','H','H','H', // 14
                                          'H','S','S','S','S','S','H','H','H','H', // 15
                                          'H','S','S','S','S','S','H','H','H','H', // 16
                                          'S','S','S','S','S','S','S','S','S','S', // 17
                                          'S','S','S','S','S','S','S','S','S','S', // 18
                                          'S','S','S','S','S','S','S','S','S','S', // 19
                                          'S','S','S','S','S','S','S','S','S','S', // 20
                                          'S','S','S','S','S','S','S','S','S','S'   // 21
                                };
const char splitting[11][10] =  {      // A   2   3   4   5   6   7   8   9   10
                                         'P','P','P','P','P','P','P','P','P','P', // AA
                                         'H','H','P','P','P','P','H','H','H','H', // 22
                                         'H','H','P','P','P','P','H','H','H','H', // 33
                                         'H','H','H','H','H','H','H','H','H','H', // 44
                                         'H','D','D','D','D','D','D','D','D','H', // 55
                                         'H','P','P','P','P','H','H','H','H','H', // 66
                                         'H','P','P','P','P','P','P','H','H','H', // 77
                                         'P','P','P','P','P','P','P','P','P','P', // 88
                                         'S','P','P','P','P','P','S','P','P','S', // 99
                                         'S','S','S','S','S','S','S','S','S','S'  // TT
                                };
const char soft_total[9][10] =  {      // A   2   3   4   5   6   7   8   9   10
                                         'H','H','H','H','D','D','H','H','H','H', // A2
                                         'H','H','H','H','D','D','H','H','H','H', // A3
                                         'H','H','H','D','D','D','H','H','H','H', // A4
                                         'H','H','H','D','D','D','H','H','H','H', // A5
                                         'H','H','D','D','D','D','H','H','H','H', // A6
                                         'H','S','D','D','D','D','S','S','H','H', // A7
                                         'S','S','S','S','S','S','S','S','S','S', // A8
                                         'S','S','S','S','S','S','S','S','S','S', // A9
                                         'S','S','S','S','S','S','S','S','S','S'  // A10
                                };

char get_hard_total_choice(int player, int dealer){
    player -= 4;
    dealer -= 1;
    return(hard_total[player][dealer]);
}

char get_soft_total_choice(int player, int dealer){
    player -= 2;
    dealer -= 1;
    return(soft_total[player][dealer]);
}

char get_splitting_choice(int player, int dealer){
    player -= 1;
    dealer -= 1;
    return(splitting[player][dealer]);
}

int get_card(){
    int card = (rand() % 13) + 1;
    if ((card == 11) || (card == 12) || (card == 13)){
        card = 10;
    }
    return(card);
}

float get_pay(int dealer, int user, float bet){
    if (dealer == 21){
        cout << "dealer hits 21..." << endl;
        return(bet * -1);
    }
    else if (user > 21){
        cout << "player bust..." << endl;
        return(bet * -1);
    }
    else if (user == 21){
        cout << "player hits 21!" << endl;
        return(bet + (bet * .5));
    }
    else if (dealer > 21){
        cout << "dealer busts!" << endl;
        return(bet);
    }
    else if (user > dealer){
        cout << "player beats dealer" << endl;
        return(bet);
    }
    else if (user == dealer){
        cout << "dealer pushes" << endl;
        return(0);
    }
    else if (user < dealer){
        cout << "user loses to dealer..." << endl;
        return(bet * -1);
    }
    return(0);
}

bool has_ace(vector<int> cards){
    for(int i = 0; i < cards.size(); i++){
        if(cards[i] == 1){
            return(true);
        }
    }
    return(false);
}

vector<int> get_dealer_hand(vector<int> cards){
    int total = cards[0];
    int new_card = get_card();
    cards.push_back(new_card);
    total += new_card;
    if (has_ace(cards)){
        total += 10;
    }
    while (total < 17) {
        cards.push_back(new_card);
        total += new_card;
    }
    if((total > 21) && (has_ace(cards))){
        total -= 10;
    }
    return(cards);
}

int count_final_hand(vector<int> cards){
    bool see_ace = false;
    int total = 0;
    for (int i = 0; i < cards.size(); i++){
        if ((cards[i] == 1) && (see_ace == false)){
            see_ace = true;
            total += 11;
        }
        else {
            total += cards[i];
        }
    }
    if ((see_ace) && (total > 21)){
        total -= 10;
    }
    return(total);
}

int count_hand(vector<int> cards){
    bool see_ace = false;
    int total = 0;
    for (int i = 0; i < cards.size(); i++){
        if ((cards[i] == 1) && (see_ace == false)){
            see_ace = true;
        }
        else {
            total += cards[i];
        }
    }
    return(total);
}

void print_cards(vector<int> cards){
    cout << "[ " ;
    for(int i = 0; i < cards.size(); i++){
        cout << cards[i] << " ";
    }
    cout << "]" << endl;
}

int play_hands(vector<int> dealer_cards, vector<vector<int> > player_cards, int bet){
    dealer_cards = get_dealer_hand(dealer_cards);
    int dealer_hand = count_final_hand(dealer_cards);
    int winnings = 0;
    cout << "dealer card up: " << dealer_cards[0] << endl;
    for (int i = 0; i < player_cards.size(); i++ ){
        int player_hand = 0; // improved commeting ;)
        char choice = 'H';
        int hand_total = 0;
        int bet_multiplier = 1;
        int new_card = 0;
        while (choice == 'H') {
            hand_total = count_hand(player_cards[i]);
            if (has_ace(player_cards[i]) == true){ // has ace = soft totals
                choice = get_soft_total_choice(hand_total, dealer_cards[0]);
                if ((player_cards[i].size() > 2) && (choice == 'D')){
                    choice = 'H';
                }
            }
            else { // no ace = hard totals
                choice = get_hard_total_choice(hand_total, dealer_cards[0]);
            }
            if (choice == 'D'){
                new_card = get_card();
                player_cards[i].push_back(new_card);
                choice = 'S';
                bet_multiplier = 2;
                cout << "DOUBLING" << endl;
            }
            else if (choice == 'H'){
                new_card = get_card();
                player_cards[i].push_back(new_card);
            }
            if (choice == 'S'){
                player_hand = count_final_hand(player_cards[i]);
            }
        }
        print_cards(dealer_cards);
        print_cards(player_cards[i]);
        winnings += get_pay(dealer_hand, player_hand, bet * bet_multiplier);
    }
    return(winnings);
}

void print_vector(vector<vector<int> > vtr){
    
    for ( int i = 0 ; i < vtr.size(); i++ ){
        for ( int j = 0; j < vtr[i].size() ; j++ ){
            cout << vtr[i][j] << " " ;
        }
        cout << endl ;
    }
}

int main(){
    srand(time(NULL));
    int cash = 100;
    int bet = 5;
    int hands_played = 0;
    int balance_high = cash;
    vector<int> dealer_cards;
    vector<vector<int> > player_cards;

    while(cash > 0) {
        cout << " - - - - - - - - - - - - - - - - " << endl;
        vector<int> dealer_cards;
        vector<vector<int> > player_cards;

        dealer_cards.push_back(get_card()); // dealer card 1

        vector<int> player;
        player.push_back(get_card());
        player.push_back(get_card());
        player_cards.push_back(player);

        bool split = true;

        while(split == true){
            split = false;
            print_vector(player_cards);
            for(int i = 0; i < player_cards.size(); i++){
                if((player_cards[i][0] == player_cards[i][1]) && (get_splitting_choice(player_cards[i][0],dealer_cards[0]) == 'P')){
                    cout << "SPLITTING" << endl;
                    player_cards[i][1] = get_card();
                    vector<int> player;
                    player.push_back(player_cards[i][0]);
                    player.push_back(get_card());
                    split = true;
                }
            }
        }

        cash += play_hands(dealer_cards, player_cards, bet);
        if (cash > balance_high){
            balance_high = cash;
        }
        hands_played += 1;
        cout << "new balanec: " << cash << endl << endl << endl;
    }
    cout << endl << endl << "hands played: " << hands_played << endl;
    cout << "highest balance: " << balance_high << endl;
    return 0;
}