#!/bin/bash
ls "$1" | xargs -- stat -c "%s %n"
