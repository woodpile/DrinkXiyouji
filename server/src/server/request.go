package server

import (
	"crypto/rc4"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
)

type requestData struct {
	Uid    int32
	User   string
	Time   string
	Method string
	Data   string
}

type NetRequest struct {
	req     requestData
	ErrCode int32
}

func NewNetRequest() *NetRequest {
	return &NetRequest{
		ErrCode: 0,
	}
}

// 解析请求内容到NetRequest
func (n *NetRequest) decodeRequest(r *http.Request) error {
	//从http请求中读取请求内容
	p := make([]byte, r.ContentLength)
	l, err := r.Body.Read(p)
	if nil != err && io.EOF != err {
		return fmt.Errorf("read http request body failed, content len: %d, err: %v", r.ContentLength, err)
	}

	//生成解密句柄
	k := []byte{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
	crp, err := rc4.NewCipher(k)
	if nil != err {
		return fmt.Errorf("create cipher failed: %v", err)
	}

	//解密
	dnp := make([]byte, l)
	crp.XORKeyStream(dnp, p[0:l])

	//对解密后的明文字符串json解码
	json.Unmarshal(dnp, &(n.req))
	return nil
}

type OutUserLogin struct {
	Uid int32
}

func (n *NetRequest) createUser() (string, error) {
	client := GetUmInstance().CreateUserClient()
	if nil == client {
		return "", fmt.Errorf("create client failed")
	}

	out := &OutUserLogin{
		Uid: client.uid,
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("parse param failed, %v", err)
	}

	return string(ret), nil
}

// 分发请求的处理方法
func (n *NetRequest) dispatchMethod() (string, error) {
	//取该用户线程
	client := GetUmInstance().GetUserClient(n.req.Uid)
	if nil == client {
		n.ErrCode = ERR_NO_LOGIN
		return "", fmt.Errorf("user not login")
	}

	ur := NewUserRequest(n)

	client.queueRequset <- ur

	up := <-ur.response

	return up.Resp, up.Err
}
