#include <iostream>
#include <stdlib.h>
#include <time.h>


const int maxDeck = 52*1;
const int autoShuffle = maxDeck/4;
int deckLen;
int deck[maxDeck];

int splits[8];
int splitLen = 0;

int pSum = 0;
int dSum = 0;

int pAces = 0;
int dAces = 0;

int bet = 10;
int money = 200;

bool canEndGame = true;
bool gameEnded = true;
bool canSplit = false;
bool isSplit = false;

bool realRound = false;

void makeDecision();
void runRound();
void shuffle();

void init() {
    srand(time(NULL));

    deckLen = maxDeck;

    for(int i = 0; i<deckLen; i++) {
        int card = i % 13 + 1;
        if( card > 10 ) card = 10;
        deck[i] = card;
    }

    shuffle();
}

void printDeck() {
    for(int i = 0; i<deckLen; i++) {
        std::cout << deck[i] << " ";
    }
    std::cout << "\n";
}

void printWholeDeck() {
    for(int i = 0; i<maxDeck; i++) {
        std::cout << deck[i] << " ";
    }
    std::cout << "\n";
}

void printKeys() {
    int keys[10];
    for(int i = 0; i<10; i++) keys[i] = 0;
    std::cout << "All Cards:\n";
    for(int i = 0; i<maxDeck; i++) {
        keys[deck[i]-1]++;
    }
    for(int i = 0; i<10; i++) {
        std::cout << keys[i] << " ";
    }

    std::cout << "\nIn Deck:\n";
    for(int i = 0; i<10; i++) keys[i] = 0;
    for(int i = 0; i<deckLen; i++) {
        keys[deck[i]-1]++;
    }
    for(int i = 0; i<10; i++) {
        std::cout << keys[i] << " ";
    }
    std::cout << "\n";
}

bool badDeck() {
    int keys[10];
    for(int i = 0; i<10; i++) keys[i] = 0;
    for(int i = 0; i<maxDeck; i++) {
        keys[deck[i]-1]++;
    }
    for(int i = 0; i<9; i++) {
        if(keys[i] != maxDeck/13) return true;
    }

    return false;
}

void shuffle() {
    deckLen = maxDeck;
}

int drawCard() {

    //deckLen--;
    //return deck[deckLen];
    int index = rand() % deckLen;
    deckLen--;

    int temp = deck[index];
    deck[index] = deck[deckLen];
    deck[deckLen] = temp;

    return temp;
}



void pAddCard() {

    int card = drawCard();

    if(card == 1) {
        card = 11;
        pAces++;
    }

    pSum += card;
    if(pSum > 21 && pAces > 0) {
        pSum -= 10;
        pAces--;
    }
}

void dAddCard() {
    int card = drawCard();
    if(card == 1) {
        card = 11;
        dAces++;
    }

    dSum += card;
    if(dSum > 21 && dAces > 0) {
        dSum -= 10;
        dAces--;
    }
}

void hitTo17() { // dealer hits untill 17
    while(dSum < 17) {
        dAddCard();
    }
}

void collectMoney() {
    if(pSum > 21) {
        money -= bet;
        if(realRound) std::cout << "pSum:" << pSum << ", dSum:" << dSum <<" [bust]\n";
        return;
    }

    if(dSum > 21){
        money += bet;
        if(realRound) std::cout << "pSum:" << pSum << ", dSum:" << dSum <<" [dealer bust]\n";
        return;
    }

    if(pSum > dSum) {
        money += bet;
        if(realRound) std::cout << "pSum:" << pSum << ", dSum:" << dSum <<" [p wins]\n";
        return;
    }

    if(dSum > pSum) {
        money -= bet;
        if(realRound) std::cout << "pSum:" << pSum << ", dSum:" << dSum <<" [d wins]\n";
        return;
    }
}

void endGame() {
    gameEnded = true;
    if(!canEndGame) return;
    hitTo17();

    collectMoney();
}

void reset() {
    if(deckLen < autoShuffle) {
        shuffle();
    }
    pSum = 0;
    dSum = 0;
    pAces = 0;
    dAces = 0;
    canSplit = false;
}

