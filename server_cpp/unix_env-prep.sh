echo ___________________________________________________________________________________
echo Running environment install for Ubuntu/Debian distros.
echo Expected runtime: 30 minutes
echo Authors: Gavin yu
echo ___________________________________________________________________________________
echo
#check admin
if [ "$(id -u)" -ne 0 ]; then
    echo "This script must be run as root"
    exit 1
fi

# Check if Ninja is installed
if command -v ninja >/dev/null 2>&1; then
    echo "Ninja has been installed"
else
    echo "Installing Ninja-build..."
    sudo apt-get update
    sudo apt install ninja-build
fi

# Check if Python is installed
if command -v python3 >/dev/null 2>&1; then
    echo "python3 is installed."
else
    echo "python3 is not installed."

    # Update package repositories
    sudo apt-get update
    # Install GCC
    sudo apt install python3.
fi

if command -v python3 >/dev/null 2>&1; then
    echo "python3 is installed."
else
    echo "python3 is not installed. python3 failed to install. python3 is needed to build mongoc..."
    exit 1
fi

# Check if GCC is installed
if command -v gcc >/dev/null 2>&1; then
    echo "GCC is installed."
else
    echo "GCC is not installed."

    # Update package repositories
    sudo apt-get update
    # Install GCC
    sudo apt-get install gcc
fi

if command -v gcc >/dev/null 2>&1; then
    echo "GCC is installed."
else
    echo "GCC is not installed. GCC failed to install. GCC is needed to build b2..."
    exit 1
fi

# Check if g++ is installed
if ! command -v g++ > /dev/null; then
    echo "g++ is not installed. Installing g++..."
    # Install cmake
    sudo apt-get install g++
else
    echo "g++ is already installed"
fi

if command -v g++ >/dev/null 2>&1; then
    echo "G++ is installed."
else
    echo "G++ is not installed. G++ failed to install. G++ is needed to build b2..."
    exit 1
fi

# Check if cmake is installed
if ! command -v cmake > /dev/null; then
    echo "CMake is not installed. Installing CMake..."
    # Install cmake
    sudo apt-get install cmake
else
    echo "CMake is already installed"
fi

if ! command -v cmake > /dev/null; then
    echo "CMAKE is not installed. CMAKE failed to install. CMAKE is needed to build mongo drivers..."
    exit 1
else
    echo "CMake is already installed"
fi



cd "/usr/local"
wget https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.gz
tar -xvf boost_1_79_0.tar.gz
rm "/usr/local/boost_1_79_0.tar.gz"
cd "/usr/local/boost_1_79_0"
./bootstrap.sh -with-toolset=gcc
./b2
./b2 install

#install prerequ for mongo
apt-get install cmake libssl-dev libsasl2-dev

cd "/usr/local"
wget https://github.com/mongodb/mongo-c-driver/releases/download/1.22.0/mongo-c-driver-1.22.0.tar.gz
tar -xvf "mongo-c-driver-1.22.0.tar.gz"
rm "/usr/local/mongo-c-driver-1.22.0.tar.gz"
cd "/usr/local/mongo-c-driver-1.22.0"
mkdir "cmake-build"
cd "cmake-build"
cmake -DMONGOC_TEST_USE_CRYPT_SHARED=OFF .. 
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_PREFIX_PATH=/usr/local ..
cmake --build .
cmake --build . --target install

cd "/usr/local"
wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.7/mongo-cxx-driver-r3.6.7.tar.gz
tar -xvf mongo-cxx-driver-r3.6.7.tar.gz
rm mongo-cxx-driver-r3.6.7.tar.gz
cd "/usr/local/mongo-cxx-driver-r3.6.7/build"
cmake .. -DBOOST_ROOT=/usr/local/boost_1_79_0 -DCMAKE_PREFIX_PATH=/usr/local/ -DCMAKE_INSTALL_PREFIX=/usr/local/
cmake --build .
cmake --build . --target install
