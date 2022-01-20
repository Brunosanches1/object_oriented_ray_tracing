#include <ncurses.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>

#ifndef TERMINAL_GUI
#define TERMINAL_GUI

namespace termGui {
    std::thread tGui;

    /* Init the ncurses terminal and set thread */
    void init(sf::RenderWindow&, Engine&);

    /* Define main terminal window */
    void main_ncurses(sf::RenderWindow&, Engine&);

    /* Join thread */
    void close();

    void init(sf::RenderWindow& window, Engine& rtEngine) {
        tGui = std::thread(main_ncurses, std::ref(window), std::ref(rtEngine));
    }

    void close() {
        tGui.join();
    }

    void main_ncurses(sf::RenderWindow& window, Engine& rtEngine) {
        initscr();			/* Start curses mode 		  */
        erase();            /* clear entire screen */
        printw("Hello World !!!");	/* Print Hello World		  */
        refresh();			/* Print it on to the real screen */
        while(window.isOpen()) {};
        printw("Fechou");
        endwin();			/* End curses mode		  */
    }   
}


#endif