#include <iostream>

const short M = 1;
const short N = 11;

class proces{
    static int num;
    public: 
        const short pid;
        uint16_t tablicaPrevodenja[16] ={0};
    proces(): pid(num++){
        std::cout <<num<<std::endl;
    }; 

};
uint16_t generirajLogAdresu(){
    srand(time(NULL));
    return (rand()%1024) & 1022;
}
int proces::num;

int main(){
    //treba mi tablica prevođenja koja bu bila class s tri elementa
    //zbog nekog razloga mi treba disk, ali s njega samo uzimam

    proces procesi[N];
    uint16_t okviri[M];

    uint16_t t =0;
    int indexOkvir =0;
    for(int i=0; i<N ;i++){

    uint16_t logAdresa = generirajLogAdresu();
    uint8_t indexRedakTablice = logAdresa >> 6; //treba nam samo višim 4 bita od 10 adrese
    uint8_t fizAdresa;
    uint16_t redakTablice = procesi[i].tablicaPrevodenja[indexRedakTablice];
    uint8_t redakTab;
    if(redakTablice & 0x20 ==0){ //provjeravamo jeli na 6 bitu nula
        std::cout << "Promasaj\n";
        if(okviri[indexOkvir] ==0){
            std::cout <<"Dodjeljujem "<<indexOkvir<<" okvir.\n";
            indexOkvir = (indexOkvir+1)%M;
            //implementirati čitanje iz diska
            okviri[indexOkvir] = 0xffff; //upisi stranicu na okvir
             redakTab = ((indexOkvir <<6 ) | (t)) | 0x20;
        }else{
            int min= INT32_MAX;
            int indexMin=-1;
            for(int j=0; j<16; j++){ //ide kroz cijelu tablicu prevođenja i traži min lru
                int lru = (procesi[i].tablicaPrevodenja[j] & 0x1f);
                if(lru <= min){
                    min = lru;
                    indexMin=j;
                }
            }
            std::cout <<"izbacujem nekoga\n";
        }
        
        // pronadi i dodjeli okvir ()

    }
    else{
        fizAdresa = (redakTablice & 0x3f) | (logAdresa & 0x3f ); 
        redakTab= (procesi[i].tablicaPrevodenja[indexRedakTablice] | t) | 0x20; //valjda
    }
    procesi[i].tablicaPrevodenja[indexRedakTablice] = redakTab;
    t=(t+1)%32; //treba provjeriti jel 32
    //postavi LRU metapodataka na t
}
}