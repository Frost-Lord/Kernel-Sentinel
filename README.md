# Kernel Sentinel
 Kernel level Security program

## Build
```
make
```

## Testing
```
gcc -o run_ksentinel run_ksentinel.c
sudo ./run_ksentinel
sudo dmesg | grep "KSentinel"

make && sudo ./run_ksentinel && sudo dmesg | grep "KSentinel"
```

# Start/Stop as Driver
```
sudo insmod ksentinel.ko
sudo rmmod ksentinel
``

## Logs:
```
dmesg
```