
using namespace std;
using std::vector;

class Menu {
private:
    string background = "games_dashboard";
    // Vector to store the config data of each game
    vector<ConfigData> _games;
    // Contains info about newly created process and thread
    PROCESS_INFORMATION processInfo;
    // Unsigned int to store exit info
    DWORD exit_code;
    // Holds the game path of selected game
    LPCSTR _gamePath;
    // Holds the executable of selected game
    LPSTR _gameExe;
    // Holds the game directory of selected game
    LPCSTR _gameDir;
    // Used to find x centre of screen
    double _center_x = 960;
    // Used to fine y centre of screen
    double _center_y = 540;
    // Stores x of mouse position
    double _x;
    // Stores y of mouse position
    double _y;
    // Checks if game is running
    bool _in_game = false;
    // Checks if program has exited
    bool _program_exit;
    // Vector of buttons
    vector<Button*> btns;
    // A selector
    Selector select;
    // The action to take
    string action;
    // Vectore to store game images
    vector<string> game_images;
    // Menu grid
    Grid _grid;
    ButtonNode *button = nullptr;

public:
    Menu(){}

    Menu(vector<ConfigData> configs)
    {
        this->_games = configs;
    }
    ~Menu(){}

    // Getters
    auto get_buttons() const -> const vector<Button*> { return this->btns; }

    // This function gets the game images from the config files and returns vector of game images.
    vector<string> get_game_sprites(vector<ConfigData> configs)
    {
        vector<string> game_images;

        for (int i = 0; i < configs.size(); i++)
        {
            // Get image dir and image name from games vector.
            string image = configs[i].folder() + "/" + configs[i].image();
            game_images.push_back(image);
        }

        return game_images;
    }

    void create_grid()
    {
        // Instantiate grid object
        Grid grid(8, 14);
        this->_grid = grid;
        // Update the background
        this->_grid.SetBackground(bitmap_named(this->background));
    }

    // This function creates the game buttons from the game images.
    void create_buttons()
    {
        // Call function to get game images.
        game_images = get_game_sprites(_games);

        for (int i = 0; i < game_images.size(); i++)
        {
            if (i == 0)
            {
                this->button = new ButtonNode(new GameScreenButton(Button::GAME, game_images[0]));
                this->button->config = _games[0];
            }
            else
            {
                string image = game_images[i];
                this->button->addBefore(new ButtonNode(new GameScreenButton(Button::GAME, image)));
                this->button->getPrev()->config = _games[i];
            }
        }
    }

    // Draw the game buttons to the window, using the carousel layout
    void update_carousel()
    {
        if (this->button)
        {
            this->_grid.UpdateCell(this->button->getPrev()->button, 2, 0, 1, false);
            this->_grid.UpdateCell(this->button->button, 2, 5, 1, false);
            this->_grid.UpdateCell(this->button->getNext()->button, 2, 10, 1, false);
        }
    }

    //Handle carousel input
    void carousel_handler()
    {
        if (key_typed(RIGHT_KEY))
        {
            this->button = button->getNext();
        }
        else if (key_typed(LEFT_KEY))
        {
            this->button = button->getPrev();
        }
    }

    // Draw the background and call set game image.
    void draw_menu_page()
    {
        carousel_handler();
        update_carousel();
        this->_grid.DrawGrid();
    }



    // Determine which game button object is clicked on and get the game info for start game.
    void button_clicked(point_2d point)
    {
        for (int i = 0; i < game_images.size(); i++)
        {
            if (this->button)
            {
                    if (key_typed(RETURN_KEY))
                    {
                        // Get game path
                        _gamePath = (this->button->config.folder() + "/" + this->button->config.exe()).c_str();
                        // Get executable name
                        _gameExe = strdup(this->button->config.exe().c_str());
                        // Get game directory
                        _gameDir = this->button->config.folder().c_str();

                        // Set the center of the game
                        this->_x = _center_x;
                        this->_y = _center_y;

                        // Call method to open game executable
                        start_game(_gamePath, _gameExe, _gameDir);
                        return;
                    }
                }
            }
        }

    // Start up the chosen game using CreateProcessA.
    void start_game(LPCSTR gamePath,LPSTR gameExe, LPCSTR gameDirectory)
    {
        // Additional info
        STARTUPINFOA startupInfo;

        // Set the size of the structures
        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        ZeroMemory(&processInfo, sizeof(processInfo));

        // Start the program up
        WINBOOL gameProcess = CreateProcessA
        (
            gamePath,               // the path
            gameExe,                // Command line
            NULL,                   // Process handle not inheritable
            NULL,                   // Thread handle not inheritable
            FALSE,                  // Set handle inheritance to FALSE
            NORMAL_PRIORITY_CLASS,     // Don't open file in a separate console
            NULL,                    // Use parent's environment block
            gameDirectory,           // Use parent's starting directory
            &startupInfo,            // Pointer to STARTUPINFO structure
            &processInfo           // Pointer to PROCESS_INFORMATION structure
        );

        OpenProcess(PROCESS_QUERY_INFORMATION,TRUE, gameProcess);

        this->_in_game = true;
    }

    // Method to keep the mouse positioned within the game window.
    void move_mouse_position(point_2d mousePoint)
    {
        if (this->_in_game == true)
        {
            // Check if game has been exited.
            this->_program_exit = GetExitCodeProcess(processInfo.hProcess, &exit_code);
            if ((this->_program_exit) && (STILL_ACTIVE != exit_code))
            {
                this->_in_game = false;
            }
            // If game is not exited, keep the mouse pointer in the game window.
            int mouse_x = mousePoint.x;
            int mouse_y = mousePoint.y;
            if (mousePoint.x > this->_x || mousePoint.x < this->_x || mousePoint.y > this->_y || mousePoint.y < this->_y)
            {
                move_mouse(_center_x, _center_y);
                this->_x = mouse_x;
                this->_y = mouse_y;
            }
        }
    }
    
};
