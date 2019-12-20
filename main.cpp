#define _CRT_SECURE_NO_WARNINGS

/******************************* BIBLIOTEKI ***********************************************/
#include <iostream>

using namespace std;

#include <math.h>
#include <fstream>
#include <string>


/******************************* STRUKTURY ***********************************************/
// opis pojedynczego indeksu
struct Index {
    int key; // klucz danych
    int page; // numer strony, na ktorej znajduja sie dane
};

// opis pojedynczego rekordu (wiersza) w pliku z danymi
struct Record {
    int key; // klucz
    double probabilityA, probabilityB, probabilityAB; // rekord - napiecie i natezenie
    int pointer; // miejsce z kolejnym rekordem

    // konstruktor
    Record() {
        key = -1;
        probabilityA = 0.0;
        probabilityB = 0.0;
        probabilityAB = 0.0;
        pointer = -1;
    }
};

/******************************* ZMIENNE GLOBALNE  ***********************************************/
const char dataFile[] = {"dane"}; // nazwa pliku z danymi
const char indexFile[] = {"indeksy"}; // nazwa pliku z indeksami
const int pageSize = 10; // rozmiar strony w pliku z danymi
const double alfa = 0.5; // wspolczynnik alfa
int primaryRecords = 0; // ilosc rekordow w primary area
int overflowRecords = 0; // ilosc rekordow w overflow area
int maxPrimary = 10; // maksymalna liczba rekordow w primary area
int maxOverflow = maxPrimary / 5; // maksymalna liczba rekordow w overflow area
/******************************* DEKLARACJE FUNKCJI ***********************************************/
void showIndex();

void createIndex(int &, int &);

int searchIndex(int, int &);

void sort(Record[], int);

void insert(int, double, double, double, int &, int &);

void initValues();

void read(int);

void showFile();

void reorganize(int &, int &);

bool remove(int, int &, int &);

void update(int, double, double, double);

