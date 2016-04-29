#!/bin/bash
ls "$1" | xargs -- stat -c "%z %n" | tac
