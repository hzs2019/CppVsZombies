/*
 * @coding: utf-8
 * @Author: Chu Wenlong
 * @FilePath: \pvz_h\source\pvz_memory.cpp
 * @Date: 2019-10-10 23:35:37
 * @LastEditTime: 2020-03-26 21:10:52
 * @Description: 内存读取函数及相关类的实现
 */

#include "libpvz.h"
#include "pvz_global.h"

namespace pvz
{

/* place */
PlaceMemory::PlaceMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0x11C);
    index = 0;
}

PlaceMemory::PlaceMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0x11C);
    index = i;
}

void PlaceMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0x11C);
}

/*  seed  */
SeedMemory::SeedMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0x144);
    index = 0;
}

SeedMemory::SeedMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0x144);
    index = i;
}

void SeedMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0x144);
}

/*  plant  */
PlantMemory::PlantMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0xAC);
    index = 0;
}

PlantMemory::PlantMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0xAC);
    index = i;
}

void PlantMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0xAC);
}

/* zombie */
ZombieMemory::ZombieMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0x90);
    index = 0;
}

ZombieMemory::ZombieMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0x90);
    index = i;
}

void ZombieMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0x90);
}

ItemMemory::ItemMemory()
{
    offset = ReadMemory<int>(g_mainobject + 0xE4);
    index = 0;
}

ItemMemory::ItemMemory(int i)
{
    offset = ReadMemory<int>(g_mainobject + 0xE4);
    index = i;
}

void ItemMemory::getOffset()
{
    offset = ReadMemory<int>(g_mainobject + 0xE4);
}

/* 一些常用的函数 */

//等待游戏开始
void WaitGameStart()
{
    while (GameUi() != 3)
        Sleep(1);
}

//等待游戏结束
void WaitGameEnd()
{
    while (GameUi() == 3)
        Sleep(10);
}

//获取指定类型植物的卡槽对象序列 植物类型与图鉴顺序相同，从0开始
//返回的卡片对象序列范围：[0,9]
int GetSeedIndex(int type, bool imitator)
{
    SeedMemory seed;
    int slot_count = seed.slotsCount();
    for (int i = 0; i < slot_count; ++i)
    {
        seed.setIndex(i);
        if (imitator)
        {
            if ((seed.type() == 48) && (seed.imitatorType() == type))
                return i;
        }
        else if (seed.type() == type)
            return i;
    }
    return -1;
}

//得到指定位置和类型的植物对象序列
//当参数type为默认值-1时该函数无视南瓜花盆荷叶
//使用示例：
//GetPlantIndex(3,4)------如果三行四列有除南瓜花盆荷叶之外的植物时，返回该植物的对象序列，否则返回-1
//GetPlantIndex(3,4,47)---如果三行四列有春哥，返回其对象序列，如果有其他植物，返回-2，否则返回-1
int GetPlantIndex(int row, int col, int type)
{
    PlantMemory plant;
    int plants_count_max = plant.countMax();
    int plant_type;
    for (int i = 0; i < plants_count_max; ++i)
    {
        plant.setIndex(i);
        if ((!plant.isDisappeared()) && (!plant.isCrushed()) &&
            (plant.row() + 1 == row) && (plant.col() + 1 == col))
        {
            plant_type = plant.type();
            if (type == -1)
            {
                //如果植物存在	且不为南瓜花盆荷叶
                if ((plant_type != 16) && (plant_type != 30) && (plant_type != 33))
                    return i; //返回植物的对象序列
            }
            else
            {
                if (plant_type == type)
                {
                    return i;
                }
                else if (type != 16 && type != 30 && type != 33 &&
                         plant_type != 16 && plant_type != 30 && plant_type != 33)
                {
                    return -2;
                }
            }
        }
    }
    return -1; //没有符合要求的植物返回-1
}

//得到一组指定位置的植物下标
//无视 荷叶、南瓜、花盆
void GetPlantIndexs(const std::vector<GRID> &grid_lst,
                    int type,
                    std::vector<int> &indexs)
{
    PlantMemory plant;
    g_mu.lock();
    indexs.assign(grid_lst.size(), -1);

    int plant_count_max = plant.countMax();
    int plant_type;

    for (int index = 0; index < plant_count_max; ++index)
    {
        plant.setIndex(index);
        if (!plant.isCrushed() && !plant.isDisappeared())
        {
            auto it = std::find(grid_lst.begin(), grid_lst.end(), GRID(plant.row() + 1, plant.col() + 1));
            if (it == grid_lst.end())
                continue;
            plant_type = plant.type();
            // 目标植物存在
            if (plant_type == type)
            {
                indexs[it - grid_lst.begin()] = index;
            }
            // 目标植物不存在但是有其他植物
            else if (type != 16 && type != 30 && type != 33 &&
                     plant_type != 16 && plant_type != 30 && plant_type != 33) // 荷叶、南瓜、花盆
            {
                indexs[it - grid_lst.begin()] = -2;
            }
        }
    }
    g_mu.unlock();
}

