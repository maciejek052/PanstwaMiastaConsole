#include <fstream>
#include <curses.h>
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std; 

char alphabet[] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','r','s','t','u','w','z' };
string difficultyLevelNames[3] = { "Latwy ", "Sredni", "Trudny" };
int difficultyLevel = 0;
int numberOfRounds = 3;
int numberOfBots = 3;
int yMax, xMax;
int botPercentageChance = 50; 
vector<int> randomIndexes;
vector<string> allGivenAnswers;
vector<vector<string>> possibleAnswers; 
vector<vector<string>> answersGivenByBots;

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
    init_pair(4, COLOR_BLACK, COLOR_WHITE); 
    init_pair(5, COLOR_WHITE, COLOR_GREEN);
    init_pair(6, COLOR_WHITE, COLOR_RED); 
    init_pair(7, COLOR_WHITE, COLOR_YELLOW);
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
    if (difficultyLevel == 0)
        botPercentageChance = 50;
    else if (difficultyLevel == 1)
        botPercentageChance = 70;
    else if (difficultyLevel == 2)
        botPercentageChance = 80;
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

void fillVector(int letterIndex) {
    possibleAnswers.clear(); 
    ifstream input; 
    string line; 
    for (int i = 0; i < 6; i++) {
        switch (i) {
        case 0:
            input.open("assets\\dictionary\\panstwa.txt");
            break;
        case 1:
            input.open("assets\\dictionary\\miasta.txt");
            break;
        case 2:
            input.open("assets\\dictionary\\zwierzeta.txt");
            break;
        case 3:
            input.open("assets\\dictionary\\zawody.txt");
            break;
        case 4:
            input.open("assets\\dictionary\\owoce_warzywa.txt");
            break; 
        case 5:
            input.open("assets\\dictionary\\kolory.txt");
            break;
        }
        vector<string> temp; 
        while (!input.eof()) {
            getline(input, line);
            if (line[0] == alphabet[letterIndex] - 32 || line[0] == alphabet[letterIndex]) {
                temp.push_back(line); 
            }
        }
        possibleAnswers.push_back(temp); 
        input.close();
    }
}

