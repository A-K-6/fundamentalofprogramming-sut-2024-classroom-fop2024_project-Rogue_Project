
// Aeen Karimian

#include<ncurses.h>
#include<stdlib.h>



void printing();
void moving_and_sleeping();
void colouring();


int main(void)
{
    initscr();

    addstr("-----------------\n| codedrome.com |\n| ncurses Demo  |\n-----------------\n\n");
    refresh();

    printing();

    //moving_and_sleeping();

    //colouring();

    addstr("\npress any key to exit...");
    refresh();

    getch();

    endwin();

    return EXIT_SUCCESS;
}


void printing()
{
    addstr("This was printed using addstr\n\n");
    refresh();

    addstr("The following letter was printed using addch:- ");
    addch('a');
    refresh();

    printw("\n\nThese numbers were printed using printw\n%d\n%f\n", 123, 456.789);
    refresh();
}