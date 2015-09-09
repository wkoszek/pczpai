#!/bin/sh

if [ "$1x" != "x" ]; then
	t=$1
	shift;
	echo "# testujê zadanie ${t}"
	./pai -a ../regression/${t} $@
	exit;
else
	./pai -a ../regression/m_t1
	sleep 4;
	./pai -a ../regression/m_t2
	sleep 4;
	./pai -a ../regression/zad1
	sleep 4;
	./pai -a ../regression/zad16
	sleep 4;
	exit;
fi
