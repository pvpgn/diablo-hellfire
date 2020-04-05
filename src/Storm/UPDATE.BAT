@echo off
copy o:\h\*.h p:\dev\h\ /v
del o:\source\*.exp
c /ads o:\source\battle\battle.cs
c /aos o:\source\battle\battle.cs
c /ads o:\source\standard\standard.cs
c /aos o:\source\standard\standard.cs
c /aos o:\source\stormst.cs
c /ads o:\source\storm.cs
c /aos o:\source\storm.cs
ss checkin -i- -k -r $/Storm
copy o:\lib\*.lib p:\dev\lib\ /v
copy o:\lib\*.res p:\dev\lib\ /v
