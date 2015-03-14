package server

import (
	"fmt"
	"net/http"
)

func StartServer() {
	http.HandleFunc("/xiyouji", hXiyoujiMain)

	err := http.ListenAndServe(":9600", nil)
	if nil != err {
		fmt.Println("ListenAndServe:", err)
		return
	}
}

func hXiyoujiMain(w http.ResponseWriter, r *http.Request) {
	if "POST" != r.Method {
		return
	}

	request := NewNetRequest()
	response := NewNetResponse(request)

	//解析请求内容
	err := request.decodeRequest(r)
	if nil != err {
		response.responseError(w, err)
		return
	}

	var ret string
	if ".UserLogin" == request.req.Method {
		//创建请求
		ret, err = request.createUser()
		if nil != err {
			response.responseError(w, err)
			return
		}
	} else {
		//分发请求
		ret, err = request.dispatchMethod()
		if nil != err {
			response.responseError(w, err)
			return
		}
	}
	//写响应
	err = response.responseData(w, ret)
	if nil != err {
		response.responseError(w, err)
		return
	}
}
