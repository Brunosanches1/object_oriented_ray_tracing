#include <ncurses.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <stdexcept>
#include <memory>
#include "sphere.hpp"
#include "moving_sphere.hpp"
#include "material.hpp"
#include "engine.hpp"
#include "vec3.hpp"

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
        wprintw(win, "c - Create a new scene\n");
        wprintw(win, "r - Recover scene from a XML file\n");
        wprintw(win, "p - Load example scene\n");
        wprintw(win, "s - Save scene in XML format\n");
        wprintw(win, "i - Save scene in image format\n");
        wprintw(win, "q - quit\n");
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

    std::string getParameter(WINDOW* optWin, WINDOW* inputWin, int line) {
        wmove(optWin, line, 0);
        wclrtoeol(optWin);
        wrefresh(optWin);
        keypad(inputWin, true);
        auto c = wgetch(inputWin);
        std::string value;
        while(c != '\n') {
            if (c == KEY_BACKSPACE || c == KEY_DC || c == 8) {
                wrefresh(optWin);
                wmove(optWin, line, 0);
                wrefresh(optWin);
                wclrtoeol(optWin);
                wrefresh(optWin);
                value.pop_back();
            }
            else {
                value.push_back(c);
            }
            mvwprintw(optWin, line, 0, value.c_str());
            wrefresh(optWin);
            c = wgetch(inputWin);
        }
        keypad(inputWin, false);
        return value;
    }

    double getDoubleParameter(WINDOW* optWin, WINDOW* inputWin, int line) {
        double val;
        while (true) {
            try {
                std::string s = getParameter(optWin, inputWin, line);
                val = std::stod(s);
                break;
            }
            catch (std::exception& e) {
                mvwprintw(optWin, line +1, 0, "Error in value. Try again");
                wrefresh(optWin);
            }
        }
        wmove(optWin, line+1, 0);
        wclrtoeol(optWin);
        wrefresh(optWin);

        return val;
    }

    int getIntParameter(WINDOW* optWin, WINDOW* inputWin, int line) {
        int val;
        while (true) {
            try {
                std::string s = getParameter(optWin, inputWin, line);
                val = std::stoi(s);
                break;
            }
            catch (std::exception& e) {
                mvwprintw(optWin, line +1, 0, "Error in value. Try again");
                wrefresh(optWin);
            }
        }
        wmove(optWin, line+1, 0);
        wclrtoeol(optWin);
        wrefresh(optWin);

        return val;
    }

    point3 getPoint3Parameter(WINDOW* optWin, WINDOW* inputWin, int line) {
        point3 val;
        char delimiter = ',';
        while (true) {
            try {
                std::string s = getParameter(optWin, inputWin, line);
                int i = 0;
                for (char c : s) {
                    if (c == ',') i++;
                }

                if (i != 2) throw std::invalid_argument("Not the correct number of commas");

                std::string x_str = s.substr(0, s.find(delimiter));

                s = s.substr(s.find(delimiter)+1);
                std::string y_str = s.substr(0, s.find(delimiter));

                std::string z_str = s.substr(s.find(delimiter)+1);

                val[0] = std::stod(x_str);
                val[1] = std::stod(y_str);
                val[2] = std::stod(z_str);

                break;
            }
            catch (std::exception& e) {
                mvwprintw(optWin, line +1, 0, "Error in value. Try again");
                wrefresh(optWin);
            }
        }
        wmove(optWin, line+1, 0);
        wclrtoeol(optWin);
        wrefresh(optWin);
        return val;
    }

    std::shared_ptr<material> selectMaterial(WINDOW* optWin, WINDOW* inputWin, int line) {
        mvwprintw(optWin, line++, 0, "------- Material Parameters -------");
        mvwprintw(optWin, line++, 0, "Select a material");
        mvwprintw(optWin, line++, 0, "1 - Lambertian");
        mvwprintw(optWin, line++, 0, "2 - Metal");
        mvwprintw(optWin, line++, 0, "3 - Dielectric");

        
        while(true) {
            int choice = getIntParameter(optWin, inputWin, line++);
            point3 color;
            switch (choice) {
                case 1:
                    mvwprintw(optWin, line++, 0, "Color R, G, B (ex: \"0.5, 0.5, 0.5\"): ");
                    color = getPoint3Parameter(optWin, inputWin, line++);

                    return std::make_shared<lambertian>(color);
                case 2:
                    double fuzz;
                    mvwprintw(optWin, line++, 0, "Color R, G, B (ex: \"0.5, 0.5, 0.5\"): ");
                    color = getPoint3Parameter(optWin, inputWin, line++);
                    mvwprintw(optWin, line++, 0, "Fuzz: ex: 2.0");
                    fuzz = getDoubleParameter(optWin, inputWin, line++);

                    return std::make_shared<metal>(color, fuzz);

                case 3:
                    double ir;
                    mvwprintw(optWin, line++, 0, "Index of refraction: ex: 2.0");
                    ir = getDoubleParameter(optWin, inputWin, line++);

                    return std::make_shared<dielectric>(ir);
                default:
                    mvwprintw(optWin, line+5, 0, "Invalid option!");
                    wrefresh(optWin);
            }
        }       

    }

    std::shared_ptr<sphere> createSphere(WINDOW* optWin, WINDOW* inputWin, int line) {
        mvwprintw(optWin, line++, 0, "------- Sphere Parameters -------");

        point3 center;
        double radius;
        mvwprintw(optWin, line++, 0, "Center of the sphere (ex: \"1, 2, 3\"): ");
        center = getPoint3Parameter(optWin, inputWin, line++);
        mvwprintw(optWin, line++, 0, "Radius: ");
        radius = getDoubleParameter(optWin, inputWin, line++);

        return std::make_shared<sphere>(center, radius, selectMaterial(optWin, inputWin, line));
    }

    std::shared_ptr<moving_sphere> createMovingSphere(WINDOW* optWin, WINDOW* inputWin, int line) {
        mvwprintw(optWin, line++, 0, "------- Moving Sphere Parameters -------");

        point3 center0, center1;
        double radius, time0, time1;
        mvwprintw(optWin, line++, 0, "Center of the sphere at time 0(ex: \"1, 2, 3\"): ");
        center0 = getPoint3Parameter(optWin, inputWin, line++);
        mvwprintw(optWin, line++, 0, "Time 0: ");
        time0 = getDoubleParameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Center of the sphere at time 1(ex: \"1, 2, 3\"): ");
        center1 = getPoint3Parameter(optWin, inputWin, line++);
        mvwprintw(optWin, line++, 0, "Time 1: ");
        time1 = getDoubleParameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Radius: ");
        radius = getDoubleParameter(optWin, inputWin, line++);

        return std::make_shared<moving_sphere>(center0, center1, time0, time1, radius, selectMaterial(optWin, inputWin, line));
    }

    void newScene(WINDOW* optWin, WINDOW* inputWin, Engine& rtEngine, sf::RenderWindow& window) {
        int line = 0;
        werase(optWin);
        wmove(optWin, 0, 0);

        int imgHeight, imgWidth, samples_per_pixel, max_depth;

        mvwprintw(optWin, line++, 0, "------- Image Parameters -------");

        mvwprintw(optWin, line++, 0, "Image Width: ");
        imgWidth = getIntParameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Image height: ");
        imgHeight = getIntParameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Samples Per Pixel: ");
        samples_per_pixel = getIntParameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Max Depth: ");
        max_depth = getIntParameter(optWin, inputWin, line++);

        rtEngine = Engine(imgWidth, imgHeight, samples_per_pixel, max_depth);

        line = 0;
        werase(optWin);
        wmove(optWin, 0, 0);
        point3 lookfrom, lookat, vup;
        double vfov, aperture, focus_dist, time0, time1;

        mvwprintw(optWin, line++, 0, "------- Camera Parameters -------");

        mvwprintw(optWin, line++, 0, "Camera Origin point: (ex: \"13, 2, 3\")");
        lookfrom = getPoint3Parameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Point the camera is looking at: (ex: \"0, 0, 0\")");
        lookat = getPoint3Parameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "View-up-vector vector: (ex: horizontal angle \"0, 1, 0\")");
        vup = lookfrom + getPoint3Parameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Vertical Field of View: ex: \"20.0\"");
        vfov = getDoubleParameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Aperture: ex: \"0.1\"");
        aperture = getDoubleParameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Focus Distance: ex: \"10.0\"");
        focus_dist = getDoubleParameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Time0: ex: \"0.0\"");
        time0 = getDoubleParameter(optWin, inputWin, line++);

        mvwprintw(optWin, line++, 0, "Time1: \"0.0\"");
        time1 = getDoubleParameter(optWin, inputWin, line++);

        rtEngine.setCamera(lookfrom, lookat, vup, vfov, aperture, focus_dist, time0, time1);

        bool exit = false;
        while(!exit) {
            line = 0;
            werase(optWin);
            wmove(optWin, line, 0);

            mvwprintw(optWin, line++, 0, "------- World Items -------");
            mvwprintw(optWin, line++, 0, "Select an Item to add");
            mvwprintw(optWin, line++, 0, "1 - Sphere");
            mvwprintw(optWin, line++, 0, "2 - Moving Sphere");
            mvwprintw(optWin, line++, 0, "0 - Exit");
            mvwprintw(optWin, line++, 0, "OBS: Don't Forget the ground material, we use a big sphere");

            wrefresh(optWin);

            int choice = getIntParameter(optWin, inputWin, line++);
            switch (choice) {
                case 1:
                    rtEngine.addToWorld(createSphere(optWin, inputWin, line));
                    break;
                case 2:
                    rtEngine.addToWorld(createMovingSphere(optWin, inputWin, line));
                    break;
                case 0:
                    exit = true;
                    break;
                default:
                    mvwprintw(optWin, line+5, 0, "Invalid option!");
                    wrefresh(optWin);
            }
        }

        sf::FloatRect visibleArea(0, 0, rtEngine.getImgWidth(), rtEngine.getImgHeight());
        window.setView(sf::View(visibleArea));

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
        sf::FloatRect visibleArea;

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
                    case 'c':
                        newScene(optionsWin, inputWin, rtEngine, window);
                        wrefresh(optionsWin);
                        break;
                    case 'r':
                        recoverXML(optionsWin, inputWin, rtEngine, window);
                        break;
                    case 'p':
                        rtEngine = Engine();
                        visibleArea = sf::FloatRect(0, 0, rtEngine.getImgWidth(), rtEngine.getImgHeight());
                        window.setView(sf::View(visibleArea));
                        wmove(optionsWin, 10, 0);
                        wclrtoeol(optionsWin);
                        wprintw(optionsWin, "Example scene loaded");
                        wrefresh(optionsWin);
                        break;
                    case 's':
                        saveXML(optionsWin, inputWin, rtEngine);
                        break;
                    case 'i':
                        if (rtEngine.hasImageReady()) {
                            saveImage(optionsWin, inputWin, rtEngine);
                            mvwprintw(optionsWin, 10, 0, "Image saved");
                            wrefresh(optionsWin);
                        }
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