/******************************* MAIN ***********************************************/
int main() {
    initValues(); // inicjujemy wartosci poczatkowe

    // zmienne pomocnicze
    bool esc = true;
    char operation;
    int key, readCount, writeCount;
    double probabilityA, probabilityB, probabilityAB;

    cout << "Skad wczytywac dane?\nk - klawiatura\np - plik\n";
    cin >> operation;
    // jezeli wczytujemy instrukcje z klawiatury
    if (operation == 'k') {
        while (esc) // dopoki nie koniec
        {
            readCount = writeCount = 0;
            cout << "Co chcesz zrobic?\n" << "d - dodawanie rekordu\n" << "a - aktualizacja rekordu\n"
                 << "o - odczyt rekordu\n" << "u - usuwanie rekordu\n" << "r - reorganizacja\n"
                 << "p - przegladaj plik\n" << "q - przegladaj indeks\n" << "k - koniec\n";
            cin >> operation;
            switch (operation) {
                case 'd': // wstawiamy nowy rekord
                    cout << "Key: ";
                    cin >> key;
                    cout << "P(A): ";
                    cin >> probabilityA;
                    cout << "P(B): ";
                    cin >> probabilityB;
                    cout << "P(AB): ";
                    cin >> probabilityAB;
                    insert(key, probabilityA, probabilityB, probabilityAB, readCount, writeCount);
                    cout << "DODAWANIE\nOdczytow = " << readCount << " Zapisow = " << writeCount << endl;
                    break;
                case 'a': // aktualizujemy rekod
                    cout << "Podaj klucz rekordu, ktory chcesz zmienic:\n";
                    cin >> key;
                    cout << "P(A): ";
                    cin >> probabilityA;
                    cout << "P(B): ";
                    cin >> probabilityB;
                    cout << "P(AB): ";
                    cin >> probabilityAB;
                    update(key, probabilityA, probabilityB, probabilityAB);
                    break;
                case 'o': // odczytujemy rekord
                    cout << "Podaj klucz:\n";
                    cin >> key;
                    read(key);
                    break;
                case 'u': // usuwamy rekord
                    cout << "Podaj klucz:\n";
                    cin >> key;
                    if (!remove(key, readCount, writeCount))
                        cout << "Nie ma rekordu z podanym kluczem\n";
                    cout << "USUWANIE REKORDU\n";
                    cout << "Odczytow = " << readCount << " Zapisow = " << writeCount << endl;
                    break;
                case 'r': // reorganizujemy plik na zadanie
                    reorganize(readCount, writeCount);
                    break;
                case 'p': // wyswietlamy plik danych wg kolejnosci kluczy
                    showFile();
                    break;
                case 'q': // wyswietla plik indeksow
                    showIndex();
                    break;
                case 'k': // koniec
                    esc = false;
                    break;
                default:
                    cout << "Musisz wybrac jedna z opcji\n";
                    break;
            }
        }
    } else {
        // wczytujemy nazwe pliku
        int i;
        string str;
        char filePath[80];
        cout << "Podaj sciezke do pliku:\n";
        getline(cin, str);
        getline(cin, str);
        // zmieniamy string na char[]
        for (i = 0; i < str.length(); i++)
            filePath[i] = str[i];
        filePath[i] = '\0';

        // otwieramy plik testowy
        ifstream test;
        test.open(filePath);

        while (esc) // dopoki nie koniec
        {
            readCount = writeCount = 0;
            test >> operation;
            switch (operation) {
                case 'd': // wstawiamy rekord
                    test >> key >> probabilityA >> probabilityB >> probabilityAB;
                    insert(key, probabilityA, probabilityB, probabilityAB, readCount, writeCount);
                    cout << "DODAWANIE\nOdczytow = " << readCount << "Zapisow = " << writeCount << endl;
                    break;
                case 'a': // aktualizujemy rekord
                    test >> key >> probabilityA >> probabilityB >> probabilityAB;
                    update(key, probabilityA, probabilityB, probabilityAB);
                    break;
                case 'o': // odczytujemy rekord
                    test >> key;
                    read(key);
                    break;
                case 'u': // usuwamy rekord
                    test >> key;
                    cout << "USUWANIE REKORDU\n";
                    if (!remove(key, readCount, writeCount))
                        cout << "Nie ma rekordu z podanym kluczem\n";
                    cout << "Odczytow = " << readCount << " Zapisow = " << writeCount << endl;
                    break;
                case 'r': // reorganizujemy na zadanie
                    reorganize(readCount, writeCount);
                    cout << "REORGANIZACJA\n";
                    cout << "Odczytow = " << readCount << " Zapisow = " << writeCount << endl;
                    break;
                case 'p': // wyswietla plik danych wg kolejnosci klucza
                    showFile();
                    break;
                case 'q': // wyswietla plik indeksow
                    showIndex();
                    break;
                case 'k': // koniec
                    esc = false;
                    break;
                default:
                    cout << "Musisz wybrac jedna z opcji\n";
                    break;
            }
        }
        test.close(); // zamykamy plik testowy
    }
    system("pause");
    return 0;
}

