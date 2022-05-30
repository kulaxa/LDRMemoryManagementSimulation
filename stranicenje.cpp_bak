#include <iostream>
#include <unistd.h>

const short M = 32;
const short N = 2;

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
    srand(time(NULL));
    return (rand() % 1024) & (1022);
}

int nadiPrviPrazni(uint8_t okviri[][16])
{
    for (int i = 0; i < M; i++)
    {
        bool empty = true;
        for (int j = 0; j < 16; j++)
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
    uint8_t okviri[M][16] = {0};
    uint8_t disk[N][16];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            disk[i][j] = rand();
        }
    }

    uint16_t t = 0;
    int indexOkvir = 0;
    while (1)
    {
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
            uint16_t logAdresa = generirajLogAdresu();
            printf("\t log. adresa: 0x%.4x\n", logAdresa);

            uint8_t indexRedakTablice = logAdresa >> 6; // treba nam samo višim 4 bita od 10 adrese
            uint8_t fizAdresa;
            uint16_t redakTablice = procesi[i].tablicaPrevodenja[indexRedakTablice];
            printf("\t index redka tablice: 0x%.4x\n", (int)indexRedakTablice);
            uint8_t redakTab;
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
                        if(z != i){
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
                    }
                    indexOkvir = (procesi[indexIzbacenog].tablicaPrevodenja[indexMin] >> 6) %M;
                    std::cout << "\t\tizbacujem proces : " << indexIzbacenog << "\n";
                    for(int j=0; j<16;j++){
                        if((procesi[indexIzbacenog].tablicaPrevodenja[j] >>6) == indexOkvir){
                           procesi[indexIzbacenog].tablicaPrevodenja[indexMin] = procesi[indexIzbacenog].tablicaPrevodenja[indexMin] & 0;

                        }
                    }
                    for (int j = 0; j < 16; j++)
                    {
                       disk[indexIzbacenog][j] = okviri[indexOkvir][j]; // spremanje stranice na disk
                    }
                }
                printf("\t\t Dodjeljujem okvir 0x%.4x\n", indexOkvir);
                for (int j = 0; j < 16; j++)
                {
                    okviri[indexOkvir][j] = disk[i][j]; // učitavanje stranice s diska
                }
                redakTab = ((indexOkvir << 6) | (t)) | 0x20;
                fizAdresa = (redakTab & 0xffc0) | (logAdresa & 0x3f);
            }
            else
            {
                std::cout << "Pogodak!\n";
                fizAdresa = (redakTablice & 0xffc0) | (logAdresa & 0x3f);
                redakTab = (procesi[i].tablicaPrevodenja[indexRedakTablice] | t) | 0x20;
            }

            printf("\tFizička adresa: 0x%.4x\n", (int)fizAdresa);
            printf("\tZapis tablice: 0x%.4x\n", (int)redakTab);
            procesi[i].tablicaPrevodenja[indexRedakTablice] = redakTab;
            t++; 
            // postavi LRU metapodataka na t
            uint8_t sadrzajAdrese = okviri[(fizAdresa >> 6) & 0x3ff][(fizAdresa & 0x3f)];
            printf("\tSadrzaj adrese: 0x%.4x\n", (int)sadrzajAdrese);
            std::cout << "--------------------------------\n";
            sleep(1);
        }
    }
}