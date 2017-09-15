cd ..

# salvar .sdf a partir do .rsdf parametrizavel
erb Models/VT_simplificado/model.rsdf > Models/VT_simplificado/model.sdf

# adicionar caminho para a textura
source /usr/share/gazebo/setup.sh
export GAZEBO_RESOURCE_PATH=$(pwd)/Textures:$GAZEBO_RESOURCE_PATH

# adicionar caminho para o modelo
export GAZEBO_MODEL_PATH=$(pwd)/Models:$GAZEBO_MODEL_PATH

# iniciar gazebo com mundo parado
gazebo Worlds/vt_bowlfield.world -u
