#include "TextureManager.h"
#include <sstream>
#include <iomanip>
#include <iostream>

/* This is a helper function that extracts all textures from a sprite sheet 
   and save each of them as an individual image. 
   To generate the individual sprites replace main with below:
#include <iostream>
#include <fstream>
#include <filesystem>
#include "include/TextureManager.h"

int main()
{
    try
    {
        TextureManager tex;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
*/

void saveTextureToFile(const sf::Texture& texture, const std::string& filename) 
{
    sf::Image image = texture.copyToImage();
    image.saveToFile(filename);
}

TextureManager::TextureManager() 
{

    std::cout << "Inside the TextureManager \n";
    sf::Texture textureSheet;
    if (!textureSheet.loadFromFile("assets/sheet.png")) 
    {
        std::cerr << "Error while opening file assets/sheet.png" << std::endl;
        exit(-1);
    }
    std::cout << "Loaded the texture sheet \n";

    // List of texture names and their corresponding IntRects
    struct TextureInfo 
    {
        std::string name;
        sf::IntRect rect;
    };

    std::cout << "Creating texture and Rect list \n";
    std::vector<TextureInfo> textures = 
    {
        {"MARIO_STAND", sf::IntRect(0, 254, TILE_SIZE, TILE_SIZE)},
        {"MARIO_RUN_1", sf::IntRect(17, 254, TILE_SIZE, TILE_SIZE)},
        {"MARIO_RUN_2", sf::IntRect(34, 254, TILE_SIZE, TILE_SIZE)},
        {"MARIO_RUN_3", sf::IntRect(51, 254, TILE_SIZE, TILE_SIZE)},
        {"MARIO_JUMP", sf::IntRect(85, 254, TILE_SIZE, TILE_SIZE)},
        {"MARIO_DRIFT", sf::IntRect(68, 254, TILE_SIZE, TILE_SIZE)},
        {"MARIO_GROWING", sf::IntRect(136, 271, TILE_SIZE, TILE_SIZE * 2)},
        {"SUPER_MARIO_STAND", sf::IntRect(0, 221, TILE_SIZE, TILE_SIZE * 2)},
        {"SUPER_MARIO_RUN_1", sf::IntRect(17, 221, TILE_SIZE, TILE_SIZE * 2)},
        {"SUPER_MARIO_RUN_2", sf::IntRect(34, 221, TILE_SIZE, TILE_SIZE * 2)},
        {"SUPER_MARIO_RUN_3", sf::IntRect(51, 221, TILE_SIZE, TILE_SIZE * 2)},
        {"SUPER_MARIO_JUMP", sf::IntRect(85, 221, TILE_SIZE, TILE_SIZE * 2)},
        {"SUPER_MARIO_DRIFT", sf::IntRect(68, 221, TILE_SIZE, TILE_SIZE * 2)},
        {"SUPER_MARIO_DUCK", sf::IntRect(102, 221, TILE_SIZE, TILE_SIZE * 2)},
        {"FLOOR", sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)},
        {"QUESTION_BLOCK_1", sf::IntRect(0, 204, TILE_SIZE, TILE_SIZE)},
        {"QUESTION_BLOCK_2", sf::IntRect(17, 204, TILE_SIZE, TILE_SIZE)},
        {"QUESTION_BLOCK_3", sf::IntRect(34, 204, TILE_SIZE, TILE_SIZE)},
        {"QUESTION_BLOCK_OFF", sf::IntRect(51, 204, TILE_SIZE, TILE_SIZE)},
        {"MUSHROOM", sf::IntRect(119, 204, TILE_SIZE, TILE_SIZE)},
        {"TUBE_TOP_LEFT", sf::IntRect(0, 34, TILE_SIZE, TILE_SIZE)},
        {"TUBE_TOP_RIGHT", sf::IntRect(17, 34, TILE_SIZE, TILE_SIZE)},
        {"TUBE_LEFT", sf::IntRect(0, 51, TILE_SIZE, TILE_SIZE)},
        {"TUBE_RIGHT", sf::IntRect(17, 51, TILE_SIZE, TILE_SIZE)},
        {"CLOUD", sf::IntRect(204, 51, TILE_SIZE, TILE_SIZE)},
        {"GOOMBA", sf::IntRect(119, 187, TILE_SIZE, TILE_SIZE)},
        {"GOOMBA_CRUSHED", sf::IntRect(102, 187, TILE_SIZE, TILE_SIZE)},
        {"BLOCK", sf::IntRect(34, 34, TILE_SIZE, TILE_SIZE)},
        {"BRICK", sf::IntRect(85, 0, TILE_SIZE, TILE_SIZE)},
        {"BUSH_LEFT", sf::IntRect(34, 0, TILE_SIZE, TILE_SIZE)},
        {"BUSH_CENTER", sf::IntRect(51, 0, TILE_SIZE, TILE_SIZE)},
        {"BUSH_RIGHT", sf::IntRect(68, 0, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_CLOUD_TOP_LEFT", sf::IntRect(0, 119, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_CLOUD_TOP_CENTER", sf::IntRect(17, 119, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_CLOUD_TOP_RIGHT", sf::IntRect(34, 119, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_CLOUD_BOTTOM_LEFT", sf::IntRect(0, 136, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_CLOUD_BOTTOM_CENTER", sf::IntRect(17, 136, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_CLOUD_BOTTOM_RIGHT", sf::IntRect(34, 136, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_MOUNTAIN_1", sf::IntRect(0, 17, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_MOUNTAIN_2", sf::IntRect(17, 17, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_MOUNTAIN_3", sf::IntRect(34, 17, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_MOUNTAIN_4", sf::IntRect(17, 0, TILE_SIZE, TILE_SIZE)},
        {"BACKGROUND_MOUNTAIN_5", sf::IntRect(51, 17, TILE_SIZE, TILE_SIZE)},
        {"BRICK_DEBRIS_1", sf::IntRect(136, 153, 8, 8)},
        {"BRICK_DEBRIS_2", sf::IntRect(144, 153, 8, 8)},
        {"BRICK_DEBRIS_3", sf::IntRect(136, 161, 8, 8)},
        {"BRICK_DEBRIS_4", sf::IntRect(144, 161, 8, 8)},
        {"COIN_1", sf::IntRect(153, 153, TILE_SIZE, TILE_SIZE)},
        {"COIN_2", sf::IntRect(170, 153, TILE_SIZE, TILE_SIZE)},
        {"COIN_3", sf::IntRect(187, 153, TILE_SIZE, TILE_SIZE)},
        {"COIN_4", sf::IntRect(204, 153, TILE_SIZE, TILE_SIZE)},
        {"TURTLE_1", sf::IntRect(119, 221, TILE_SIZE, TILE_SIZE)},
        {"TURTLE_2", sf::IntRect(136, 221, TILE_SIZE, TILE_SIZE)},
        {"TURTLE_SHELL_1", sf::IntRect(153, 237, TILE_SIZE, TILE_SIZE)},
        {"TURTLE_SHELL_2", sf::IntRect(170, 237, TILE_SIZE, TILE_SIZE)},
        {"MARIO_DEAD", sf::IntRect(102, 254, TILE_SIZE, TILE_SIZE)},
        {"FLAG_TOP", sf::IntRect(51, 34, TILE_SIZE, TILE_SIZE)},
        {"FLAG_RIGHT", sf::IntRect(51, 51, TILE_SIZE, TILE_SIZE)},
        {"FLAG_LEFT", sf::IntRect(34, 51, TILE_SIZE, TILE_SIZE)},
        {"FLAG_POLE", sf::IntRect(187, 119, TILE_SIZE, TILE_SIZE)},
        {"CASTLE_1", sf::IntRect(0, 68, TILE_SIZE, TILE_SIZE)},
        {"CASTLE_2", sf::IntRect(17, 68, TILE_SIZE, TILE_SIZE)},
        {"CASTLE_3", sf::IntRect(34, 68, TILE_SIZE, TILE_SIZE)},
        {"CASTLE_4", sf::IntRect(0, 85, TILE_SIZE, TILE_SIZE)},
        {"CASTLE_5", sf::IntRect(17, 85, TILE_SIZE, TILE_SIZE)},
        {"CASTLE_6", sf::IntRect(34, 85, TILE_SIZE, TILE_SIZE)},
        {"CASTLE_DOOR", sf::IntRect(17, 102, TILE_SIZE, TILE_SIZE)},
        {"CASTLE_8", sf::IntRect(34, 102, TILE_SIZE, TILE_SIZE)},
        {"COIN_SMALL_1", sf::IntRect(119, 254, TILE_SIZE, TILE_SIZE)},
        {"COIN_SMALL_2", sf::IntRect(129, 254, TILE_SIZE, TILE_SIZE)},
        {"COIN_SMALL_3", sf::IntRect(124, 262, TILE_SIZE, TILE_SIZE)},
        {"ONE_UP", sf::IntRect(136, 204, TILE_SIZE, TILE_SIZE)},
        {"ONE_UP_LABEL", sf::IntRect(184, 271, TILE_SIZE, TILE_SIZE)},
        {"ONEHUNDRED", sf::IntRect(153, 287, TILE_SIZE, TILE_SIZE)},
        {"TWOHUNDRED", sf::IntRect(153, 279, TILE_SIZE, TILE_SIZE)},
        {"THOUSAND", sf::IntRect(153, 287, TILE_SIZE, TILE_SIZE)},
        {"MARIO_FLAG_1", sf::IntRect(170, 170, TILE_SIZE, TILE_SIZE)},
        {"MARIO_FLAG_2", sf::IntRect(187, 170, TILE_SIZE, TILE_SIZE)}
    };
    std::cout << "Complete creating texture and Rect list \n";

    std::cout << "Creating each textures in loop and saving it \n";
    for (const auto& textureInfo : textures) 
    {
        sf::Texture t;
        t.loadFromImage(textureSheet.copyToImage(), textureInfo.rect);
        m_allTextureMap.insert_or_assign(textureInfo.name, t);

        std::stringstream filename;
        filename << "assets/sheetTex/" << textureInfo.name << ".png";
        saveTextureToFile(t, filename.str());
    }

    std::cout << "Completed Creating each textures in loop and saving it \n";
}
