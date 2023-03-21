#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>


const double bets[16] = {0.23320325, 0.25955225, 0.2826595, 0.2749049375, 0.2841924375, 0.2745745, 0.289193125, 0.3029555, 0.314505, 0.3106998125, 0.317134, 0.352458375, 0.3262433125, 0.3814700625, 0.3033185625, 0.307467375};

// let fitnessFunc = function(x,y) {
//   return -((100-x)**2 + (30-y)**2) - Math.random()*10000;
// }
const int dim = 2;
// TODO: add max bounds to swarm
double fitnessFunc(double pos[]);

double randm() {
    return ((double) (rand()%10000))/10000;
}

// let gb = -Infinity;
// let gx = 0;
// let gy = 0;

double gb = -9999999;
double gbp[dim];


// function Particle() {
//   this.x = Math.random()*10;
//   this.y = Math.random()*10;
//   this.xv = 0;
//   this.yv = 0;

//   this.step = 0;

//   this.best = -Infinity;
//   this.bx = this.x;
//   this.by = this.y;
// }

class Particle {
    public:
        double pos[dim];
        double vel[dim];
        double best = -9999999;
        double bestPos[dim];

        Particle() {
            for(int i = 0; i<dim; i++){
                pos[i] = randm()*20;
                vel[i] = 0;
                bestPos[i] = pos[i];
            }
        }

        void update() {
            const double w = 0.7;
            const double c1 = 1.4;
            const double c2 = 1.4;
            const double r1 = randm();
            const double r2 = randm();

            for(int i = 0; i<dim; i++) {
                vel[i] = w*vel[i] + c1*r1*(bestPos[i]-pos[i]) + c2*r2*(gbp[i]-pos[i]);
                pos[i] += vel[i];
            }

            double score = fitnessFunc(pos);
            if(score > best) {
                for(int i = 0; i<dim; i++) bestPos[i] = pos[i];
                best = score;
            }
            if(score > gb){
                for(int i = 0; i<dim; i++) gbp[i] = pos[i];
                gb = score;
            }

        }
};


// Particle.prototype.update = function() {
//   const w  = 0.7
//   const c1 = 1.4;
//   const c2 = 1.4;
//   const r1 = Math.random();
//   const r2 = Math.random();

//   this.step++;

//   this.xv = w*this.xv + c1*r1*(this.bx-this.x) + c2*r2*(gx-this.x);
//   this.yv = w*this.yv + c1*r1*(this.by-this.y) + c2*r2*(gy-this.y);

//   this.x += this.xv;
//   this.y += this.yv;

//   let score = fitnessFunc(this.x, this.y);
//   if(score > this.best) {
//     this.bx = this.x;
//     this.by = this.y;
//     this.best = score;
//   }
//   if(score > gb) {
//     gx = this.x;
//     gy = this.y;
//     gb = score;
//   }
// }


// let points = [];
// for(let i = 0; i<50; i++) {
//   points.push(new Particle());
// }

Particle points[50];

void printCenter() {
    double pos[dim];
    for(int i = 0; i<dim; i++) pos[i] = 0;
    for(int i = 0; i<50; i++){
        for(int j = 0; j<dim; j++) {
            pos[j] += points[j].pos[j]/50;
        }
    }
    for(int i = 0; i<dim; i++) {
        std::cout << pos[i] << ", ";
    }
    std::cout << "\n";
}

void printBest() {
    for(int i = 0; i<dim; i++) {
        std::cout << gbp[i] << ", ";
    }
    std::cout << "\n";
}


const int maxDeck = 52*1;
const int autoShuffle = maxDeck/4;
int deckLen = maxDeck;
int deck[maxDeck];

int sum = 0;
int aces = 0;
int bet = 10;
int money = 0;
int playerCards = 0;
bool canSplit = false;
bool isSplit = true;

int dSum = 0;
int dAces = 0;

int _sum = 0;
int _aces = 0;
int _money = 0;
int _playerCards = 0;
int _dSum = 0;
int _dAces = 0;
bool _canSplit = false;
int _deck[maxDeck];
bool gameEnded = true;

bool dbug = true;

void endGame();

void shuffle() {
    deckLen = maxDeck;
}

int drawCard() {
    int index = rand() % deckLen;
    deckLen--;

    int temp = deck[index];
    deck[index] = deck[deckLen];
    deck[deckLen] = temp;

    return temp;
}

int addCard() {
    int card = drawCard();
    if(card == 1){
        aces++;
        card = 11;
    }
    sum += card;
    if(sum > 21 && aces > 0) {
        aces--;
        sum-=10;
    }
    playerCards++;

    return card;
}

