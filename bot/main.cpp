#pragma once
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
#include "bindings.h"

extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/luaconf.h"
#include "lua/lualib.h"
}


#define GetCurrentDir _getcwd

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);
long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;

	}

	return DefWindowProcW(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(
	const char* windowName,
	const char* className) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = (WNDPROC)WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	window = CreateWindowA(
		className,
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClassW((LPCWSTR)windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}
ImFont* font;

TextEditor editor;
bool new_file;
std::string last;
void create_file() {
	if (new_file) {
		ImGui::SetNextWindowSize(ImVec2(167, 100));
		ImGui::SetNextWindowPos(ImVec2(15, 15));
		ImGui::Begin("New lua file", &new_file, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		static char file_name[40];
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 20);
		ImGui::InputText("##.lua", file_name, IM_ARRAYSIZE(file_name));
		ImGui::Spacing();
		if (ImGui::Button("Create", ImVec2(ImGui::GetWindowWidth() - 20, 26))) {
			std::string haha = (string)"C:\\scripts\\" + file_name + ".lua";
			std::ofstream outfile(haha);

			outfile << " " << std::endl;

			outfile.close();
			new_file = !new_file;
		}
		ImGui::End();
	}
}


int total_login=0;

string wchar_t2string(const wchar_t* wchar) {
	string str = "";
	int index = 0;
	while (wchar[index] != 0) {
		str += (char)wchar[index];
		++index;
	}
	return str;
}
wchar_t* string2wchar_t(const string& str) {
	wchar_t wchar[260];
	int index = 0;
	while (index < str.size()) {
		wchar[index] = (wchar_t)str[index];
		++index;
	}
	wchar[index] = 0;
	return wchar;
}
// cba to fix, fix if u want ~Slash
//vector<string> listFilesInDirectory(string directoryName) {
//	WIN32_FIND_DATA FindFileData;
//	wchar_t* FileName = string2wchar_t(directoryName);
//	HANDLE hFind = FindFirstFile((LPCWSTR)FileName, &FindFileData);
//
//	vector<string> listFileNames;
//	listFileNames.push_back(wchar_t2string((wchar_t*)FindFileData.cFileName));
//
//	while (FindNextFile(hFind, &FindFileData))
//		listFileNames.push_back(wchar_t2string((wchar_t*)FindFileData.cFileName));
//
//	return listFileNames;
//}
std::string executorname = "test.lua";
std::vector<string> files;
static int item_current = 1;
static bool showtool;
void execute_thread(lua_State* state, std::string text) {
	luaL_dostring(state, text.c_str());

}
void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();
	auto lang = TextEditor::LanguageDefinition::Lua();
	static const char* identifiers[] = { "SendPacket" };
	static const char* idecls[] = { "SendPacket(type, packet)" };
	for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i) {
		TextEditor::Identifier id;
		//id.mDeclaration = std::string(idecls[i]);

		lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
	}
	editor.SetLanguageDefinition(lang);
	editor.SetPalette(TextEditor::GetDarkPalette());
	font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 17);
	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}


void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}
GrowtopiaBot bot = {"", ""};

void style() {

#pragma region theme
	
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] 					= ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] 			= ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] 				= ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] 					= ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] 					= ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_Border] 					= ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow] 			= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] 					= ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] 			= ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] 			= ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_TitleBg] 					= ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] 			= ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] 		= ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] 				= ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] 				= ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] 			= ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] 	= ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] 		= ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] 				= ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] 				= ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] 		= ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
	style.Colors[ImGuiCol_Button] 					= ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] 			= ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] 			= ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Header] 					= ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] 			= ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] 			= ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Separator] 				= style.Colors[ImGuiCol_Border];
	style.Colors[ImGuiCol_SeparatorHovered] 		= ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] 			= ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_ResizeGrip] 				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] 		= ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] 		= ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Tab] 						= ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
	style.Colors[ImGuiCol_TabHovered] 				= ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
	style.Colors[ImGuiCol_TabActive] 				= ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] 			= ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] 		= ImVec4(0.13f, 0.14f, 0.15f, 1.00f);

	style.Colors[ImGuiCol_PlotLines] 				= ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] 		= ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] 			= ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] 	= ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] 			= ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] 			= ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] 			= ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] 	= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] 		= ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] 		= ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	style.GrabRounding = style.FrameRounding 		= 2.3f;

