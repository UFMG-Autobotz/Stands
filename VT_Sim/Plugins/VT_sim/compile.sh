mkdir build
mkdir import
cd build
cmake ..
make
cp libvt_sim_plugin.so ../import/
rm -rf *