void deal() { // start round
    if(!gameEnded) {
        std::cout << "you can't deal if the game is still going\n";
        exit(1);
    }
    gameEnded = false;

    int card1 = addCard();

    dSum += drawCard();
    if(dSum == 1){
        dSum = 11;
        dAces = 1;
    }
    int card2 = addCard();
    if(card1 == card2) {
        canSplit = true;
    }
    if(sum == 21) {
        sum = -1;
        endGame();
    }
}

void hit() {
    if(gameEnded) return;
    addCard();
    canSplit = false;
    if(sum > 21) {
        endGame();
    }
    if(dbug) std::cout << "hit\n";
}

void stand() {
    if(gameEnded) return;
    endGame();
}

void doubleDown() {
    if(gameEnded) return;
    bet*=2;
    hit();
    stand();
    bet/=2;
}

void hitTo17() {
    if(!gameEnded){
        std::cout << "hitTo17 should only be called when game is ended";
        exit(1);
    }
    while(dSum < 17){
        int card = drawCard();
        if(card == 1){
            dAces++;
            card = 11;
        }
        dSum += card;
        if(dSum > 21 && dAces > 0) {
            dAces--;
            dSum-=10;
        }
    }
}

void save() {
    _sum = sum;
    _aces = aces;
    _money = money;
    _dSum = dSum;
    _dAces = dAces;
    _playerCards = playerCards;
    _canSplit = canSplit;
    for(int i = 0; i<maxDeck; i++) {
        _deck[i] = deck[i];
    }
}

void restore () {
    sum = _sum;
    aces = _aces;
    money = _money;
    dSum = _dSum;
    dAces = _dAces;
    playerCards = _playerCards;
    gameEnded = false;
    canSplit = _canSplit;
    for(int i = 0; i<maxDeck; i++) {
        deck[i] = _deck[i];
    }
}

void collectMoney() {
    int bet2 = bet;
    if(isSplit) bet2*=2;
    //if(isSplit) std::cout<<"is split\n";
    isSplit = false;

    if(sum > 21){
        money -= bet2;
        return;
    }
    if(sum == -1) { // blackjack
        if(dSum != 21){
            money += bet2*1.5;
        }
        return;
    }
    if(sum == -2) {
        money -= bet/2; // surrender;
        return;
    }
    if(dSum > 21) {
        money += bet2;
        return;
    }
    if(sum > dSum){
        money += bet2;
        return;
    }
    if(dSum > sum){
        money -= bet2;
        return;
    }
}

void endGame() {
    if(gameEnded) return;
    if(deckLen < autoShuffle) {
        shuffle();
    }
    gameEnded = true;
    hitTo17();
    collectMoney();
    sum = 0;
    dSum = 0;
    aces = 0;
    dAces = 0;
    playerCards = 0;
    canSplit = false;
}


void init() {
    srand(time(NULL));

    for(int i = 0; i<maxDeck; i++){
        int card = i%13 + 1;
        if(card > 10) card = 10;
        deck[i] = card;
    }

    deckLen = maxDeck;
}






int simRound(int &gains) {
    int hitMoney = 0;
    int hit2Money = 0;
    int standMoney = 0;

    const int TRIALS = 1000; //TODO: set to 500

    for(int i = 0; i<TRIALS; i++) {
        save();
        stand();
        standMoney += (money-_money);
        restore();
    }
    for(int i = 0; i<TRIALS; i++) {
        save();
        hit();
        stand();
        hitMoney += (money-_money);
        restore();
    }
    for(int i = 0; i<TRIALS; i++) {
        save();
        hit();
        hit();
        stand();
        hit2Money += (money-_money);
        restore();
    }

    int maxHit;
    if(hitMoney > hit2Money) {
        if(hitMoney > 0) {
            gains = hitMoney*2;
            return 2;
        }
        maxHit = hitMoney;
    }else{
        maxHit = hit2Money;
    }

    if(maxHit > standMoney) { // hit
        gains = maxHit;
        return 1;
    }

    gains = standMoney;
    return 0; // stand
}

int makeDecision() {
    if(gameEnded){
        std::cout << "you can't make a decision when the game is ended\n";
        exit(1);
    }

    if(playerCards == 2) {
        if((sum == 15 && dSum == 10) || (sum == 16 && dSum >= 9)){ // surender
            sum = -2;
            stand();
            return 4;
        }
    }

    int gains = 0;
    int gains2 = 0;
    int dec2 = -1;
    int dec = simRound(gains);

    if(canSplit){
        //std::cout << "can split\n";
        sum /= 2;
        aces /= 2;
        dec2 = simRound(gains2);
    }
    
    if(dec2 != -1){ // can split
        if(gains > gains2) { // don't split
            sum *= 2;
            aces *= 2;
        }else{ // split
            dec = dec2;
            isSplit = true;
            return 3;
        }
    }
    
    switch(dec) {
        case 0: stand(); return 0;
        case 1: hit(); return 1;
        case 2: doubleDown(); return 2;
        default: std::cout << dec << " is not a valid return value\n"; exit(1);
    }
}

