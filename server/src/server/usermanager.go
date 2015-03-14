package server

import (
	"fmt"
	"reflect"
	"strings"
	"sync"
	"time"
)

var gUserIndex int32 = 1000

type UserRequest struct {
	net      *NetRequest
	request  requestData
	response chan *UserResponse
}
type UserResponse struct {
	Resp string
	Err  error
}

func NewUserRequest(r *NetRequest) *UserRequest {
	return &UserRequest{
		net:      r,
		request:  r.req,
		response: make(chan *UserResponse, 1),
	}
}

// 解析Method字符串
func (u *UserRequest) parseMethodString() (string, string, error) {
	//将Method字符串截为两段
	ps := strings.Split(u.request.Method, ".")
	if 2 != len(ps) {
		return "", "", fmt.Errorf("invalid method struct: %v", ps)
	}

	if 0 == len(ps[0]) || 0 == len(ps[1]) {
		return ps[0], ps[1], fmt.Errorf("invalid method string")
	}

	//两段分别为模块名module和方法名method
	return ps[0], ps[1], nil
}

type UserClient struct {
	uid          int32
	queueRequset chan *UserRequest
	sessionData  map[string]interface{}
}

type UserManager struct {
	lock    sync.Mutex
	clients map[int32]*UserClient
}

var gUserManagerInstance *UserManager = nil

func GetUmInstance() *UserManager {
	if nil == gUserManagerInstance {
		gUserManagerInstance = &UserManager{
			clients: make(map[int32]*UserClient),
		}
	}

	return gUserManagerInstance
}

func (u *UserManager) CreateUserClient() *UserClient {
	client := &UserClient{
		queueRequset: make(chan *UserRequest, 10),
		sessionData:  make(map[string]interface{}, 0),
	}

	u.lock.Lock()
	defer u.lock.Unlock()

	gUserManagerInstance.clients[gUserIndex] = client
	client.uid = gUserIndex

	gUserIndex++

	go client.StartClient()

	return client
}

func (u *UserManager) GetUserClient(uid int32) *UserClient {
	client, ok := gUserManagerInstance.clients[uid]
	if false == ok {
		return nil
	}

	return client
}
func (u *UserManager) LogoffUserClient(uid int32) {
	delete(gUserManagerInstance.clients, uid)
}

func (u *UserClient) StartClient() {
	fmt.Println("start client", u.uid)

	u.SetSessionData("uid", u.uid)

	timeout := 0
	for {
		select {
		case r := <-u.queueRequset:
			ret, err := u.dispatchMethod(r)
			resp := &UserResponse{
				Resp: ret,
				Err:  err,
			}
			r.response <- resp
			timeout = 0
		case <-time.After(10 * time.Minute):
			timeout += 1
		}
		if 2 == timeout {
			u.logoff()
			break
		}
	}
	fmt.Println("end client", u.uid)
}

// 分发请求的处理方法
func (u *UserClient) dispatchMethod(r *UserRequest) (string, error) {
	invalidRet := ""

	//解析Method字符串
	mo, me, err := r.parseMethodString()
	if nil != err {
		return invalidRet, fmt.Errorf("method parse failed: %v, module: %s, method: %s", err, mo, me)
	}

	var frame ModuleList

	//取出相应的模块的反射类型
	rtf := reflect.ValueOf(frame)
	rmo := rtf.FieldByName(mo)
	if false == rmo.IsValid() {
		r.net.ErrCode = ERR_WRONG_METHOD
		return invalidRet, fmt.Errorf("invalid module", mo)
	}

	//取出相应的方法的反射类型
	rme := reflect.New(rmo.Type()).MethodByName(me)
	if false == rme.IsValid() {
		r.net.ErrCode = ERR_WRONG_METHOD
		return invalidRet, fmt.Errorf("invalid method", me)
	}

	fmt.Printf("user %d dispatch method:%s.%s, data%s\n", r.request.Uid, mo, me, r.request.Data)

	//调用方法
	ret := rme.Call([]reflect.Value{reflect.ValueOf(u), reflect.ValueOf(r.request.Data)})
	if 2 != len(ret) {
		fmt.Println("method return param num is not 2, is %d", len(ret))
	}

	//返回值0位为返回字符串，1位为error
	if nil != ret[1].Interface() {
		r.net.ErrCode = ERR_REQUEST_ERR
		return invalidRet, fmt.Errorf("method call failed: %v", ret[1].Interface())
	}

	return ret[0].String(), nil
}

func (u *UserClient) SetSessionData(k string, v interface{}) {
	u.sessionData[k] = v
}

func (u *UserClient) GetSessionData(k string) interface{} {
	return u.sessionData[k]
}

func (u *UserClient) UnsetSessionData(k string) {
	delete(u.sessionData, k)
}

func (u *UserClient) logoff() {
	var r Room
	r.LeaveRoom(u, "")

	GetUmInstance().LogoffUserClient(u.uid)
}
