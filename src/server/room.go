package server

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"sync"
	"time"
)

type Room struct {
}

type InCreateRoom struct {
}
type OutCreateRoom struct {
	Success bool
}

func (r *Room) CreateRoom(u *UserClient, data string) (string, error) {
	room_id := u.GetSessionData("room_id")
	if nil != room_id {
		return "", fmt.Errorf("user already create room")
	}

	rm := GetRoomMapInstance()
	room := rm.createRoom()
	if nil == room {
		return "", fmt.Errorf("create room failed")
	}

	u.SetSessionData("room_id", room.room_id)
	room.creator = u.uid

	out := &OutCreateRoom{
		Success: true,
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InGetRoomsList struct {
}
type OutGetRoomsList struct {
	Rooms []OutRoomData
}
type OutRoomData struct {
	Room_id int32
}

func (r *Room) GetRoomsList(u *UserClient, data string) (string, error) {
	rm := GetRoomMapInstance()

	out := &OutGetRoomsList{
		Rooms: rm.getRooms(),
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InJoinRoom struct {
	Room_id int32
}
type OutJoinRoom struct {
	Success bool
}

func (r *Room) JoinRoom(u *UserClient, data string) (string, error) {
	//解析参数
	var param InJoinRoom
	err := json.Unmarshal([]byte(data), &param)
	if nil != err {
		fmt.Println("parse param failed,", err)
	}

	rm := GetRoomMapInstance()
	room := rm.getRoom(param.Room_id)
	if nil == room {
		return "", fmt.Errorf("invalid room id")
	}

	var out OutJoinRoom

	if false == room.join(u) {
		out.Success = false
	} else {
		u.SetSessionData("room_id", room.room_id)
		out.Success = true
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InLeaveRoom struct {
}
type OutLeaveRoom struct {
	Success bool
}

func (r *Room) LeaveRoom(u *UserClient, data string) (string, error) {
	room_id := u.GetSessionData("room_id")
	if nil == room_id {
		return "", fmt.Errorf("user not join room")
	}

	rm := GetRoomMapInstance()
	room := rm.getRoom(room_id.(int32))
	if nil == room {
		return "", fmt.Errorf("invalid room id")
	}

	var out OutLeaveRoom

	out.Success = room.leave(u)
	if true == out.Success {
		u.UnsetSessionData("room_id")
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type OutPlayerData struct {
	Uid   int32
	Uname string
}
type InGetRoomInfo struct {
	Room_id int32
}
type OutGetRoomInfo struct {
	Status    int32
	Creator   int32
	MaxPlayer int32
	CardMulti int32
	Players   []OutPlayerData
}

func (r *Room) GetRoomInfo(u *UserClient, data string) (string, error) {
	//解析参数
	var param InGetRoomInfo
	err := json.Unmarshal([]byte(data), &param)
	if nil != err {
		fmt.Println("parse param failed,", err)
	}

	rm := GetRoomMapInstance()
	room := rm.getRoom(param.Room_id)

	var out OutGetRoomInfo

	out.Players = room.getPlayers()
	out.Status = room.status
	out.MaxPlayer = room.num_player_max
	out.CardMulti = room.card_multi
	out.Creator = room.creator

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InSetCardMulti struct {
	Multi int32
}
type OutSetCardMulti struct {
	Multi   int32
	Success bool
}

func (r *Room) SetCardMulti(u *UserClient, data string) (string, error) {
	//解析参数
	var param InSetCardMulti
	err := json.Unmarshal([]byte(data), &param)
	if nil != err {
		fmt.Println("parse param failed,", err)
	}

	room_id := u.GetSessionData("room_id")
	if nil == room_id {
		return "", fmt.Errorf("user not join room")
	}

	room := GetRoomMapInstance().getRoom(room_id.(int32))
	if nil == room {
		return "", fmt.Errorf("invaild room id %v", room_id.(int32))
	}
	if u.uid != room.creator {
		return "", fmt.Errorf("not room creator")
	}

	var out OutSetCardMulti
	out.Success = room.setCardMulti(param.Multi)
	if true == out.Success {
		out.Multi = param.Multi
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InStartGame struct {
}
type OutStartGame struct {
	Success bool
}

func (r *Room) StartGame(u *UserClient, data string) (string, error) {
	sd := u.GetSessionData("room_id")
	if sd == nil {
		fmt.Println("user not join room")
		return "", fmt.Errorf("user not join room")
	}

	room_id := sd.(int32)

	rm := GetRoomMapInstance()
	room := rm.getRoom(room_id)

	if u.uid != room.creator {
		return "", fmt.Errorf("user is not room creator")
	}

	success := room.start()

	var out OutStartGame
	out.Success = success

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type OutCardData struct {
	Cid   int32
	Type  int32
	Order int32
}
type OutPlayerInGame struct {
	Uid         int32
	Uname       string
	NumCard     int32
	Cards       []int32
	TotalPunish int32
}
type InGetRoomGameData struct {
	Room_id int32
}
type OutGetRoomGameData struct {
	Players []OutPlayerInGame
	Cards   []OutCardData
}

func (r *Room) GetRoomGameData(u *UserClient, data string) (string, error) {
	//解析参数
	var param InGetRoomGameData
	err := json.Unmarshal([]byte(data), &param)
	if nil != err {
		fmt.Println("parse param failed,", err)
	}

	rm := GetRoomMapInstance()
	room := rm.getRoom(param.Room_id)

	var out OutGetRoomGameData

	out.Cards = room.getCards()

	players := room.getPlayers()
	for _, p := range players {
		var op OutPlayerInGame
		op.Uid = p.Uid
		op.Uname = p.Uname
		op.NumCard = 0
		op.Cards = make([]int32, 0)

		for _, c := range room.cards {
			if c.own_uid != op.Uid {
				continue
			}
			op.NumCard += 1
			if c.own_uid == u.uid {
				op.Cards = append(op.Cards, c.cid)
			}
		}

		out.Players = append(out.Players, op)
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InGetRoomGameStatus struct {
	Room_id int32
}
type OutGetRoomGameStatus struct {
	Players     []OutPlayerInGame
	LastStep    []int32
	LastStepOwn int32
	JockerTo    []int32
	CurrentTurn int32
	Round       int32
}

func (r *Room) GetRoomGameStatus(u *UserClient, data string) (string, error) {
	//解析参数
	var param InGetRoomGameStatus
	err := json.Unmarshal([]byte(data), &param)
	if nil != err {
		fmt.Println("parse param failed,", err)
	}

	rm := GetRoomMapInstance()
	room := rm.getRoom(param.Room_id)

	var out OutGetRoomGameStatus

	for _, p := range room.players {
		var op OutPlayerInGame
		op.Uid = p.client.uid
		op.Uname = p.uname
		op.TotalPunish = p.total_punish
		op.NumCard = 0
		op.Cards = make([]int32, 0)

		for _, c := range room.cards {
			if c.own_uid != op.Uid {
				continue
			}
			op.NumCard += 1
			if c.own_uid == u.uid {
				op.Cards = append(op.Cards, c.cid)
			}
		}

		out.Players = append(out.Players, op)
	}
	out.CurrentTurn = room.during.current_player
	out.Round = room.round

	last := room.last()
	if 0 == last.uid {
		out.LastStep = make([]int32, 0)
		out.LastStepOwn = 0
		out.JockerTo = make([]int32, 0)
	} else {
		out.LastStep = last.cids
		out.LastStepOwn = last.uid
		out.JockerTo = last.jocker_to
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InPushCards struct {
	Room_id int32
	Cids    []int32
	Jockers []int32
}
type OutPushCards struct {
	Success bool
}

func (r *Room) PushCards(u *UserClient, data string) (string, error) {
	//解析参数
	var param InPushCards
	err := json.Unmarshal([]byte(data), &param)
	if nil != err {
		fmt.Println("parse param failed,", err)
		return "", fmt.Errorf("parse param failed, %v", err)
	}
	if 0 != (len(param.Jockers) % 2) {
		return "", fmt.Errorf("jockers param num is %v", len(param.Jockers))
	}

	rm := GetRoomMapInstance()
	room := rm.getRoom(param.Room_id)

	var out OutPushCards

	for i := 0; i < len(param.Jockers); i += 2 {
		if false == room.setJockerTO(u.uid, param.Jockers[i], param.Jockers[i+1]) {
			return "", fmt.Errorf("jocker set wrong %v", param.Jockers)
		}
	}
	out.Success = room.pushCards(u.uid, param.Cids)

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InPassTurn struct {
	Room_id int32
}
type OutPassTurn struct {
	Success bool
}

func (r *Room) PassTurn(u *UserClient, data string) (string, error) {
	//解析参数
	var param InPassTurn
	err := json.Unmarshal([]byte(data), &param)
	if nil != err {
		fmt.Println("parse param failed,", err)
	}

	rm := GetRoomMapInstance()
	room := rm.getRoom(param.Room_id)

	var out OutPassTurn

	out.Success = room.pass(u.uid)

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

//------------
type PlayData struct {
	client       *UserClient
	total_punish int32
	uname        string
	numCard      int32
}
type CardData struct {
	cid       int32
	ctype     int32
	corder    int32
	own_uid   int32
	jocker_to int32
}
type StepData struct {
	uid       int32
	cids      []int32
	jocker_to []int32
}
type DuringData struct {
	order          []int32
	current_player int32
	steps          []StepData
	pass_num       int32
}

type RoomData struct {
	room_id        int32
	creator        int32
	num_player_max int32
	status         int32
	playersLock    sync.Mutex
	players        map[int32]*PlayData
	cardslock      sync.Mutex
	cards          map[int32]*CardData

	during DuringData
	round  int32

	card_multi int32
}

type RoomMap struct {
	rooms map[int32]*RoomData
	index int32
	lock  sync.Mutex
}

var gRoomMapInstance *RoomMap = nil

func GetRoomMapInstance() *RoomMap {
	if nil == gRoomMapInstance {
		gRoomMapInstance = &RoomMap{
			index: 1,
			rooms: make(map[int32]*RoomData, 0),
		}
	}
	return gRoomMapInstance
}

func (r *RoomMap) createRoom() *RoomData {
	room := &RoomData{
		room_id:        0,
		num_player_max: 4,
		players:        make(map[int32]*PlayData, 0),
		cards:          make(map[int32]*CardData, 0),

		status: ROOM_STATUS_WAIT,

		round: 0,

		card_multi: 1,
	}

	r.lock.Lock()
	defer r.lock.Unlock()

	r.rooms[r.index] = room
	room.room_id = r.index

	r.index++

	return room
}

func (r *RoomMap) getRooms() []OutRoomData {
	out := make([]OutRoomData, 0)

	for _, room := range r.rooms {
		if room.status != ROOM_STATUS_WAIT {
			continue
		}
		out = append(out, OutRoomData{room.room_id})
	}

	return out
}

func (r *RoomMap) getRoom(room_id int32) *RoomData {
	room, ok := r.rooms[room_id]
	if false == ok {
		return nil
	}
	return room
}

func (r *RoomMap) deleteRoom(room_id int32) {
	delete(r.rooms, room_id)
}

func (r *RoomData) join(u *UserClient) bool {
	//检查是否处于等待状态
	if ROOM_STATUS_WAIT != r.status {
		return false
	}
	//检查是否已经加入
	_, exist := r.players[u.uid]
	if true == exist {
		return false
	}

	var uname string
	name := u.GetSessionData("Uname")
	if nil == name {
		uname = fmt.Sprintf("User %d", u.uid)
	} else {
		uname = name.(string)
	}

	player := &PlayData{
		client:       u,
		total_punish: 0,
		uname:        uname,
		numCard:      0,
	}

	r.playersLock.Lock()
	defer r.playersLock.Unlock()

	//检查是否人满
	if int32(len(r.players)) >= r.num_player_max {
		return false
	}

	r.players[u.uid] = player

	return true
}

func (r *RoomData) leave(u *UserClient) bool {
	//检查是否已经加入
	_, exist := r.players[u.uid]
	if false == exist {
		return false
	}

	r.playersLock.Lock()
	defer r.playersLock.Unlock()

	delete(r.players, u.uid)

	if ROOM_STATUS_READY == r.status && 0 < len(r.players) {
		r.reset()
	} else if 0 == len(r.players) {
		GetRoomMapInstance().deleteRoom(r.room_id)
	}
	return true
}

func (r *RoomData) setCardMulti(multi int32) bool {
	if multi < 1 || multi > 3 {
		return false
	}

	r.card_multi = multi
	return true
}

func (r *RoomData) start() bool {
	r.status = ROOM_STATUS_READY

	r.generateCards()

	return true
}

func (r *RoomData) getPlayers() []OutPlayerData {
	out := make([]OutPlayerData, len(r.players))

	index := 0
	for _, p := range r.players {
		out[index].Uid = p.client.uid
		out[index].Uname = p.uname

		index++
	}

	return out
}

func (r *RoomData) getCards() []OutCardData {
	// if len(r.cards) != 34*card_multi {
	//	r.generateCards()
	// }

	out := make([]OutCardData, len(r.cards))

	index := 0
	for _, c := range r.cards {
		out[index].Cid = c.cid
		out[index].Type = c.ctype
		out[index].Order = c.corder

		index++
	}

	return out
}

func (r *RoomData) generateCards() {
	r.cardslock.Lock()
	defer r.cardslock.Unlock()

	if len(r.cards) != 0 {
		return
	}

	for m := int32(1); m <= r.card_multi; m++ {
		for i := CARD_TYPE_SHIFU; i <= CARD_TYPE_XIAOYAOJING; i++ {
			for j := int32(1); j <= 4; j++ {
				id := m*100000 + i*1000 + j
				r.cards[id] = &CardData{cid: id, ctype: i, corder: j}
			}
		}
		jocker_id := m*100000 + CARD_TYPE_JOCKER*1000 + 1
		r.cards[jocker_id] = &CardData{cid: jocker_id, ctype: CARD_TYPE_JOCKER, corder: 1, jocker_to: 0}
		jocker_id += 1
		r.cards[jocker_id] = &CardData{cid: jocker_id, ctype: CARD_TYPE_JOCKER, corder: 1, jocker_to: 0}
	}

	cids := make([]int32, len(r.cards))
	index := 0
	for _, v := range r.cards {
		cids[index] = v.cid
		index++
	}

	rand.Seed(time.Now().Unix())

	n := int32(len(r.cards))
	for i := 0; i < 1000; i++ {
		ka := rand.Int31() % n
		kb := rand.Int31() % n

		t := cids[ka]
		cids[ka] = cids[kb]
		cids[kb] = t
	}

	uids := make([]int32, len(r.players))
	index = 0
	for _, v := range r.players {
		uids[index] = v.client.uid
		index++
	}

	for k, v := range cids {
		id := uids[k%len(uids)]
		r.cards[v].own_uid = id
		r.players[id].numCard += 1
	}

	n = int32(len(uids))
	for i := 0; i < 100; i++ {
		ka := rand.Int31() % n
		kb := rand.Int31() % n

		t := uids[ka]
		uids[ka] = uids[kb]
		uids[kb] = t
	}

	r.during.order = uids
	r.during.current_player = uids[0]
	r.during.steps = make([]StepData, 0)
}

func (r *RoomData) findNextPlayer(now_uid int32) int32 {
	if 1 == len(r.during.order) {
		return now_uid
	}

	index := 0
	for k, v := range r.during.order {
		if v == now_uid {
			index = k
			break
		}
	}

	for i := index + 1; i != index; i++ {
		if i >= len(r.during.order) {
			i = -1
			continue
		}

		t_uid := r.during.order[i]
		if 0 != r.players[t_uid].numCard {
			return t_uid
		}
	}

	return now_uid
}

func (r *RoomData) pass(uid int32) bool {
	if r.during.current_player != uid {
		return false
	}

	r.during.current_player = r.findNextPlayer(uid)

	cardnum := int32(0)
	for _, v := range r.cards {
		if uid == v.own_uid {
			cardnum++
		}
	}
	r.players[uid].total_punish += cardnum
	r.during.pass_num += 1

	if 0 == len(r.during.steps) {
		return true
	}

	ls := r.during.steps[len(r.during.steps)-1]
	vn := int32(0)
	for _, v := range r.players {
		if ls.uid == v.client.uid {
			continue
		}
		if 0 == v.numCard {
			continue
		}
		vn += 1
	}

	if vn == r.during.pass_num {
		r.during.steps = append(r.during.steps, StepData{0, make([]int32, 0), make([]int32, 0)})
	}

	return true
}

func (r *RoomData) setJockerTO(uid, jocker_id, cid int32) bool {
	c, ok := r.cards[jocker_id]
	if false == ok {
		return false
	}

	if uid != c.own_uid || CARD_TYPE_JOCKER != c.ctype {
		return false
	}

	tc, ok := r.cards[cid]
	if false == ok {
		return false
	}
	if CARD_TYPE_JOCKER == tc.ctype {
		return false
	}

	c.jocker_to = tc.cid

	return true
}
func (r *RoomData) getCardUType(cid int32) int32 {
	c, ok := r.cards[cid]
	if false == ok {
		return 0
	}

	if CARD_TYPE_JOCKER != c.ctype {
		return c.ctype
	}

	c, ok = r.cards[c.jocker_to]
	if false == ok {
		return 0
	}

	return c.ctype
}
func (r *RoomData) getCardUOrder(cid int32) int32 {
	c, ok := r.cards[cid]
	if false == ok {
		return 0
	}

	if CARD_TYPE_JOCKER != c.ctype {
		return c.corder
	}

	c, ok = r.cards[c.jocker_to]
	if false == ok {
		return 0
	}

	return c.corder
}

func (r *RoomData) pushCards(uid int32, cids []int32) bool {
	if false == r.checkPushGroup(uid, cids) {
		return false
	}

	if false == r.checkWinOld(uid, cids) {
		return false
	}

	r.players[uid].numCard -= int32(len(cids))
	r.removeCardFromUser(cids)

	jokers := make([]int32, 0)
	for _, v := range cids {
		if r.cards[v].ctype == CARD_TYPE_JOCKER {
			jokers = append(jokers, v, r.cards[v].jocker_to)
		}
	}
	r.during.steps = append(r.during.steps, StepData{uid, cids, jokers})
	r.during.pass_num = 0
	r.during.current_player = r.findNextPlayer(uid)

	tn := int32(0)
	for _, p := range r.players {
		tn += p.numCard
	}
	if 0 == tn {
		r.reset()
	}

	return true
}

func (r *RoomData) removeCardFromUser(cids []int32) {
	for _, v := range cids {
		r.cards[v].own_uid = 0
	}
}

//检查牌组是否能一起出
func (r *RoomData) checkPushGroup(uid int32, cids []int32) bool {
	//检查牌组包含的卡片是否有效
	for _, cid := range cids {
		card, ok := r.cards[cid]
		if false == ok {
			return false
		}
		if card.own_uid != uid {
			return false
		}
	}

	if 0 == len(cids) {
		return false
	}

	t := r.getCardUType(cids[0])
	for _, v := range cids {
		if r.getCardUType(v) != t {
			return false
		}
	}

	return true
}

//检查牌组是否能压过牌锅
func (r *RoomData) checkWinOld(uid int32, cids []int32) bool {
	if r.during.current_player != uid {
		return false
	}

	if 0 == len(r.during.steps) {
		return true
	}

	last_step := r.during.steps[len(r.during.steps)-1]
	if 0 == last_step.uid || uid == last_step.uid {
		return true
	}

	if 4 <= len(cids) {
		//炸弹赢普通组
		if 4 > len(last_step.cids) {
			return true
		}
		//大炸弹赢小炸弹
		if len(cids) < len(last_step.cids) {
			return false
		} else if len(cids) > len(last_step.cids) {
			return true
		} else {
			//数量相同
			if r.cmpCardsSameNum(cids, last_step.cids) > 0 {
				return true
			}
			return false
		}
	}

	if len(cids) != len(last_step.cids) {
		return false
	}

	if r.cmpCardsSameNum(cids, last_step.cids) > 0 {
		return true
	}
	return false
}

//比较牌的类型
func (r *RoomData) cmpCardType(a, b int32) int {
	ta := r.getCardUType(a)
	tb := r.getCardUType(b)

	if ta == CARD_TYPE_SHIFU && (tb >= CARD_TYPE_LAOYAOJING && tb <= CARD_TYPE_XIAOYAOJING) {
		return -1
	}
	if tb == CARD_TYPE_SHIFU && (ta >= CARD_TYPE_LAOYAOJING && ta <= CARD_TYPE_XIAOYAOJING) {
		return 1
	}

	if ta < tb {
		return 1
	} else if ta > tb {
		return -1
	} else {
		return 0
	}
}

//比较同数量牌组的大小
func (r *RoomData) cmpCardsSameNum(a, b []int32) int {
	tc := r.cmpCardType(a[0], b[0])
	//先比较类型
	if tc > 0 {
		return 1
	} else if tc < 0 {
		return -1
	} else {
		//类型相同时比较最高顺位
		now_order := int32(100)
		for _, v := range a {
			if r.getCardUOrder(v) < now_order {
				now_order = r.getCardUOrder(v)
			}
		}
		old_order := int32(100)
		for _, v := range b {
			if r.getCardUOrder(v) < old_order {
				old_order = r.getCardUOrder(v)
			}
		}

		if now_order < old_order {
			return 1
		}
		return -1
	}
}

//获取上一次出牌的信息
func (r *RoomData) last() StepData {
	if 0 == len(r.during.steps) {
		return StepData{uid: 0}
	}

	return r.during.steps[len(r.during.steps)-1]
}

//重置游戏状态
func (r *RoomData) reset() {
	for _, p := range r.players {
		p.numCard = 0
	}

	r.cards = make(map[int32]*CardData, 0)
	r.during.steps = make([]StepData, 0)
	r.during.current_player = r.findNextPlayer(r.during.current_player)

	r.generateCards()

	r.round += 1
}