#pragma endregion
	
}

vector<GrowtopiaBot> bots;

GrowtopiaBot create(string username, string password) {
	GrowtopiaBot bot = { username, password };
	bot.userInit();
	bots.push_back(bot);
	return bot;
}

static int sellected = 0;
static bool Selectall;
static bool punch;
static bool place;
static int hit = 4;
static int id = 340;

static char text[120];

static char spamt[120];
static int interval = 4000;
static bool spammer;

void Spam() {
	static types::time timerlol = std::chrono::system_clock::now();
	if (Selectall) {
		if (spammer) {
			std::string packet = (string)"action|input\n|text|" + spamt;
			
			if (run_at_interval(timerlol, interval)) {
				for (int i = 0; i < bots.size(); i++) {
					bots.at(i).SendPacket(2, packet, bots.at(i).peer);
				}
			}
			
		}
	}
	else {
		if (run_at_interval(timerlol, interval)) {
			for (int i = 0; i < bots.size(); i++) {
				if (bots.at(i).spammer) {
					std::string packet = (string)"action|input\n|text|" + bots.at(i).spamt;
					bots.at(i).SendPacket(2, packet, bots.at(i).peer);
				}
			}
		}
	}
}

//bool pasang = false;
void punchf() {
	static types::time timerlol = std::chrono::system_clock::now();
	
	if (punch) {
		if(!Selectall) {
			if (run_at_interval(timerlol, 150)) {
				bots.at(sellected).punch();
			}
		} else {
			if(run_at_interval(timerlol, 150)) {
				for (int i = 0; i < bots.size(); i++) {
					bots.at(i).punch();
				}
			}
		}
	}
}

void placef() {
	static types::time timerlol = std::chrono::system_clock::now();

	if (place) {
		if (!Selectall) {
			if (run_at_intervals(timerlol, 155)) {
				bots.at(sellected).place ();
			}
		}
		else {
			if (run_at_intervals(timerlol, 155)) {
				for (int i = 0; i < bots.size(); i++) {
					bots.at(i).place();
				}
			}
		}
	}
}

static char msgtext[89];
static bool allppl;

void msgall() {

		static types::time timerlol = std::chrono::system_clock::now();

		for (int i = 0; i < bots.at(sellected).ppluids.size(); i++) {
			std::string packet1; //first
			std::string packet2;
			std::string packet3;
			std::string packet4;
			std::string packet5; //last

			packet1 = "action|friends\ndelay|0";
			packet2 = "action|dialog_return\ndialog_name|socialportal\nbuttonClicked|showfriend";
			packet3 = "action|dialog_return\ndialog_name|friends\nbuttonClicked|" + (string)bots.at(sellected).frienduid;
			packet4 = "action|dialog_return\ndialog_name|friends_edit\nfriendID|" + (string)bots.at(sellected).frienduid + "|\nbuttonClicked|msg";
			packet5 = "action|dialog_return\ndialog_name|friends_message\nfriendID|" + to_string(bots.at(sellected).ppluids.at(i)) + "|\nbuttonClicked|send\n\ntext|" + (string)msgtext + " " + gen_random(8);

			bots.at(sellected).SendPacket(2, packet1, bots.at(sellected).peer);

			bots.at(sellected).SendPacket(2, packet2, bots.at(sellected).peer);

			bots.at(sellected).SendPacket(2, packet3, bots.at(sellected).peer);

			bots.at(sellected).SendPacket(2, packet4, bots.at(sellected).peer);

			bots.at(sellected).SendPacket(2, packet5, bots.at(sellected).peer);

			Sleep(150);
		}
}



