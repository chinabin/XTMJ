echo "start to build"

echo "building XLCenterServer"

cd ./CenterServer
sh make.sh
make -j4

cd ..
echo "building XLLoginGate"
cd ./LoginServer
sh make.sh
make -j4

cd ..
echo "building XLLogicDB"
cd ./LogicDB
sh make.sh
make -j4

cd ..
echo "building XLLogicManager"
cd ./LogicManager
sh make.sh
make -j4

cd ..
echo "building XLLogicServer"
cd ./LogicServer
sh make.sh
make -j4

cd ..
echo "building XLGateServer"
cd ./GateServer
sh make.sh
make -j4

cd ..
echo "building XLCoinsServer"
cd ./CoinsServer
sh make.sh
make -j4

echo "building finished!!!"

cd ..

echo "please check creation date of all executable files"

ls -R -l|grep XL