double fitnessFunc(double pos[]) {
    double deltaMoney = 0;
    
        for(int k = 0; k<5; k++) {
            money = 200;
            for(int i = 0; i<=120; i++) {

                if(money < 20){
                    money = 0;
                    break;
                }

                int count = 0;
                for(int j = 0; j<deckLen; j++) {
                    int card = deck[j];
                    if(card >=4 && card <=6) {
                        count-=2;
                    }
                    if(card == 2 || card==3 || card == 7){
                        count-=1;
                        continue;
                    }
                    if(card == 1){
                        count+=1;
                        continue;
                    }
                    if(deck[j] == 10){
                        count+=2;
                    }
                }

                int index = count+12;
                if(index < 0) index = 0;
                if(index > 31) index = 31;

                if(count < 4){
                    bet = 0;
                }else {
                    bet = (int) ((pos[0]*0.001*((double) count)+pos[1]*0.001)*((double) money)*0.9);
                }
                if(bet > money) bet = money;
                if(bet < 0) bet = 0;

                deal();
                while(!gameEnded) {
                    makeDecision();
                }

                // if(i%1000 == 1) {
                //     std::cout << "money: " << money << ", bet: " << bet << ", count: " << count << ", round: " << i;
                //     std::cout << "\n";
                // }
        }
        //std::cout << "money: " << money << ", bet: " << bet << "\n";
        deltaMoney += ((double) money);
    }

    return deltaMoney;
}


int main(int argc, char **argv) {
    dbug = false;
    init();

    int cardsOnTable = argc-12; // 0 = getBet | 2 = getDecision 

    if(cardsOnTable < 0){
        std::cout << "invalid input. cards on table is less than 0 ("<<cardsOnTable<<")\n";
        exit(1);
    }

    std::cout << "cardsontable:("<<cardsOnTable<<")\n";

    deckLen = 0;
    for(int i = 1; i<11; i++) {
        int numCards = (int) *argv[i]-48;
        if(i == 10) numCards += ((int) *argv[11]-48)*10;
        std::cout << "numcards: "<<numCards<<"\n";
        for(int j = 0; j<numCards; j++){
            if(deckLen > maxDeck) {
                std::cout << "decklen > maxDeck ("<<deckLen<<")\n";
                exit(1);
            }
            deck[deckLen++] = i;
        }
    }

    std::cout << "deckkeys:\n";
    for(int i = 1; i<12; i++) {
        std::cout << i << ":" << *argv[i] << " "; // amount of each card
    }
    std::cout << "\n";

    if(cardsOnTable == 0) { // get Bet
        std::cout << "get bet\n";
        int count = 0;
        for(int j = 0; j<deckLen; j++) {
            int card = deck[j];
            if(card >=4 && card <=6) {
                count-=2;
            }
            if(card == 2 || card==3 || card == 7){
                count-=1;
                continue;
            }
            if(card == 1){
                count+=1;
                continue;
            }
            if(deck[j] == 10){
                count+=2;
            }
        }
        double dbet = 0;
        if(count >= 4){
            dbet = (0.018*((double) count)-0.034);
        }
        if(dbet<0) bet = 0;

        std::cout << "bet: " << dbet << "\n";

    }else { // get Decision
        std::cout << "get decision:\n";
        int card = (int) *argv[12]-48;
        if(card == 0) card = 10;
        if(card == 1){
            dAces++;
            card = 11;
        }
        dSum += card;
        std::cout << "sum: " << sum << "aces: " << aces << "dsum: " << dSum << "dAces: " << dAces <<  "\n";

        if(cardsOnTable == 3 && (*argv[13] == *argv[14])) canSplit = true;

        for(int i = 13; i<12+cardsOnTable; i++) {
            card = (int) *argv[i]-48;
            if(card == 0) card = 10;
            std::cout << "card " << card << "\n";
            if(card == 1){
                aces++;
                card = 11;
            }
            sum += card;
            if(sum > 21 && aces > 0) {
                aces--;
                sum-=10;
            }
            playerCards++;
        }
        gameEnded = false;
        std::cout << "sum:" << sum << " aces:" << aces << " dSum:" << dSum << " dAces:" << dAces <<  "\n";

        int dec = makeDecision();
        switch(dec){
            case 0: std::cout << "stand\n"; break;
            case 1: std::cout << "hit\n"; break;
            case 2: std::cout << "double\n"; break;
            case 3: std::cout << "split\n"; break;
            case 4: std::cout << "surrender\n"; break;
        }
    }

    exit(0);

    
    init();
    money = 200;

    

    deal();
    while(!gameEnded) {
        makeDecision();
    }

    // if(i%100 == 1) {
    //     std::cout << "money: " << money << ", bet: " << bet << ", count: " << count << ", round: " << i;
    //     std::cout << "\n";
    // }

    std::cout << "end\n";
    return 0;
}