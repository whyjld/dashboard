#/usr/bin/bash
python3 generator.py animate.json template.h template.cpp DashBoard
cd ../prj
make
cd ../src
