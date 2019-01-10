#pragma once
class HttpConnect
{
public:
	HttpConnect();
	~HttpConnect();
	void socketHttp(std::string host, std::string request);
	void postData(std::string host, std::string path, std::string post_content);
	void getData(std::string host, std::string path, std::string get_content);
private:

};