/******************************* DEFINICJE FUNKCJI ***********************************************/
void read(int key) {
    int readCount = 0, pointer, i;
    cout << "ODCZYTYWANIE\n";
    int page = searchIndex(key, readCount); // szukamy odpowiedniej strony
    Record *buffer = new Record[pageSize]; // tworzymy bufor danych
    FILE *file = fopen(dataFile, "rb"); // otwieramy plik
    fseek(file, page * sizeof(Record), SEEK_SET); // ustawiamy kursor w pliku
    fread(buffer, sizeof(Record), pageSize, file); // wczytujemy odpowiednia strone
    readCount++;

    // przeszukujemy wczytana strone
    for (i = 0; i < pageSize; i++) {
        // jezeli klucz jest mniejszy zmieniamy wskaznik i szukamy dalej
        if (buffer[i].key < key)
            pointer = buffer[i].pointer;
            // jezeli klucz jest wiekszy badz rowny to konczymy
        else
            break;
    }

    // jezeli znalezlismy
    if (buffer[i].key == key)
        cout << buffer[i].key << " " << buffer[i].probabilityA << " " << buffer[i].probabilityB << " "
             << buffer[i].probabilityAB << endl;
        // jezeli nie ma rekordu z podanym kluczem
    else if (pointer == -1)
        cout << "Nie ma rekordu o podanym kluczu\n";
        // przeszukujemy overflow
    else {
        // przeskakujemy primary area i przechodzimy do overflow area
        fseek(file, maxPrimary * sizeof(Record), SEEK_SET);
        // wczytujemy strony z overflow
        while (fread(buffer, sizeof(Record), pageSize, file) > 0) {
            readCount++;
            // przeszukujemy wczytana strone
            for (i = 0; i < pageSize; i++) {
                if (buffer[i].key < key)
                    pointer = buffer[i].pointer;
                else
                    break;
            }
        }
        // jezeli znalezlismy
        if (buffer[i].key == key)
            cout << buffer[i].key << " " << buffer[i].probabilityA << " " << buffer[i].probabilityB << " "
                 << buffer[i].probabilityAB << endl;
            // jezeli nie ma rekordu z podanym kluczem
        else
            cout << "Nie ma rekordu o podanym kluczu\n";
    }
    fclose(file); // zamykamy plik
    delete[] buffer; // zwalniamy bufor
    cout << "Odczytow = " << readCount << endl;
}

/**************************************************************************************************/
void initValues() // inicjujemy wartosci poczatkowe
{
    Record *buffer = new Record[pageSize]; // tworzymy plik danych
    FILE *file = fopen(dataFile, "wb"); // otwieramy plik
    int readCount = 0, writeCount = 0;

    // tworzymy plik danych zapisujac go 'pustymi' rekordami
    int i = (int) ceil(((double) (maxPrimary + maxOverflow)) / (double) pageSize);
    while (i--) {
        fwrite(buffer, sizeof(Record), pageSize, file);
        writeCount++;
    }
    delete[] buffer; // usuwamy tablice
    fclose(file); // zamykamy plik
    createIndex(readCount, writeCount); // tworzymy indeks na podstawie utworzonych rekordow

    // wyswietlamy statystyki
    cout << "USTAWIENIE POCZATKOWYCH WARTOSCI\n";
    cout << "Odczytow = " << readCount << " Zapisow = " << writeCount << endl;

    // zmieniamy pierwszy rekord aby mial najmniejszy mozliwy klucz
    //insert(1, 0.0, 0.999, 0.001, readCount, writeCount);
}

