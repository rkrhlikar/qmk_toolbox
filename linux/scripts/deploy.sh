#!/bin/sh

source_dir=$1
binary_dir=$2

if [ ! "$source_dir" ]; then
    exit -1
fi
if [ ! "$binary_dir" ]; then
    exit -1
fi

deploy_dir=$binary_dir/deploy

# Create deploy directory
mkdir -p $deploy_dir

# Copy assets and configs
cp -r $binary_dir/assets $deploy_dir/assets
cp -r $binary_dir/config $deploy_dir/config

# Copy executable binaries
cp -r $binary_dir/bin $deploy_dir/bin
# Add qmk_toolbox executable
cp $binary_dir/qmk_toolbox $deploy_dir/bin/qmk_toolbox

# Copy dependencies
mkdir -p $deploy_dir/lib
dependencies=$(ldd $binary_dir/qmk_toolbox | awk 'NF == 4 {print $3}; NF == 2 {print $1}' | grep ^/usr/)

for library in $dependencies; do
    cp $library $deploy_dir/lib/.
done

# Copy run script
cp $source_dir/scripts/run_script.sh $deploy_dir/qmk_toolbox
chmod +x $deploy_dir/qmk_toolbox