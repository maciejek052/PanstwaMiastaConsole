#include <iostream>
#include <fstream>
#include <curses.h>
#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <string>
using namespace std; 

char alphabet[] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','r','s','t','u','w','z' };
int difficultyLevel = 0;

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
}

int randomLetterIndex() {
    return rand() % 22; 
}

void letterAnimation(int index) {
    attron(COLOR_PAIR(2));
    string line; 
    ifstream asciiAlphabet("C:\\Users\\macie\\Desktop\\assets\\asciiart\\alfabet.txt");
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
    ifstream logo("C:\\Users\\macie\\Desktop\\assets\\asciiart\\logo.txt");
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
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    WINDOW* menuwin = newwin(6, xMax - 12, yMax - 8, 5);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    keypad(menuwin, true);
    string choices[4] = { "Nowa gra", "Poziom trudnosci: Latwy", "O grze", "Wyjscie" };
    int choice;
    int highlight = 0;
    while (1) {
        for (int i = 0; i < 4; i++) {
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
            if (highlight == 4)
                highlight = 3;
            break;
        case KEY_LEFT:
            if (highlight != 1)
                break;
            else {
                difficultyLevel--;
                if (difficultyLevel == -1)
                    difficultyLevel = 2;
                if (difficultyLevel == 2)
                    choices[1] = "Poziom trudnosci: Trudny";
                else if (difficultyLevel == 1)
                    choices[1] = "Poziom trudnosci: Sredni";
                else if (difficultyLevel == 0)
                    choices[1] = "Poziom trudnosci: Latwy ";
            }
            break;
        case KEY_RIGHT:
            if (highlight != 1)
                break;
            else {
                difficultyLevel++;
                if (difficultyLevel == 3)
                    difficultyLevel = 0;
                if (difficultyLevel == 2)
                    choices[1] = "Poziom trudnosci: Trudny";
                else if (difficultyLevel == 1)
                    choices[1] = "Poziom trudnosci: Sredni";
                else if (difficultyLevel == 0)
                    choices[1] = "Poziom trudnosci: Latwy ";
            }
            break;

        default:
            break;
        }
        if (choice == 10)
            break;
    }
}

int main()
{
    initscr();
    initColorPairs();
    mainMenu();
    srand(time(NULL));
    int litera = randomLetterIndex();
    clear();
    letterAnimation(litera); 
    endwin(); 
}
