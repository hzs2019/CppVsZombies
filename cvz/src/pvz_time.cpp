/*
 * @coding: utf-8
 * @Author: Chu Wenlong
 * @FilePath: \pvz_h\source\pvz_time.cpp
 * @Date: 2019-10-10 23:11:52
 * @LastEditTime : 2020-01-31 14:14:51
 * @Description: 时间函数的实现
 */

#include "libpvz.h"
#include "pvz_global.h"
#include "pvz_error.h"

namespace pvz
{

//预判时间
void Prejudge(int t, int w)
{
    if (g_examine_level == CVZ_IGNORE_TIME)
        return;
    wave = w;
    if (g_examine_level == CVZ_INFO)
    {
        g_mu.lock();
        std::printf("  ################## wave:%d ##################\n\n", wave);
        g_mu.unlock();
    }
    int base_time;
    if (wave == 1)
        base_time = 550;
    else if (wave == 10 || wave == 20)
        base_time = 750;
    else
        base_time = 200;
    int now_wave = NowWave();
    //预判的目标波已刷出(这种情况在wave==10||wave==20时基本不可能出现，所以不考虑）
    if (now_wave == wave)
        g_zombie_refresh_time = Countdown() + GameClock() - InitialCountdown();
    //预判的目标波未刷出
    else if (now_wave < wave)
    {
        //等待下一波
        while (NowWave() < (wave - 1))
            Sleep(1);

        //如果是大波
        if (wave == 10 || wave == 20)
        {
            while (Countdown() > 4)
                Sleep(1);

            while (HugeWaveCountdown() > base_time)
                Sleep(1);
            g_zombie_refresh_time = HugeWaveCountdown() + GameClock();
        }
        else
        {
            while (Countdown() > base_time)
                Sleep(1);
            g_zombie_refresh_time = Countdown() + GameClock();
        }
    }

    else
        PrintError("Prejudge无法预判出僵尸刷新时间戳，请调整其他时间函数的使用");
    //如果时间已超过预定时间戳
    if ((GameClock() - g_zombie_refresh_time) > t)
        PrintError("预判的时间为 %d，现在的时间已到 %d，请检查其他函数的使用", t, GameClock() - g_zombie_refresh_time);

    if (g_examine_level == CVZ_INFO)
    {
        g_mu.lock();
        std::printf("	Prejudge time :%dcs...\n\n", t);
        g_mu.unlock();
    }
    //等待时间到达预定时间戳
    while ((GameClock() - g_zombie_refresh_time) < t - 2)
        Sleep(1);

    while ((GameClock() - g_zombie_refresh_time) < t)
        ;
}

//延迟一定时间
void Delay(int time)
{
    if (g_examine_level == CVZ_IGNORE_TIME)
        return;

    if (time >= 0)
    {
        //获取当前时钟
        int in_time = GameClock();

        if (g_examine_level == CVZ_INFO)
        {
            g_mu.lock();
            std::printf("	Delay time :%dcs...\n\n", time);
            g_mu.unlock();
        }

        while (GameClock() - in_time < time - 2)
            Sleep(1);

        while (GameClock() - in_time < time)
            ;
    }
    else
        PrintError("Delay函数参数不能为负数,当前的参数为 %d", time);
}

//等待时间到
void Until(int time)
{
    if (g_examine_level == CVZ_IGNORE_TIME)
        return;

    if ((GameClock() - g_zombie_refresh_time) > time)
        PrintError("Until目标时间戳为 %d，现在的时间已到 %d，请检查其他函数的使用",
                   time, GameClock() - g_zombie_refresh_time);
    if (g_examine_level == CVZ_INFO)
    {
        g_mu.lock();
        std::printf("	Until time arrive :%dcs...\n\n", time);
        g_mu.unlock();
    }
    //等待时间到达预定时间戳
    while ((GameClock() - g_zombie_refresh_time) < time - 2)
        Sleep(1);

    while ((GameClock() - g_zombie_refresh_time) < time)
        ;
}

void Ice3(int delay_time)
{
    if (g_examine_level == CVZ_IGNORE_TIME)
        return;

    RunningInThread([=]() {
        int ice3_time = delay_time + GameClock();
        Sleep((delay_time - 90) * 10);

        PlantMemory ice;
        int ice_index;
        int count_max = ice.countMax();
        for (ice_index = 0; ice_index < count_max; ++ice_index)
        {
            ice.setIndex(ice_index);
            // 寻找目标寒冰菇
            if (ice.type() == 14 && ice.state() == 2)
            {
                break;
            }
        }

        int plant_offset = ReadMemory<int>(g_mainobject + 0xAC);

        // 写入四次数据
        for (int i = 0; i < 4; ++i)
        {
            Sleep(20);
            int clock = GameClock();
            while (clock == GameClock())
                ;
            int adjust_countdown = ice3_time - GameClock() + 1;
            if (adjust_countdown != ice.activeCountdown())
                WriteMemory<int>(adjust_countdown, plant_offset + 0x50 + 0x14C * ice_index);
        }
    });
}

} // namespace pvz