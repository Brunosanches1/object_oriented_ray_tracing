#include <ncurses.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include "engine.hpp"

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

    void updateProgressBar(Engine& rtEngine) {
        double progress = (double) (rtEngine.getImgHeight() - rtEngine.getRemainingLines()) / rtEngine.getImgHeight() * 100.0;
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end_time - rtEngine.workStartTime();

        double time_to_finish = (diff.count() / (rtEngine.getImgHeight() - rtEngine.getRemainingLines())) * 
                                    rtEngine.getRemainingLines();

        int x, y;
        getmaxyx(stdscr, y, x);
        move(y-2, 0);
        clrtoeol();
        move(y-1, 0);
        clrtoeol();

        mvprintw(y-2, 0, "[Elapsed time %7.1lf s]   [Remaining time %7.1lf s]", diff, time_to_finish);

        mvprintw(y-1, 0, "[");
        for (auto i = 0; i <= progress; i += 2){
            mvprintw(y-1, (i / 2) + 1, "#");
        }
        for (auto i = 100; i > progress; i -= 2) {
            mvprintw(y-1, (i / 2) + 1, " ");
        }
        mvprintw(y-1, 52, "] %5.1lf %%", progress);
        refresh();

    }

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
        while(window.isOpen()) {
            if (rtEngine.isWorking()) {
                updateProgressBar(rtEngine);
            }
        };
        printw("Fechou");
        endwin();			/* End curses mode		  */
    }   
}


#endif