static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled(XorStr("(?)").c_str());
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static int speed = 1;
static int lua_sendpacket(lua_State* L) {
	if (lua_isstring(L, 2) && lua_isnumber(L, 1)) {
		if (!Selectall) {
			bots.at(sellected).SendPacket(lua_tonumber(L, 1), lua_tostring(L, 2), bots.at(sellected).peer);
		}

	}
	return 0;
}

// cba to fix, so fix if u want to ~Slash
//vector<string> get_all_files_names_within_folder(string folder)
//{
//	vector<string> names;
//	string search_path = folder;
//	WIN32_FIND_DATA fd;
//	HANDLE hFind = ::FindFirstFile((LPCWSTR)search_path.c_str(), &fd);
//	if (hFind != INVALID_HANDLE_VALUE) {
//		do {
//			// read all (real) files in current folder
//			// , delete '!' read other 2 default folder . and ..
//			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
//				names.push_back(fd.cFileName);
//			}
//		} while (::FindNextFile(hFind, &fd));
//		::FindClose(hFind);
//	}
//	return names;
//}



void gui::Executor()
{
	lua_State* state = luaL_newstate();
	luaL_openlibs(state);

	lua_newtable(state);
	luaL_setfuncs(state, imguilib, 0);
	PushImguiEnums(state, "constant");
	lua_setglobal(state, "imgui");

	lua_register(state, "SendPacket", lua_sendpacket);

	//files = get_all_files_names_within_folder("C:\\scripts\\*.lua"); cba to fix so fix if u want to ~Slash

	/*

	const char* items[100];
	for (int i = 0; i < files.size(); i++) {
		items[i] = files[i].c_str();
	}
	ImGui::BeginChild("##childre12", ImVec2(128, 315), false, 0);
	if (ImGui::Button("New", ImVec2(140, 30))) {
		new_file = !new_file;
	}
	ImGui::SetNextItemWidth(120);
	if (ImGui::ListBox("##dsd2222222323", &item_current, items, files.size(), (ImGui::GetWindowHeight() - 84) / 19)) { // 120 be child
		ifstream infile{ "C:\\scripts\\" + files[item_current] };
		string file_contents{ istreambuf_iterator<char>(infile), istreambuf_iterator<char>() };
		file_contents.pop_back();
		editor.SetText(file_contents);
		last = files[item_current];
		executorname = last;

	}
	ImVec2 m;
	if (ImGui::IsItemHovered() && GetAsyncKeyState(VK_RBUTTON)) {
		if (!showtool) {
			m = ImGui::GetIO().MousePos;
			ImGui::SetNextWindowPos(ImVec2(m.x - 10, m.y));
			showtool = true;
		}
		else {
			showtool = false;
		}
		Sleep(200);
	}
	if (showtool) {


		ImGui::Begin("Child Tooltip?", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		if (ImGui::Button("Delete")) {
			remove(std::string("C:\\scripts\\" + files[item_current]).c_str());
			showtool = !showtool;
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();

	ImGui::SameLine();
	*/
	ImGui::BeginChild("##childrenlol", ImVec2(WIDTH, 270), false, 0);
	const char* items[100];
	for (int i = 0; i < files.size(); i++) {
		items[i] = files[i].c_str();
	}
	// cba to fix so if you want to fix, go ahead ~Slash
	//if (ImGui::Combo("Select file", &item_current, items, files.size(), 9)) {
	//	ifstream infile{ "C:\\scripts\\" + files[item_current] };
	//	string file_contents{ istreambuf_iterator<char>(infile), istreambuf_iterator<char>() };
	//	file_contents.pop_back();
	//	editor.SetText(file_contents);
	//	last = files[item_current];
	//	executorname = last;
	//}
	if (ImGui::Button("Execute")) {
		std::thread thr(execute_thread, state, editor.GetText());
		thr.detach();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		ofstream myfile;
		myfile.open("C:\\scripts\\" + executorname);
		myfile << editor.GetText();
		myfile.close();
	}
	ImGui::SameLine();
	if (ImGui::Button("New")) {
		new_file = !new_file;
	}
	if (last != "") { ImGui::SameLine(); ImGui::Text("%d lines | %s", editor.GetTotalLines(), last.c_str()); }

	editor.Render("", ImVec2(ImGui::GetWindowWidth() - 10, 220), false); //height - 30
	ImGui::EndChild();

}


