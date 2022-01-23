#include <ncurses.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <stdexcept>
#include "engine.hpp"

#ifndef TERMINAL_GUI
#define TERMINAL_GUI

namespace termGui {
    std::thread tGui;
    bool timeToClose = false;

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
        wprintw(win, "[Elapsed time %7.1lf s]  [Remaining time %7.1lf s]\n", diff, time_to_finish);
        // mvwprintw(win, 0, 0, "[Elapsed time %7.1lf s]   [Remaining time %7.1lf s]\n", diff, time_to_finish);
        // wclrtoeol(win);
        wprintw(win, "[");
        for (auto i = 2; i <= progress; i += 2){
            // mvwprintw(win, 1, (i / 2)+1, "#");
            wprintw(win, "#");
        }
        // for (auto i = 98; i >= progress; i -= 2) {
        //     mvwprintw(win, 2, (i / 2) +1, " ");
        // }
        mvwprintw(win, 1, 51, "] %5.1lf %%", progress);
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

    void initOptWin(WINDOW* win) {
        werase(win);
        wmove(win, 0, 0);
        wprintw(win, "Press the key indicated to perform an action\n");
        wprintw(win, "Enter - Render the scene in a window\n");
        wprintw(win, "r - Recover scene from a XML file\n");
        wprintw(win, "p - Load example scene\n");
        wprintw(win, "s - Save scene in XML format\n");
        wprintw(win, "i - Save scene in image format\n");
        wprintw(win, "q - quit\n");
        wprintw(win, "There is already a preloaded scene\n");
        wrefresh(win);
    }

    std::string getFilename(WINDOW* optWin, WINDOW* inputWin) {
        werase(optWin);
        wmove(optWin, 0, 0);
        wprintw(optWin, "File path and name:\n");
        wrefresh(optWin);
        keypad(inputWin, true);
        auto c = wgetch(inputWin);
        std::string filename;
        while(c != '\n') {
            if (c == KEY_BACKSPACE || c == KEY_DC || c == 8) {
                wrefresh(optWin);
                wmove(optWin, 1, 0);
                wclrtoeol(optWin);
                wrefresh(optWin);
                filename.pop_back();
            }
            else {
                filename.push_back(c);
            }
            mvwprintw(optWin, 1, 0, filename.c_str());
            wrefresh(optWin);
            c = wgetch(inputWin);
        }
        keypad(inputWin, false);
        return filename;
    }

    void recoverXML(WINDOW* optWin, WINDOW* inputWin, Engine& rtEngine, sf::RenderWindow& rtWindow) {
        std::string filename;
        
        while(true) {
            filename = getFilename(optWin, inputWin);
            try {
                rtEngine = Engine(filename.c_str());
                break;
            }
            catch(std::exception& e) {
                mvwprintw(optWin, 10, 0, "Error while handling file, try again or another file");
                mvwprintw(optWin, 11, 0, "Press enter to try again");
                wrefresh(optWin);
                auto c = wgetch(inputWin);
                while(c != '\n') {c = wgetch(inputWin); }
            }
        }

         // update the view to the new size of the window
        sf::FloatRect visibleArea(0, 0, rtEngine.getImgWidth(), rtEngine.getImgHeight());
        rtWindow.setView(sf::View(visibleArea));
        mvwprintw(optWin, 10, 0, "Loaded! Press enter to return");
        wrefresh(optWin);
        auto c = wgetch(inputWin);
        while(c != '\n') {c = wgetch(inputWin); }
        initOptWin(optWin);
    }

    void saveXML(WINDOW* optWin, WINDOW* inputWin, Engine& rtEngine) {
        auto filename = getFilename(optWin, inputWin);
        rtEngine.saveXmlDocument(filename.c_str());

        mvwprintw(optWin, 10, 0, "Saved! Press enter to return");
        wrefresh(optWin);
        auto c = wgetch(inputWin);
        while(c != '\n') {c = wgetch(inputWin); }
        initOptWin(optWin);
    }

    void saveImage(WINDOW* optWin, WINDOW* inputWin, Engine& rtEngine) {
        auto filename = getFilename(optWin, inputWin);
        rtEngine.getTexture().copyToImage().saveToFile(filename);
        mvwprintw(optWin, 10, 0, "Saved! Press enter to return");
        wrefresh(optWin);
        auto c = wgetch(inputWin);
        while(c != '\n') {c = wgetch(inputWin); }
        initOptWin(optWin);
    }

    void main_ncurses(sf::RenderWindow& window, Engine& rtEngine) {
        initscr();			/* Start curses mode 		  */
        erase();            /* clear entire screen */
        // raw();
        cbreak();
        noecho();           /* Disable echoing */
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

        /* Construct input window */
        height = 0;
        width = 20;
        startx = 0;
        starty = (LINES -1);
        WINDOW* inputWin = newwin(height, width, starty, startx);

        /* Construct options window */
        height = 30;
        width = 61;
        startx = 0;
        starty = 10;
        WINDOW* optionsWin = newwin(height, width, starty, startx);
        initOptWin(optionsWin);

        sf::Sprite sprite(rtEngine.getTexture());

        while(!timeToClose) {
            if (rtEngine.isWorking()) {
                updateProgressBar(rtEngine, progressBarWindow);
                // window.setVisible(false);
            }
            else {
                wclear(progressBarWindow);
                // window.setVisible(true);
                auto c = wgetch(inputWin);
                // auto c = KEY_IL;
                switch(c) {
                    case '\n':
                        rtEngine.setToWork();
                        mvwprintw(optionsWin, 10, 0, "Working.... wait render to finish before pressing any key");
                        wrefresh(optionsWin);
                        break;
                    case 'r':
                        recoverXML(optionsWin, inputWin, rtEngine, window);
                        break;
                    case 'p':
                        rtEngine = Engine();
                        mvwprintw(optionsWin, 10, 0, "Example scene loaded");
                        wrefresh(optionsWin);
                        break;
                    case 's':
                        saveXML(optionsWin, inputWin, rtEngine);
                        break;
                    case 'i':
                        if (rtEngine.hasImageReady())
                            saveImage(optionsWin, inputWin, rtEngine);
                        else {
                            mvwprintw(optionsWin, 10, 0, "Must render a scene first");
                            mvwprintw(optionsWin, 11, 0, "Press enter to return");
                            wrefresh(optionsWin);
                            auto c = wgetch(inputWin);
                            while(c != '\n') {c = wgetch(inputWin); }
                            initOptWin(optionsWin);                            
                        }
                        break;
                    case 'q':
                        timeToClose = true;
                        erase();
                        break;
                    default:
                        if(has_colors() == FALSE) {	
                            start_color();			/* Start color 			*/
                            init_pair(1, COLOR_RED, COLOR_BLACK);
                            attron(COLOR_PAIR(1));
                        }
                            
                        mvwprintw(optionsWin, 10, 0, "Invalid option!");
                        wrefresh(optionsWin);
                }

                // initOptWin(optionsWin);
                // wrefresh(inputWin);
                // refresh();
            }
        };
        endwin();			/* End curses mode		  */
    }   
}


#endif