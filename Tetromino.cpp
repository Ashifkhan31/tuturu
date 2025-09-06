#include "Eigen/Dense"
#include <map>
#include "raylib/raylib.h"

Eigen::Matrix2i rot0
{
    {1,0},
    {0,1}
};

Eigen::Matrix2i rot90
{
    {0,1},
    {-1,0}
};

Eigen::Matrix2i rot180
{
    {-1,0},
    {0,-1}
};

Eigen::Matrix2i rot270
{
    {0,-1},
    {1,0}
};

enum Tetromino_Type
{
    T,
    Z_left,
    Z_right,
    L_left,
    L_right,
    I,
    O,
    NONE
};

const std::map<Tetromino_Type, Color> TetrominoToColor
{
    {T, GREEN},
    {Z_left, BLUE},
    {Z_right, PINK},
    {L_left, VIOLET},
    {L_right, RED},
    {I, ORANGE},
    {O, YELLOW},
    {NONE, BLANK}
}; 

class BLOCK
{
    std::vector<Eigen::Vector2i> block;
    Tetromino_Type type;

    public:

        BLOCK(std::vector<Eigen::Vector2i> _block, Tetromino_Type _type)
        : block(_block), type(_type) {}
          
        Tetromino_Type getType() const
        {
            return type;
        }

        std::vector<Eigen::Vector2i> getBlocks(int turn) const 
        {
            Eigen::Matrix2i rotationMat;

            int modulo4 = turn % 4;

            switch (modulo4)
            {
                case 1:
                    rotationMat = rot270;
                    break;
                case 2:
                    rotationMat = rot180;
                    break;
                case 3:
                    rotationMat = rot90;
                    break;
                default:
                    rotationMat = rot0;
                    break;
            }

            std::vector<Eigen::Vector2i> transformedBlocks;
            transformedBlocks.resize(block.size());

            for(int i = 0; i < transformedBlocks.size(); i += 1)
            {
                transformedBlocks[i] = rotationMat * block[i];
            }

            return transformedBlocks;
        }
}; 


const std::vector<BLOCK> blockTypes
{
    BLOCK{ { {0,0}, {1,0}, {-1,0}, {0,1} }, T},
    BLOCK{ { {0,0}, {-1,0}, {1,0}, {1,1} }, L_left},
    BLOCK{ { {0,0}, {-1,0}, {1,0}, {-1,1} }, L_right},
    BLOCK{ { {0,0}, {-1,0}, {0,1}, {1,1} }, Z_left},
    BLOCK{ { {0,0}, {1,0}, {0,1}, {-1,1} }, Z_right},
    BLOCK{ { {0,0}, {-1,0}, {1,0}, {2,0} }, I},
    BLOCK{ { {0,0}, {1,0}, {1,1}, {0,1}}, O}
};


