source ~/workspace/toolkit/tman/tman.sh


rm -rf ~/trash/tmantest
tman init
echo "test env" | tman env add test
tman add test1
tman add test2
tman add test3
tman add test4
