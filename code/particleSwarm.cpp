#include <iostream>
#include <stdlib.h>
#include <time.h>

const double winprob[] = {0.5035298876753168, 0.5067426095609271, 0.514212639496454, 0.5180527115017372, 0.5212701659770329, 0.5231154236857699, 0.533289717919849, 0.5366497439123012, 0.5407975665195882, 0.543629465697123, 0.5468830499742401, 0.5694207836456558, 0.54725, 0.5851598173515982, 0.5369875222816399, 0.5482536764705882};
int focus = 0;
double prob;

double fitnessFunc(double bet) {
    double money = 0;
    double totalMon = 0;
    if(bet < 0) bet = 0;

    for(int j = 0; j<1000; j++) {
        money = 200;
        for(int i = 0; i<1000; i++) {
            if(bet > 1) bet = 1;
            if(rand()%10000 < prob) {
                money += money*bet;
            }else {
                money -= money*bet;
            }
            if(money <= 20) {
                money = 0;//*bet;
                totalMon += money;
                break;
            }
            // if(money > 1000) {
            //     money += (1000-j)*50;
            //     totalMon += money;
            //     break;
            // }
            totalMon += money;
        }
    }
    return totalMon;
}


double gb = -999999;
double gpos = 0;

class Particle {
    public:
        double pos = ((double) (rand()%1000))/1000;
        double vel = 0;
        double best = -999999;
        double bestPos = pos;
        int id = rand()%1000;

        void update() {
            const double r1 = ((double) (rand()%10000))/10000;
            const double r2 = ((double) (rand()%10000))/10000;

            vel = 0.7*vel + 1.4*r1*(bestPos-pos) + 1.4*r2*(gpos-pos);
            pos += vel;

            double score = fitnessFunc(pos);
            if(score > bestPos) {
                best = score;
                bestPos = pos;
            }
            if(score > gb) {
                gb = score;
                gpos = pos;
            }
        }
};


// let points = [];
// for(let i = 0; i<50; i++) {
//   points.push(new Particle());
// }


// for(let j = 0; j<1; j++){
//     for(let i = 0; i<points.length; i++) {
//       points[i].update();
//       point((points[i].pos[0]*scale+tr.x), ((points[i].pos[1] || 0)*scale+tr.y));
//     }
//   }

int main() {
    srand(time(NULL));
    double data[16];
    for(int i = 0; i<16; i++){data[i] = 0;}

    //focus = 15;
    //prob = winprob[focus]*10000;

    std::cout << "prob: " << prob/10000 << "\n";

    Particle points[50];

    double last = 0;


    for(int t = 0; t<16; t++){
        prob = winprob[t]*10000;
        for(int k = 0; k<8; k++){
            gb = -999999;
            gpos = 0;
            last = 0;
            for(int i3 = 0; i3<50; i3++) {
                points[i3].pos = ((double) (rand()%1000))/1000;
                points[i3].vel = 0;
                points[i3].bestPos = points[i3].pos;
                points[i3].best = -999999;
            }
            for(int j = 0; j<75; j++){
                for(int i4 = 0; i4<50; i4++) {
                    points[i4].update();
                }
                if(gb-last > 0.000001){
                    last = gb;
                    std::cout << j << "J| bestpos: " << gpos << "\n";
                }
            }
            data[t] += gpos/8;
        }
        std::cout << "data:\n";
        for(int i2 = 0; i2<16; i2++) {
            std::cout << data[i2] << ",";
        }
        std::cout << "\n";
    }



    


    return 0;
}