/**************************************************************************************************/
void insert(int key, double probabilityA, double probabilityB, double probabilityAB, int &readCount,
            int &writeCount) // wstawiamy nowy rekord
{
    // jesli overflow jest pelne to reorganizujemy plik
    if (overflowRecords == maxOverflow)
        reorganize(readCount, writeCount);


    // zmienne pomocnicze
    int pointer, i, j, page;
    double pomoc, zmianaIndeksow = false;

    Record *buffer = new Record[pageSize]; // bufor pliku danych
    FILE *readFile = fopen(dataFile, "rb"); // plik danych
    FILE *writeFile = fopen("dane2", "wb"); // plik tymczasowy
    page = searchIndex(key, readCount); // szukamy odpowiedniej strony

    // przepisujemy wszystkie strony z primary area do pliku tymczasowego (do odpowiedniej strony)
    for (i = 0; i < page; i++) {
        fread(buffer, sizeof(Record), pageSize, readFile);
        readCount++;
        fwrite(buffer, sizeof(Record), pageSize, writeFile);
        writeCount++;
    }

    // wczytujemy strone do ktorej mamy dodac rekord
    fread(buffer, sizeof(Record), pageSize, readFile);
    readCount++;

    // na stronie sa wolne miejsca
    if (buffer[pageSize - 1].key == -1) {
        // wstawiamy nowy rekord
        for (i = 0; i < pageSize; i++) {
            // klucz juz istnieje
            if (buffer[i].key == key) {
                cout << "Rekord o podanym kluczu juz istnieje\n";
                fclose(readFile);
                fclose(writeFile);
                remove("dane2");
                delete[] buffer;
                return;
            }
                // dodajemy do pustej strony
            else if (buffer[i].key == -1) {
                // jesli zmieniamy pierwszy rekord na stronie to aktualizujemy indeks
                if (i == 0) zmianaIndeksow = true;
                // zapisujemy na wolne miejsce
                buffer[i].key = key;
                buffer[i].probabilityA = probabilityA;
                buffer[i].probabilityB = probabilityB;
                buffer[i].probabilityAB = probabilityAB;
                buffer[i].pointer = -1;
                primaryRecords++;
                // sortujemy strone
                sort(buffer, i);
                // zapisujemy zmieniona strone
                fwrite(buffer, sizeof(Record), pageSize, writeFile);
                writeCount++;
                // przepisujemy reszte primary area
                for (i = 0; i < page; i++) {
                    fread(buffer, sizeof(Record), pageSize, readFile);
                    readCount++;
                    fwrite(buffer, sizeof(Record), pageSize, writeFile);
                    writeCount++;
                }
                // przepisujemy overflow
                i = (int) ceil((double) maxOverflow / (double) pageSize);;
                for (int j = 0; j < i; j++) {
                    fread(buffer, sizeof(Record), pageSize, readFile);
                    readCount++;
                    fwrite(buffer, sizeof(Record), pageSize, writeFile);
                    writeCount++;
                }
                delete[] buffer; // usuwamy tablice
                fclose(readFile); // zamykamy plik
                fclose(writeFile); // zamykamy plik
                remove(dataFile); // usuwamy plik danych
                rename("dane2", dataFile); // zmieniamy nazwe pliku tymczasowego - nowy plik danych
                break; // konczymy
            }
        }
        if (zmianaIndeksow) createIndex(readCount, writeCount);
    }
        // dodajemy do overflow
    else {
        for (i = 0; i < pageSize; i++) {
            // klucz juz istnieje
            if (buffer[i].key == key) {
                cout << "Rekord o podanym kluczu juz istnieje\n";
                fclose(readFile);
                fclose(writeFile);
                remove("dane2");
                delete[] buffer;
                return;
            }
                // znelezlismy odpowiedni rekord
            else if (buffer[i].key > key)
                break;
        }
        i--;
        // ustawiamy wskaznik
        pointer = buffer[i].pointer;
        if (buffer[i].pointer == -1 || buffer[i].pointer > key) buffer[i].pointer = key;

        // zapisujemy strone
        fwrite(buffer, sizeof(Record), pageSize, writeFile);
        writeCount++;

        // przepisujemy reszte primary area
        for (i = 0; i < page; i++) {
            fread(buffer, sizeof(Record), pageSize, readFile);
            readCount++;
            fwrite(buffer, sizeof(Record), pageSize, writeFile);
            writeCount++;
        }
        // przepisujemy overflow
        delete[] buffer;
        buffer = new Record[maxOverflow];
        fread(buffer, sizeof(Record), maxOverflow, readFile);
        readCount++;

        // dodajemy plik w na koncu overflow odpowiednio zmieniajac wskazniki innych rekordow po drodze
        for (j = 0; j < maxOverflow; j++) {
            // dodajemy na wolne miejsce w overflow
            if (buffer[j].key == -1) {
                buffer[j].key = key;
                buffer[j].probabilityA = probabilityA;
                buffer[j].probabilityB = probabilityB;
                buffer[j].probabilityAB = probabilityAB;
                buffer[j].pointer = pointer;
                sort(buffer, j);
                break;
            }
                // znalezlismy odpowiedni rekord
            else if (buffer[j].key == pointer) {
                // jezeli klucz znalezionego rekordu jest mniejszy
                if (buffer[j].key < key) {
                    // jezeli nie wskazuje na kolejny element to pokazuje na nowy rekord
                    if (buffer[j].pointer == -1) {
                        buffer[j].pointer = key;
                        pointer = -1;
                    }
                        // jezeli pokazuje na element wiekszy to zmieniamy wskaznik
                    else if (buffer[j].pointer > key) {
                        pointer = buffer[j].pointer;
                        buffer[j].pointer = key;
                    } else // jezeli pokazuje na element mniejszy to zmieniamy wskaznik i szukamy dalej
                    {
                        pointer = buffer[j].pointer;
                    }
                } else // jezeli klucz znalezionego rekordu jest wiekszy
                {
                    pointer = buffer[j].key;
                }
            }
        }
        overflowRecords++; // zwiekszamy ilosc rekordow w overflow
        fwrite(buffer, sizeof(Record), maxOverflow, writeFile); // zapisujemy overflow
        writeCount++;
        delete[] buffer; // usuwamy tablice
        fclose(readFile); // zamykamy plik
        fclose(writeFile); // zamykamy plik
        remove(dataFile); // usuwamy plik danych
        rename("dane2", dataFile); // plik tymczasowy zostaje nowym plikiem danych
    }
}

