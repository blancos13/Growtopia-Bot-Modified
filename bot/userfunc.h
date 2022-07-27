#pragma once
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>
#include <limits>
#include <math.h>
#include <string>
#include <iostream>
#include <regex>
#include <iterator>
#include <algorithm>
#include <ctime>
#include "utilsfunc.h"
#include "corefunc.h"
#include "userfunc.h"
#include "enet/include/enet.h"
#include <ctime>
#include "proton/rtparam.hpp"
#include <fstream>
#include <fstream>
#include <filesystem>
#include <future>
#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include "utils.h"
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <future>
#include <thread>
#include <fstream>
#include <chrono>
#include "corefunc.h"
#include <sstream>
#include "userfunc.h"
#include "json.hpp"
#include <string>
#include <windows.h>
#include <direct.h>
#include <d3d9.h>
#include "gui.h"
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include <algorithm>
#include "imgui/TextEditor.h"
#include <fstream>
#include <fstream>
#include <filesystem>
#include <future>
#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include "HTTPRequest.hpp"

using namespace std;

char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

std::string hexStr(unsigned char data)
{
	std::string s(2, ' ');
	s[0] = hexmap[(data & 0xF0) >> 4];
	s[1] = hexmap[data & 0x0F];
	return s;
}

string generateMac()
{
	string x;
	for (int i = 0; i < 6; i++)
	{
		x += hexStr(rand());
		if (i != 5)
			x += ":";
	}
	return x;
}

string generateRid()
{
	string x;
	for (int i = 0; i < 16; i++)
	{
		x += hexStr(rand());
	}
	for (auto & c : x) c = toupper(c);
	return x;
}

/*string generateFlag() {
	string flag[] = {"ad", "ae", "af", "ag", "ai", "al", "am", "an", "ao", "ar", "as", "at", "au", "aw", "ax", "az", "ba", "bb", "bd", "be", "bf", "bg", "bh", "bi", "bj", "bm", "bn", "bo", "br", "bs", "bt", "bv", "bw", "by", "bz", "ca", "cc", "cd", "cf", "cg", "ch", "ci", "ck", "cl", "cm", "cn", "co", "cr", "cs", "cu", "cv", "cx", "cy", "cz", "de", "dj", "dk", "dm", "do", "dz", "ec", "ee", "eg", "eh", "er", "es", "et", "fi", "fj", "fk", "fm", "fo", "fr", "ga", "gb", "gd", "ge", "gf", "gh", "gi", "gl", "gm", "gn", "gp", "gq", "gr", "gs", "gt", "gu", "gw", "gy", "ha", "hk", "hm", "hn", "hr", "ht", "hu", "id", "ie", "il", "in", "io", "iq", "ir", "is", "it", "jm", "jo", "jp", "ke", "kg", "kh", "ki", "km", "kn", "kp", "kr", "kw", "ky", "kz", "la", "lb", "lc", "lg", "li", "lk", "lr", "ls", "lt", "lu", "lv", "ly", "ma", "mc", "md", "me", "mg", "mh", "mk", "ml", "mm", "mn", "mo", "mp", "mq", "mr", "ms", "mt", "mu", "mv", "mw", "mx", "my", "mz", "na", "nc", "ne", "nf", "ng", "ni", "nl", "no", "np", "nr", "nu", "nz", "om", "pa", "pe", "pf", "pg", "ph", "pk", "pl", "pm", "pn", "pr", "ps", "pt", "pw", "py", "qa", "re", "ro", "rs", "rt", "ru", "rw", "sa", "sb", "sc", "sd", "se", "sg", "sh", "si", "sj", "sk", "sl", "sm", "sn", "so", "sr", "st", "sv", "sy", "sz", "tc", "td", "tf", "tg", "th", "tj", "tk", "tl", "tm", "tn", "to", "tr", "tt", "tv", "tw", "tz", "ua", "ug", "um", "us", "uy", "uz", "va", "vc", "ve", "vg", "vi", "vn", "vu", "wf", "ws", "ye", "yt", "za", "zm", "zw"};
	return flag[rand()%244];
}*/

