#include "Eigen/Dense"
#include "Tetromino.cpp"
#include <iostream>
#include <random>
#include <ctime>

const int NO_OF_BLOCKS_X = 10;
const int NO_OF_BLOCKS_Y = 17;

const float SCREEN_WIDTH = 1000/2.0;
const float SCREEN_HEIGHT = 1700/2.0;

const float BLOCK_SIZE = SCREEN_HEIGHT/NO_OF_BLOCKS_Y;

const float NORMAL_TICK = 0.7;
const float FAST_TICK = 0.08;

const Eigen::Vector2i BLOCK_START_POS{4,0};

typedef Eigen::Matrix<int, NO_OF_BLOCKS_X, NO_OF_BLOCKS_Y> Canvas;

std::mt19937 ranGen;

int score = 0;

struct Tetromino_info
{
    BLOCK Block;
    Eigen::Vector2i position;
    int orientation = 0;   
};

class Game_Canvas
{
    Canvas mainCanvas;
    Canvas tetrominoCanvas;
   
    public:

        Game_Canvas()
        {
            mainCanvas.fill(NONE);

            tetrominoCanvas.fill(NONE);
        }

        void drawTetromino(const Tetromino_info info)
        {
            std::vector<Eigen::Vector2i> blocks = info.Block.getBlocks(info.orientation);

            for(Eigen::Vector2i vec : blocks)
            {
                int indexX = info.position.x() + vec.x();
                int indexY = info.position.y() + vec.y();

                assert(indexX > -1 && indexX < NO_OF_BLOCKS_X);
                assert(indexY > -1 && indexY < NO_OF_BLOCKS_Y);
                
                tetrominoCanvas(indexX, indexY) = info.Block.getType();
            }
        }

        void burnTetromino(const Tetromino_info info)
        {
            std::vector<Eigen::Vector2i> blocks = info.Block.getBlocks(info.orientation);

            for(Eigen::Vector2i vec : blocks)
            {
                int indexX = info.position.x() + vec.x();
                int indexY = info.position.y() + vec.y();

                assert(indexX > -1 && indexX < NO_OF_BLOCKS_X);
                assert(indexY > -1 && indexY < NO_OF_BLOCKS_Y);
                
                mainCanvas(indexX, indexY) = info.Block.getType();
            }
        }

        void drawGameCanvas()
        {
            for(int y = 0; y < NO_OF_BLOCKS_Y; y += 1)
            {
                for(int x = 0; x < NO_OF_BLOCKS_X; x += 1)
                {
                    float posX = x * BLOCK_SIZE;
                    float posY = y * BLOCK_SIZE;

                    Tetromino_Type type1 = static_cast<Tetromino_Type>(mainCanvas(x,y));
                    Color color1 = TetrominoToColor.find(type1)->second;

                    Tetromino_Type type2 = static_cast<Tetromino_Type>(tetrominoCanvas(x,y));
                    Color color2 = TetrominoToColor.find(type2)->second;
                    
                    DrawRectangle(posX, posY, BLOCK_SIZE, BLOCK_SIZE, color1);                  
                    DrawRectangle(posX, posY, BLOCK_SIZE, BLOCK_SIZE, color2);
                }
            }

            tetrominoCanvas.fill(NONE);
        }

        Canvas* getmainCanvas()
        {
            return &mainCanvas;
        }

        const Canvas* getTetrominoCanvas()
        {
            return &tetrominoCanvas;
        }
};

bool isValidTetrominoPosition(const Canvas* mainCanvas, Tetromino_info info)
{
    for(Eigen::Vector2i vec : info.Block.getBlocks(info.orientation))
    {
        int blockX = vec.x() + info.position.x();
        int blockY = vec.y() + info.position.y();

        if(blockX < 0 || blockX >= NO_OF_BLOCKS_X)
        {
            return false;    
        }
        if(blockY < 0 || blockY >= NO_OF_BLOCKS_Y)
        {
            return false;
        }
        if((*mainCanvas)(blockX, blockY) != NONE)
        {
            return false;
        }
    }

    return true;
}

void handleInput(Tetromino_info& Ti, const Canvas* mainCanvas, float& tickTime)
{
    if(IsKeyPressed(KEY_LEFT))
    {
        Eigen::Vector2i newPos = Ti.position - Eigen::Vector2i(1,0);

        if(isValidTetrominoPosition(mainCanvas, {Ti.Block, newPos, Ti.orientation}))
        {
            Ti.position = newPos;    
        }
    }
    else if(IsKeyPressed(KEY_RIGHT))
    {
        Eigen::Vector2i newPos = Ti.position + Eigen::Vector2i(1,0);

        if(isValidTetrominoPosition(mainCanvas, {Ti.Block, newPos, Ti.orientation}))
        {
            Ti.position = newPos;    
        }
    }

    if(IsKeyPressed(KEY_UP))
    {
        int newOrientation = Ti.orientation + 1;

        if (Ti.Block.getType() == O)
        {
            // do nothing
        }
        else if(Ti.Block.getType() == I)
        {
            if(isValidTetrominoPosition(mainCanvas, {Ti.Block, Ti.position, newOrientation % 2}))
            {
                Ti.orientation = newOrientation % 2;
            }
            else if(isValidTetrominoPosition(mainCanvas, {Ti.Block, Ti.position, 2}))
            {
                Ti.orientation = 2;
            }
        }
        else
        {
            if(isValidTetrominoPosition(mainCanvas, {Ti.Block, Ti.position, newOrientation}))
            {
                Ti.orientation = newOrientation;
            }
        }
    }

    if(IsKeyDown(KEY_DOWN))
    {
        tickTime = FAST_TICK;
    }
    else
    {
        tickTime = NORMAL_TICK;
    }
}