/**************************************************************************************************/
void sort(Record buffer[], int howMany) // sortujemy bufor o wielkosci i (babelkowo)
{
    // zmienne pomocnicze
    Record temp;
    bool swap;

    // przechodzmi po calym buforze i zmieniamy kolejnosc
    for (int j = howMany; j >= 0; j--) {
        swap = false;
        for (int i = 0; i < j; i++) {
            if (buffer[i].key > buffer[i + 1].key) // niewlasciwa kolejnosc
            {
                swap = true;
                temp = buffer[i];
                buffer[i] = buffer[i + 1];
                buffer[i + 1] = temp;
            }
        }
        if (!swap) break; // jesli nie bylo zmian konczymy
    }
}

/**************************************************************************************************/
int searchIndex(int key, int &readCount) // szukamy numeru strony z poszukiwanym kluczem
{
    Index *buffer = new Index[pageSize]; // bufor pliku indeksow
    FILE *file = fopen(indexFile, "rb"); // otwieramy plik indeksow
    int i = maxPrimary / pageSize, j, k, l, page = -1; // zmienne pomocnicze

    // wczytujemy indeksy stronami i poszukujemy odpowiedniego indeksu
    while (i--) {
        l = fread(buffer, sizeof(Index), pageSize, file);
        readCount++;
        for (k = 0; k < l; k++) {
            if (buffer[k].key > key) {
                page = buffer[k].page - 1;
                break;
            } else
                page = -1;
        }
        if (page != -1)
            break;
    }
    fclose(file); // zamykamy plik
    if (page == -1) page = buffer[l - 1].page;; // jesli primary area jest puste
    delete[] buffer; // usuwamy tablice

    return page; // zwracamy wartosc strony
}

/**************************************************************************************************/
void createIndex(int &readCount, int &writeCount) // tworzy indeks na podstawie rekordow w primary area
{
    int i = maxPrimary / pageSize, j, k; // ilosc indeksow
    Record *buffer1 = new Record[pageSize]; // bufor pliku danych
    Index *buffer2 = new Index[pageSize]; // bufor pliku indeksow
    FILE *file1 = fopen(dataFile, "rb"); // plik z danymi
    FILE *file2 = fopen(indexFile, "wb");

    // pobieramy stronami i zapisujemy pierwszy klucz na kazdej tronie do indeksu
    for (j = 0, k = 0; j < i; j++, k++) {
        if (k == pageSize) {
            fwrite(buffer2, sizeof(Index), k, file2);
            writeCount++;
            k = 0;
        }
        fread(buffer1, sizeof(Record), pageSize, file1);
        readCount++;
        buffer2[k].key = buffer1[0].key;
        buffer2[k].page = j;
    }
    fwrite(buffer2, sizeof(Index), k, file2);
    writeCount++;

    fclose(file1); // zamykamy plik
    fclose(file2);
    delete[] buffer1; // usuwamy bufor
    delete[] buffer2; // usuwamy bufor
}