//检查僵尸是否存在
//使用示例
//ExamineZombieExist()-------检查场上是否存在僵尸
//ExamineZombieExist(23)-------检查场上是否存在巨人僵尸
//ExamineZombieExist(-1,4)-----检查第四行是否有僵尸存在
//ExamineZombieExist(23,4)-----检查第四行是否有巨人僵尸存在
bool ExamineZombieExist(int type, int row)
{
    ZombieMemory zombie;
    int zombies_count_max = zombie.countMax();

    for (int i = 0; i < zombies_count_max; i++)
    {
        zombie.setIndex(i);
        if (zombie.isExist() && !zombie.isDead())
            if (type < 0 && row < 0)
                return true;
            else if (type >= 0 && row >= 0)
            {
                if (zombie.row() == row - 1 && zombie.type() == type)
                    return true;
            }
            else if (type < 0 && row >= 0)
            {
                if (zombie.row() == row - 1)
                    return true;
            }
            else //if (type >= 0 && row < 0)
            {
                if (zombie.type() == type)
                    return true;
            }
    }

    return false;
}

//判断该格子是否能种植物
//该函数不适用于屋顶场景
//此函数只能判断此格子能不能种植物，但并不能判断此格子是否有植物
//使用示例：
//IsPlantable(3,3)-----3行3列如果能种植物(即使此格子有非南瓜花盆荷叶的植物)，返回true,否则返回false
bool IsPlantable(int row, int col, bool hui_jin)
{
    PlaceMemory place;

    int places_count_max = place.countMax();
    for (int i = 0; i < places_count_max; i++)
    {
        place.setIndex(i);
        //如果该位置存在弹坑或墓碑
        if (place.isExist() && (place.type() == 1 || place.type() == 2) &&
            place.row() == row - 1 && place.col() == col - 1)
            return false;
    }
    //如果该位置有冰道
    if (col * 80 + 30 > IceAbscissa(row - 1))
        return false;
    //如果不是灰烬
    if (!hui_jin)
    {
        ZombieMemory zombie;
        int ZCM = zombie.countMax();
        for (int i = 0; i < ZCM; i++)
        {
            zombie.setIndex(i);
            if (zombie.isExist() && !zombie.isDead())
            {
                int Ztype = zombie.type();
                int Zrow = zombie.row() + 1;
                float Zabsci = zombie.abscissa();
                //如果该行存在冰车
                if (Ztype == 12 && Zrow == row)
                {
                    if (Zabsci < col * 80 + 30)
                        return false;
                }
            }
        }
    }
    return true;
}

