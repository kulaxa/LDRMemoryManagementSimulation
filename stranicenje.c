#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>


int N;
int M;
int t; //vrijednost se koristi kao sat

void init_tablica (short *tablica, int p)
{
    //postavimo sve vrijednosti na 0, jos niti jedna stranica nije ucitana u RAM
    for (int i = 0; i < 16; i++) {
        *((tablica + p*N) + i) = 0;
    }
}

int generiraj_logicku_adresu (void)
{
    int x;
    x = rand() & 0xff;
    x |= (rand() & 0xff) << 8;
    x |= (rand() & 0xff) << 16;
    x |= (rand() & 0xff) << 24;
    return x & 0x3fe;
}

int prisutnost_adrese (short redak_u_tablici)
{
    int bit_prisutnosti =  redak_u_tablici & 0x20;
    if(bit_prisutnosti) {
        //bit_prisutnosti je 1, stranica se nalazi u okviru u RAM-u
        return 1;
    } else return 0;
}

void resetiraj_t_i_tabl(int r, short *tablica) {
    t = 0;
    printf("Tu sam\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 16; j++) {
            *((tablica + i*N) + j) = *((tablica + i*N) + j) & (0x0);
            if (j == r) {
                *((tablica + i*N) + j) = 1;
            }
        }
    }
}

int pronadi_i_dodijeli_okvir (short *tablica, char *okvir, char *disk, int pomak) //LRU
{
    int LRU_min = 999;
    int LRU;
    int adresa;
    int proces = 0;
    int index_tablice = 0;
    int br = 0;
    //provjera jel postoji prazan okvir
    for (int i = 0; i < M && !br; i++) {
        for (int j = 0; j < 64; j++) {
            if(*(okvir + i + j) != 0) {
                br = 1;
                break;
            }
            else {
                adresa = i*64;
            }
        }
    }
    if(!br) {
        //printf("Sve je 0\n");
        return adresa;
    }

    for (int p = 0; p < N; p++) {
        for (int i = 0; i < 16; i++) {
            printf("%d", *((tablica+p*N)+i));
            if ((*((tablica + p*N) + i) & 0x20) == 0) {
                br = 0;
                int adr = (*((tablica + p*N) + i) & 0xffc0);
                for (int j = 0; j < 64; j++) {
                    if (okvir[adr + j] != 0) {
                        br = 1;
                        break;
                    }
                }
                if(!br) {
                    printf("p = %d, i = %d, ako je bit prisutnosti 0, mozemo dodijeliti taj okvir, ako je on prazan\n", p, i);
                    return adr;
                }
            }
            
            //inace trazimo okvir koji smo najdalje u proslosti koristili, LRU metapodatak mu je najmanji
            LRU = *((tablica + p*N) + i) & 0x1f;
            /*if (LRU == 31) {
                resetiraj_t_i_tabl(i, tablica);
                return 
            } else*/
            
            if (LRU_min > LRU && (*((tablica + p*N) + i) & 0x20)) { 
                //printf("Tu sam\n");
                LRU_min = LRU;
                adresa = (*((tablica + p*N) + i) & 0xffc0);
                proces = p;
                index_tablice = i;
            }
        
        }

    }

    printf("-->Izbacujem stranicu 0x%04X iz procesa %d\n", (index_tablice << 6), proces);
    //printf("Prije: %X\n", *((tablica + proces*N) + index_tablice));
    *((tablica + proces*N) + index_tablice) = *((tablica + proces*N) + index_tablice) & ~(0x20); //maknemo bit prisutnosti
    //printf("Poslije: %X\n", *((tablica + proces*N) + index_tablice));
    //pohrani na disk
    *((disk + proces*N) + index_tablice*16 + pomak) = okvir[adresa + pomak];
    //printf("Izbacena stranica na disku: %d\n", *((disk + proces*N) + index_tablice + pomak));

    printf("-->LRU izbacene stranice: 0x%04X\n", LRU_min);
    return adresa;
}

void update_tablica_promasaj (short *tablica_redak, int adr_okvira, int i, short *tablica)
{
    //dodati adresu okvira, postaviti bit prisutnosti i LRU metapodatak
    *tablica_redak = (adr_okvira << 6) + 0x20 + t;
    if (t == 32) {
        resetiraj_t_i_tabl(i, tablica);
    }
    //printf("Bit prisutnosti nakon update-a: %d\n", ((*tablica_redak & 0x20) >> 5));
}

