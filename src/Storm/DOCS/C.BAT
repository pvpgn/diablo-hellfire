@echo off
if exist storm.hlp del storm.hlp
hc31 storm.hpj
if exist storm.ph del storm.ph
if exist *.bak del *.bak
if exist storm.hlp copy storm.hlp o:\help\storm.hlp /v
if exist storm.hlp start winhlp32 storm.hlp