// !!!!!!!!!!，待修正
bool IsExplode(int row, int col)
{
    ZombieMemory zombie;
    int ZCM = zombie.countMax();
    for (int i = 0; i < ZCM; i++)
    {
        zombie.setIndex(i);
        if (zombie.isExist() && !zombie.isDead())
        {
            int Ztype = zombie.type();
            int Zrow = zombie.row() + 1;
            float Zabsci = zombie.abscissa();
            //如果存在小丑且开盒
            if (Ztype == 15 && zombie.state() == 16)
            {
                if (Zrow == row - 1 && Zabsci < col * 80 + 42 && Zabsci > col * 80 - 162 ||
                    Zrow == row && Zabsci < col * 80 + 61 && Zabsci > col * 80 - 181 ||
                    Zrow == row + 1 && Zabsci < col * 80 + 52 && Zabsci > col * 80 - 173)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

// !!!!!!!!!!，待修正
bool IsHammering(int row, int col, bool pumpkin)
{
    int increased_distance = 0;

    if (pumpkin)
        increased_distance = 30;
    ZombieMemory zombie;
    int ZCM = zombie.countMax();
    for (int i = 0; i < ZCM; i++)
    {
        zombie.setIndex(i);
        if (!zombie.isDead() && zombie.isExist())
        {
            int Ztype = zombie.type();
            int Zrow = zombie.row();
            float Zabsci = zombie.abscissa();
            if (Zrow + 1 == row && (Ztype == 23 || Ztype == 32) &&
                Zabsci < (col + 0.60) * 80 + increased_distance && Zabsci > (col - 0.40) * 80)
            {
                //如果巨人举锤
                if (zombie.isHammering())
                    return true;
            }
        }
    }

    return false;
}

//得到僵尸出怪类型
void GetZombieType(std::vector<int> &zombie_type_list)
{
    zombie_type_list.clear();

    ZombieMemory zombie_memory;
    int zombie_cnt_max = zombie_memory.countMax();
    int zombie_type = 0;
    for (int index = 0; index < zombie_cnt_max; ++index)
    {
        zombie_memory.setIndex(index);
        if (!zombie_memory.isExist())
        {
            continue;
        }
        zombie_type = zombie_memory.type();
        if (std::find(zombie_type_list.begin(), zombie_type_list.end(), zombie_type) == zombie_type_list.end())
        {
            zombie_type_list.push_back(zombie_type);
        }
    }
}

//得到相应波数的出怪类型
void GetWaveZombieType(std::vector<int> &zombie_type_list, int _wave)
{
    int type = 0;
    zombie_type_list.clear();
    for (int i = (_wave - 1) * 50; i < _wave * 50; ++i)
    {
        type = ReadMemory<int>(g_mainobject + 0x6B4 + i * 4);
        if (std::find(zombie_type_list.begin(), zombie_type_list.end(), type) == zombie_type_list.end())
        {
            zombie_type_list.push_back(type);
        }
    }
}

void UpdateZombiesPreview()
{
    int z_cnt_max = ZombieMemory::countMax();
    uintptr_t z_offset = ReadMemory<uintptr_t>(g_mainobject + 0x90);
    for (int index = 0; index < z_cnt_max; ++index)
    {
        WriteMemory<bool>(true, z_offset + 0xEC + index * 0x15C);
        WriteMemory<int>(3, z_offset + 0x28 + index * 0x15C);
    }
    WriteMemory<bool>(false, g_mainobject + 0x15C, 0x35);
}

void SetZombies(const std::vector<int> &zombie_type)
{
    // 等待 "Surivial Endless" 字样消失
    while (ReadMemory<bool>(g_mainobject + 0x140, 0x88))
    {
        Sleep(1);
    }

    std::vector<int> zombie_type_vec;

    for (const auto &type : zombie_type)
    {
        // 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
        if (type != QZ_1 &&
            type != BW_9 &&
            type != XQ_13 &&
            type != XR_19 &&
            type != BJ_20 &&
            type != XG_24)
        {
            zombie_type_vec.push_back(type);
        }
    }

    std::array<uint32_t, 1000> zombie_list;
    for (int index = 0; index < 1000; ++index)
    {
        zombie_list[index] = zombie_type_vec[index % zombie_type_vec.size()];
    }

    // 生成旗帜
    for (auto index : {450, 950})
    {
        zombie_list[index] = QZ_1;
    }

    // 生成蹦极
    for (auto index : {451, 452, 453, 454, 951, 952, 953, 954})
    {
        zombie_list[index] = BJ_20;
    }

    WriteMemory(zombie_list, g_mainobject + 0x6B4);

    if (GameUi() == 2)
    {
        UpdateZombiesPreview();
    }
}

void SetWaveZombies(int _wave, const std::vector<int> &zombie_type)
{
    while (ReadMemory<bool>(g_mainobject + 0x140, 0x88))
    {
        Sleep(1);
    }

    std::vector<int> zombie_type_vec;

    for (const auto &type : zombie_type)
    {
        // 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
        if (type != QZ_1 &&
            type != BW_9 &&
            type != XQ_13 &&
            type != XR_19 &&
            type != BJ_20 &&
            type != XG_24)
        {
            zombie_type_vec.push_back(type);
        }
    }

    std::array<uint32_t, 50> zombie_list;
    for (int index = 0; index < 50; ++index)
    {
        zombie_list[index] = zombie_type_vec[index % zombie_type_vec.size()];
    }

    WriteMemory(zombie_list, g_mainobject + 0x6B4 + (_wave - 1) * 50 * 4);

    // 生成旗帜
    for (auto index : {450, 950})
    {
        WriteMemory<uint32_t>(QZ_1, g_mainobject + 0x6B4 + index * 4);
    }

    // 生成蹦极
    for (auto index : {451, 452, 453, 454, 951, 952, 953, 954})
    {
        WriteMemory<uint32_t>(BJ_20, g_mainobject + 0x6B4 + index * 4);
    }
}
} // namespace pvz
