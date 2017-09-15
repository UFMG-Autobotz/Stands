source /usr/share/gazebo/setup.sh

cd ..
export GAZEBO_RESOURCE_PATH=$GAZEBO_RESOURCE_PATH:$(pwd)
cd VT_sim_plugin

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$(pwd)/plugins
gazebo --verbose VT_sim.world -u
