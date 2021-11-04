#include <iostream>
#include <fstream>
#include <curses.h>
#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std; 

char alphabet[] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','r','s','t','u','w','z' };
int difficultyLevel = 0;
string difficultyLevelNames[3] = { "Latwy ", "Sredni", "Trudny" };
int numberOfRounds = 3;
int numberOfBots = 3;
int yMax, xMax;
vector<int> randomIndexes;

void print_centered(WINDOW* win, int start_row, string text) {
    int center_col = win->_maxx / 2; 
    int half_length = text.length() / 2;
    int adjusted_col = center_col - half_length; 

    mvwprintw(win, start_row, adjusted_col, text.data()); 
}

void initColorPairs() {
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // first value is text color and second is bg color
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, 15, COLOR_BLUE);
}

void drawRandomIndexes() {
    while (randomIndexes.size() < numberOfRounds) {
        int random = rand() % 22;
        // ensure that every letter will be unique
        if (find(randomIndexes.begin(), randomIndexes.end(), random) == randomIndexes.end())
            randomIndexes.push_back(random);
    }
}

void letterAnimation(int index) {
    attron(COLOR_PAIR(2));
    string line; 
    ifstream asciiAlphabet("assets\\asciiart\\alphabet.txt");
    for (int i = 0; i <= index; i++) {
        if (i == index) {
            attron(COLOR_PAIR(1));
        }
        for (int j = 0; j < 16; j++) {
            getline(asciiAlphabet, line);
            print_centered(stdscr, j, line);
            refresh();
        }
        Sleep(100);
        if (i != index) {
            clear();
        }
    }
    print_centered(stdscr, 17, "Oto twoja wylosowana litera");
    attroff(COLOR_PAIR(1)); 
    getch();
}

void mainMenu() {
    // load ascii art logo
    string line;
    ifstream logo("assets\\asciiart\\logo.txt");
    attron(COLOR_PAIR(1)); // green text on black bg
    for (int i = 0; i < 6; i++) {
        getline(logo, line);
        print_centered(stdscr, i, line); 
        refresh();
    }
    attron(COLOR_PAIR(2));
    print_centered(stdscr, 7, "Autor: Maciej Czech"); 
    attroff(COLOR_PAIR(2));
    // create a window for input
    WINDOW* menuwin = newwin(7, xMax - 12, yMax - 8, 5);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    keypad(menuwin, true);
    string choices[5] = { "Nowa gra", "Poziom trudnosci: Latwy", "Liczba rund: 3", "Liczba botow: 3",  "Wyjscie"};
    int choice;
    int highlight = 0;
    while (1) {
        for (int i = 0; i < 5; i++) {
            if (i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i + 1, 1, choices[i].data());
            wattroff(menuwin, A_REVERSE);
        }
        choice = wgetch(menuwin);
        switch (choice) {
        case KEY_UP:
            highlight--;
            if (highlight == -1)
                highlight = 0;
            break;
        case KEY_DOWN:
            highlight++;
            if (highlight == 5)
                highlight = 4;
            break;
        case KEY_LEFT:
            if (highlight == 1) {
                difficultyLevel--;
                if (difficultyLevel == -1)
                    difficultyLevel = 2;
                choices[1] = "Poziom trudnosci: " + difficultyLevelNames[difficultyLevel];
            }
            else if (highlight == 2) {
                numberOfRounds--;
                if (numberOfRounds == 0)
                    numberOfRounds = 5;
                choices[2] = "Liczba rund: " + to_string(numberOfRounds);
            }
            else if (highlight == 3) {
                numberOfBots--;
                if (numberOfBots == 0)
                    numberOfBots = 5;
                choices[3] = "Liczba botow: " + to_string(numberOfBots);
            }
            else
                break;
            break;
        case KEY_RIGHT:
            if (highlight == 1) {
                difficultyLevel++;
                if (difficultyLevel == 3)
                    difficultyLevel = 0;
                choices[1] = "Poziom trudnosci: " + difficultyLevelNames[difficultyLevel];
            }
            else if (highlight == 2) {
                numberOfRounds++;
                if (numberOfRounds == 6)
                    numberOfRounds = 1;
                choices[2] = "Liczba rund: " + to_string(numberOfRounds);
            }
            else if (highlight == 3) {
                numberOfBots++;
                if (numberOfBots == 6)
                    numberOfBots = 1;
                choices[3] = "Liczba botow: " + to_string(numberOfBots);
            }
            else
                break;
            break;
        default:
            break;
        }
        // this check is to prevent starting the game while user is selecting bot/round count
        if (choice == 10 && highlight == 0) {
            flash(); // neat effect
            break;
        }
        else if (choice == 10 && highlight == 4) { // user picked exit option
            flash();
            exit(0);
        }
    }
    clear();
}