bool GrowtopiaBot::rep(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void GrowtopiaBot::solve_captcha(std::string text) 
{
	GrowtopiaBot::rep(text,
		"set_default_color|`o\nadd_label_with_icon|big|`wAre you Human?``|left|206|\nadd_spacer|small|\nadd_textbox|What will be the sum of the following "
		"numbers|left|\nadd_textbox|",
		"");
	GrowtopiaBot::rep(text, "|left|\nadd_text_input|captcha_answer|Answer:||32|\nend_dialog|captcha_submit||Submit|", "");
	auto number1 = text.substr(0, text.find(" +"));
	auto number2 = text.substr(number1.length() + 3, text.length());
	int result = atoi(number1.c_str()) + atoi(number2.c_str());
	SendPacket(2, "action|dialog_return\ndialog_name|captcha_submit\ncaptcha_answer|" + std::to_string(result), peer);
}

string stripMessage(string msg)
{
	regex e("\\x60[a-zA-Z0-9!@#$%^&*()_+\\-=\\[\\]\\{};':\"\\\\|,.<>\\/?]");
	string result = regex_replace(msg, e, "");
	result.erase(std::remove(result.begin(), result.end(), '`'), result.end());
	return result;
}

void GrowtopiaBot::onLoginRequested()
{
	srand(time(NULL));
	string token;
	char lmode = '0';
	if (!login_user && !login_token) { token = ""; } else { token = "\nuser|" + std::to_string(login_user) + "\ntoken|" + std::to_string(login_token) + "\nUUIDToken|" + login_UUIDtoken + "\ndoorID|" + login_doorID; lmode = '1'; }
	http::Request request{ "http://a104-125-3-135.deploy.static.akamaitechnologies.com/growtopia/server_data.php" };
    const auto response = request.send("POST", "version=3.95&protocol=162&platform=0", { "Host: www.growtopia1.com" });
    rtvar var1 = rtvar::parse({ response.body.begin(), response.body.end() });
    // cout << var1.get("meta") << endl;
	// onShowCaptcha("add_puzzle_captcha|0098/captcha/generated/acba1e90-b738-429c-8c23-5018282fee1f-PuzzleWithMissingPiece.rttex|0098/captcha/generated/acba1e90-b738-429c-8c23-5018282fee1f-TrimmedPuzzlePiece.rttex|ubistatic-a.akamaihd.net|86232|");
	auto packet = "tankIDName|" + uname + "\ntankIDPass|" + upass + "\nrequestedName|RatTiny\nf|1\nprotocol|162\ngame_version|" + gameVersion + "\nfz|17040192\nlmode|" + lmode + "\ncbits|0\nplayer_age|34\nGDPR|1\ncategory|_-5100hash2|0" + "\nmeta|" + var1.get("meta") + "\nfhash|-716928004\nrid|" + generateRid() + "\nplatformID|0,1,1\ndeviceVersion|0\ncountry|us\nhash|553335509" + "\nmac|" + generateMac() + token + "\nwk|" + generateRid() + "\nzf|-770910015";
	dbgPrint(packet);
	SendPacket(2, packet, peer);
}

void GrowtopiaBot::packet_type3(string text)
{
	dbgPrint("Some text is here: " + text);
	if (text.find("LOGON ATTEMPTS") != string::npos) {
		
	}
	else if (text.find("password is wrong") != string::npos) {
		
	}
	else if (text.find("action|logon_fail") != string::npos) {
		connectClient();
		objects.clear();
	}
	else if (text.find("maintenance") != string::npos) {
		cout << "Maintenance banh" << endl;

	} 
	else if (text.find("UPDATE REQUIRED!") != string::npos) {
		fstream d;
		nlohmann::json j;
		d.open("config.json", ios::out | ios::trunc);
		d.close();
		d.open("config.json", ios::out);

		j["gtversion"] = text.substr(text.find("`$V")+3,4);
		d << j;
		d.close();

		gameVersion = text.substr(text.find("`$V")+3,4);
	}
}

void GrowtopiaBot::packet_type6(string text)
{
	SendPacket(2, "action|enter_game\n", peer);
	enet_host_flush(client);
}

void GrowtopiaBot::packet_unknown(ENetPacket* packet)
{

}

void GrowtopiaBot::OnSendToServer(string address, string doorID, string UUIDtoken, int port, int userId, int token)
{
	login_user = userId;
	login_token = token;
	login_doorID = doorID;
	login_UUIDtoken = UUIDtoken;
	connectClient(address, port);
}

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

void GrowtopiaBot::onShowCaptcha(string text) {
	if (text.find("`wAre you Human?``") != -1){
		solve_captcha(text);

	} else {
		auto g = split(text,"|");
		std::string captchaid = g[1];
        utils::replace(captchaid, "0098/captcha/generated/", "");
        utils::replace(captchaid, "PuzzleWithMissingPiece.rttex", "");
        captchaid = captchaid.substr(0, captchaid.size() - 1);

        http::Request request{ "http://api.surferstealer.com/captcha/index?CaptchaID=" + captchaid };
        const auto response = request.send("GET");
        std::string output = std::string{ response.body.begin(), response.body.end() };
        if (output.find("Answer|Failed") != std::string::npos) {
        	dbgPrint("[Puzzle] CaptchaID Failed ");
        	SendPacket(2, "action|dialog_return\ndialog_name|puzzle_captcha_submit\ncaptcha_answer|0.624505|CaptchaID|" + g[4], peer);
           	//failed
        } else if (output.find("Answer|") != std::string::npos) {
            utils::replace(output, "Answer|", "");
            dbgPrint("[Puzzle] CaptchaID Success | Answer: " + output + " ID: " + g[4]);
            SendPacket(2, "action|dialog_return\ndialog_name|puzzle_captcha_submit\ncaptcha_answer|" + output + "|CaptchaID|" + g[4], peer);
         	// success
        } else {
        	dbgPrint("[Puzzle] CaptchaID Failed ");
        	SendPacket(2, "action|dialog_return\ndialog_name|puzzle_captcha_submit\ncaptcha_answer|0.624505|CaptchaID|" + g[4], peer);
        }
    }
}

void GrowtopiaBot::OnDialogRequest(string text) {
	if (text.find("end_dialog|captcha_submit||Submit|") != -1)
		solve_captcha(text);
}

void GrowtopiaBot::use() {
	GamePacket pkt{ 0 };
	pkt.type = 7;
	pkt.tile.x = (int)(localx / 32);
	pkt.tile.y = (int)(localy / 32);
	SendPacketRaw(4, &pkt, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
}

void GrowtopiaBot::move(std::string to, int blocks) {
	if (to == "right") {
		GamePacket pkt{ 0 };
		pkt.type = 0;
		pkt.pos.x = localx + 32 * blocks;
		pkt.pos.y = localy;
		SendPacketRaw(4, &pkt, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
		localy = localy;
		localx = localx + 32 * blocks;
		cout << blocks << endl;
	}
	else if (to == "left") {
		GamePacket pkt{ 0 };
		pkt.type = 0;
		pkt.pos.x = localx - 32 * blocks;
		pkt.pos.y = localy;
		SendPacketRaw(4, &pkt, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
		localy = localy;
		localx = localx - 32 * blocks;
	}
	else if (to == "down") {
		GamePacket pkt{ 0 };
		pkt.type = 0;
		pkt.pos.x = localx;
		pkt.pos.y = localy + 32 * blocks;
		SendPacketRaw(4, &pkt, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
		localy = localy + 32 * blocks;
		localx = localx;
	}
	else if (to == "up") {
		GamePacket pkt{ 0 };
		pkt.type = 0;
		pkt.pos.x = localx;
		pkt.pos.y = localy - 32 * blocks;
		SendPacketRaw(4, &pkt, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
		localy = localy - 32 * blocks;
		localx = localx;
	}
}
void GrowtopiaBot::punch() {
	GamePacket pkt{ 0 };
	pkt.type = 3;
	pkt.int_data = 18;
	pkt.pos.x = localx;
	pkt.pos.y = localy;
	pkt.tile.x = (localx / 32) + 1;
	pkt.tile.y = (localy / 32);
	SendPacketRaw(4, &pkt, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
}

void GrowtopiaBot::place() {
	GamePacket pkt{ 0 };
	pkt.type = 3;
	pkt.int_data = 2;
	pkt.item_id = 2;
	pkt.pos.x = localx;
	pkt.pos.y = localy;
	pkt.tile.x = (localx / 32) + 1;
	pkt.tile.y = (localy / 32);
	SendPacketRaw(4, &pkt, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
}


void GrowtopiaBot::collect() {
	GamePacket pkt{ 0 };
	pkt.type = PACKET_ITEM_ACTIVATE_OBJECT_REQUEST;
	pkt.netid = 5;
	pkt.pos.x = localx;
	pkt.pos.y = localy;
	SendPacketRaw(4, &pkt, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);

}

bool niegegr = false;
void GrowtopiaBot::sleepforgo() {
	niegegr = true;
	SendPacket(2, "action|input\ntext|/go", peer);
	Sleep(17000);
	niegegr = false;
}

void GrowtopiaBot::OnConsoleMessage(string message) {
	string strippedMessage = stripMessage((string)message);
	chatoutput = chatoutput + strippedMessage + "\n";
	if (strippedMessage.find("[SB]") != -1 && automsg2) {
		SendPacket(2, "action|input\n|text|/go", peer);
	}
	if (strippedMessage.find("[SB]") != -1 && !niegegr && gettingppl) {
		if (!dontdo) {
			std::thread x(&GrowtopiaBot::sleepforgo, this);
			x.detach();
		}
	}
}

void GrowtopiaBot::OnPlayPositioned(string sound)
{

}

void GrowtopiaBot::OnSetFreezeState(int state)
{

}

void GrowtopiaBot::OnRemove(string data) // "netID|x\n"
{
	std::stringstream ss(data.c_str());
	std::string to;
	int netID = -1;
	while (std::getline(ss, to, '\n')) {
		string id = to.substr(0, to.find("|"));
		string act = to.substr(to.find("|") + 1, to.length() - to.find("|"));
		if (id == XorStr("netID"))
		{
			netID = atoi(act.c_str());
		}
		else {
			
		}
	}
	for (int i = 0; i < objects.size(); i++) {
		if (objects.at(i).netId == netID) {
			objects.at(i).isGone = true;
		}
	}
}

std::string gen_random(const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string tmp_s;
	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_s;
}

rtvar var2;

bool Wait(const unsigned long& Time)
{
	clock_t Tick = clock_t(float(clock()) / float(CLOCKS_PER_SEC) * 1000.f);
	if (Tick < 0) // if clock() fails, it returns -1
		return 0;
	clock_t Now = clock_t(float(clock()) / float(CLOCKS_PER_SEC) * 1000.f);
	if (Now < 0)
		return 0;
	while ((Now - Tick) < Time)
	{
		Now = clock_t(float(clock()) / float(CLOCKS_PER_SEC) * 1000.f);
		if (Now < 0)
			return 0;
	}
	return 1;
}
void GrowtopiaBot::doautomsg2() {
	
}
namespace types {
	typedef	std::chrono::duration<double> elapsed;
	typedef std::chrono::system_clock::time_point time;
}



bool run_at_interval(types::time& timer, double interval) {
	types::time now = std::chrono::system_clock::now();
	types::elapsed elapsed_sec = now - timer;
	interval /= 1000;

	if (elapsed_sec.count() >= interval) {
		timer = now;
		return true;
	}
	return false;
}

bool run_at_intervals(types::time& timer, double interval) {
	types::time now = std::chrono::system_clock::now();
	types::elapsed elapsed_sec = now - timer;
	interval /= 1000;

	if (elapsed_sec.count() >= interval) {
		timer = now;
		return true;
	}
	return false;
}

bool eh;

void GrowtopiaBot::OnSpawn(string data)
{
	
	       var2 = rtvar::parse(data); 
		   if (!dontdo) {
			   if (gettingppl) {
				   if (ppluids.size() <= selectmax) {
					   int uid = var2.get_int("userID");
					   string name = var2.get("name");
					   if (std::find(ppluids.begin(), ppluids.end(), uid) == ppluids.end()) {
						   ppluids.push_back(uid);
						   pplnames.push_back(name);
					   }
				   }
			   }
			   if (gettingppl && selectmax >= 180) {
				   selectmax = 50;
				   ppluids.clear();
			   }
		   }
		   
			   

		  if (automsg2) {
			  auto name = var2.get("name");
			  utils::replace(name, "`w", "");
			  utils::replace(name, "``", "");
			  utils::replace(name, "`2", "");
			  utils::replace(name, "`#@", "");
			  utils::replace(name, "`^", "");

			  std::string packet;
			  packet = "action|input\n|text|/msg " + name + " " + message + " " + gen_random(5);
			  SendPacket(2, packet, peer);
		  }

		  if (automsg == true) {
			  auto name = var2.get("name");
			  utils::replace(name, "`w", "");
			  utils::replace(name, "``", "");
			  utils::replace(name, "`2", "");
			  utils::replace(name, "`#@", "");
			  utils::replace(name, "`^", "");

			  std::string packet;
			  if (insurance)
				  packet = "action|input\n|text|/msg " + name + " " + message + " " + gen_random(5);
			  else
				  packet = "action|input\n|text|/msg " + name + " " + message;
			  
			  

			  if (Wait(450))
				  SendPacket(2, packet, peer);
		  }

	ObjectData objectData;
	bool actuallyOwner = false;

	   auto name = var2.find(XorStr("name"));
            auto netid = var2.find(XorStr("netID"));
	
	
			objectData.country = var2.get(XorStr("country"));
		 
			if (stripMessage(var2.get(XorStr("name"))) == ownerUsername) actuallyOwner = true;
			objectData.name = var2.get(XorStr("name"));
		 
			if (actuallyOwner) owner = var2.get_int(XorStr("netID"));
			objectData.netId = var2.get_int(XorStr("netID"));  
			objectData.userId =  var2.get_int(XorStr("userID"));		 
		
	auto pos = var2.find(XorStr("posXY"));
                    if (pos && pos->m_values.size() >= 2) {
                        auto x = atoi(pos->m_values[0].c_str());
                        auto y = atoi(pos->m_values[1].c_str());
                        //ply.pos = vector2_t{ float(x), float(y) };
			    objectData.x = x;
			objectData.y = y;
                    }
	
	
	 if (data.find(XorStr("type|local")) != -1) {
                objectData.isLocal = true;
				localx = objectData.x;
				localy = objectData.y;
				localnetid = objectData.netId;
                }
	 if (var2.get(XorStr("mstate")) == "1" || var2.get(XorStr("smstate")) == "1" ||var2.get(XorStr("invis"))== "1"){
		
			objectData.isMod = true;
	 }
	
	 objectData.isGone = false;


	objects.push_back(objectData);
	
}

void GrowtopiaBot::OnAction(string command)
{
	SendPacket(2, "action|input\n|text|Why do you "+command.substr(1, command.length())+"?", peer);

}

void GrowtopiaBot::SetHasGrowID(int state, string name, string password)
{

}

void GrowtopiaBot::SetHasAccountSecured(int state)
{

}

void GrowtopiaBot::OnTalkBubble(int netID, string bubbleText, int type)
{

}

void GrowtopiaBot::SetRespawnPos(int respawnPos)
{
	respawnX = respawnPos % 100; // hacky!!! TODO: get from world data (100)
	respawnY = respawnPos / 100; // hacky!!! TODO: get from world data (100)
}

void GrowtopiaBot::OnEmoticonDataChanged(int val1, string emoticons)
{

}

void GrowtopiaBot::OnSetPos(float x, float y)
{

}

void GrowtopiaBot::OnAddNotification(string image, string message, string audio, int val1)
{
}


void GrowtopiaBot::AtApplyTileDamage(int x, int y)
{

}

void GrowtopiaBot::AtApplyLock(int x, int y, int itemId)
{
}

void GrowtopiaBot::AtPlayerMoving(PlayerMoving* data)
{
	//cout << "-----------\n\ntype " << ", netid " << data->netID << ", x " << data->x << ", y " << data->y << ", characterstate " << data->characterState << ", plantingtree " << data->plantingTree << ", xspeed " << data->XSpeed << ", yspeed " << data->YSpeed << ", punchx " << data->punchX << ", punchy" << data->punchY<< "\n\n--------";
	int object = -1;
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects.at(i).netId == data->netID)
		{
			object = i;
		}
	}
	if (object != -1)
	{
		objects[object].x = data->x;
		objects[object].y = data->y;
	}
	if (isFollowing && data->netID == owner && data->punchX == -1 && data->punchY == -1 && data->plantingTree == 0)
	{
		if (backwardWalk)
			data->characterState ^= 0x10;
		if ((data->characterState & 0x800) && (data->characterState & 0x100)) {
			SendPacket(2, XorStr("action|respawn"), peer);
		}
		for (int i = 0; i < objects.size(); i++)
			if (objects.at(i).isLocal) {
				objects.at(i).x = data->x;
				objects.at(i).y = data->y;
			}
		SendPacketRaw(4, packPlayerMoving(data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
	}

	if (dofollow) {
		if (data->netID == followplayer && data->punchX == -1 && data->punchY == -1 && data->plantingTree == 0) {
			if (backwardWalk)
				data->characterState ^= 0x10;
			if ((data->characterState & 0x800) && (data->characterState & 0x100)) {
				SendPacket(2, XorStr("action|respawn"), peer);
			}
			for (int i = 0; i < objects.size(); i++)
				if (objects.at(i).isLocal) {
					objects.at(i).x = data->x;
					objects.at(i).y = data->y;
				}
			SendPacketRaw(4, packPlayerMoving(data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
		}
	}
}

void GrowtopiaBot::AtAvatarSetIconState(int netID, int state)
{

}

void GrowtopiaBot::WhenConnected()
{
	connected = true;
	
}

void GrowtopiaBot::WhenDisconnected()
{
	connected = false;
	currentWorld = "Not in world";
	
	connectClient();
	total_login += 1;
	cout << "Total Login: " << total_login << endl;
}

int counter = 0; // 10ms per step

vector<string> explode(const string &delimiter, const string &str)
{
	vector<string> arr;

	int strleng = str.length();
	int delleng = delimiter.length();
	if (delleng == 0)
		return arr;//no change

	int i = 0;
	int k = 0;
	while (i < strleng)
	{
		int j = 0;
		while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
			j++;
		if (j == delleng)//found delimiter
		{
			arr.push_back(str.substr(k, i - k));
			i += delleng;
			k = i;
		}
		else
		{
			i++;
		}
	}
	arr.push_back(str.substr(k, i - k));
	return arr;
}


void GrowtopiaBot::EnterWorld(std::string name)
{
	std::string packet = (string)XorStr("action|join_request\nname|");
	std::string fullpacket = (string)packet + (string)name;
	SendPacket(3, fullpacket, peer); // MARRKS
}

void GrowtopiaBot::userLoop() 
{
	if (timeFromWorldEnter > 200 && currentWorld != worldName) {
		if (worldName == "" || worldName == "-") {
			timeFromWorldEnter = 0;
		} else {
			
			objects.clear();
		}
		timeFromWorldEnter = 0;
	}
	timeFromWorldEnter++;
	counter++;
	if ((counter % 1800) == 0)
	{
		string name = "";
		float distance = std::numeric_limits<float>::infinity();
		float ownerX;
		float ownerY;
		for (ObjectData x : objects)
		{
			if (x.netId == owner)
			{
				ownerX = x.x;
				ownerY = x.y;
			}
		}
	}
}

void GrowtopiaBot::userInit() {
	connectClient();
}

void GrowtopiaBot::respawn()
{
	PlayerMoving data;
	data.characterState = 0x924;
	SendPacket(2, "action|respawn", peer);
	for (int i = 0; i < objects.size(); i++)
		if (objects.at(i).isLocal)
		{
			data.x = objects.at(i).x;
			data.y = objects.at(i).y;
			data.netID = objects.at(i).netId;
			SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
			
			break;
		}
}
