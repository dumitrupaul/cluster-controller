#!/bin/bash
out=$(sudo batctl o | wc -l)
ifBat=$(ip a show bat0 up | wc -l)
if [ "$ifBat" -gt 1 ] && [ "$out" -gt 2 ]; then
	echo "B.A.T.M.A.N mesh network is already set up!"
else
	echo "Setting up B.A.T.M.A.N mesh"
	#Variabile de configurare
	mesh_dev="wlxf8d11101dc04"
	ip="172.27.0.3/16"
	password="licenta__paul"

	#Setam domeniul de reglementare in functie de tara
	sudo iw reg set RO
	sudo modprobe batman-adv
	sudo ifconfig $mesh_dev down

	#Eliminam procesul de wpa pentru functionarea IBSS
	sudo killall wpa_supplicant

	sleep 1s
	sudo iw $mesh_dev set type ibss
	sleep 1s
	sudo ifconfig $mesh_dev up
	sudo iw dev $mesh_dev ibss join jamez-mesh 2412 key d:0:$password
	iw dev
	sudo batctl if add $mesh_dev
	sleep 1s
	sudo ifconfig bat0 up
	sleep 1s
	sudo ifconfig bat0 $ip
fi
