/****************************************************************************
*
*  chat.h
*
*  By Michael Morhaime
*
***/


// If connection is so bad, disable ability to create/join games
extern BOOL gbConnectionSucks;	


// Defined in Chat.cpp, and chatchnnl.cpp
void ChatAddUser (SNADDUSERPTR pAddUserRec);
void ChatDeleteUser(SNDELETEUSERPTR pDeleteUserRec);
void ChatReceiveMsg(SNDISPLAYSTRINGPTR pDispStringRec);
void ChatAddChannel(LPCSTR szChannel);
void ChatDeleteChannel(LPCSTR szChanell);
void ChatJoinChannel(SNJOINCHANNELPTR pJoinChannelRec);
void ChatUpdatePingTime(SNUPDATEPINGTIMEPTR pUpdatePingTimeRec);
void ChatChannelFull(LPCSTR szChannel);
void ChatChannelDoesNotExist(LPCSTR szChannel);
void ChatSetUserName(LPCSTR szUserName);
void ChatChangeUserFlags(SNCHANGEUSERFLAGSPTR pChangeUserFlagsRec);
void ChatChannelRestricted(LPCSTR szChannel);
void ChatSquelchUser(SNSQUELCHUSERPTR pSquelchUserRec);
void ChatUnsquelchUser(SNSQUELCHUSERPTR pSquelchUserRec);


// Defined in Ad.cpp
void AdSetInfo(SNADINFOPTR pAdInfo);
void AdDisplay(LPVOID pVoid, DWORD dwSize);
