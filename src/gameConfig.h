#pragma once

#include <cstdint>

namespace Keg {

static const uint64_t GAME_MEMORY_SIZE  = 2ul * 1024 * 1024 * 1024;

static const int32_t MAX_ASSET_PATH_LENGTH = 2048;
static const int32_t MAX_ASSETS = 2048;

static const int32_t MAX_ENTITIES_COUNT = 4096;

static const int32_t SHADER_POSITION_ATTRIBUTE_LOCATION = 0;
static const int32_t SHADER_NORMAL_ATTRIBUTE_LOCATION = 1;
static const int32_t SHADER_TEXCOORDS_ATTRIBUTE_LOCATION = 2;

static const int32_t MAX_SHADERS_COUNT = 256;
static const int32_t MAX_SHADER_SOURCE_SIZE = 4096;
static const int32_t MAX_SHADER_ERROR_LOG_SIZE = 4096;
static const int32_t MAX_SHADER_PROGRAM_ERROR_LOG_SIZE = 4096;
static const int32_t MAX_SHADER_UNIFORMS = 16;
static const int32_t MAX_SHADER_UNIFORM_NAME_LENGTH = 32;

static const int32_t MAX_MESHES_COUNT = 256;
static const int32_t MAX_MESHES_DATA_SIZE = 32*1024*1024;
static const int32_t MESHES_DATA_BLOCK_SIZE = 4096;

static const int32_t MAX_TEXTURES_COUNT = 256;
static const int32_t MAX_TEXTURE_CUBES_COUNT = 256;

static const int32_t MAX_MATERIALS_COUNT = 256;
static const int32_t MAX_MATERIAL_TEXTURES_COUNT = 8;
static const int32_t MAX_MATERIAL_TEXTURE_CUBES_COUNT = 8;

static const int32_t MAX_IMAGES_COUNT = 256;
static const int32_t MAX_IMAGES_DATA_SIZE = 128*1024*1024;
static const int32_t IMAGES_DATA_BLOCK_SIZE = 16*4096;

}
