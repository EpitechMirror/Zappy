/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** AssetsManager
*/

#include "AssetsManager.hpp"

AssetsManager::AssetsManager(int width, int height) 
    : mapWidth(width), mapHeight(height) {}

AssetsManager::~AssetsManager()
{
}

void AssetsManager::loadAllResources()
{
    loadModels();
    loadTextures();
    loadShaders();
}

void AssetsManager::unloadAllResources()
{
    //NE PAS LIBÉRER LES TEXTURES, SINON ÇA CRASH VA SAVOIR PQ
    UnloadModel(floorModel);
    UnloadModel(playerModel);
    UnloadModel(wallLong);
    UnloadModel(wallShort);
    UnloadModel(deskModel);
    
    shaders.unloadAll();
    
    if (IsAudioDeviceReady()) {
        UnloadMusicStream(loadingMusic);
        UnloadMusicStream(mainMusic);
        CloseAudioDevice();
    }
}

void AssetsManager::applyShaders() {
    Shader& pbr = shaders.getPBR();
    for (int i = 0; i < floorModel.materialCount; ++i)
        floorModel.materials[i].shader = pbr;
    for (int i = 0; i < playerModel.materialCount; ++i)
        playerModel.materials[i].shader = pbr;
}

void AssetsManager::loadModels()
{
    float cellSize = 1.0f;
    float roomWidth = mapWidth * cellSize;
    float roomDepth = mapHeight * cellSize;
    float roomHeight = 4.0f;
    float wallThickness = 0.2f;

    floorModel = LoadModel("../resources/models/plane.glb");
    playerModel = LoadModel("../resources/models/pixar_lamp/scene.gltf");
    deskModel = LoadModel("../resources/models/wooden_desk.glb");
    wallLong = LoadModelFromMesh(GenMeshCube(roomWidth + wallThickness, roomHeight, wallThickness));
    wallShort = LoadModelFromMesh(GenMeshCube(wallThickness, roomHeight, roomDepth + wallThickness));
}

void AssetsManager::loadTextures()
{
    Texture2D floorAlbedo = LoadTexture("../resources/textures/wood_8.jpg");
    if (floorAlbedo.id > 0) {
        GenTextureMipmaps(&floorAlbedo);
        SetTextureFilter(floorAlbedo, TEXTURE_FILTER_ANISOTROPIC_16X);
        for (int i = 0; i < floorModel.materialCount; ++i) {
            floorModel.materials[i].maps[MATERIAL_MAP_ALBEDO].texture = floorAlbedo;
        }
    }

    Texture2D lampAlbedo = LoadTexture("../resources/models/pixar_lamp/PixarLamp_baseColor.jpeg");
    Texture2D lampNormal = LoadTexture("../resources/models/pixar_lamp/PixarLamp_normal.jpeg");
    Texture2D lampMRA = LoadTexture("../resources/models/pixar_lamp/PixarLamp_metallicRoughness.jpeg");
    Texture2D lampEmissive = LoadTexture("../resources/models/pixar_lamp/PixarLamp_emissive.jpeg");

    if (lampAlbedo.id > 0 && lampNormal.id > 0 && lampMRA.id > 0 && lampEmissive.id > 0) {
        for (int i = 0; i < playerModel.materialCount; ++i) {
            playerModel.materials[i].maps[MATERIAL_MAP_ALBEDO].texture = lampAlbedo;
            playerModel.materials[i].maps[MATERIAL_MAP_NORMAL].texture = lampNormal;
            playerModel.materials[i].maps[MATERIAL_MAP_METALNESS].texture = lampMRA;
            playerModel.materials[i].maps[MATERIAL_MAP_EMISSION].texture = lampEmissive;
        }
    }

    Texture2D wallTex = wallpaperTexture;
    if (wallTex.id > 0) {
        wallLong.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = wallTex;
        wallShort.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = wallTex;
    }

    Texture2D deskTex = LoadTexture("../resources/textures/wood_desk.jpg");
    if (deskTex.id > 0) {
        for (int i = 0; i < deskModel.materialCount; ++i) {
            deskModel.materials[i].maps[MATERIAL_MAP_ALBEDO].texture = deskTex;
        }
    }
}

void AssetsManager::loadShaders()
{
    shaders.loadPBR();
}

void AssetsManager::loadFonts() {
    toyFont = LoadFont("../resources/fonts/Woody.ttf");
}

void AssetsManager::loadAudio()
{
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }
    mainMusic = LoadMusicStream("../resources/music/main_music.ogg");
    if (mainMusic.stream.buffer != nullptr) {
        mainMusic.looping = true;
    }
}

// CHECK IF ASSETS ARE LOADED

bool AssetsManager::isModelLoaded(const std::string &modelPath) const {
    Model model = LoadModel(modelPath.c_str());
    bool isLoaded = (model.meshCount > 0);
    UnloadModel(model);
    return isLoaded;
}

bool AssetsManager::isTextureLoaded(const std::string &texturePath) const {
    Texture2D texture = LoadTexture(texturePath.c_str());
    bool isLoaded = (texture.id > 0);
    UnloadTexture(texture);
    return isLoaded;
}

bool AssetsManager::isShaderLoaded(const std::string &shaderPath) const {
    Shader shader = LoadShader(shaderPath.c_str(), nullptr);
    bool isLoaded = (shader.id > 0);
    UnloadShader(shader);
    return isLoaded;
}

bool AssetsManager::isFontLoaded(const std::string &fontPath) const {
    Font font = LoadFont(fontPath.c_str());
    bool isLoaded = (font.baseSize > 0);
    UnloadFont(font);
    return isLoaded;
}

bool AssetsManager::isAudioLoaded(const std::string &audioPath) const {
    Music music = LoadMusicStream(audioPath.c_str());
    bool isLoaded = (music.stream.buffer != nullptr && music.stream.sampleRate > 0);
    UnloadMusicStream(music);
    return isLoaded;
}
