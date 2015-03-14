//
//  Net.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/27.
//
//

#include "Net.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "UtilRc4.h"

#include "../LoadScene.h"
#include "../RoomScene.h"
#include "../GameScene.h"
#include "../data/MyData.h"
#include "../data/Struct.h"

USING_NS_CC;
using namespace cocos2d::network;

//std::string g_strHost = "http://192.168.29.104:9600/xiyouji";
std::string g_strHost = "http://114.215.193.215:9600/xiyouji";

bool UtilNet::dispatchErrCode(int errcode)
{
    return true;
}

//---------Recv Registe
UtilNetRecv* UtilNetRecv::create(const UtilNet::recvCallback& h)
{
    auto p = new UtilNetRecv();
    p->_h = h;
    return p;
}

Map<std::string, UtilNetRecv*> pMapRecv;

bool UtilNet::regRecvList(void)
{
    pMapRecv.insert("userlogin", UtilNetRecv::create(UtilNet::recvUserLogin));

    pMapRecv.insert("createroom", UtilNetRecv::create(UtilNet::recvCreateRoom));
    pMapRecv.insert("getroomlist", UtilNetRecv::create(UtilNet::recvGetRoomList));
    pMapRecv.insert("joinroom", UtilNetRecv::create(UtilNet::recvJoinRoom));
    pMapRecv.insert("leaveroom", UtilNetRecv::create(UtilNet::recvLeaveRoom));
    pMapRecv.insert("getroominfo", UtilNetRecv::create(UtilNet::recvGetRoomInfo));

    pMapRecv.insert("getroomgamedata", UtilNetRecv::create(UtilNet::recvGetRoomGameData));
    pMapRecv.insert("getroomgamestatus", UtilNetRecv::create(UtilNet::recvGetRoomGameStatus));
    pMapRecv.insert("passturn", UtilNetRecv::create(UtilNet::recvPassTurn));
    pMapRecv.insert("pushcards", UtilNetRecv::create(UtilNet::recvPushCards));
    pMapRecv.insert("startgame", UtilNetRecv::create(UtilNet::recvStartGame));

    pMapRecv.insert("usersetname", UtilNetRecv::create(UtilNet::recvUserSetName));

    return true;
}

bool UtilNet::sendUserLogin(void)
{
    return UtilNet::sendData(".UserLogin", "", "userlogin");
}

bool UtilNet::recvUserLogin(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Uid"))
    {
        return false;
    }
    if (false == da["Uid"].IsInt())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto uid = da["Uid"].GetInt();
    log("recv uid %d", uid);

    auto p = LoadScene::getInstance();
    if (nullptr != p)
    {
        p->netLoginCallback(uid);
    }

    return true;
}

bool decodeNetRoomInfo(rapidjson::Document::ValueIterator it, NetRoomInfo* outAttr)
{
    if (false == it->IsObject())
    {
        log("recv refresh sub type wrong");
        return false;
    }
    if (false == it->HasMember("Room_id"))
    {
        log("recv refresh sub type member wrong");
        return false;
    }
    outAttr->Room_id = (*it)["Room_id"].GetInt();

    return true;
}
bool UtilNet::sendGetRoomList(void)
{
    return UtilNet::sendData("Room.GetRoomsList", "", "getroomlist");
}
bool UtilNet::recvGetRoomList(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Rooms"))
    {
        return false;
    }
    if (false == da["Rooms"].IsArray())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    int size = da["Rooms"].Size();

    NetRoomInfo roominfo[size];
    if (0 != size)
    {
        int index = 0;
        for (auto it = da["Rooms"].onBegin(); it != da["Rooms"].onEnd(); it++)
        {
            if (false == decodeNetRoomInfo(it, &(roominfo[index])))
            {
                continue;
                size -= 1;
            }
            index++;
        }
    }

    auto p = RoomScene::getInstance();
    if (nullptr != p)
    {
        p->netGetRoomListCallback(roominfo, size);
    }

    return true;
}

bool UtilNet::sendCreateRoom(void)
{
    return UtilNet::sendData("Room.CreateRoom", "", "createroom");
}

bool UtilNet::recvCreateRoom(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Success"))
    {
        return false;
    }
    if (false == da["Success"].IsBool())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto success = da["Success"].GetBool();
    log("recv success %d", success);

    return true;
}

bool UtilNet::sendJoinRoom(int room_id)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("Room_id", room_id, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Room.JoinRoom", sb.GetString(), "joinroom");
}

