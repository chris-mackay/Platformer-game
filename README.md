# Platformer-game
2D platformer game written in `C++`. Meant to be a template/sandbox for experimenting. Not a complete game.

#### Utilizes [SDL 2.0 (Simple Directmedia Layer)](https://www.libsdl.org/)

![Game](game.png?raw=true "Game")

#### Draw text on the screen with `DrawText()`

```cpp
void DrawText(renderer, text, font, color, x, y);
```


#### Levels are created with [Tiled](https://www.mapeditor.org/) (See image below).

![Level1](levels/level1.png?raw=true "Level1")

#### Export levels as `CSV files (*.csv)` and save next to source code files.

![Export](export.png?raw=true "Export")


#### Below is the `void` that will load in the level file and create all the platforms for collision detection. 
#### Pressing Enter on the main keyboard (SDL_SCANCODE_RETURN) will cycle through the level files in the directory.
```cpp    
void LoadPlatforms(SDL_Renderer* ren)
{
    
    file = "level" + to_string(level) + ".csv";

    ifstream in(file);
    string line, field;
    vector<vector<string>> array; // 2D array
    vector<string> v; // array of values for one line only
    platforms.clear();

    while (getline(in, line)) // get next line in file
    {
        v.clear();
        stringstream ss(line);

        while (getline(ss, field, ',')) // break line into comma delimitted fields
        {
            v.push_back(field); // add each field to the 1D array
        }
        array.push_back(v); // add the 1D array to the 2D array
    }

    for (int i = 0; i < array.size(); ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (array[i][j] == "0") // 0 represents a platform in the csv file
            {
                int l = 100; // platform width
                int h = 15; // platform height

                // 8 columns * 100 (platform width) = 800 screen width
                int x = ((j + 1) * 100) - 100; 

                // 40 rows * 15 (platform height) = 600 screen height
                int y = ((i + 1) * 15) - 15;
                
                SDL_Rect platform{ x, y, l, h };
                platforms.push_back(platform); // add platform to the 1D array for collision testing
                SDL_RenderFillRect(ren, &platform);
            }
        }
    }
}
```






