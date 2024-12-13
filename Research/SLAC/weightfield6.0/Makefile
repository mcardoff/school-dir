#################################
#			Makefile			#
#################################

weightfield: WFGUI.h WFGUI.cxx Potentials.h Potentials.cxx Field.h Field.cxx Carriers.h Carriers.cxx
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -O3 -Wall -Wextra -o weightfield Weightfield.cxx WFGUI.cxx Potentials.cxx Field.cxx Carriers.cxx `root-config --cflags --glibs` Dict.cxx  
all: WFGUI.h WFGUI.cxx Potentials.h Potentials.cxx Field.h Field.cxx Carriers.h Carriers.cxx
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -O3 -Wall -Wextra -o weightfield Weightfield.cxx WFGUI.cxx Potentials.cxx Field.cxx Carriers.cxx `root-config --cflags --glibs` Dict.cxx  
debug: WFGUI.h WFGUI.cxx Potentials.h Potentials.cxx Field.h Field.cxx Carriers.h Carriers.cxx
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -g -O0 -Wall -Wextra -o weightfield Weightfield.cxx WFGUI.cxx Potentials.cxx Field.cxx Carriers.cxx `root-config --cflags --glibs` Dict.cxx 
