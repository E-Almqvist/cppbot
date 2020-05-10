#!/bin/bash

BOTPATH=$1 # first parameter is the bot path
BOTNAME=$2 # second parameter is the bots name

if [ -z "$BOTPATH" ]; then
	echo -e "\e[31mYou need to specify the install location!\e[0m"
	echo "Example:"
	echo "./install.sh ~/somewhere/bots goblinBot"
	echo "                   ^"
	echo "                  Here"
	exit
fi

if [ -z "$BOTNAME" ]; then
	BOTNAME="cppbot"
fi

install() {

	# File structure
	echo "Installing...";
	mkdir $BOTPATH/$BOTNAME
	cd $BOTPATH/$BOTNAME
	mkdir Source
	mkdir Builds

	# Source-code stuff
	cd Source
	git clone https://github.com/E-Almqvist/cppbot.git
	mv cppbot/* ./
	mv config.json ../Builds
	
	# CMakeLists.txt generation
	echo "Generating CMakeLists.txt"
	rm CMakeLists.txt
	echo "cmake_minimum_required (VERSION 3.6)
project($BOTNAME)
add_executable($BOTNAME bot.cpp)
add_subdirectory(lib/sleepy-discord)
target_link_libraries($BOTNAME sleepy-discord)" > CMakeLists.txt

	# start.sh generation
	echo "Generating start.sh"
	echo "#!/bin/bash
echo Starting bot...
nohup Builds/$BOTNAME &" > ../start.sh

	# Dependencies
	echo "Installing dependencies..."
	cd lib

	# Sleepy-Discord
	git clone https://github.com/yourWaifu/sleepy-discord.git
	cd sleepy-discord
	git checkout develop
	git pull
	cd ..

	# JSON Parser
	wget https://github.com/nlohmann/json/releases/download/v3.7.3/json.hpp  
	# dumb workaround but it works
	cd ..

	# Cleaning
	echo "Cleaning unwanted stuff..."
	sudo rm -r cppbot # empty folder, not needed
	sudo rm -r demo # dont need the demo stuff
	rm install.sh

	# Compiling the bot
	read -p "Do you want to compile the bot? This might take a while. (y/n): " ans2
	
	if [ "$ans2" == "y" ] || [ $ans2 == "Y" ]; then
		echo "Compiling... This might take a while."
		cd ../Builds
		cmake ../Source
		make
		echo -e "\e[32mInstallation & compilation complete!\e[0m"
		exit
	else
		echo -e "\e[32mInstallation complete!\e[0m 
Keep in mind that you have to compile the bot yourself now."
		exit
	fi
}

read -p "Do you want to install cppbot at '$1'? (y/n): " ans

if [ "$ans" == "y" ] || [ $ans == "Y" ]; then
	install
else
	echo "Aborting..."
	exit
fi