bool UtilNet::recvJoinRoom(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Success"))
    {
        return false;
    }
    if (false == da["Success"].IsBool())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto success = da["Success"].GetBool();
    log("recv success %d", success);

    auto p = RoomScene::getInstance();
    if (nullptr != p)
    {
        p->netJoinRoomCallback(success);
    }

    return true;
}

bool decodeNetPlayerInfo(rapidjson::Document::ValueIterator it, NetPlayerInfo* outAttr)
{
    if (false == it->IsObject())
    {
        log("recv refresh sub type wrong");
        return false;
    }
    if (false == it->HasMember("Uid") ||
        false == it->HasMember("Uname"))
    {
        log("recv refresh sub type member wrong");
        return false;
    }
    outAttr->Uid = (*it)["Uid"].GetInt();
    outAttr->Uname = (*it)["Uname"].GetString();

    return true;
}
bool UtilNet::sendGetRoomInfo(int room_id)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("Room_id", room_id, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Room.GetRoomInfo", sb.GetString(), "getroominfo");
}
bool UtilNet::recvGetRoomInfo(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Players") ||
        false == da.HasMember("MaxPlayer") ||
        false == da.HasMember("Status") ||
        false == da.HasMember("CardMulti") ||
        false == da.HasMember("Creator"))
    {
        return false;
    }
    if (false == da["Players"].IsArray() ||
        false == da["MaxPlayer"].IsInt() ||
        false == da["Status"].IsInt() ||
        false == da["CardMulti"].IsInt() ||
        false == da["Creator"].IsInt())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    int size = da["Players"].Size();

    NetRoomInner RoomInner;
    if (0 != size)
    {
        int index = 0;
        for (auto it = da["Players"].onBegin(); it != da["Players"].onEnd(); it++)
        {
            auto tp = new NetPlayerInfo;
            tp->autorelease();

            if (false == decodeNetPlayerInfo(it, tp))
            {
                continue;
                size -= 1;
            }
            index++;

            RoomInner.Players.pushBack(tp);
        }
    }

    RoomInner.MaxPlayer = da["MaxPlayer"].GetInt();
    RoomInner.Status = da["Status"].GetInt();
    RoomInner.CardMulti = da["CardMulti"].GetInt();
    RoomInner.Creator = da["Creator"].GetInt();

    auto p = RoomScene::getInstance();
    if (nullptr != p)
    {
        p->netGetRoomInfoCallback(RoomInner);
    }

    return true;
}

bool UtilNet::sendStartGame(void)
{
    return UtilNet::sendData("Room.StartGame", "", "startgame");
}

bool UtilNet::recvStartGame(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Success"))
    {
        return false;
    }
    if (false == da["Success"].IsBool())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto success = da["Success"].GetBool();
    log("recv success %d", success);

    return true;
}

bool decodeNetGameCardStatus(rapidjson::Document::ValueIterator it, NetGameCardStatus* outAttr)
{
    if (false == it->IsObject())
    {
        log("recv refresh sub type wrong");
        return false;
    }
    if (false == it->HasMember("Cid") ||
        false == it->HasMember("Type") ||
        false == it->HasMember("Order"))
    {
        log("recv refresh sub type member wrong");
        return false;
    }
    outAttr->Cid = (*it)["Cid"].GetInt();
    outAttr->Type = (*it)["Type"].GetInt();
    outAttr->Order = (*it)["Order"].GetInt();

    return true;
}
bool decodeNetGamePlayerStatus(rapidjson::Document::ValueIterator it, NetGamePlayerStatus* outAttr)
{
    if (false == it->IsObject())
    {
        log("recv refresh sub type wrong");
        return false;
    }
    if (false == it->HasMember("Uid") ||
        false == it->HasMember("Uname") ||
        false == it->HasMember("NumCard") ||
        false == it->HasMember("Cards") ||
        false == it->HasMember("TotalPunish"))
    {
        log("recv refresh sub type member wrong");
        return false;
    }
    outAttr->Uid = (*it)["Uid"].GetInt();
    outAttr->Uname = (*it)["Uname"].GetString();
    outAttr->NumCard = (*it)["NumCard"].GetInt();
    outAttr->TotalPunish = (*it)["TotalPunish"].GetInt();
    for (auto sit = (*it)["Cards"].onBegin(); sit != (*it)["Cards"].onEnd(); sit++)
    {
        outAttr->Cards.push_back(sit->GetInt());
    }

    return true;
}

