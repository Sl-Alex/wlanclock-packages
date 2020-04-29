#!/bin/sh /etc/rc.common
START=99
STOP=1

start() {
    killall wlanclock-app
    killall wlanclock-input
    killall wlanclock-sensors

    echo "Starting..."
    wlanclock-app &
    wlanclock-input &
    wlanclock-sensors &
    echo "Started"
}                 
 
stop() {
    killall wlanclock-app
    killall wlanclock-input
    killall wlanclock-sensors
}
