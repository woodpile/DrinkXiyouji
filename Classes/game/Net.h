//
//  Net.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/27.
//
//

#ifndef __DrinkXiyouji__Net__
#define __DrinkXiyouji__Net__

#include "cocos2d.h"
#include "HttpClient.h"

class UtilNet
{
public:
    typedef std::function<bool(int errcode, const char*)> recvCallback;
    static bool regRecvList(void);

    static bool sendUserLogin(void);

    static bool sendGetRoomList(void);
    static bool sendCreateRoom(void);
    static bool sendJoinRoom(int room_id);
    static bool sendLeaveRoom(void);
    static bool sendGetRoomInfo(int room_id);
    static bool sendStartGame(void);
    static bool sendSetCardMulti(int multi);

    static bool sendGetRoomGameData(int room_id);
    static bool sendGetRoomGameStatus(int room_id);
    static bool sendPassTurn(int room_id);
    static bool sendPushCards(int room_id, std::vector<int> cards, std::vector<int> jockers);

    static bool sendUserSetName(std::string name);

private:
    static bool dispatchErrCode(int errcode);

    static bool recvUserLogin(int errcode, const char* pRes);

    static bool recvGetRoomList(int errcode, const char* pRes);
    static bool recvCreateRoom(int errcode, const char* pRes);
    static bool recvJoinRoom(int errcode, const char* pRes);
    static bool recvLeaveRoom(int errcode, const char* pRes);
    static bool recvGetRoomInfo(int errcode, const char* pRes);
    static bool recvStartGame(int errcode, const char* pRes);
    static bool recvSetCardMulti(int errcode, const char* pRes);

    static bool recvGetRoomGameData(int errcode, const char* pRes);
    static bool recvGetRoomGameStatus(int errcode, const char* pRes);
    static bool recvPassTurn(int errcode, const char* pRes);
    static bool recvPushCards(int errcode, const char* pRes);

    static bool recvUserSetName(int errcode, const char* pRes);

    static bool cryptoData(int len, const unsigned char* pIn, unsigned char* pOut);
    static bool sendData(const char* pMethod, const char* pData, const char* pTag);
    static bool sendMsg(const char* pMsg, int len, const char* pTag);

    static void recvMsg(cocos2d::network::HttpClient* c, cocos2d::network::HttpResponse* r);
    static void recvData(cocos2d::network::HttpResponse* r);

    static void connectErr(cocos2d::network::HttpResponse* r);
};

class UtilNetRecv : public cocos2d::Ref
{
public:
    static UtilNetRecv* create(const UtilNet::recvCallback& h);
    UtilNet::recvCallback _h;
};

#endif /* defined(__DrinkXiyouji__Net__) */