bool UtilNet::sendGetRoomGameData(int room_id)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("Room_id", room_id, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Room.GetRoomGameData", sb.GetString(), "getroomgamedata");
}
bool UtilNet::recvGetRoomGameData(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Players") ||
        false == da.HasMember("Cards"))
    {
        return false;
    }
    if (false == da["Players"].IsArray() ||
        false == da["Cards"].IsArray())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    NetGameData gameData;
    for (auto it = da["Players"].onBegin(); it != da["Players"].onEnd(); it++)
    {
        auto tp = new NetGamePlayerStatus;
        tp->autorelease();

        if (false == decodeNetGamePlayerStatus(it, tp))
        {
            continue;
        }

        gameData.Players.pushBack(tp);
    }
    for (auto sit = da["Cards"].onBegin(); sit != da["Cards"].onEnd(); sit++)
    {
        auto tp = new NetGameCardStatus;
        tp->autorelease();

        if (false == decodeNetGameCardStatus(sit, tp))
        {
            continue;
        }

        gameData.Cards.pushBack(tp);
    }

    auto p = GameScene::getInstance();
    if (nullptr != p)
    {
        p->netGetGameDataCallback(gameData);
    }
    
    return true;
}

bool UtilNet::sendGetRoomGameStatus(int room_id)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("Room_id", room_id, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Room.GetRoomGameStatus", sb.GetString(), "getroomgamestatus");
}
bool UtilNet::recvGetRoomGameStatus(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Players") ||
        false == da.HasMember("CurrentTurn") ||
        false == da.HasMember("LastStep") ||
        false == da.HasMember("JockerTo") ||
        false == da.HasMember("LastStepOwn") ||
        false == da.HasMember("Round"))
    {
        return false;
    }
    if (false == da["Players"].IsArray() ||
        false == da["CurrentTurn"].IsInt() ||
        false == da["LastStep"].IsArray() ||
        false == da["JockerTo"].IsArray() ||
        false == da["LastStepOwn"].IsInt() ||
        false == da["Round"].IsInt())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    NetGameStatus gameStatus;
    for (auto it = da["Players"].onBegin(); it != da["Players"].onEnd(); it++)
    {
        auto tp = new NetGamePlayerStatus;
        tp->autorelease();

        if (false == decodeNetGamePlayerStatus(it, tp))
        {
            continue;
        }

        gameStatus.Players.pushBack(tp);
    }
    gameStatus.CurrentTurn = da["CurrentTurn"].GetInt();
    for (auto sit = da["LastStep"].onBegin(); sit != da["LastStep"].onEnd(); sit++)
    {
        gameStatus.LastStep.push_back(sit->GetInt());
    }
    for (auto sit = da["JockerTo"].onBegin(); sit != da["JockerTo"].onEnd(); sit++)
    {
        gameStatus.JockerTo.push_back(sit->GetInt());
    }
    gameStatus.LastStepOwn = da["LastStepOwn"].GetInt();
    gameStatus.Round = da["Round"].GetInt();

    auto p = GameScene::getInstance();
    if (nullptr != p)
    {
        p->netGetGameStatusCallback(gameStatus);
    }

    return true;
}

bool UtilNet::sendPassTurn(int room_id)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("Room_id", room_id, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Room.PassTurn", sb.GetString(), "passturn");
}

bool UtilNet::recvPassTurn(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Success"))
    {
        return false;
    }
    if (false == da["Success"].IsBool())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto success = da["Success"].GetBool();
    log("recv success %d", success);

    return true;
}

bool UtilNet::sendPushCards(int room_id, std::vector<int> cards, std::vector<int> jockers)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("Room_id", room_id, al);

    rapidjson::Value array(rapidjson::kArrayType);
    for (auto& c : cards)
    {
        array.PushBack(c, al);
    }
    d.AddMember("Cids", array, al);

    rapidjson::Value array_j(rapidjson::kArrayType);
    for (auto& c : jockers)
    {
        array_j.PushBack(c, al);
    }
    d.AddMember("Jockers", array_j, al);

    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Room.PushCards", sb.GetString(), "pushcards");
}

bool UtilNet::recvPushCards(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Success"))
    {
        return false;
    }
    if (false == da["Success"].IsBool())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto success = da["Success"].GetBool();
    log("recv success %d", success);

    auto p = GameScene::getInstance();
    if (nullptr != p)
    {
        p->netPushCardsCallback(success);
    }

    return true;
}

bool UtilNet::sendUserSetName(std::string name)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("Name", name.c_str(), al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("User.SetName", sb.GetString(), "usersetname");
}

bool UtilNet::recvUserSetName(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Success"))
    {
        return false;
    }
    if (false == da["Success"].IsBool())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto success = da["Success"].GetBool();
    log("recv success %d", success);

    return true;
}

