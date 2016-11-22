# Installation
(On linux - if not already installed)

sudo apt-get install binutils-msp430 gcc-msp430 gdb-msp430 msp430-libc msp430mcu mspdebug 
sudo apt-get -o Dpkg::Options::="--force-overwrite" install gdb-msp430 


# USAGE  

cd ../exercise_1
                                                                                                           
make         :buids assembly (.s), and executable (.elf)
                            
make flash   :installs the executable on the msp430
                           
make clean   :deletes files created by make
                           
make reset   :resets the msp430 