/**************************************************************************************************/
void showIndex() // wyswietlamy plik indeksow
{
    int readCount = 0, k, l; // zmienne pomocnicze
    Index *buffer = new Index[pageSize]; // bufor pliku indeksow
    FILE *file = fopen(indexFile, "rb"); // otwieramy plik indeksow
    cout << "WYSWIETLENIE INDEKSU\n";
    // wczytujemy i wypisujemy indeksy
    while (true) {
        l = fread(buffer, sizeof(Index), pageSize, file);
        readCount++;
        if (l == 0) break;
        for (k = 0; k < l; k++)
            cout << buffer[k].key << " " << buffer[k].page << endl;
    }
    fclose(file); // zamykamy plik
    delete[] buffer; // usuwamy tablice
    cout << "Odczytow = " << readCount << endl; // wyswietlamy statystyki
}

/**************************************************************************************************/
void showFile() {
    int readCount = 0, i = maxPrimary / pageSize, j, k, l = 0, pointer; // zmienne pomiconicze
    Record *primaryBuffer = new Record[pageSize]; // bufor stron primary area
    Record *overflowBuffer = new Record[maxOverflow]; // bufor stron overflow area
    FILE *file = fopen(dataFile, "rb"); // plik danych

    // wczytujemy overflow do bufora
    fseek(file, maxPrimary * sizeof(Record), SEEK_SET);
    fread(overflowBuffer, sizeof(Record), maxOverflow, file);
    readCount++;

    fseek(file, 0, SEEK_SET); // przechodzimy na poczatek pliku
    cout << "WYSWIETLENIE PLIKU\n";
    for (j = 0; j < i; j++) {
        fread(primaryBuffer, sizeof(Record), pageSize, file); // wczytujemy strone z primary area
        readCount++;
        for (k = 0; k < pageSize; k++) {
            // wyswietlamy rekord z primary area
            cout << primaryBuffer[k].key << " " << primaryBuffer[k].probabilityA << " " << primaryBuffer[k].probabilityB
                 << " " << primaryBuffer[k].probabilityAB << endl;
            pointer = primaryBuffer[k].pointer;
            // jezeli wskaznik pokazuje na rekord w overflow to wypisujemy rekordy z overflow zgodnie z wskaznikami
            while (l < maxOverflow && overflowBuffer[l].key != -1) {
                if (overflowBuffer[l].key == pointer) {
                    cout << overflowBuffer[l].key << " " << overflowBuffer[l].probabilityA << " "
                         << overflowBuffer[l].probabilityB << " " << overflowBuffer[l].probabilityAB << endl;
                    pointer = overflowBuffer[l].pointer;
                }
                l++;
            }
        }
    }

    fclose(file); // zamykamy plik
    // zwalniamy bufory
    delete[] primaryBuffer;
    delete[] overflowBuffer;
    // wyswietlamy statystyki
    cout << "Odczytow = " << readCount << endl;
}