bool UtilNet::sendSetCardMulti(int multi)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("Multi", multi, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Room.SetCardMulti", sb.GetString(), "setcardmulti");
}

bool UtilNet::recvSetCardMulti(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Success") ||
        false == da.HasMember("Multi"))
    {
        return false;
    }
    if (false == da["Success"].IsBool() ||
        false == da["Multi"].IsInt())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto success = da["Success"].GetBool();
    log("recv success %d", success);
    auto multi = da["Multi"].GetInt();

    auto p = RoomScene::getInstance();
    if (nullptr != p)
    {
        p->netSetCardMultiCallback(success, multi);
    }

    return true;
}

bool UtilNet::sendLeaveRoom(void)
{
    return UtilNet::sendData("Room.LeaveRoom", "", "leaveroom");
}

bool UtilNet::recvLeaveRoom(int errcode, const char* pRes)
{
    if (0 != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Success"))
    {
        return false;
    }
    if (false == da["Success"].IsBool())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto success = da["Success"].GetBool();
    log("recv success %d", success);

    return true;
}

//---------Net Framework

bool UtilNet::sendData(const char* pMethod, const char* pData, const char* pTag)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();

    d.AddMember("uid", MyData::getInstance()->getMyUid(), al);
    d.AddMember("time", "20140827100000", al);
    d.AddMember("method", pMethod, al);
    d.AddMember("data", pData, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendMsg(sb.GetString(), (int)strlen(sb.GetString()), pTag);
}

bool UtilNet::sendMsg(const char* pMsg, int len, const char* pTag)
{
    char pCryMsg[len + 1];
    memset(pCryMsg, 0, len + 1);
    UtilNet::cryptoData(len, (unsigned char*)pMsg, (unsigned char*)pCryMsg);

    HttpRequest* r = new HttpRequest();
    r->setUrl(g_strHost.c_str());
    r->setRequestType(HttpRequest::Type::POST);

    r->setRequestData(pCryMsg, len);
    r->setTag(pTag);

    r->setResponseCallback(UtilNet::recvMsg);

    HttpClient::getInstance()->send(r);

    return true;
}

bool UtilNet::cryptoData(int len, const unsigned char* pIn, unsigned char* pOut)
{
    unsigned char k[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    int kl = 8;
    mineRC4_KEY key;
    mineRC4_set_key(&key, kl, k);
    mineRC4(&key, len, pIn, (unsigned char*)pOut);

    return true;
}

void UtilNet::recvMsg(HttpClient* c, HttpResponse* r)
{
    if (false == r->isSucceed())
    {
        log("error buffer: %s", r->getErrorBuffer());
        UtilNet::connectErr(r);
    }
    else{
        log("response tag %s", r->getHttpRequest()->getTag());

        UtilNet::recvData(r);
    }

    r->getHttpRequest()->release();

    return;
}

void UtilNet::recvData(HttpResponse* r)
{
    auto rData = r->getResponseData();
    int len = (int)rData->size();
    if (0 == len)
    {
        return;
    }

    char acBuf[len + 1];
    memset(acBuf, 0, len + 1);

    UtilNet::cryptoData(len, (unsigned char*)rData->data(), (unsigned char*)acBuf);

    //    log("recv(%d) %s", len, acBuf);

    rapidjson::Document d;
    d.Parse<rapidjson::kParseDefaultFlags>(acBuf);
    for (auto it = d.MemberonBegin(); it != d.MemberonEnd(); it++)
    {
        if (true == it->value.IsString())
        {
            log("key %s, value %s", it->name.GetString(), it->value.GetString());
        }
        else if (true == it->value.IsInt())
        {
            log("key %s, value %d", it->name.GetString(), it->value.GetInt());
        }
    }

    if (false == d.HasMember("Err") ||
        false == d.HasMember("Time") ||
        false == d.HasMember("Data") ||
        false == d.HasMember("ErrCode"))
    {
        log("recv invalid msg : %s", d.GetString());
        return;
    }

    auto err = __String::create(d["Err"].GetString());
    if (0 != err->compare("ok"))
    {
        log("return error(code %d) : %s", d["ErrCode"].GetInt(), err->getCString());
    }

    auto c = pMapRecv.at(r->getHttpRequest()->getTag());
    if (nullptr != c)
    {
        c->_h(d["ErrCode"].GetInt(), d["Data"].GetString());
    }

    return;
}

void UtilNet::connectErr(HttpResponse* r)
{
}
