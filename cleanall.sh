echo "start to clean"

echo "cleaning XLCenterServer"

cd ./CenterServer
sh make.sh
gmake clean

cd ..
echo "cleaning XLLoginServer"
cd ./LoginServer
sh make.sh
gmake clean

cd ..
echo "cleaning XLLogicDB"
cd ./LogicDB
sh make.sh
gmake clean

cd ..
echo "cleaning XLLogicManager"
cd ./LogicManager
sh make.sh
gmake clean

cd ..
echo "cleaning XLLogicServer"
cd ./LogicServer
sh make.sh
gmake clean

cd ..
echo "cleaning XLGateServer"
cd ./GateServer
sh make.sh
gmake clean

cd ..
echo "cleaning XLCoinsServer"
cd ./CoinsServer
sh make.sh
gmake clean

echo "cleaning finished!!!"

cd ..
