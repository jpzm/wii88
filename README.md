# About wii88

This project is a fork from [QUASI88](https://www.eonet.ne.jp/~showtime/quasi88/) version 0.6.4, a NEC PC-8801 emulator by Showzoh Fukunaga.
Specifically, the source [quasi88-0.6.4.tgz](https://www.eonet.ne.jp/~showtime/quasi88/release/quasi88-0.6.4.tgz) (md5sum '21d268aaa290471f60142fea49485010').
The main purpose of this fork is to be used with Nintendo Wii.
We also used for reference the [QUASI88 Wii port](http://wiibrew.org/wiki/QUASI88) version 0.6.3 beta6 by [Rako](http://wiibrew.org/wiki/User:Rako).

## Download

For public releases, please check the [releases](https://github.com/jpzm/wii88/releases) page. For the latest development version, you may clone the `master` branch with the below command.
```
$ git clone https://github.com/jpzm/wii88.git
```
The content of the repository folder `hbc` has the files of the latest compiled version.

## Setup

In order to setup the port, you have to:
  + copy the content of the download release (or the `hbc` repository folder) to your SD/SDHC card.
  + copy system roms files to sd:/wii88/rom directory.
  + copy game roms files to sd:/wii88/disk directory.

The emulation performace is better with original NEC PC8801 roms, but you can also use the pseudo BIOS available on [this](http://www.retropc.net/cisc/m88/download.html) site.

## Build

To build the Wii port, please use the provided `Makefile.wii`. In the terminal, you may type `make -f Makefile.wii`. If everything proceed sucessfully, there will be a new `wii.dol` file. This file is also copied to `hbc/apps/wii88/boot.dol`.

## Credits

In this Wii port I tried to modify the original code from QUASI88 as less as possible. The additional code and build files are almost always apart the original files. Therefore, the emulation process itself is cretited to Showzoh Fukunaga (the original QUASI88 author). Also, the included `icon.png` file was created by [NeoRame](http://neorame.de/pre/).
