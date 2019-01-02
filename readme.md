# LASER - [**L**]c3 [**AS**]sembl[**ER**]

## Description

An assembler for the LC3 written entirely in C. I've been trying to move away from LC3Edit provided by my university, which is old and hurts my eyes, and an assembler was the last thing I needed LC3Edit for.

## Motivation

The completion of this tool will allow me to do everything LC3-related from VSCode, my editor of choice. Or it would, except that I've already finished the course and am just using this project to practice my C coding. Hopefully someone down the line will appreciate my efforts in creating a command line LC3 assembler.

## Building from Source

Please read the notice below explaining what has and has not yet been completed. If you would like to try out the parts that have been completed, here are the steps to building on Mac, Linux, or MINGW:

1. Clone this repository: `git clone https://github.com/PaperFanz/laser.git`
2. `cd laser/src`
3. `make`
4. `sudo make install` (if you want to use it without selecting the directory and using ./laser)

## Notice

NOTE: This project is not yet complete. Here are the things that still need to be implemented:

1. ~~Generating the symbol table~~ Completed 12/21/2018
2. Generating the binary file	*in progress*
3. Generating the hex file		*in progress*
4. Generating the list file
5. Generating the object file

I've got a lot of work to do. Good thing winter break is forever.