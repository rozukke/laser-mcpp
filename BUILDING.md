# Building Laser from Source

# Linux

Dependencies: `gcc, make`

```shell
git clone https://github.com/rozukke/laser-mcpp.git
git checkout testing
cd laser/src
make
sudo make install
# alternatively, move the compiled binary to your preferred location
```

# Mac OSX

Dependencies: `xcode command line tools`

```shell
xcode-select --install
git clone https://github.com/rozukke/laser-mcpp.git
git checkout testing
cd laser/src
make
sudo make install
# alternatively, move the compiled binary to your preferred location
```