/**************************************************************************************************/
void reorganize(int &readCount, int &writeCount) {
    int counter = 0, i = maxPrimary / pageSize, j, k, l = 0, pointer, pages = 0; // zmienne pomocnicze
    Record *primaryBuffer = new Record[pageSize]; // bufor stron z primary area
    Record *overflowBuffer = new Record[maxOverflow]; // bufor stron z overflow area
    Record *writeBuffer = new Record[pageSize]; // bufor do zapisu
    FILE *readFile = fopen(dataFile, "rb"); // plika odczytu - plik danych
    FILE *writeFile = fopen("dane2", "wb"); // plik zapisu - nowy plik

    // wczytanie overflow
    fseek(readFile, maxPrimary * sizeof(Record), SEEK_SET);
    fread(overflowBuffer, sizeof(Record), maxOverflow, readFile);
    readCount++;
    fseek(readFile, 0, SEEK_SET);

    for (j = 0; j < i; j++) {
        fread(primaryBuffer, sizeof(Record), pageSize, readFile); // wczytanie strony z primary area
        readCount++;
        for (k = 0; k < pageSize; k++) {
            if (counter ==
                pageSize * alfa) // jezeli bufor zapisu jest zapelniony w stopniu alfa to zapisujemy go do pliku
            {
                for (int m = 0; m < counter; m++)
                    writeBuffer[m].pointer = -1;
                fwrite(writeBuffer, sizeof(Record), pageSize, writeFile);
                writeCount++;
                counter = 0;
                pages++;
            }
            writeBuffer[counter] = primaryBuffer[k];
            counter++;
            pointer = primaryBuffer[k].pointer;
            // jezeli rekord z primary wskazuje na strone w overflow
            while (pointer != -1 && l < maxOverflow) {
                if (counter ==
                    pageSize * alfa) // jezeli bufor zapisu jest zapelniony w stopniu alfa to zapisujemy go do pliku
                {
                    for (int m = 0; m < counter; m++)
                        writeBuffer[m].pointer = -1;
                    fwrite(writeBuffer, sizeof(Record), pageSize, writeFile);
                    writeCount++;
                    counter = 0;
                    pages++;
                }
                writeBuffer[counter] = overflowBuffer[l]; // zapisujemy rekord z overflow do bufora zapisu
                counter++;
                pointer = overflowBuffer[l].pointer;
                l++;
            }
        }
    }
    if (counter > 0) // jezeli w buforze zapisu cos sie znajduje
    {
        for (int m = 0; m < counter; m++)
            writeBuffer[m].pointer = -1;
        fwrite(writeBuffer, sizeof(Record), pageSize, writeFile);
        writeCount++;
        pages++;
    }
    // zwalniamy bufory
    delete[] primaryBuffer;
    delete[] overflowBuffer;
    delete[] writeBuffer;
    // zamykamy pliki
    fclose(readFile);
    fclose(writeFile);

    remove(dataFile); // usuwamy dotychczasowy plik z danymi
    rename("dane2", dataFile); // zmieniamy plik tymczasowy na plik danych

    // zmieniamy wartosci zmiennych
    maxPrimary = pages * pageSize;
    maxOverflow = maxPrimary / 2;
    primaryRecords += overflowRecords;
    overflowRecords = 0;

    createIndex(readCount, writeCount); // tworzymy nowy indeks na podstawie zmienionych wartosci
}

