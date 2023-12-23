#include <iostream>
#include <fstream>
#include <stdio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

std::string AVAILABLE_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";

void SetColor(int ForgC)
{
    WORD wColor;

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

                       //We use csbi for the wAttributes word.
     if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
     {
                     //Mask out all but the background attribute, and add in the forgournd color
          wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
          SetConsoleTextAttribute(hStdOut, wColor);
     }
     return;
}

void hackingPrintSim(int durationSeconds) {
    const std::string characters = "#.$&*@%^![]~_+=|<>";
    const int charactersSize = characters.size();

    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    const std::clock_t start = std::clock();
    bool loopFlag = true;

    SetColor(2);
    while(loopFlag) {
        for (int i = 0; i < 200; ++i) {
            std::cout << characters[std::rand() % charactersSize];
        }
        std::cout << std::endl;

        const std::clock_t startHelper = std::clock();
        while (1000.0 * (std::clock() - startHelper) / CLOCKS_PER_SEC < 10) {
            // Something (this loop acts as a delay)
        }

        const std::clock_t end = std::clock();
        const double elapsedSeconds = static_cast<double>(end - start) / CLOCKS_PER_SEC;

        if (elapsedSeconds >= durationSeconds)
            loopFlag = false;
    }
}

#define PROGRESS_BAR_SIZE 100

void print_n_chars(int n, int c) {
    while (n-- > 0) putchar(c);
}

void display_progress_bar(int p) {
    putchar('\r');
    putchar('[');
    print_n_chars(PROGRESS_BAR_SIZE * p / 100, '|');
    print_n_chars(PROGRESS_BAR_SIZE - PROGRESS_BAR_SIZE * p / 100, ' ');
    putchar(']');
    printf(" %d%%", p);
}

void loadingAnimation()
{
    int p;
    SetColor(15);
    std::cout << "\n\nPlease wait - loading...\n";
    SetColor(2);
    for (p = 0; p <= 100; ++p) {
        display_progress_bar(p);
        Sleep(40);
    }
}

const char fillchar = '=';
const std::string cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64_encode(const std::vector<char>& data)
{
  std::string::size_type i;
  char c;
  unsigned int len = data.size();
  std::string ret;

  for (i = 0; i < len; ++i)
  {
    c = (data[i] >> 2) & 0x3f;
    ret.append(1, cvt[c]);
    c = (data[i] << 4) & 0x3f;
    if (++i < len)
      c |= (data[i] >> 4) & 0x0f;

    ret.append(1, cvt[c]);
    if (i < len)
    {
      c = (data[i] << 2) & 0x3f;
      if (++i < len)
        c |= (data[i] >> 6) & 0x03;

      ret.append(1, cvt[c]);
    }
    else
    {
      ++i;
      ret.append(1, fillchar);
    }

    if (i < len)
    {
      c = data[i] & 0x3f;
      ret.append(1, cvt[c]);
    }
    else
    {
      ret.append(1, fillchar);
    }
  }

  return(ret);
}

std::vector<char> base64_decode(const std::string& data)
{
  std::string::size_type  i;
  char c;
  char c1;
  std::string::size_type  len = data.length();
  std::vector<char>  ret;

  for (i = 0; i < len; ++i)
  {
    c = (char) cvt.find(data[i]);
    ++i;
    c1 = (char) cvt.find(data[i]);
    c = (c << 2) | ((c1 >> 4) & 0x3);
    ret.push_back(c);
    if (++i < len)
    {
      c = data[i];
      if (fillchar == c)
        break;
      c = (char) cvt.find(c);
      c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
      ret.push_back(c1);
    }
    if (++i < len)
    {
      c1 = data[i];
      if (fillchar == c1)
        break;
      c1 = (char) cvt.find(c1);
      c = ((c << 6) & 0xc0) | c1;
      ret.push_back(c);
    }
  }
  return(ret);
}

std::string encrypt(std::string text, int s)
{
    std::string result = "";


    for (int i = 0; i < text.size(); i++) {
        char c = text[i];
        if (isupper(c)) {
            result += char(int(c + s - 'A') % 26 + 'A');
        } else if (islower(c)) {
            result += char(int(c + s - 'a') % 26 + 'a');
        } else if (isdigit(c)) {
            result += char(int(c + s - '0') % 10 + '0');
        } else {
            result += s;
        }
    }

    return result;
}

