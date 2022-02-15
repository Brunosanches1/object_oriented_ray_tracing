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
    class term {

        public:
            /* Constructor */
            term(sf::RenderWindow&, Engine&);

            /* Init the ncurses terminal and set thread */
            void init();

            /* Join thread */
            void close();

            /* Return true if user demanded closing the application*/
            bool isTimeToClose();

        private:
            std::thread tGui;
            bool timeToClose = false;
            sf::RenderWindow& rtWindow;
            Engine& rtEngine;
            WINDOW* progressBarWindow, *headerWindow, *inputWin, *optWin;

             /* Define main terminal window */
            void main_ncurses();

            /* Update progress bar if the engine is working */
            void updateProgressBar();

            /* Create header*/
            void initHeaderWindow();
            
            /* Print options for the user */
            void initOptWin();

            /* Get input from the user as a string*/
            std::string getParameter(int line);

            /* Create a filename window and get input */
            std::string getFilename();
            
            /* Get input as a double*/
            double getDoubleParameter(int line);

            /* Get Input as an int*/
            int getIntParameter(int line);

            /* Get Input as a Point3 (eg. Vec3 or Color)*/
            point3 getPoint3Parameter(int line);

            /* Get a XML file and initialize the Ray Tracing Engine from it */
            void recoverXML();

            /* Save the current Ray Tracing Engine to a XML file that can be recharged */
            void saveXML();

            /* Save the rendered scene to an image*/
            void saveImage();

            /* Create a scene with all parameters as user inputs */
            void newScene();

            /* Create a moving sphere pointer with the user's inputs*/
            std::shared_ptr<moving_sphere> createMovingSphere(int line);

            /* Create a sphere pointer with user's input*/
            std::shared_ptr<sphere> createSphere(int line);

            /* Create a material pointer with user's input*/
            std::shared_ptr<material> selectMaterial(int line);
            
    };

    term::term(sf::RenderWindow& window, Engine& engine) : rtWindow(window), rtEngine(engine), progressBarWindow(), headerWindow(), optWin() {}

    void term::init() {
        tGui = std::thread(&term::main_ncurses, this);
    }

    void term::close() {
        tGui.join();
    }

    bool term::isTimeToClose() { return timeToClose; }

    void term::main_ncurses() {
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
        headerWindow = newwin(height, width, starty, startx);
        initHeaderWindow();
        // refresh();			/* Print it on to the real screen */

        /* Construct input window */
        height = 0;
        width = 20;
        startx = 0;
        starty = (LINES -1);
        inputWin = newwin(height, width, starty, startx);

        /* Construct options window */
        height = 30;
        width = 61;
        startx = 0;
        starty = 10;
        optWin = newwin(height, width, starty, startx);

        /* Construct progress bar window */
        height = 2;
        width = 61;
        // startx = (COLS - width) / 2;
        startx = 0;
        starty = (LINES - 2);
        progressBarWindow = newwin(height, width, starty, startx);

        sf::Sprite sprite(rtEngine.getTexture());

        while(!timeToClose) {
            if (rtEngine.isWorking()) {
                updateProgressBar();
                // window.setVisible(false);
            }
            else {
                wclear(progressBarWindow);
                wrefresh(progressBarWindow);

                initOptWin();
            }
        };
        endwin();			/* End curses mode		  */
    }   

    void term::updateProgressBar() {
        double progress = (double) (rtEngine.getImgHeight() - rtEngine.getRemainingLines()) / rtEngine.getImgHeight() * 100.0;
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end_time - rtEngine.workStartTime();

        double time_to_finish = (diff.count() / (rtEngine.getImgHeight() - rtEngine.getRemainingLines())) * 
                                    rtEngine.getRemainingLines();

        werase(progressBarWindow);
        wmove(progressBarWindow, 0, 0);
        wprintw(progressBarWindow, "[Elapsed time %7.1lf s]  [Remaining time %7.1lf s]\n", diff, time_to_finish);
        wprintw(progressBarWindow, "[");
        for (auto i = 2; i <= progress; i += 2){
            wprintw(progressBarWindow, "#");
        }
        mvwprintw(progressBarWindow, 1, 51, "] %5.1lf %%", progress);
        wrefresh(progressBarWindow);
    }

    void term::initHeaderWindow() {
        werase(headerWindow);
        wmove(headerWindow, 0, 0);
        wprintw(headerWindow, "//////////////////////////////////////////////////////");
        wprintw(headerWindow, "/              IN204 Project - Ray Tracer            /");
        wprintw(headerWindow, "/                                                    /");
        wprintw(headerWindow, "/             Authors: MACEDO SANCHES Bruno          /");
        wprintw(headerWindow, "/                OLIVEIRA DA SILVA Alexis            /");
        wprintw(headerWindow, "/                                                    /");
        wprintw(headerWindow, "//////////////////////////////////////////////////////");
        wrefresh(headerWindow);
    }

    void term::initOptWin() {
        werase(optWin);
        wmove(optWin, 0, 0);
        wprintw(optWin, "Press the key indicated to perform an action\n");
        wprintw(optWin, "Enter - Render the scene in a window\n");
        wprintw(optWin, "c - Create a new scene\n");
        wprintw(optWin, "r - Recover scene from a XML file\n");
        wprintw(optWin, "p - Load example scene\n");
        wprintw(optWin, "s - Save scene in XML format\n");
        wprintw(optWin, "i - Save scene in image format\n");
        wprintw(optWin, "q - quit\n");
        wrefresh(optWin);

        sf::FloatRect visibleArea;

        auto c = wgetch(inputWin);
        switch(c) {
            case '\n':
                rtEngine.setToWork();
                mvwprintw(optWin, 10, 0, "Working.... wait render to finish before pressing any key");
                wrefresh(optWin);
                break;
            case 'c':
                newScene();
                wrefresh(optWin);
                break;
            case 'r':
                recoverXML();
                break;
            case 'p':
                rtEngine = Engine();
                visibleArea = sf::FloatRect(0, 0, rtEngine.getImgWidth(), rtEngine.getImgHeight());
                rtWindow.setView(sf::View(visibleArea));
                wmove(optWin, 10, 0);
                wclrtoeol(optWin);
                wprintw(optWin, "Example scene loaded");
                mvwprintw(optWin, 11, 0, "Press enter to return");
                wrefresh(optWin);
                c = wgetch(inputWin);
                while(c != '\n') {c = wgetch(inputWin); } 
                wrefresh(optWin);
                break;
            case 's':
                saveXML();
                break;
            case 'i':
                if (rtEngine.hasImageReady()) {
                    saveImage();
                    mvwprintw(optWin, 10, 0, "Image saved");
                    wrefresh(optWin);
                }
                else {
                    mvwprintw(optWin, 10, 0, "Must render a scene first");
                    mvwprintw(optWin, 11, 0, "Press enter to return");
                    wrefresh(optWin);
                    c = wgetch(inputWin);
                    while(c != '\n') {c = wgetch(inputWin); }                      
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
                    
                mvwprintw(optWin, 10, 0, "Invalid option!");
                wrefresh(optWin);
        }
    }

    std::string term::getParameter(int line) {
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

    std::string term::getFilename() {
        werase(optWin);
        wmove(optWin, 0, 0);
        wprintw(optWin, "File path and name:\n");
        wrefresh(optWin);
        
        return getParameter(1);
    }

    double term::getDoubleParameter(int line) {
        double val;
        while (true) {
            try {
                std::string s = getParameter(line);
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

    int term::getIntParameter(int line) {
        int val;
        while (true) {
            try {
                std::string s = getParameter(line);
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

    point3 term::getPoint3Parameter(int line) {
        point3 val;
        char delimiter = ',';
        while (true) {
            try {
                std::string s = getParameter(line);
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

    void term::recoverXML() {
        std::string filename;
        
        while(true) {
            filename = getFilename();
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
    }

    void term::saveXML() {
        auto filename = getFilename();
        rtEngine.saveXmlDocument(filename.c_str());
        mvwprintw(optWin, 10, 0, "Saved! Press enter to return");
        wrefresh(optWin);
        auto c = wgetch(inputWin);
        while(c != '\n') {c = wgetch(inputWin); }
    }

    void term::saveImage() {
        auto filename = getFilename();
        rtEngine.getTexture().copyToImage().saveToFile(filename);
        mvwprintw(optWin, 10, 0, "Saved! Press enter to return");
        wrefresh(optWin);
        auto c = wgetch(inputWin);
        while(c != '\n') {c = wgetch(inputWin); }
    }

    void term::newScene() {
        int line = 0;
        werase(optWin);
        wmove(optWin, 0, 0);

        int imgHeight, imgWidth, samples_per_pixel, max_depth;

        mvwprintw(optWin, line++, 0, "------- Image Parameters -------");

        mvwprintw(optWin, line++, 0, "Image Width: ");
        imgWidth = getIntParameter(line++);

        mvwprintw(optWin, line++, 0, "Image height: ");
        imgHeight = getIntParameter(line++);

        mvwprintw(optWin, line++, 0, "Samples Per Pixel: ");
        samples_per_pixel = getIntParameter(line++);

        mvwprintw(optWin, line++, 0, "Max Depth: ");
        max_depth = getIntParameter(line++);

        rtEngine = Engine(imgWidth, imgHeight, samples_per_pixel, max_depth);

        line = 0;
        werase(optWin);
        wmove(optWin, 0, 0);
        point3 lookfrom, lookat, vup;
        double vfov, aperture, focus_dist, time0, time1;

        mvwprintw(optWin, line++, 0, "------- Camera Parameters -------");

        mvwprintw(optWin, line++, 0, "Camera Origin point: (ex: \"13, 2, 3\")");
        lookfrom = getPoint3Parameter(line++);

        mvwprintw(optWin, line++, 0, "Point the camera is looking at: (ex: \"0, 0, 0\")");
        lookat = getPoint3Parameter(line++);

        mvwprintw(optWin, line++, 0, "View-up-vector vector: (ex: horizontal angle \"0, 1, 0\")");
        vup = lookfrom + getPoint3Parameter(line++);

        mvwprintw(optWin, line++, 0, "Vertical Field of View: ex: \"20.0\"");
        vfov = getDoubleParameter(line++);

        mvwprintw(optWin, line++, 0, "Aperture: ex: \"0.1\"");
        aperture = getDoubleParameter(line++);

        mvwprintw(optWin, line++, 0, "Focus Distance: ex: \"10.0\"");
        focus_dist = getDoubleParameter(line++);

        mvwprintw(optWin, line++, 0, "Time0: ex: \"0.0\"");
        time0 = getDoubleParameter(line++);

        mvwprintw(optWin, line++, 0, "Time1: \"0.0\"");
        time1 = getDoubleParameter(line++);

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

            int choice = getIntParameter(line++);
            switch (choice) {
                case 1:
                    rtEngine.addToWorld(createSphere(line));
                    break;
                case 2:
                    rtEngine.addToWorld(createMovingSphere(line));
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
        rtWindow.setView(sf::View(visibleArea));
    }

    std::shared_ptr<moving_sphere> term::createMovingSphere(int line) {
        mvwprintw(optWin, line++, 0, "------- Moving Sphere Parameters -------");

        point3 center0, center1;
        double radius, time0, time1;
        mvwprintw(optWin, line++, 0, "Center of the sphere at time 0(ex: \"1, 2, 3\"): ");
        center0 = getPoint3Parameter(line++);
        mvwprintw(optWin, line++, 0, "Time 0: ");
        time0 = getDoubleParameter(line++);

        mvwprintw(optWin, line++, 0, "Center of the sphere at time 1(ex: \"1, 2, 3\"): ");
        center1 = getPoint3Parameter(line++);
        mvwprintw(optWin, line++, 0, "Time 1: ");
        time1 = getDoubleParameter(line++);

        mvwprintw(optWin, line++, 0, "Radius: ");
        radius = getDoubleParameter(line++);

        return std::make_shared<moving_sphere>(center0, center1, time0, time1, radius, selectMaterial(line));
    }

    std::shared_ptr<sphere> term::createSphere(int line) {
        mvwprintw(optWin, line++, 0, "------- Sphere Parameters -------");

        point3 center;
        double radius;
        mvwprintw(optWin, line++, 0, "Center of the sphere (ex: \"1, 2, 3\"): ");
        center = getPoint3Parameter(line++);
        mvwprintw(optWin, line++, 0, "Radius: ");
        radius = getDoubleParameter(line++);

        return std::make_shared<sphere>(center, radius, selectMaterial(line));
    }

    std::shared_ptr<material> term::selectMaterial(int line) {
        mvwprintw(optWin, line++, 0, "------- Material Parameters -------");
        mvwprintw(optWin, line++, 0, "Select a material");
        mvwprintw(optWin, line++, 0, "1 - Lambertian");
        mvwprintw(optWin, line++, 0, "2 - Metal");
        mvwprintw(optWin, line++, 0, "3 - Dielectric");

        
        while(true) {
            int choice = getIntParameter(line++);
            point3 color;
            switch (choice) {
                case 1:
                    mvwprintw(optWin, line++, 0, "Color R, G, B (ex: \"0.5, 0.5, 0.5\"): ");
                    color = getPoint3Parameter(line++);

                    return std::make_shared<lambertian>(color);
                case 2:
                    double fuzz;
                    mvwprintw(optWin, line++, 0, "Color R, G, B (ex: \"0.5, 0.5, 0.5\"): ");
                    color = getPoint3Parameter(line++);
                    mvwprintw(optWin, line++, 0, "Fuzz: ex: 2.0");
                    fuzz = getDoubleParameter(line++);

                    return std::make_shared<metal>(color, fuzz);

                case 3:
                    double ir;
                    mvwprintw(optWin, line++, 0, "Index of refraction: ex: 2.0");
                    ir = getDoubleParameter(line++);

                    return std::make_shared<dielectric>(ir);
                default:
                    mvwprintw(optWin, line+5, 0, "Invalid option!");
                    wrefresh(optWin);
            }
        }       

    }

}


#endif