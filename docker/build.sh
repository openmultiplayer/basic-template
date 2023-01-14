#!/bin/bash

# Available options:

# -c|--config <config> - Available configs: Debug, [RelWithDebInfo], Release
# -s|--shared <shared> - Available options: [true], false
# -u|--ubuntu <version> - Available options: 18.04, [20.04], 22.04
# -t|--tools <tools> - Available options: true, [false]
# -S|--server <server> - Available options: [true], false

while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -c|--config)
            config="$2"
            shift
            shift
            ;;
        -s|--shared)
            build_shared="$2"
            shift
            shift
            ;;
        -u|--ubuntu)
            ubuntu_version="$2"
            shift
            shift
            ;;
        -t|--tools)
            build_tools="$2"
            shift
            shift
            ;;
        -S|--server)
            build_server="$2"
            shift
            shift
            ;;
        -h|--help)
            echo "Available options:"
            echo ""
            echo "-c|--config <config> - Available configs: Debug, [RelWithDebInfo], Release"
            echo "-s|--shared <shared> - Available options: [true], false"
            echo "-u|--ubuntu <version> - Available options: 18.04, [20.04], 22.04"
            echo "-t|--tools <tools> - Available options: true, [false]"
            echo "-S|--server <server> - Available options: [true], false"
            exit 0
            ;;
        *)
            shift
            ;;
    esac
done

# Available configs: Debug, [RelWithDebInfo], Release

if [[ -z "${config}" ]]; then
    config="RelWithDebInfo"
fi

# Available options: 18.04, [20.04], 22.04

if [[ -z "${ubuntu_version}" ]]; then
    ubuntu_version="20.04"
fi

# Available options: [true], false

if [[ -z "${build_shared}" ]]; then
    build_shared=1
fi

# Available options: [true], false

if [[ -z "${build_server}" ]]; then
    build_server=1
fi

# Available options: true, [false]

if [[ -z "${build_tools}" ]]; then
    build_tools=0
fi


docker build \
    -t open.mp/build:ubuntu-${ubuntu_version} \
    build_ubuntu-${ubuntu_version}/ \
|| exit 1

folders=('build' 'conan')
for folder in "${folders[@]}"; do
    if [[ ! -d "./${folder}" ]]; then
        mkdir ${folder} &&
        chown 1000:1000 ${folder} || exit 1
    fi
done

docker run \
    --rm \
    -t \
    -w /code \
    -v $PWD/..:/code \
    -v $PWD/build:/code/build \
    -v $PWD/conan:/home/user/.conan \
    -e CONFIG=${config} \
    -e BUILD_SHARED=${build_shared} \
    -e BUILD_SERVER=${build_server} \
    -e BUILD_TOOLS=${build_tools} \
    -e OMP_BUILD_VERSION=$(git rev-list $(git rev-list --max-parents=0 HEAD) HEAD | wc -l) \
    -e OMP_BUILD_COMMIT=$(git rev-parse HEAD) \
    open.mp/build:ubuntu-${ubuntu_version}
