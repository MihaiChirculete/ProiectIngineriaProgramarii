cd /home/devteam/
git clone --recurse-submodules https://github.com/pistacheio/pistache.git
cd pistache
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
