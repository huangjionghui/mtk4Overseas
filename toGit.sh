#!/bin/sh

#copy driver file
	cp -r /cygdrive/c/WINCE600/PLATFORM/MT3360/SRC/drivers/flydrv ./driver-code/
	cp -r /cygdrive/c/WINCE600/PLATFORM/MT3360/SRC/inc/flyinc ./driver-code/
	cp -r /cygdrive/c/WINCE600/PLATFORM/MT3360/SRC/common/FlyLib ./driver-code/

#copy exe file
	cp -r /cygdrive/f/git-work/mtk-project/dll ./exe-code
	cp -r /cygdrive/f/git-work/mtk-project/doc ./exe-code
	cp -r /cygdrive/f/git-work/mtk-project/exe ./exe-code
	cp -r /cygdrive/f/git-work/mtk-project/inc ./exe-code
	cp -r /cygdrive/f/git-work/mtk-project/lib ./exe-code
	
#delete ex file
	cd ./driver-code/
	for file in `find -name obj`; do 
		rm -fr "$file"
	done
	for file in `find -name Build.*`; do 
		rm -f "$file"
	done
	rm -f flydrv/dirs
	
	echo "copy file to git success!!!"