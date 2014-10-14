#!/bin/bash

# Kill the sender a
sudo kill -9 `ps aux | grep 'bin/a' | grep -v grep | awk '{print $2}'`
sudo kill -9 `ps aux | grep 'bin/b' | grep -v grep | awk '{print $2}'`