void update_tablica_pogodak (short *tablica_redak, int i, short *tablica) 
{
    //promijeniti LRU metapodatak
    if (t == 32) {
        resetiraj_t_i_tabl(i, tablica);
    } else {
        *tablica_redak &= ~(0x1f); //izbrisemo postojeci LRU metapodatak
        *tablica_redak += t; //zapisemo t na mjesto LRU metapodatka
        *tablica_redak += 0x20; //dodamo bit prisutnosti
    }
}

int log_u_fiz_adr(int x, short redak_u_tablici)
{
    int pomak = x & 0x3f;
    int fiz_adr_okvir = redak_u_tablici & 0xffc0;

    return fiz_adr_okvir + pomak;
}

int main (int argc, char *argv[]) //argv[1] = N, argv[2] = M
{
    //ulazni parametri N-broj procesa u sustavu i M-broj okvira u sustavu
  //   N = (int) *argv[1] - '0';
  //  M = (int) *argv[2] - '0';
    N=2;
    M=1;
    //printf("broj=%d, N = %d, M = %d\n", argc, N, M);

    char disk[N][16][64]; //Simulirani disk koji služi za pohranu sadržaja stranica, 16 stranice po 64 okteta, a 1 char je 1 oktet
    char okvir[M*64]; //Simulirani radni spremnik od M okvira veličine 64 okteta
    short tablica[N][16]; //Tablica prevođenja za svaki od N procesa, 16 jer se koriste 4 bita za adresiranje, a zapis u tablici ima 16 bita = 1 short

    //postavimo sve vrijednosti u disku na 0
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 16; j++) {
            for (int k = 0; k < 64; k++) {
                disk[i][j][k] = 0;
            }
        }
    }
    //postavimo sve vrijednosti okvira na 0
    for (int i = 0; i < M*64; i++) {
        okvir[i] = 0;
    }

    int proces[N];

    for (int i = 0; i < N; i++) {
        proces[i] = i; //stvori proces i, ovo treba nekak popraviti!!
        init_tablica(&tablica[i][0], i); //inicijaliziraj tablicu straničenja procesa i;
    }

    t = 0;

    srand((unsigned) time(NULL));
    int x = generiraj_logicku_adresu();

    while (1) {
        for (int i = 0; i < N; i++) {
            printf("Proces: %d\n", proces[i]);
            printf("t: %d\n", t);
            //int x = generiraj_logicku_adresu(); //generiraj nasumičnu logičku adresu
            printf("Log. adresa: 0x%04X\n", x);
            int index_tablice_stranicenja = (x & 0x3c0) >> 6;
            int pomak = (x & 0x3f);
            char s;
            if (!prisutnost_adrese(tablica[i][index_tablice_stranicenja])) { //saljemo vrijednost retka
                printf("Promasaj!\n");
                int dodijeljen_okvir_index = pronadi_i_dodijeli_okvir(&tablica[0][0], &okvir[0], &disk[0][0][0], pomak); 
                printf("-->Dodijeljen okvir 0x%04X\n", dodijeljen_okvir_index);
                s = disk[i][index_tablice_stranicenja][pomak]; //s = *((disk + i*N) + index_tablice_stranicenja*16 + pomak);
                update_tablica_promasaj(&tablica[i][index_tablice_stranicenja], dodijeljen_okvir_index, index_tablice_stranicenja,  &tablica[0][0]);
                okvir[dodijeljen_okvir_index + pomak] = s + 1; //spremi inkrementiran sadrzaj
                //printf("index: %d\n", dodijeljen_okvir_index + pomak);
                //printf("sadrzaj okvira nakon ++: %d\n", okvir[dodijeljen_okvir_index + pomak]);
                //printf("Tablica za p 0: %X\n", tablica[0][index_tablice_stranicenja]);
                //printf("Tablica za p 1: %X\n", tablica[1][index_tablice_stranicenja]);
            }
            else {
                printf("Pogodak!\n");
                printf("Adresa okvira: 0x%04X\n", ((tablica[i][index_tablice_stranicenja] & 0xffc0) >> 6));
                update_tablica_pogodak(&tablica[i][index_tablice_stranicenja], index_tablice_stranicenja, &tablica[0][0]);
                int okvir_u_kojem_je = ((tablica[i][index_tablice_stranicenja] & 0xffc0) >> 6);
                s = okvir[okvir_u_kojem_je + pomak];
                okvir[okvir_u_kojem_je + pomak] = s + 1; //spremi inkrementiran sadrzaj
            }
            int y = log_u_fiz_adr(x, tablica[i][index_tablice_stranicenja]); 
            printf("Fiz. adresa: 0x%04X\n", y);
            printf("Zapis tablice: 0x%04X\n", tablica[i][index_tablice_stranicenja]);
            printf("Sadrzaj adrese: %d\n", s);
            t++;
            sleep(1);
            printf("\n");
        }

    }



    return 0;
}