int asciiHash(const std::string& input) {
    int sum = 0;

    for (int i = 0; i < input.size(); i++) {
        char c = input[i];
        sum += int(c);
    }

    return sum;
}

void encrypt_pin(std::string pin, std::string key)
{
    // int shift = asciiHash(key) % 26;
    int shift = asciiHash(key) % 10;
    std::string shiftedPin = encrypt(pin, shift);
    std::vector<char> shiftedVectorData;

    for (size_t i = 0; i < shiftedPin.size(); ++i) {
        shiftedVectorData.push_back(shiftedPin[i]);
    }

    std::string b64_cipher = base64_encode(shiftedVectorData);

    std::ofstream cipherFile("szyfr.txt");

    if (cipherFile.is_open()) {
        cipherFile << b64_cipher << '\n';

        cipherFile.close();
    }
}

int crack_cipher(std::string & cipher, std::string & key)
{
    if (asciiHash(key) != 456)
    {
        return -1;
    }
    int shift = 10 - (asciiHash(key) % 10);
    std::vector<char> plain_cipher = base64_decode(cipher);
    std::string plain_cipher_str;

    for (int i = 0; i < 16 && i < plain_cipher.size(); i++) {
        plain_cipher_str += plain_cipher[i];
    }

    std::string plain_text = encrypt(plain_cipher_str, shift);

    std::ofstream outputFile("prezent.txt");

    if (outputFile.is_open()) {
        outputFile << "Chyba nie musze tlumaczyc co z tym dalej zrobic. Wszystkiego dobrego na Swieta!\n";
        for (int i = 0; i < 4; i++)
        {
            int start = i * 4;
            outputFile << plain_text.substr(start, 4) << " ";
        }
        outputFile << "\n";
        outputFile.close();
    }
    return 0;
}

int treasure_hunt()
{
    hackingPrintSim(9);
    SetColor(15);
    std::cout << "\n\nAby odszyfrowac zagadke musisz podac wlasciwy klucz. Powstanie on poprzez polaczenie odpowiedzi na ponizsze pytania.";
    SetColor(13);
    std::cout << "\n\nPodaj dzien urodzenia: ";
    SetColor(15);
    std::string day;
    std::cin >> day;
    std::cout << "\nWybierz miesiac urodzenia.\n";

    std::string months[] = {
        "styczen", "luty", "marzec", "kwiecien", "maj", "czerwiec",
        "lipiec", "sierpien", "wrzesien", "pazdziernik", "listopad", "grudzien"
    };

    for (int i = 0; i < 12; i++)
    {
        std::cout << "[" << (i+1) << "]   " << months[i] << "\n";
    }
    SetColor(13);
    std::cout << "\n\nWybor: ";
    SetColor(15);
    std::string month;
    std::cin >> month;
    SetColor(13);
    std::cout << "\n\nPodaj rok urodzenia: ";
    SetColor(15);
    std::string year;
    std::cin >> year;
    SetColor(13);
    std::cout << "\n\nPodaj literke z nazwy swojej klasy (mala literka! Np. 'a'; 'b'...): ";
    SetColor(15);
    std::string class_name;
    std::cin >> class_name;

    std::ifstream inputFile("szyfr.txt");

    std::string cipher;
    if (inputFile.is_open()) {
        std::getline(inputFile, cipher);
        inputFile.close();
    }

    std::string key = day+month+year+class_name;

    return crack_cipher(cipher, key);
}

int main()
{
    int exit_code = treasure_hunt();

    if (exit_code == -1)
    {
        SetColor(4);
        std::cout << "\n\nPodane dane sa nieprawidlowe! Sprobuj uruchomic program jeszcze raz.\n\n\n\n\n\n";
        SetColor(15);
    }
    else
    {
        loadingAnimation();

        SetColor(15);
        std::cout << "\n\n";

        for (int i = 0; i < 100; i++)
        {
            std::cout << "_";
        }

        SetColor(13);
        std::cout << "\n\nODSZYFROWYWANIE ZOSTALO UKONCZONE. SPRAWDZ PLIK prezent.txt\n";
        SetColor(15);
        for (int i = 0; i < 100; i++)
        {
            std::cout << "_";
        }
        std::cout << "\n\n\n\n\n\n";
    }

    return 0;
}
