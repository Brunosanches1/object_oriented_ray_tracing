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

    /* Update progress bar if the engine is working */
    void updateProgressBar(Engine&, WINDOW*);

    void updateProgressBar(Engine& rtEngine, WINDOW* win) {
        double progress = (double) (rtEngine.getImgHeight() - rtEngine.getRemainingLines()) / rtEngine.getImgHeight() * 100.0;
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end_time - rtEngine.workStartTime();

        double time_to_finish = (diff.count() / (rtEngine.getImgHeight() - rtEngine.getRemainingLines())) * 
                                    rtEngine.getRemainingLines();
        // wclear(win);
        werase(win);
        wmove(win, 0, 0);
        wprintw(win, "[Elapsed time %7.1lf s]   [Remaining time %7.1lf s]\n", diff, time_to_finish);
        // mvwprintw(win, 0, 0, "[Elapsed time %7.1lf s]   [Remaining time %7.1lf s]\n", diff, time_to_finish);
        // wclrtoeol(win);
        wprintw(win, "[");
        for (auto i = 0; i <= progress; i += 2){
            // mvwprintw(win, 1, (i / 2)+1, "#");
            wprintw(win, "#");
        }
        // for (auto i = 98; i >= progress; i -= 2) {
        //     mvwprintw(win, 2, (i / 2) +1, " ");
        // }
        mvwprintw(win, 1, 52, "] %5.1lf %%", progress);
        wrefresh(win);


    }

    void init(sf::RenderWindow& window, Engine& rtEngine) {
        tGui = std::thread(main_ncurses, std::ref(window), std::ref(rtEngine));
    }

    void close() {
        tGui.join();
    }

    void initHeaderWindow(WINDOW* win) {
        werase(win);
        wmove(win, 0, 0);
        wprintw(win, "//////////////////////////////////////////////////////");
        wprintw(win, "/              IN204 Project - Ray Tracer            /");
        wprintw(win, "/                                                    /");
        wprintw(win, "/             Authors: MACEDO SANCHES Bruno          /");
        wprintw(win, "/                OLIVEIRA DA SILVA Alexis            /");
        wprintw(win, "/                                                    /");
        wprintw(win, "//////////////////////////////////////////////////////");
        wrefresh(win);
    }

    void main_ncurses(sf::RenderWindow& window, Engine& rtEngine) {
        initscr();			/* Start curses mode 		  */
        erase();            /* clear entire screen */

        /* Construct header*/
        auto height = 7;
        auto width = 54;
        // auto startx = (COLS - width) / 2;
        auto startx = 0;
        auto starty = 0;
        WINDOW* headerWindow = newwin(height, width, starty, startx);
        initHeaderWindow(headerWindow);
        // refresh();			/* Print it on to the real screen */

        /* Construct progress bar window */
        height = 2;
        width = 61;
        // startx = (COLS - width) / 2;
        startx = 0;
        starty = (LINES - 2);
        WINDOW* progressBarWindow = newwin(height, width, starty, startx);

        while(window.isOpen()) {
            if (rtEngine.isWorking()) {
                updateProgressBar(rtEngine, progressBarWindow);
            }
            else {
                werase(progressBarWindow);
            }
        };
        printw("Fechou");
        endwin();			/* End curses mode		  */
    }   
}


#endif