
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd ${SCRIPT_DIR}
python car.py
make
./datagen > rom.v
cd ..
cp data/rom.v src/rom.v
