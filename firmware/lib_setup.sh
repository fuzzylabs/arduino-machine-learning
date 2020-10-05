# This downloads an Arduino build of Tensorflow Lite
# Ideally the library should be on PlatformIO's library list, or at least we should reference a Git repo in the Platformio dependencies

wget http://downloads.arduino.cc/libraries/github.com/bcmi-labs/Arduino_TensorFlowLite-2.1.0-ALPHA-precompiled.zip
unzip Arduino_TensorFlowLite-2.1.0-ALPHA-precompiled.zip
mkdir lib
mv Arduino_TensorFlowLite-2.1.0-ALPHA-precompiled lib
rm Arduino_TensorFlowLite-2.1.0-ALPHA-precompiled.zip