void hit() { // player hit once
    if(realRound) std::cout << "pSum:" << pSum << ", dSum:" << dSum <<" (hit)\n";
    pAddCard();
    if(pSum >= 21) {
        endGame();
    }
    canSplit = false;
}

void doubleDown() {
    if(realRound) std::cout << "pSum:" << pSum << ", dSum:" << dSum <<" (double)\n";
    bet *= 2;
    pAddCard();
    endGame();
    bet /= 2;
    canSplit = false;
}

void stand() {
    if(realRound) std::cout << "pSum:" << pSum << ", dSum:" << dSum <<" (stand)\n";
    endGame();
    canSplit = false;
}

void split() { // TODO test
    if(realRound) std::cout << "pSum:" << pSum << ", dSum:" << dSum <<" (split)\n";
    canSplit = false;
    pSum /= 2;
    pAces /= 2;
    bet *= 2;
    isSplit = true;
}

void unsplit() {
    canSplit = true;
    pSum *= 2;
    pAces *= 2;
    bet /= 2;
    isSplit = false;
}

void deal() {
    pAddCard();
    int c1 = pSum*2;
    dAddCard();
    pAddCard();
    c1 -= pSum;
    //dAddCard();

    if(pSum == 21) {
        gameEnded = true;
        money += bet*1.5;
        if(realRound) std::cout << "pSum:" << pSum << ", dSum:" << dSum <<" (blackjack)\n";
        return;
    }

    if(c1 == 0){ // both cards are the same
        canSplit = true;
        return;
    }

}

int simulate(int &gains) {
    bool globalRealRound = realRound;
    realRound = false;
    int _deckLen = deckLen;
    int _deck[maxDeck] = {};
    for(int i = 0; i<maxDeck; i++) _deck[i] = deck[i];

    int _pSum = pSum;
    int _dSum = dSum;
    int _pAces = pAces;
    int _dAces = dAces;
    int _money = money;

    const int TRIALS = 500;

    int ernedHit = 0;
    for (int i = 0; i<TRIALS; i++) {
        hit();
        if(!gameEnded) endGame();
        pSum = _pSum;
        dSum = _dSum;
        pAces = _pAces;
        dAces = _dAces;
        ernedHit += (money-_money);
        money = _money;
        gameEnded = false;
        deckLen = _deckLen;
        for(int j = 0; j<maxDeck; j++) deck[j] = _deck[j];
    }

    int ernedHit2 = 0;
    for (int i = 0; i<TRIALS; i++) {
        hit();
        if(!gameEnded){
            hit();
            if(!gameEnded) endGame();
        }
        pSum = _pSum;
        dSum = _dSum;
        pAces = _pAces;
        dAces = _dAces;
        ernedHit2 += (money-_money);
        money = _money;
        gameEnded = false;
        deckLen = _deckLen;
        for(int j = 0; j<maxDeck; j++) deck[j] = _deck[j];
    }

    int maxHit = std::max(ernedHit, ernedHit2);

    int ernedStand = 0;
    for (int i = 0; i<TRIALS; i++) {
        stand();
        if(!gameEnded) endGame();
        pSum = _pSum;
        dSum = _dSum;
        pAces = _pAces;
        dAces = _dAces;
        ernedStand += (money-_money);
        money = _money;
        gameEnded = false;
        deckLen = _deckLen;
        for(int j = 0; j<maxDeck; j++) deck[j] = _deck[j];
    }

    //std::cout << ernedHit << "," << ernedHit2 << " (h|s) " << ernedStand << ", spl:" << ", " << dSum << " (dsum|psum) " << pSum;
    //if(isSplit) std::cout << " (split)";
    //std::cout << "\n";

    realRound = globalRealRound;

    if(ernedHit > ernedStand) {
        //return 1; //TODO remove
        if(maxHit == ernedHit && maxHit > 0){
            gains = maxHit*2;
            return 2; // double down
        }
        gains = maxHit;
        return 1; // hit
    }
    gains = ernedStand;
    return 0; // stand
}

