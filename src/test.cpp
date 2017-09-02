#include <string>
#include <iostream>
#include <sstream>
#include "shareMemory.h"
#include "datas.h"

void Usage(char* app)
{
    std::cout << "Usage:" << app << " name value" << std::endl;
    std::cout << "Names:" << std::endl;
    std::cout << "\tmode:\t显示模式 [1, 4]" << std::endl;
    std::cout << "\tgear:\t档位 1-9 前进档 10:P 11:R 12:N 13:D" << std::endl;
    std::cout << "\tsph:\t时速 [0, 240]" << std::endl;
    std::cout << "\tes:\t发动机转速 [0, 8000]" << std::endl;
    std::cout << "\tom:\t油量 [0, 100]" << std::endl;
    std::cout << "\twt:\t水温 [0, 99]" << std::endl;
    std::cout << "\tmls:\t历程 [0, 1000000]" << std::endl;
    std::cout << "\ttrip:\t旅程 [0, 1000]" << std::endl;
    std::cout << "\tat:\t气温 [0, 99]" << std::endl;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        return 1;
    }
    
    try
    {
        ShareMemory<Datas> datas("DashBoard");
        std::string opt = argv[1];
    
        int val;
        try
        {
            std::stringstream ss(argv[2]);
            ss >> val;
        }
        catch(...)
        {
            std::cout << "Invalid value." << std::endl;
            Usage(argv[0]);
            return 1;
        }

        if(opt == "mode")
        {
            datas->Mode = val;
        }
        else if(opt == "sph")
        {
            datas->Speed = val;
        }
        else if(opt == "es")
        {
            datas->EngineSpeed = val;
        }
        else if(opt == "om")
        {
            datas->OilMass = val;
        }
        else if(opt == "wt")
        {
            datas->WaterTemperature = val;
        }
        else if(opt == "mls")
        {
            datas->Mileage = val;
        }
        else if(opt == "trip")
        {
            datas->Trip = val;
        }
        else if(opt == "at")
        {
            datas->AirTemperature = val;
        }
        else if(opt == "gear")
        {
            datas->Gears = val;
        }
        else
        {
            std::cout << "Invalid mode." << std::endl;
            Usage(argv[0]);
            return 1;
        }
    }
    catch(...)
    {
        std::cout << "Please run dashboard first." << std::endl;
        return 1;
    }
    
    return 0;
}