void gui::Render() noexcept
{
	style();
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	auto test = XorStr(" Altanb#3535");
	ImGui::PushFont(font);
	ImGui::Begin(test.c_str(), &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	{
		ImGui::BeginChild(XorStr("##childrenxdxd").c_str(), ImVec2(168, 315), true);
		{	
			ImGui::Checkbox(XorStr("Select all").c_str(), &Selectall);
			const char* names[50] = { "" };
			for (int i = 0; i < bots.size(); i++) {
				names[i] = bots.at(i).uname.c_str();
			}
			ImGui::SetNextItemWidth(150);
			ImGui::ListBox(XorStr("##name").c_str(), &sellected, names, bots.size(), 9);

			static char username[20];
			static char password[20];
			ImGui::SetNextItemWidth(150);
			ImGui::InputTextWithHint("##username", "Username", username, IM_ARRAYSIZE(username));
			ImGui::SetNextItemWidth(150);
			ImGui::InputTextWithHint("##password", "Password", password, IM_ARRAYSIZE(password));
			if (ImGui::Button(XorStr("Add").c_str(), ImVec2(71, 25))) {
				// create("babucindyc111", "babu@1330");
				create(username, password);
				loggedin = true;
			}
			ImGui::SameLine();
			if (ImGui::Button(XorStr("Remove").c_str(), ImVec2(71, 25))) {
				bots.erase(bots.begin()+sellected);
				if (sellected > 0) {
					sellected -= 1;
				} else if (bots.size() == 0) {
					loggedin = false;
				}
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild(XorStr("##childrenxdxd3").c_str(), ImVec2(WIDTH - 190, 315), true);
		{
			if (ImGui::BeginTabBar(XorStr("##tabbar").c_str()))
			{
				if (ImGui::BeginTabItem(XorStr("Main").c_str())) {
					static char worldname[25];
					if (loggedin) {
						ImGui::InvisibleButton(XorStr("##test").c_str(), ImVec2(10, 3));
						ImGui::SetNextItemWidth(320);
						ImGui::InputTextWithHint("##World", "World", worldname, IM_ARRAYSIZE(worldname));
						ImGui::SameLine();
						if (ImGui::Button(XorStr("Warp").c_str())) {
							if (loggedin) {
								if (!Selectall)
									bots.at(sellected).EnterWorld(worldname);
								else {
									for (int i = 0; i < bots.size(); i++) {
										bots.at(i).EnterWorld(worldname);
									}
								}
							}
						}
						ImGui::InvisibleButton(XorStr("##test").c_str(), ImVec2(10, 3));
						if (!Selectall) {
							ImGui::BeginChild(XorStr("##child42fsdf").c_str(), ImVec2(384, 78), true, ImGuiWindowFlags_MenuBar);
							{
								ImGui::BeginMenuBar();
								ImGui::Text("Bot %s", bots.at(sellected).uname.c_str());
								ImGui::EndMenuBar();
								if (bots.at(sellected).connected)
									ImGui::Text(XorStr("Status : Connected").c_str());
								else
									ImGui::Text(XorStr("Status : Not Connected").c_str());
								ImGui::Text("Current world: %s", bots.at(sellected).currentWorld.c_str());
							}
							ImGui::EndChild();
							ImGui::InvisibleButton(XorStr("##test1").c_str(), ImVec2(10, 4));
						}
						ImGui::BeginChild(XorStr("##childrenlol2").c_str(), ImVec2(125, 137), true, ImGuiWindowFlags_MenuBar);
						{
							ImGui::BeginMenuBar();
							ImGui::Text(XorStr("Movement").c_str());
							ImGui::SameLine();
							HelpMarker(XorStr("Use this to move your bot").c_str());
							ImGui::EndMenuBar();
							ImGui::InvisibleButton(XorStr("##test2").c_str(), ImVec2(30, 30));
							ImGui::SameLine();
							if (ImGui::Button(XorStr("##Up").c_str(), ImVec2(30, 30)))
							{
								if (!Selectall)
									bots.at(sellected).move(XorStr("up"), speed);
								else {
									for (int i = 0; i < bots.size(); i++) {
										bots.at(i).move(XorStr("up"), speed);
									}
								}
							}
							if (ImGui::Button(XorStr("##Left").c_str(), ImVec2(30, 30)))
							{
								if (!Selectall)
									bots.at(sellected).move(XorStr("left"), speed);
								else {
									for (int i = 0; i < bots.size(); i++) {
										bots.at(i).move(XorStr("left"), speed);
									}
								}
							}
							ImGui::SameLine();
							if (ImGui::Button(XorStr("##Door").c_str(), ImVec2(30, 30)))
							{
								if (!Selectall)
									bots.at(sellected).use();
								else {
									for (int i = 0; i < bots.size(); i++) {
										bots.at(i).use();
									}
								}
							}
							ImGui::SameLine();
							if (ImGui::Button(XorStr("##Right").c_str(), ImVec2(30, 30)))
							{
								if (!Selectall)
									bots.at(sellected).move(XorStr("right"), speed);
								else {
									for (int i = 0; i < bots.size(); i++) {
										bots.at(i).move(XorStr("right"), speed);
									}
								}
							}
							ImGui::InvisibleButton(XorStr("##test24").c_str(), ImVec2(30, 30));
							ImGui::SameLine();
							if (ImGui::Button(XorStr("##Down").c_str(), ImVec2(30, 30)))
							{
								if (!Selectall)
									bots.at(sellected).move(XorStr("down"), speed);
								else {
									for (int i = 0; i < bots.size(); i++) {
										bots.at(i).move(XorStr("down"), speed);
									}
								}
							}
						}
						ImGui::EndChild();
						ImGui::SameLine();
						ImGui::BeginChild(XorStr("##MovementSpeed").c_str(), ImVec2(0, 40), true);
						{
							ImGui::SliderInt("Tile", &speed, 1, 5);
						}
						ImGui::EndChild();
						}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem(XorStr("Auto").c_str())) {
						if (loggedin) {
							ImGui::Checkbox(XorStr("Break").c_str(), &punch);
							ImGui::SameLine();
							ImGui::InputInt("Hit", &hit);
							ImGui::Checkbox(XorStr("Place").c_str(), &place);
							ImGui::SameLine();
							ImGui::InputInt("Item ID", &id);
							if(ImGui::Button(XorStr("cout").c_str(), ImVec2(50, 25))){
								//cout << bot.OnSendToServerStruct.address;
							}
						}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem(XorStr("World").c_str())) {
						if (loggedin) {
							if (ImGui::BeginTabBar(XorStr("##tabbar222").c_str())) {
								if (ImGui::BeginTabItem(XorStr("Players").c_str()))
								{
									if (bots.at(sellected).currentWorld != XorStr("EXIT").c_str()) {
										for (int i = 0; i < bots.at(sellected).objects.size(); i++) {
											if (!bots.at(sellected).objects.at(i).isGone) { //complex
												utils::replace(bots.at(sellected).objects.at(i).name, "``", "");
												utils::replace(bots.at(sellected).objects.at(i).name, "`2", "");
												utils::replace(bots.at(sellected).objects.at(i).name, "`w", "");
												utils::replace(bots.at(sellected).objects.at(i).name, "`#@", "");
												utils::replace(bots.at(sellected).objects.at(i).name, "`^", "");
												if (ImGui::CollapsingHeader(bots.at(sellected).objects.at(i).name.c_str())) {
													ImGui::Text("Netid: %d", bots.at(sellected).objects.at(i).netId);
													ImGui::Text("Country: %s", bots.at(sellected).objects.at(i).country.c_str());
													ImGui::Text("UserID: %d", bots.at(sellected).objects.at(i).userId);
													ImGui::Text("X: %d, Y: %d", (int)bots.at(sellected).objects.at(i).x / 32, (int)bots.at(sellected).objects.at(i).y / 32);
													ImGui::Checkbox("Follow", &bots.at(sellected).dofollow);
													if (bots.at(sellected).dofollow)
														bots.at(sellected).followplayer = bots.at(sellected).objects.at(i).netId;
												}
											}
										}
									}
									ImGui::EndTabItem();
								}
							}
							ImGui::EndTabBar();	
						}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem(XorStr("Spammer").c_str()))
					{
						if (loggedin) {
								if (Selectall)
								{
									ImGui::Checkbox(XorStr("Enable auto spam").c_str(), &spammer);
									ImGui::InputTextWithHint("##spam", "Spam text", spamt, IM_ARRAYSIZE(spamt));
									ImGui::InputInt(XorStr("interval").c_str(), &interval);
									ImGui::SameLine();
									HelpMarker(XorStr("interval (ms)").c_str());
								} else {
									ImGui::Checkbox(XorStr("Enable auto spam").c_str(), &bots.at(sellected).spammer);
									ImGui::InputTextWithHint("##spam", "Spam text", bots.at(sellected).spamt, IM_ARRAYSIZE(bots.at(sellected).spamt));
									ImGui::InputInt("##interval", &interval);
									ImGui::SameLine();
									HelpMarker(XorStr("interval (ms)").c_str());
								}
								ImGui::Checkbox("Enable auto msg", &bots.at(sellected).automsg);
								ImGui::SameLine();
								HelpMarker("Select all does not support these features.");
								ImGui::InputTextWithHint("##msg", "Message text", bots[sellected].message, IM_ARRAYSIZE(bots[sellected].message));
								ImGui::Checkbox("Insurance", &bots.at(sellected).insurance);
								ImGui::SameLine();
								HelpMarker("Prevent gt detection for auto msg bots.");
								ImGui::Checkbox("Auto msg v2", &bots.at(sellected).automsg2);
								ImGui::SameLine();
								HelpMarker("Enters every sb world and msgs all people in it\n(Insurance on)");
							}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem(XorStr("Settings").c_str())) {
						ImGui::Checkbox("Hide console", &gui::hideconsole);
						ImGui::InputText("Server", &SERVER_HOST[0], 25);
						ImGui::InputInt("Port", &SERVER_PORT);
						//ImGui::InputFloat("Game Version", &gameVersion, 0.01f);
						ImGui::Text(XorStr("Connected to Server!").c_str());
						
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Executor")) {
						Executor();
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();


		}
		ImGui::EndChild();

		
		Spam();
		punchf();
		placef();

		if (loggedin) {
			//bots.at(sellected).eventLoop();
			//bots.at(sellected).userLoop();
			for (int i = 0; i < bots.size(); i++) {
				bots.at(i).eventLoop();
				bots.at(i).userLoop();
			}
		}
	}

	ImGui::End();
	ImGui::PopFont();
}

using namespace std;
using json = nlohmann::json;

inline bool exists_test(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}

int main() {
	SetConsoleTitleA(XorStr("Altanb Console").c_str());
	version();
	init();

	HINSTANCE instance;
	HINSTANCE previousInstance;
	PWSTR arguments;
	int commandShow;

	gui::CreateHWindow(XorStr("Cheat Menu").c_str(), XorStr("Cheat Menu Class").c_str());
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::exit)
	{
		if (gui::hideconsole) {
			::ShowWindow(::GetConsoleWindow(), SW_HIDE);
		}
		gui::BeginRender();
		gui::Render();
		gui::EndRender();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

}
