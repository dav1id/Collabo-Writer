#include <ncurses.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
    initscr();
    addstr("--------------------------\n"
           "| Collabo-Writer          |\n|"
           "A Docs and Word Replica   |\n|");
    refresh();

    addstr("\npress any key to exit...\n");
    refresh();

    getch();
    endwin();
}