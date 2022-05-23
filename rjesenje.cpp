#include <iostream>
#include <vector>
#include <unistd.h>

const int N = 5;
const int M = 1;

class okvir{
    public:
    int poljeOkvir[64] ={-1};
    okvir(){};
    //okvir(int brojOkvira) :{};
};
class stranica{
    public:
    int poljeStranica[64] ={-1};
    //okvir(int brojOkvira) :{};
};



class proces{
    public:
      int pid;

      proces(int pid): pid(pid){};
      proces(){};

      void pokreni_proces(){
          sleep(5);
      }
       
};

class redakTablPrev{
   public:
     int redak[6]={0};


     friend std::ostream& operator<<(std::ostream& os, const  redakTablPrev redakTab)
{
    os<<"{";
   for(int i=0; i<6; i++){
       if(i!=5)
       os << redakTab.redak[i] <<",";
       else os << redakTab.redak[i] ;
   }
   os <<"}";
    return os;
} 
     
};

int firstEmpty(uint16_t okvirRam[]){
    for(int i=0; i<M; i++){
        if(okvirRam[i]==0){
            return i;
        }
    }
    return -1;
}


int main(){


   uint16_t okvirRam[M];
   for( int i=0; i<M; i++){
       okvirRam[i] =  0;
   }
   uint16_t disk[N];
   for(int i=0; i<N; i++){
       disk[i] = rand();
   }
   //std::cout <<okvirRam[10].poljeOkvir <<std::endl;

   proces tablica[N];

    u_int16_t tablicaPrev[16];


//  disk[N] - Simulirani disk koji služi za pohranu sadržaja stranica,
// • okvir[M] - Simulirani radni spremnik od M okvira veličine 64 okteta,
// • tablica[N] - Tablica prevođenja za svaki od N procesa


    for(int i=1; i<=N ; i++){
        proces p(i);
        tablica[i] = p;

    }

    int i=0;
    
    //while(1){
        
        for(int i=1; i<=N ; i++){
            
            int x= (rand()%1024) & 1022;
            std::cout << x<<std::endl;
            int redakT  = x >> 6;
            //std:: cout << "redak: "<<redakT <<std::endl;
            //std::cout << tablicaPrev[redakT] <<std::endl;
            if(tablicaPrev[redakT] >> 11 == 0){
                std::cout <<"promašaj\n";
                int ind = firstEmpty(okvirRam);
                if(ind == -1){
                    std::cout <<"izbacujem okvir \n";
                    ind = 0;
                }
                else{
                     tablicaPrev[redakT] = (ind<<6)+64;
                     std::cout <<"Dodjeljen okvir: "<< ind <<"\n";
                     
                }
                okvirRam[ind] = disk[i];
                
               
            }

            std::cout <<"Zapis tablice: "<<tablicaPrev[redakT] << "\n";
            
        }
    //}

//     za i = 1 do N čini
// 2 stvori proces i;
// 3 inicijaliziraj tablicu straničenja procesa i;
// 4 kraj
// 5 t ← 0;
// 6 ponavljaj
// 7 za svaki proces p čini
// 8 x ← nasumična logička adresa;
// 9 nađi zapis tablice straničenja procesa p za adresu x;
// 10 ako adresa x nije prisutna onda
// 11 ispiši promašaj;
// 12 pronađi i dodijeli okvir;
// 13 učitaj sadržaj stranice s diska;
// 14 ažuriraj tablicu prevođenja procesa p;
// 15 kraj
// 16 ispiši adresu x, adresu njenog okvira te sadržaj zapisa u
// tablici prevođenja;
// 17 dohvati sadržaj adrese x;
// 18 inkrementiraj dohvaćeni sadržaj i zapiši ga na adresu x;
// 19 t ← t + 1;
// 20 spavaj;
// 21 kraj
// }
   
  
}