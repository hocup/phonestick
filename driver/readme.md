#The driver element

This simple kernel module is fairly dumb. It registeres a proc file, `/proc/phonestick` 
and responds to characters written to it. 
TODO: clarify what characters `echo -n -e 'y4444\00' > /proc/phonestick`

## Building
If you have all of the appropriate kernel headers installed, you should be able to just
run 
`make`
Otherwise, install the appropriate kernel headers and then run `make`

## Running
Once the build succeeds, you can install the kernel module using the insmod command
`sudo insmod joystickTweaker.ko`
TODO: change the .ko filename once I actually change it elsewhere
Remove the module with 
`sudo rmmod joystickTweaker`
TODO: Change the module name once it's changed elsewhere



## Clean
To remove build artifacts, run 
`make clean`