void startingGameAnimation() {
    // load ascii art for starting game text
    string line;
    ifstream startingGameLogo("assets\\asciiart\\starting.txt");
    attron(COLOR_PAIR(3)); // green text on black bg
    for (int i = 0; i < 8; i++) {
        getline(startingGameLogo, line);
        print_centered(stdscr, i, line);
        refresh();
    }
    attroff(COLOR_PAIR(3));
    WINDOW* parameters = newwin(5, xMax - 12, 9, 5);
    box(parameters, 0, 0); 
    mvwprintw(parameters, 1, 1, ("Poziom trudnosci: " + difficultyLevelNames[difficultyLevel]).data());
    mvwprintw(parameters, 2, 1, ("Liczba rund: " + to_string(numberOfRounds)).data());
    mvwprintw(parameters, 3, 1, ("Liczba botow: " + to_string(numberOfBots)).data()); 
    wrefresh(parameters);
    attron(COLOR_PAIR(1));
    for (int i = 5; i > 0; i--) {
        print_centered(stdscr, 15, "Zaczynamy za " + to_string(i)); 
        refresh();
        Sleep(1000);
    }
    attroff(COLOR_PAIR(1));
}

void game() {
    // vectors to keep possible answers for each category
    vector<pair<int, string>> possibleAnswers; 
    vector<string> allGivenAnswers;
    string input[6]; 
    bool cheatWasUsed = false; 
    int playerPoints = 0;
    int botsPoints[5] = { 0 };
    string cheatText; 
    string choices[7] = { "Panstwo:", "Miasto:", "Zwierze:", "Zawod:",  "Owoc lub warzywo:", "Kolor:", "ZAKONCZ RUNDE" };
    for (size_t x = 0; x < randomIndexes.size(); x++) { // iterate through every round
        for (auto& a : input) a = ""; // clear input array
        cheatWasUsed = false; 
        cheatText = "Masz do dyspozycji jedna podpowiedz, aby jej uzyc nacisnij F2"; 
        clear();
        int letterIndex = randomIndexes[x]; 
        letterAnimation(letterIndex);
        clear();
        string line;
        ifstream roundArt("assets\\asciiart\\round.txt");
        attron(COLOR_PAIR(1)); // green text on black bg
        // print ascii art for current round number
        for (int i = 0; i < x+1; i++) {
            for (int j = 0; j < 6; j++) {
                getline(roundArt, line);
                print_centered(stdscr, j, line);
                refresh();
            }
        }
        string letterMessage = "Wylosowana litera: ";
        letterMessage.push_back(alphabet[letterIndex] - 32); 
        attron(COLOR_PAIR(2));
        print_centered(stdscr, 8, letterMessage); 
        attroff(COLOR_PAIR(2));
        WINDOW* gamewin = newwin(9, xMax - 12, 11, 5);
        box(gamewin, 0, 0);
        refresh();
        wrefresh(gamewin);
        keypad(gamewin, true);
        int choice;
        int highlight = 0;
        noecho();
        while (1) {
            wclear(gamewin); 
            box(gamewin, 0, 0); 
            for (int i = 0; i < 7; i++) {
                if (i == highlight)
                    wattron(gamewin, A_REVERSE);
                print_centered(gamewin, i+1, choices[i] + " " + input[i]);
                wattroff(gamewin, A_REVERSE);
                refresh();
                wrefresh(gamewin);
            }
            if (difficultyLevel != 2) {
                print_centered(stdscr, 22, cheatText);
                refresh(); 
            }
            choice = wgetch(gamewin);
            switch (choice) {
            case KEY_UP:
                highlight--;
                if (highlight == -1)
                    highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight == 7)
                    highlight = 6;
                break;
            case KEY_F(2):
                if (difficultyLevel != 2 && cheatWasUsed == false) {
                    move(22, 0);
                    clrtoeol();
                    cheatWasUsed = true; 
                    cheatText = "W tej rundzie wykorzystales juz podpowiedz"; 
                }
                break;
            case 8: // backspace
                if (!input[highlight].empty()) {
                    input[highlight].pop_back(); 
                }
                break;
            case 10: // enter 
                break; 
            default:
                // it should be changed to only accept cases 65-122 (ascii characters)
                if (highlight != 6) {
                    // check if user pressed allowed key (only big/small letters, space and dash)
                    if (choice >= 65 && choice <= 90 || choice >= 97 && choice <= 122 || choice == 32 || choice == 45)
                        input[highlight] += choice; 
                }
            }
            if (choice == 10 && highlight == 6) {
                flash(); // neat effect
                break;
            }
        }
        clear();
        string a; 
        for (int i = 0; i < 6; i++) {
            a = choices[i] + " " + input[i] + '\n';
            printw(a.data());
        }
        getch();
    }
}

int main()
{
    initscr(); // start ncurses mode
    getmaxyx(stdscr, yMax, xMax);
    initColorPairs(); // initialize color pairs used in program
    mainMenu(); // print main menu
    startingGameAnimation(); // shows what game options user picked
    srand(time(NULL)); // start randomizing
    drawRandomIndexes(); // fill vector with unique random letter indexes
    clear(); // clear screen
    game(); // game
}