float tickTime = NORMAL_TICK;
float currentTick;

bool gameCore(Tetromino_info& currentTetromino, Game_Canvas& gameCanvas)
{        
    handleInput(currentTetromino, gameCanvas.getmainCanvas(), tickTime);

    if((GetTime() - currentTick) > tickTime)
    {
        Eigen::Vector2i newPos = currentTetromino.position + Eigen::Vector2i(0,1);
        Tetromino_info newInfo = {currentTetromino.Block, newPos, currentTetromino.orientation};
        
        if(isValidTetrominoPosition(gameCanvas.getmainCanvas(), newInfo))
        {
            currentTetromino.position = newPos;
        }
        else
        {
            gameCanvas.burnTetromino(currentTetromino);

            for(int y = 0; y < NO_OF_BLOCKS_Y; y += 1)
            {
                bool rowFull = true;

                for(int x = 0; x < NO_OF_BLOCKS_X; x += 1)
                {
                    if( (*gameCanvas.getmainCanvas())(x,y) == NONE )
                    {
                        rowFull = false; 
                    }            
                }

                if(rowFull)
                {
                    Canvas& rMainCanvas = (*gameCanvas.getmainCanvas());
                    Eigen::MatrixXi mat = rMainCanvas.block(0, 0, NO_OF_BLOCKS_X, y);

                    (*gameCanvas.getmainCanvas()).block(0, 1, NO_OF_BLOCKS_X, y) = mat;

                    gameCanvas.getmainCanvas()->block(0, 0, NO_OF_BLOCKS_X, 1).fill(NONE);

                    score += 1;
                    std::cout<<"SCORE: "<<score<<"\n";
                }
            }

            int randomIndex = ranGen() % blockTypes.size();
            currentTetromino = Tetromino_info{blockTypes[randomIndex], BLOCK_START_POS, 0};

            if(!isValidTetrominoPosition(gameCanvas.getmainCanvas(), currentTetromino))
            {
                return true;
            }    
        }

        currentTick = GetTime();    
    }

    return false;
}
   
void playMusicIfNonePlaying(std::vector<Sound>& array)
{
    static Sound currentMusic;
    
    static std::vector<int> knapSack{0,1,2,3,4};
    
    bool silent = true;

    for(Sound& m : array)
    {
        if(IsSoundPlaying(m))
        {
            silent = false;
        }
    }

    if(silent)
    {
        if(knapSack.empty())
        {
            knapSack = {0,1,2,3,4};
            int index = ranGen() % knapSack.size();
            currentMusic = array[knapSack[index]];
            knapSack.erase(knapSack.begin() + index);
            PlaySound(currentMusic);
        }
        else
        {
            int index = ranGen() % knapSack.size();
            currentMusic = array[knapSack[index]];
            knapSack.erase(knapSack.begin() + index);
            PlaySound(currentMusic);
        }
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "tuturu");

    InitAudioDevice();

    ranGen.seed(static_cast<unsigned int>(std::time(nullptr)));

    std::vector<Sound> musicArray
    {
        LoadSound("audio/1.mp3"),
        LoadSound("audio/2.mp3"),
        LoadSound("audio/3.mp3"),
        LoadSound("audio/4.mp3"),
        LoadSound("audio/5.mp3")
    };
    
    playMusicIfNonePlaying(musicArray);

    while(!WindowShouldClose())
    {
        Game_Canvas gameCanvas{};
    
        Tetromino_info currentTetromino = {blockTypes[ranGen() % blockTypes.size()], BLOCK_START_POS};   

        std::cout<<"\n----GAME START----\n";
        bool gameOver = false;

        currentTick = GetTime();
        while(!gameOver)
        {
            if(WindowShouldClose())
            {
                break;
            }
            
            gameOver = gameCore(currentTetromino, gameCanvas);

            BeginDrawing();

                ClearBackground(BLACK);
        
                gameCanvas.drawTetromino(currentTetromino);
                gameCanvas.drawGameCanvas();

            EndDrawing();

            playMusicIfNonePlaying(musicArray);
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();

        score = 0;
        std::cout<<"----GAME OVER----\n\n";

        std::cout<<"Press ESC to quit or press Enter to continue\n\n";

        while(!WindowShouldClose())
        {
            playMusicIfNonePlaying(musicArray);
            if(IsKeyPressed(KEY_ENTER))
            {
                break;   
            }
            PollInputEvents();
        };
    }

    for(Sound& m : musicArray)
    {
        UnloadSound(m);
    }

    CloseAudioDevice();
    CloseWindow();
}
