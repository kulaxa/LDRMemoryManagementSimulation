#include <iostream>
#include <unistd.h>

const short M = 1;
const short N = 2;
const short velicinaOkvira = 32;
class proces
{
    static int num;

public:
    const short pid;
    uint16_t tablicaPrevodenja[16] = {0};
    proces() : pid(num++){};
};
uint16_t generirajLogAdresu()
{
    return (rand() % 1024) & (0x3fe);
}

int nadiPrviPrazni(uint16_t okviri[][velicinaOkvira])
{
    for (int i = 0; i < M; i++)
    {
        bool empty = true;
        for (int j = 0; j < velicinaOkvira; j++)
        {
            if (okviri[i][j] != 0)
            {
                empty = false;
                break;
            }
            if (empty)
            {
                return i;
            }
        }
    }
    return -1;
}
int proces::num;

int main()
{

    srand(time(NULL));
    proces procesi[N];
    uint16_t okviri[M][velicinaOkvira] = {0};
    uint16_t disk[N][16][velicinaOkvira];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            for(int k=0; k<velicinaOkvira; k++){
            disk[i][j][k] =1;
            }
        }
    }

    uint16_t t = 0;
    int indexOkvir = 0;
    //for(int ii=0; ii<100000; ii++){
    while (1){
        for (int i = 0; i < N; i++)
        {
            if (t == 32)
            {
                t = 0;
                for (int k = 0; k < N; k++)
                {
                    for (int l = 0; l < 16; l++)
                    {
                        procesi[k].tablicaPrevodenja[l] = procesi[k].tablicaPrevodenja[l] & 0xffe0;
                    }
                }
            }
            std::cout << "Proces: " << i << "\n";
            std::cout << "\t t: " << t << "\n";
           // uint16_t logAdresa = 785;
           u_int16_t logAdresa = (rand() % 1024) & (0x3fe);
            printf("\t log. adresa: 0x%.4x\n", logAdresa);

            uint8_t indexRedakTablice = logAdresa >> 6; // treba nam samo višim 4 bita od 10 adrese
            uint16_t fizAdresa;
            uint16_t redakTablice = procesi[i].tablicaPrevodenja[indexRedakTablice];
            printf("\t index redka tablice: 0x%.4x\n", (int)indexRedakTablice);
            if ((redakTablice & 0x20) == 0)
            { // provjeravamo jeli na 6 bitu nula
                std::cout << "\t Promasaj!\n";
                indexOkvir = nadiPrviPrazni(okviri);
                if (indexOkvir == -1)
                {
                    int min = INT32_MAX;
                    int indexMin = -1;
                    int indexIzbacenog = -1;
                    for (int z = 0; z < N; z++)
                    {
                       // if(z != i){
                        for (int j = 0; j < 16; j++)
                        { // ide kroz cijelu tablicu prevođenja i traži min lru
                            if (((procesi[z].tablicaPrevodenja[j] >> 5) & 1) == 1)
                            {
                                int lru = (procesi[z].tablicaPrevodenja[j] & 0x1f);
                                if (lru <= min)
                                {
                                    min = lru;
                                    indexMin = j;
                                    indexIzbacenog = z;
                                }
                            }
                        }
                        }
                   // }
                    indexOkvir = (procesi[indexIzbacenog].tablicaPrevodenja[indexMin] >> 6) %M;
                    std::cout << "\t\tizbacujem proces : " << indexIzbacenog << "\n";
                    for(int j=0; j<16;j++){
                        if((procesi[indexIzbacenog].tablicaPrevodenja[j] >>6) == indexOkvir){
                           procesi[indexIzbacenog].tablicaPrevodenja[indexMin] = procesi[indexIzbacenog].tablicaPrevodenja[indexMin] & 0;

                        }
                    }
                    for (int j = 0; j < velicinaOkvira; j++)
                    {
                       disk[indexIzbacenog][indexMin][j] = okviri[indexOkvir][j]; // spremanje stranice na disk
                       
                    }
                }
                printf("\t\t Dodjeljujem okvir 0x%.4x\n", indexOkvir);
                for (int j = 0; j < velicinaOkvira; j++)
                {
                    okviri[indexOkvir][j] = disk[i][indexRedakTablice][j]; // učitavanje stranice s diska
                }
                redakTablice = ((indexOkvir << 6) | (t)) | 0x20;
                fizAdresa = (redakTablice & 0xffc0) | (logAdresa & 0x3f);
            }
            else
            {
                std::cout << "Pogodak!\n";
                fizAdresa = (redakTablice & 0xffc0) | (logAdresa & 0x3f);
                redakTablice = (procesi[i].tablicaPrevodenja[indexRedakTablice] | t) | 0x20;
            }

            printf("\tFizička adresa: 0x%.4x\n", (int)fizAdresa);
            printf("\tZapis tablice: 0x%.4x\n", (int)redakTablice);
            procesi[i].tablicaPrevodenja[indexRedakTablice] = redakTablice;
            t++; 
            // postavi LRU metapodataka na t
            uint16_t sadrzajAdrese = okviri[(fizAdresa >> 6) & 0x3ff][(fizAdresa & 0x3f)/2]++ - 1;
            printf("\tSadrzaj adrese: 0x%.4x\n", (int)sadrzajAdrese);
            //okviri[(fizAdresa >> 6) & 0x3ff][(fizAdresa & 0x3f)]++;
               // disk[i][indexRedakTablice][(fizAdresa & 0x3f)] =  sadrzajAdrese+1;
                for(int p=0; p<N ;p++){
                    for(int c=0; c<32; c++){
                        for(int v=0; v<32; v++){
                            if(disk[p][c][v] == 2){
                               // std::cout <<"JEEEES \n";
                            }
                        }
                    }
                }
                
            std::cout << "--------------------------------\n";
            sleep(1);
        }
    }
}