/**************************************************************************************************/
bool remove(int key, int &readCount, int &writeCount) {
    if (key == 0) {
        cout << "Nie mozna usunac rekordu z kluczem 0\n";
        return false;
    }
    // zmienne pomocnicze
    int i, j, pointer, page = searchIndex(key, readCount);
    bool removed = false;

    Record *primaryBuffer = new Record[pageSize]; // bufor pliku danych
    Record *overflowBuffer = new Record[pageSize];
    FILE *readFile = fopen(dataFile, "rb"); // plik danych
    FILE *writeFile = fopen("dane2", "wb"); // plik tymczasowy

    // wczytujemy overflow
    fseek(readFile, maxPrimary * sizeof(Record), SEEK_SET);
    fread(overflowBuffer, sizeof(Record), maxOverflow, readFile);
    readCount++;
    fseek(readFile, 0, SEEK_SET);

    // przepisujemy wszystkie strony z primary area do pliku tymczasowego (do odpowiedniej strony)
    for (i = 0; i < page; i++) {
        fread(primaryBuffer, sizeof(Record), pageSize, readFile);
        readCount++;
        fwrite(primaryBuffer, sizeof(Record), pageSize, writeFile);
        writeCount++;
    }

    // wczytujemy strone na ktorej znajduje sie szukany rekord
    fread(primaryBuffer, sizeof(Record), pageSize, readFile);
    readCount++;
    pointer = -1;

    // szukamy rekordu na stronie
    for (i = 0; i < pageSize; i++) {
        if (primaryBuffer[i].key == key) // jezeli znalezlismy klucz
        {
            if (primaryBuffer[i].pointer == -1) // jezeli rekord nie wskazuje na kolejny
            {
                // przesuwamy rekordy o 1 w gore
                for (; i < pageSize - 1; i++)
                    primaryBuffer[i] = primaryBuffer[i + 1];
                // zmieniamy wartosci ostatniego rekordu na stronie - jest wolny
                primaryBuffer[i].key = primaryBuffer[i].pointer = -1;
                primaryBuffer[i].probabilityA = primaryBuffer[i].probabilityB = primaryBuffer[i].probabilityAB = 0.0;
                removed = true;
                primaryRecords--;
            } else // jezeli rekord z primary wskazuje na rekord w overflow
            {
                for (j = 0; j < maxOverflow; j++) {
                    if (overflowBuffer[j].key == pointer) // znalezlismy poszukiwany rekord
                    {
                        // przenosimy rekord z overflow do primary
                        primaryBuffer[i] = overflowBuffer[j];
                        for (i++; i < pageSize; i++)
                            if (primaryBuffer[i].key == -1) break;
                        // sortujemy strone primary
                        sort(primaryBuffer, i);
                        // przepisujemy rekordy z overflow o 1 w gore
                        for (; j < maxOverflow - 1; j++)
                            overflowBuffer[j] = overflowBuffer[j + 1];
                        // ostatni rekord w overflow jest wolny
                        overflowBuffer[j].key = overflowBuffer[j].pointer = -1;
                        overflowBuffer[j].probabilityA = overflowBuffer[j].probabilityB = overflowBuffer[j].probabilityAB = 0.0;
                        removed = true;
                        overflowRecords--;
                        break;
                    }
                }
            }
            break;
        } else if (primaryBuffer[i].key > key) // jezeli poszukiwanego rekordu nie ma w primary
        {
            if (pointer != -1) {
                // szukamy rekordu w overflow
                j = 0;
                while (j < maxOverflow && pointer != -1) {
                    if (overflowBuffer[j].key == key) {
                        primaryBuffer[i - 1].pointer = overflowBuffer[j].pointer;
                        for (; j < maxOverflow - 1; j++)
                            overflowBuffer[j] = overflowBuffer[j + 1];
                        overflowBuffer[j].key = overflowBuffer[j].pointer = -1;
                        overflowBuffer[j].probabilityA = overflowBuffer[j].probabilityB = 0.0;
                        removed = true;
                        overflowRecords--;
                    } else
                        pointer = overflowBuffer[j].pointer;
                    j++;
                }
            }
        }
        pointer = primaryBuffer[i].pointer;
    }

    fwrite(primaryBuffer, sizeof(Record), pageSize, writeFile); // zapisujemy zmieniona strone
    writeCount++;

    // przepisujemy reszte primary area
    for (i = page + 1; i < maxPrimary / pageSize; i++) {
        fread(primaryBuffer, sizeof(Record), pageSize, readFile);
        readCount++;
        fwrite(primaryBuffer, sizeof(Record), pageSize, writeFile);
        writeCount++;
    }
    primaryRecords--;

    // przepisujemy overflow
    fwrite(overflowBuffer, sizeof(Record), maxOverflow, writeFile); // zapisujemy overflow
    writeCount++;

    // zwalniamy tablice
    delete[] primaryBuffer;
    delete[] overflowBuffer;
    // zamykamy pliki
    fclose(readFile);
    fclose(writeFile);
    remove(dataFile); // usuwamy plik danych
    rename("dane2", dataFile); // plik tymczasowy zostaje nowym plikiem danych

    return removed;
}

/**************************************************************************************************/
void update(int key, double probabilityA, double probabilityB, double probabilityAB) {
    cout << "AKTUALIZACJA\n";
    int readCount = 0, writeCount = 0;
    if (remove(key, readCount, writeCount)) // usuwamy rekord
        insert(key, probabilityA, probabilityB, probabilityAB, readCount,
               writeCount); // jezeli usunieto do dodajemy nowy
    else
        cout << "Nie ma rekordu z podanym kluczem\n";
    // wyswietlamy statystyki
    cout << "Odczytow = " << readCount << " Zapisow = " << writeCount << endl;
}
/**************************************************************************************************/