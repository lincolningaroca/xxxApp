@echo off
sqlite3.exe %1 ".backup  %2"