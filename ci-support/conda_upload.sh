#!/usr/bin/env bash
PKG_NAME=libdrs
USER=cdat
export VERSION="3.1.2"
echo "Trying to upload to conda"
echo ""
echo "Activating base env"
source activate base
echo "Making sure conda-build is installed"
conda install conda-build
echo "Updating conda"
conda update -y -q conda
if [ `uname` == "Linux" ]; then
    OS=linux-64
    echo "Linux OS"
else
    echo "Mac OS"
    OS=osx-64
fi

mkdir ~/conda-bld
conda config --set anaconda_upload no
export CONDA_BLD_PATH=${HOME}/conda-bld
echo "Cloning recipes"
git clone git://github.com/CDAT/conda-recipes
cd conda-recipes
# uvcdat creates issues for build -c uvcdat confises package and channel
export BRANCH=${CIRCLE_BRANCH}
if [[ -d uvcdat ]]; then
    rm -rf uvcdat
fi
if [[ -d libdrs_c ]]; then
    rm -rf libdrs_c
fi
if [[ -d libdrs_f ]]; then
    rm -rf libdrs_f
fi
ln -s ../recipe/libdrs_c libdrs_c
ln -s ../recipe/libdrs_f libdrs_f
python ./prep_for_build.py  -b ${BRANCH}
conda build ${PKG_NAME}_c -c conda-forge 
conda build ${PKG_NAME}_f -c conda-forge 
anaconda -t $CONDA_UPLOAD_TOKEN upload -u $USER -l $LABEL $CONDA_BLD_PATH/$OS/${PKG_NAME}-$VERSION.`date +%Y*`0.tar.bz2 --force
anaconda -t $CONDA_UPLOAD_TOKEN upload -u $USER -l $LABEL $CONDA_BLD_PATH/$OS/${PKG_NAME}_f-$VERSION.`date +%Y*`0.tar.bz2 --force