int round = 0;
void makeDecision() {

    if(gameEnded) {
        round ++;
        reset();
        gameEnded = false;
        deal();
    }

    if(pSum == 0) deal();

    if(gameEnded) return;

    int gains1 = 0;
    int gains2 = 0;
    int dec2 = -1;
    int dec = simulate(gains1);

    // if(canSplit) {
    //     pSum /= 2;
    //     pAces /= 2;
    //     int gains3 = 0;
    //     dec2 = simulate(gains3);
    //     gains2 = gains3*2; // temporary
    //     pSum *= 2;
    //     pAces *= 2;
    // }

    if(dec2 != -1 && gains2 > gains1) {
        pSum /= 2;
        pAces /= 2;
        canSplit = false;
        dec = dec2;
    }

    if(dec==1) {
        hit();
    }else if(dec==0){
        stand();
    }else {
        doubleDown();
    }

}

int calcBet() {

    if(pSum != 0 || dSum != 0) {
        std::cout << "pSum or dSum is not zero when calcBet is called";
        exit(1);
    }

    int deltaMoney = 0;

    const int TRIALS = 100;

    int _bet = bet;
    bet = 10;

    int _deckLen = deckLen;
    int _deck[maxDeck] = {};
    for(int j = 0; j<maxDeck; j++){_deck[j] = deck[j];}

    int _money = money;

    //std::cout << "pSum:" << pSum << ", dSum:" << dSum << ", money:" << money << "\n";

    for(int i = 0; i<TRIALS; i++){
        runRound();

        deckLen = _deckLen;

        for(int j = 0; j<maxDeck; j++){
            deck[j] = _deck[j];    
        }

        deltaMoney += (money-_money);
        money = _money;

    }
    bet = _bet;


    //std::cout << "delta$: " << deltaMoney << "\n";

    if(deltaMoney > 0) return 10;
    return 0;

    return std::max(deltaMoney, 0);
}


void runRound(){
    do{
        gameEnded = false;
        deal();
    }while(gameEnded);

    while(!gameEnded) {
        makeDecision();
        //gameEnded = true;
    }
    if(deckLen < autoShuffle) {
        shuffle();
    }
    pSum = 0;
    dSum = 0;
    pAces = 0;
    dAces = 0;
    canSplit = false;
    gameEnded = true;
}

int main(int argc, char **argv) {

    /*std::cout << "you put" << "\n";
    std::cout << *argv[1];

    int argLen = *argv[1];
    for(int i = 0; i<argLen; i++) {

    }*/

    init();

    int lastMoney = money;
    int sumWinCount = 0;

    for(int i = 0; i<=5000; i++) {
        //std::cout << money << ", round:" << round << "\n";

        int count = 0;
        for(int j = 0; j<deckLen; j++) {
            if(deck[j] == 10) count++;
            if(deck[j] <= 6) {
                if(deck[j] == 1){
                    count++;
                }else{
                    count--;
                }
            }
        }
        bet = 0;
        if(count == -1) bet = 5;
        if(count == 0) bet = 10;
        if(count == 1) bet = 20;
        if(count == 2) bet = 25;
        if(count >= 3) bet = 50;


        //bet = 10;//calcBet();
        //if(bet == 0) bet = 2;
        realRound = false;
        runRound();
        realRound = false;

        if(!(i%50)){
            std::cout << "money: " << money << ", bet: " << bet << ", count: " << count << ", round: " << i;
            if(money > lastMoney) std::cout << " win";
            std::cout << "\n";
            std::cout << "_______________";
            std::cout << "\n";
        };

        if(money > lastMoney) {
            sumWinCount += count;
        }

        lastMoney = money;
    }

    std::cout << "average count on win: " << (sumWinCount/10000) << "\n";

    int count = 0;
    for(int j = 0; j<deckLen; j++) {
        if(deck[j] == 10) count++;
        if(deck[j] <= 6) {
            if(deck[j] == 1){
                count++;
            }else{
                count--;
            }
        }
    }
    std::cout << "count is: " << count << "\n";
    printDeck();
    printKeys();

    std::cout << "end" << "\n";

    return 0;
}