void game() {
    string input[7];
    bool cheatWasUsed = false;
    int playerPoints = 0;
    int botsPoints[5] = { 0 };
    string cheatText;
    string choices[7] = { "Panstwo:", "Miasto:", "Zwierze:", "Zawod:",  "Owoc lub warzywo:", "Kolor:", "ZAKONCZ RUNDE" };
    int botAnswerPoints[6][6];
    int playerAnswerPoints[6]; 
    for (size_t x = 0; x < randomIndexes.size(); x++) { // iterate through every round
        allGivenAnswers.clear();
        answersGivenByBots.clear();
        for (auto& a : input) a = ""; // clear input array
        memset(playerAnswerPoints, 0, sizeof(playerAnswerPoints));
        memset(botAnswerPoints, 0, 36 * (sizeof(int))); 
        cheatWasUsed = false;
        int possibleAnswersAmount2 = 0;

        cheatText = "Masz do dyspozycji jedna podpowiedz, aby jej uzyc nacisnij F2";
        clear();
        int letterIndex = randomIndexes[x];
        letterAnimation(letterIndex);
        fillVector(letterIndex);
        clear();
        string line;
        ifstream roundArt("assets\\asciiart\\round.txt");
        attron(COLOR_PAIR(1)); // green text on black bg
        // print ascii art for current round number
        for (int i = 0; i < x + 1; i++) {
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
        wbkgd(gamewin, COLOR_PAIR(4));
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
                print_centered(gamewin, i + 1, choices[i] + " " + input[i]);
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
                possibleAnswersAmount2 = possibleAnswers[highlight].size();
                if (difficultyLevel != 2 && cheatWasUsed == false && highlight != 6 && possibleAnswersAmount2 != 0) {
                    move(22, 0);
                    clrtoeol();
                    // cheatWasUsed = true;
                    cheatText = "W tej rundzie wykorzystales juz podpowiedz";
                    input[highlight] = possibleAnswers[highlight][rand() % possibleAnswersAmount2];
                }
                break;
            case 8: // backspace
                if (!input[highlight].empty())
                    input[highlight].pop_back();
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

        // bots work
        int chanceForGuessing; 
        int randomAnswerIndex; 
        int possibleAnswersAmount = 0; 
        for (int i = 0; i < numberOfBots; i++) {
            vector<string> temp;
            for (int j = 0; j < 6; j++) {
                chanceForGuessing = rand() % 100 + 1;
                if (chanceForGuessing <= botPercentageChance) {
                    possibleAnswersAmount = possibleAnswers[j].size(); 
                    if (possibleAnswersAmount == 0)
                        temp.push_back("-"); 
                    else {
                        randomAnswerIndex = rand() % possibleAnswersAmount;
                        temp.push_back(possibleAnswers[j][randomAnswerIndex]);
                        allGivenAnswers.push_back(temp.back());
                    }
                }
                else {
                    temp.push_back("-");
                }
            }
            answersGivenByBots.push_back(temp);
        }

        for (int i = 0; i < 6; i++) {
            allGivenAnswers.push_back(input[i]); 
        }

        // count points 
        for (int i = 0; i < numberOfBots; i++) {
            for (int j = 0; j < 6; j++) {
                if (answersGivenByBots[i][j] == "-") {
                    botAnswerPoints[i][j] = 0;
                }
                else {
                    int repeats = count(allGivenAnswers.begin(), allGivenAnswers.end(), answersGivenByBots[i][j]);
                    if (repeats == 1) {
                        botAnswerPoints[i][j] = 15;
                        botsPoints[i] += 15;
                    }
                    else {
                        botAnswerPoints[i][j] = 10;
                        botsPoints[i] += 10;
                    }
                }
            }
        }
        for (int i = 0; i < 6; i++) {
            if (find(possibleAnswers[i].begin(), possibleAnswers[i].end(), input[i]) != possibleAnswers[i].end()) {
                int repeats = count(allGivenAnswers.begin(), allGivenAnswers.end(), input[i]);
                if (repeats == 1) {
                    playerAnswerPoints[i] = 15;
                    playerPoints += 15; 
                }
                else {
                    playerAnswerPoints[i] = 10;
                    playerPoints += 10;
                }
                // printw("Powtorzenia %s to %d\n", input[i].data(), repeats);
            }
            else {
                playerAnswerPoints[i] = 0;
            }
           
        }

        // print points (temporarily)
        string ooo = "Koniec rundy " + to_string(x+1);
        attron(COLOR_PAIR(3)); 
        print_centered(stdscr,0,ooo); 
        attroff(COLOR_PAIR(3));

        for (int i = 0; i < numberOfBots; i++) {
            printw("\n\nBot %d, suma %d\n", i+1, botsPoints[i]);
            for (int j = 0; j < 6; j++) {
                if (botAnswerPoints[i][j] == 0)
                    attron(COLOR_PAIR(6));
                else if (botAnswerPoints[i][j] == 15)
                    attron(COLOR_PAIR(5));
                else
                    attron(COLOR_PAIR(7));
                printw(answersGivenByBots[i][j].data()); 
                printw(" %d, ", botAnswerPoints[i][j]); 
                attroff(COLOR_PAIR(5)); attroff(COLOR_PAIR(6)); attroff(COLOR_PAIR(7));
            }
        }
        attroff(COLOR_PAIR(5)); attroff(COLOR_PAIR(6)); attroff(COLOR_PAIR(7));
        printw("\n\nGracz, suma %d\n", playerPoints); 
        for (int i = 0; i < 6; i++) {
            if (playerAnswerPoints[i] == 0)
                attron(COLOR_PAIR(6));
            else if (playerAnswerPoints[i] == 15)
                attron(COLOR_PAIR(5));
            else
                attron(COLOR_PAIR(7));
            printw(input[i].data()); 
            printw(" %d, ", playerAnswerPoints[i]);
        }
        getch(); 

    }
}
int main() {
    initscr(); // start ncurses mode
    curs_set(0); // make cursor invisible
    getmaxyx(stdscr, yMax, xMax);
    initColorPairs(); // initialize color pairs used in program
    
    mainMenu(); // print main menu
    startingGameAnimation(); // shows what game options user picked
    
    srand(time(NULL)); // start randomizing
    drawRandomIndexes(); // fill vector with unique random letter indexes
    clear(); // clear screen
    game(); // game
}
