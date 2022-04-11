// KeyVolumeView.cpp : implementation of the CKeyVolumeView class
//

#include "stdafx.h"
#include "KeyVolume.h"

#include "KeyVolumeDoc.h"
#include "KeyVolumeView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool g_bSendSpeakerVolumeKey{};
bool g_bAutoShift{};
bool g_bMouseJump{};
int g_nMonitor{};
enum class eZONE : int { main = 0, zone_2 } g_eZone;
std::string g_strZone;
bool g_bReceiverVolume{};
int g_minRepeatCount{2};
int g_receiverVolume{};

bool SendVolume(bool bUp);
bool SendReceiverVolume(int diff);
LRESULT AutoShift(int nCode, WPARAM wParam, LPARAM lParam);

// CKeyVolumeView

IMPLEMENT_DYNCREATE(CKeyVolumeView, CFormView)

BEGIN_MESSAGE_MAP(CKeyVolumeView, CFormView)
	ON_WM_CREATE()
	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_VOLUME_UP, OnVolumeUp)
	ON_BN_CLICKED(IDC_VOLUME_DOWN, OnVolumeDown)
	ON_BN_CLICKED(IDC_CB_SEND_SPEAKER_VOLUME_KEY, &CKeyVolumeView::OnCbSendSpeakerVolumeKey)
	ON_BN_CLICKED(IDC_CB_AUTO_SHIFT, &CKeyVolumeView::OnCbAutoShift)
	ON_BN_CLICKED(IDC_CB_MOUSE_JUMP, &CKeyVolumeView::OnCbMouseJump)
	ON_BN_CLICKED(IDC_CB_RECEIVER_VOLUME, &CKeyVolumeView::OnCbReceiverVolume)
	ON_BN_CLICKED(IDC_QUIT, &CKeyVolumeView::OnQuit)
	ON_BN_CLICKED(IDC_DEVICE_LIST, &CKeyVolumeView::OnDeviceList)
	ON_WM_INPUT()
	ON_BN_CLICKED(IDC_RECEIVER_POWER_ON, &CKeyVolumeView::OnBnClickedReceiverPowerOn)
	ON_BN_CLICKED(IDC_RECEIVER_POWER_OFF, &CKeyVolumeView::OnBnClickedReceiverPowerOff)
	ON_COMMAND(ID_RECEIVER_POWER_ON, &CKeyVolumeView::OnBnClickedReceiverPowerOn)
	ON_COMMAND(ID_RECEIVER_POWER_OFF, &CKeyVolumeView::OnBnClickedReceiverPowerOff)
	ON_COMMAND(ID_RECEIVER_VOLUME_UP, &CKeyVolumeView::OnVolumeUp)
	ON_COMMAND(ID_RECEIVER_VOLUME_DOWN, &CKeyVolumeView::OnVolumeDown)
	ON_COMMAND(ID_CONTEXTMENU_AUTO_SHIFT, &CKeyVolumeView::OnContextmenuAutoShift)
	ON_UPDATE_COMMAND_UI(ID_CONTEXTMENU_AUTO_SHIFT, &CKeyVolumeView::OnUpdateAutoShift)
	ON_COMMAND(ID_CONTEXTMENU_QUIT, &CKeyVolumeView::OnQuit)
	ON_CBN_SELCHANGE(IDC_RECEIVER_ZONE, &CKeyVolumeView::OnSelchangeReceiverZone)
END_MESSAGE_MAP()

// CKeyVolumeView construction/destruction

CKeyVolumeView::CKeyVolumeView() : CFormView(CKeyVolumeView::IDD) {
}

CKeyVolumeView::~CKeyVolumeView() {
}

void CKeyVolumeView::DoDataExchange(CDataExchange* pDX) {
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RECEIVER_ZONE, m_cmbZone);
}

BOOL CKeyVolumeView::PreCreateWindow(CREATESTRUCT& cs) {
	return CFormView::PreCreateWindow(cs);
}

int CKeyVolumeView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect(0, 0, 100, 50);
	m_box.CreateEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_LAYERED, AfxRegisterWndClass(0), _T("VolumeBox"), WS_VISIBLE|WS_POPUP, rect, nullptr, 0);
	m_box.ShowWindow(SW_HIDE);

#ifdef _DEBUG
	//UINT uiNumDevices = 0;
	//GetRawInputDeviceList(NULL, &uiNumDevices, sizeof(RAWINPUTDEVICELIST));
	//if (uiNumDevices > 0) {
	//	RAWINPUTDEVICELIST* pList = new RAWINPUTDEVICELIST[uiNumDevices];
	//	GetRawInputDeviceList(pList, &uiNumDevices, sizeof(RAWINPUTDEVICELIST));

	//	for (int i = 0; i < uiNumDevices; i++) {
	//		RID_DEVICE_INFO di;
	//		memset(&di, 0, sizeof(di));
	//		di.cbSize = sizeof(di);

	//		UINT uiSize = sizeof(di);
	//		GetRawInputDeviceInfo(pList[i].hDevice, RIDI_DEVICEINFO, &di, &uiSize);

	//	}
	//	TRACE("Device : %d\n", uiNumDevices);

	//	delete [] pList;
	//}

	//RAWINPUTDEVICE rid;
	//for (int iPage = 1; iPage < 2; iPage++) {
	//	rid.usUsagePage = iPage;
	//	rid.usUsage = 0;
	//	rid.hwndTarget = m_hWnd;
	//	rid.dwFlags = 0;
	//	for (int i = 0; i < 1024; i++) {
	//		rid.usUsage = i;
	//		UINT nResult = RegisterRawInputDevices(&rid, 1, sizeof(rid));
	//	}
	//}
#endif

	return 0;
}

void CKeyVolumeView::OnInitialUpdate() {
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	g_bSendSpeakerVolumeKey = theApp.GetProfileInt(_T("misc"), _T("SendSpeakerVolumeKey"), 1) ? true : false;
	g_bAutoShift = theApp.GetProfileInt(_T("misc"), _T("AutoShift"), 1) ? true : false;
	g_bMouseJump = theApp.GetProfileInt(_T("misc"), _T("MouseJump"), 1) ? true : false;
	g_nMonitor = GetSystemMetrics(SM_CMONITORS);
	g_minRepeatCount = theApp.GetProfileInt(_T("misc"), _T("MinRepeatCount"), 2);
	g_bReceiverVolume = theApp.GetProfileInt(_T("misc"), _T("ReceiverVolume"), 1) ? true : false;
	g_eZone = theApp.GetProfileInt(_T("misc"), _T("Zone"), 0) == (int)eZONE::main ? eZONE::main : eZONE::zone_2;

	CheckDlgButton(IDC_CB_SEND_SPEAKER_VOLUME_KEY, g_bSendSpeakerVolumeKey ? 1 : 0);
	CheckDlgButton(IDC_CB_AUTO_SHIFT, g_bAutoShift ? 1 : 0);
	CheckDlgButton(IDC_CB_MOUSE_JUMP, g_bMouseJump ? 1 : 0);
	SetDlgItemInt(IDC_MIN_REPEAT_COUNT, g_minRepeatCount);
	CheckDlgButton(IDC_CB_RECEIVER_VOLUME, g_bReceiverVolume ? 1 : 0);
	m_cmbZone.SetCurSel((int)g_eZone);
	OnSelchangeReceiverZone();

	SendReceiverVolume(0);

	SetTimer(T_UPDATE_UI, 100, nullptr);
}

// CKeyVolumeView diagnostics

#ifdef _DEBUG
void CKeyVolumeView::AssertValid() const { CFormView::AssertValid(); }
void CKeyVolumeView::Dump(CDumpContext& dc) const { CFormView::Dump(dc); }
CKeyVolumeDoc* CKeyVolumeView::GetDocument() const { ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKeyVolumeDoc))); return (CKeyVolumeDoc*)m_pDocument; }
#endif //_DEBUG

// CKeyVolumeView message handlers


void CKeyVolumeView::OnTimer(UINT_PTR nIDEvent) {
	switch (nIDEvent) {
	case T_UPDATE_UI :
		{
			// Receiver Volume
			auto str = fmt::format(_T("{:.1f}"), g_receiverVolume/10.);
			CString strUI;
			GetDlgItemText(IDC_RECEIVER_VOLUME, strUI);
			if (str != (LPCTSTR)strUI) {
				SetDlgItemText(IDC_RECEIVER_VOLUME, str.c_str());
				m_box.ShowVolume(str.c_str(), 1500ms);
			}

			// min Repeat Count (AutoShift)
			int minRepeatCount = std::clamp((int)GetDlgItemInt(IDC_MIN_REPEAT_COUNT), 2, 100);
			if (minRepeatCount && g_minRepeatCount != minRepeatCount) {
				g_minRepeatCount = minRepeatCount;
				theApp.WriteProfileInt(_T("misc"), _T("MinRepeatCount"), g_minRepeatCount);
			}
		}
		return;
		break;
	}

	CFormView::OnTimer(nIDEvent);
}

void CKeyVolumeView::OnVolumeUp() {
	SendVolume(true);
}
void CKeyVolumeView::OnVolumeDown() {
	SendVolume(false);
}

void CKeyVolumeView::OnCbSendSpeakerVolumeKey() {
	g_bSendSpeakerVolumeKey = IsDlgButtonChecked(IDC_CB_SEND_SPEAKER_VOLUME_KEY);
	theApp.WriteProfileInt(_T("misc"), _T("SendSpeakerVolumeKey"), g_bSendSpeakerVolumeKey ? 1 : 0);
}
void CKeyVolumeView::OnCbAutoShift() {
	g_bAutoShift = IsDlgButtonChecked(IDC_CB_AUTO_SHIFT);
	theApp.WriteProfileInt(_T("misc"), _T("AutoShift"), g_bAutoShift ? 1 : 0);
}

void CKeyVolumeView::OnCbMouseJump() {
	g_bMouseJump = IsDlgButtonChecked(IDC_CB_MOUSE_JUMP);
	theApp.WriteProfileInt(_T("misc"), _T("MouseJump"), g_bMouseJump? 1 : 0);
}

void CKeyVolumeView::OnContextmenuAutoShift() {
	CheckDlgButton(IDC_CB_AUTO_SHIFT, g_bAutoShift ? 0 : 1);	// inverse
	OnCbAutoShift();
}

void CKeyVolumeView::OnUpdateAutoShift(CCmdUI* pCmdUI) {
	if (pCmdUI)
		pCmdUI->SetCheck(g_bAutoShift ? 1 : 0);
}

void CKeyVolumeView::OnCbReceiverVolume() {
	g_bReceiverVolume = IsDlgButtonChecked(IDC_CB_RECEIVER_VOLUME);
	theApp.WriteProfileInt(_T("misc"), _T("ReceiverVolume"), g_bReceiverVolume ? 1 : 0);
}

void CKeyVolumeView::OnQuit() {
	AfxGetMainWnd()->PostMessage(WM_SYSCOMMAND, MAKEWPARAM(SC_CLOSE2, 0), 0);
}

void CKeyVolumeView::OnDeviceList() {
	UINT nDevice = 0;
	UINT nResult = GetRawInputDeviceList(NULL, &nDevice, sizeof(RAWINPUTDEVICELIST));
	if (nDevice <= 0) {
		int eError = GetLastError();
		CString str;
		str.Format(_T("%d"), eError);
		MessageBox(str);
		return;
	}
	std::vector<RAWINPUTDEVICELIST> lst((size_t)nDevice, RAWINPUTDEVICELIST{});
	GetRawInputDeviceList(lst.data(), &nDevice, sizeof(RAWINPUTDEVICELIST));

	CString str;
	str.Format(_T("Number : %d"), nDevice);
	MessageBox(str);

}

void CKeyVolumeView::OnRawInput(UINT nInputcode, HRAWINPUT hRawInput) {

	RAWINPUT ri;
	memset(&ri, 0, sizeof(ri));
	ri.header.dwSize = sizeof(ri);

	UINT size = sizeof(ri);
	UINT uiResult = GetRawInputData(hRawInput, RID_INPUT, &ri, &size, sizeof(ri.header));
	TRACE("MakeCode (%3d), Flags(0x%08x), VKey(%3d), Message(%3d), ExtraInformation(0x%08d)\n",
		ri.data.keyboard.MakeCode, ri.data.keyboard.Flags, ri.data.keyboard.VKey, ri.data.keyboard.Message, ri.data.keyboard.ExtraInformation);

	CFormView::OnRawInput(nInputcode, hRawInput);
}



// Volume Up/Down Signal (VK_VOLUME_UP/DOWN)
bool SendVolume(bool bUp) {

	if (g_bReceiverVolume) {
		SendReceiverVolume(bUp ? 10 : -10);
		return true;
	}

	INPUT ip;
	memset(&ip, 0, sizeof(ip));
	ip.type = INPUT_KEYBOARD;
	ip.ki.wVk = bUp ? VK_VOLUME_UP : VK_VOLUME_DOWN;
	ip.ki.wScan = 0;
	ip.ki.dwFlags = 0;
	ip.ki.dwExtraInfo = 0;
	SendInput(1, &ip, sizeof(ip));
	return 0;
}

std::optional<std::string> SendReceiverCommand(boost::beast::tcp_stream& stream, std::string const& strHostIP, std::string_view svXML) {
	namespace beast = boost::beast;     // from <boost/beast.hpp>
	namespace http = beast::http;       // from <boost/beast/http.hpp>
	namespace net = boost::asio;        // from <boost/asio.hpp>
	using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

	try {
		// Set up an HTTP request message
		auto const target = "/YamahaRemoteControl/ctrl"s;
		auto const version = /*("1.0"sv == "1.1"sv) ? 10 :*/ 11;
		http::request<http::string_body> req{http::verb::post, target, version};
		req.set(http::field::host, strHostIP);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		req.set(http::field::content_type, "text/xml");
		req.set(http::field::referer, fmt::format("http://{}/index.html?zone={}", strHostIP, g_eZone == eZONE::main ? 1 : 2));

		{
			req.body() = svXML;//R"xxx(<YAMAHA_AV cmd="PUT"><Zone_2><Volume><Lvl><Val>-450</Val><Exp>1</Exp><Unit>dB</Unit></Lvl></Volume></Zone_2></YAMAHA_AV>)xxx"s;
			req.set(http::field::content_length, fmt::format("{}", req.body().size()));
			std::ostringstream ss;
			ss << req;
			auto str = ss.str();

		}

		// Send the HTTP request to the remote host
		http::write(stream, req);

		// This buffer is used for reading and must be persisted
		beast::flat_buffer buffer;

		// Declare a container to hold the response
		http::response<http::dynamic_body> res;

		// Receive the HTTP response
		http::read(stream, buffer, res);

		auto body = res.body();
		auto strResponse = beast::buffers_to_string(body.data());

		return strResponse;

		// If we get here then the connection is closed gracefully
	}
	catch(std::exception const& )
	{
		//std::cerr << "Error: " << e.what() << std::endl;
		//return EXIT_FAILURE;
		return {};
	}
}

bool SendReceiverVolume(int diff) {

	namespace beast = boost::beast;     // from <boost/beast.hpp>
	namespace http = beast::http;       // from <boost/beast/http.hpp>
	namespace net = boost::asio;        // from <boost/asio.hpp>
	using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

	static auto const strHostIP = "192.168.10.11"s;

	try
	{
		auto const port = "80"s;

		// The io_context is required for all I/O
		net::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		// Look up the domain name
		auto const addr = resolver.resolve(strHostIP, "80"s);

		// Make the connection on the IP address we get from a lookup
		stream.connect(addr);

		auto const svReadVolume = fmt::format(R"xx(<YAMAHA_AV cmd="GET"><{0}><Volume><Lvl>GetParam</Lvl></Volume></{0}></YAMAHA_AV>)xx"sv, g_strZone);
		// ===>>>                      <YAMAHA_AV rsp="GET" RC="0"><Zone_2><Volume><Lvl><Val>-500</Val><Exp>1</Exp><Unit>dB</Unit></Lvl></Volume></Zone_2></YAMAHA_AV>
		if (auto const res = SendReceiverCommand(stream, strHostIP, svReadVolume); res) {
			auto const& str = res.value();
			//<YAMAHA_AV rsp="GET" RC="0"><Zone_2><Volume><Lvl><Val>-500</Val><Exp>1</Exp><Unit>dB</Unit></Lvl></Volume></Zone_2></YAMAHA_AV>
			static auto const tag = "<Val>"sv;
			auto pos = str.find(tag);
			if (pos < str.size()) {
				int volume = std::atoi(str.substr(pos + tag.size()).c_str());

				constexpr int const maxVol{-10*10};
				constexpr int const minVol{-80*10};

				volume = std::clamp(volume+diff, minVol, maxVol);

				auto strSetVolume = fmt::format(
					R"xxx(<YAMAHA_AV cmd="PUT"><{0}><Volume><Lvl><Val>{1}</Val><Exp>1</Exp><Unit>dB</Unit></Lvl></Volume></{0}></YAMAHA_AV>)xxx"sv,
					g_strZone, volume);

				SendReceiverCommand(stream, strHostIP, strSetVolume);

				g_receiverVolume = volume;
			}
		}


		// Gracefully close the socket
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if(ec && ec != beast::errc::not_connected)
			throw beast::system_error{ec};

	} catch(std::exception const& ) {
		//std::cerr << "Error: " << e.what() << std::endl;
		//return EXIT_FAILURE;
		return false;
	}


	return true;
}

bool SendReceiverCommand(std::string_view svCommand) {

	namespace beast = boost::beast;     // from <boost/beast.hpp>
	namespace http = beast::http;       // from <boost/beast/http.hpp>
	namespace net = boost::asio;        // from <boost/asio.hpp>
	using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

	static auto const strHostIP = "192.168.10.11"s;

	try
	{
		auto const port = "80"s;

		// The io_context is required for all I/O
		net::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		// Look up the domain name
		auto const addr = resolver.resolve(strHostIP, "80"s);

		// Make the connection on the IP address we get from a lookup
		stream.connect(addr);

		auto const res = SendReceiverCommand(stream, strHostIP, svCommand);

		// Gracefully close the socket
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if(ec && ec != beast::errc::not_connected)
			throw beast::system_error{ec};

	} catch(std::exception const& ) {
		//std::cerr << "Error: " << e.what() << std::endl;
		//return EXIT_FAILURE;
		return false;
	}


	return true;
}


void CKeyVolumeView::OnSelchangeReceiverZone() {
	g_eZone = m_cmbZone.GetCurSel() == 0 ? eZONE::main : eZONE::zone_2;
	g_strZone = (g_eZone == eZONE::main) ? "Main_Zone"s : "Zone_2"s;
	theApp.WriteProfileInt(_T("misc"), _T("Zone"), (int)g_eZone);
}


void CKeyVolumeView::OnBnClickedReceiverPowerOn() {
	auto const svPowerOn = fmt::format(R"xx(<YAMAHA_AV cmd="PUT"><{0}><Power_Control><Power>On</Power></Power_Control></{0}></YAMAHA_AV>)xx"sv, g_strZone);
	SendReceiverCommand(svPowerOn);
}


void CKeyVolumeView::OnBnClickedReceiverPowerOff() {
	auto const svPowerOff = fmt::format(R"xx(<YAMAHA_AV cmd="PUT"><{0}><Power_Control><Power>Standby</Power></Power_Control></{0}></YAMAHA_AV>)xx"sv, g_strZone);
	SendReceiverCommand(svPowerOff);
}


//#define BIT(x) (0x01<<(x))
//LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
//	if ((code < 0) || !g_bSendSpeakerVolumeKey) {
//		return CallNextHookEx(NULL, code, wParam, lParam);
//	}
//
//	do {
//		if (lParam & (/*BIT(30)|*/BIT(31)))	// Bit 30 : before state. Bit 31 : Pressed if 1.
//			break;
//		if ( ( (wParam == VK_UP) || (wParam == VK_DOWN) )
//			&& (GetAsyncKeyState(VK_RSHIFT) < 0)
//			&& (GetAsyncKeyState(VK_RCONTROL) < 0)
//			&& (GetAsyncKeyState(VK_MENU) >= 0)
//			&& (GetAsyncKeyState(VK_LWIN) >= 0)
//			&& (GetAsyncKeyState(VK_RWIN) >= 0)
//			&& (GetAsyncKeyState(VK_APPS) >= 0)
//			)
//		{
//			SendVolume(wParam == VK_UP);
//			return 1L;
//		}
//	} while(0);
//	return CallNextHookEx(NULL, code, wParam, lParam);;
//}
struct T_MOUSE_JUMP__MONITOR_PROC {
	int iMonitor{};
	int iLast{-1};
	CPoint ptCurrent;
	//CSize size;
};
BOOL CALLBACK MouseJump_MonitorProc(HMONITOR, HDC, LPRECT pRect, LPARAM pMonitorProc) {
	auto& mp = *(T_MOUSE_JUMP__MONITOR_PROC*)pMonitorProc;
	CRect rect(pRect);
	mp.iLast++;
	if (mp.iMonitor >= mp.iLast) {
		auto pt = rect.CenterPoint();
		SetCursorPos(pt.x, pt.y);
	}
	return true;
}
LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
	//TRACE("here\n");
	if (code < 0) {
		return CallNextHookEx(NULL, code, wParam, lParam);
	}

	if (wParam == WM_KEYDOWN && lParam) {

		KBDLLHOOKSTRUCT* pKey = reinterpret_cast< KBDLLHOOKSTRUCT* > (lParam);

		if (g_bSendSpeakerVolumeKey) {
			if ( ( (pKey->vkCode == VK_UP) || (pKey->vkCode == VK_DOWN) )
				&& (GetKeyState(VK_RSHIFT) < 0)
				&& (GetKeyState(VK_RCONTROL) < 0)
				&& (GetKeyState(VK_MENU) >= 0)
				&& (GetKeyState(VK_LWIN) >= 0)
				&& (GetKeyState(VK_RWIN) >= 0)
				&& (GetKeyState(VK_APPS) >= 0)
				)
			{
				SendVolume(pKey->vkCode == VK_UP);
				return -1L;
			}
		}

		if (g_bMouseJump) {
			if ( true
				and ((pKey->vkCode >= '1') and (pKey->vkCode < '1'+g_nMonitor))
				and (GetKeyState(VK_CONTROL) < 0)
				and (GetKeyState(VK_MENU) < 0)
				and (GetKeyState(VK_SHIFT) < 0)
				and (GetKeyState(VK_APPS) >= 0)
				and (GetKeyState(VK_LWIN) >= 0)
				and (GetKeyState(VK_RWIN) >= 0)
			) {
				if (pKey->vkCode == '1') {
					g_nMonitor = GetSystemMetrics(SM_CMONITORS);
				}
				T_MOUSE_JUMP__MONITOR_PROC mp;
				mp.iMonitor = pKey->vkCode - '1';
				GetCursorPos(&mp.ptCurrent);
				EnumDisplayMonitors(nullptr, nullptr, MouseJump_MonitorProc, std::bit_cast<LPARAM>(&mp));
			}
		}
	}

	if (g_bAutoShift) {
		if (auto l = AutoShift(code, wParam, lParam); l != 0)
			return l;
	}

	return CallNextHookEx(NULL, code, wParam, lParam);;
}

void SendKey(WORD vk, WORD scanCode) {
	//INPUT ip{};
	//memset(&ip, 0, sizeof(ip));
	//if ( (vk>='a') && (vk <='z') ) {
	//	ip.ki.wVk = VkKeyScan(vk);
	//	ip.ki.wScan = 0;
	//} else if ( (vk >= 'A') && (vk <= 'Z') ) {
	//	ip.ki.wScan = vk;
	//	ip.ki.wVk = 0;
	//} else {
	//	return;
	//}
	//ip.type = INPUT_KEYBOARD;
	//ip.ki.time = 0;
	//ip.ki.dwFlags = KEYEVENTF_UNICODE;
	//ip.ki.dwExtraInfo = 0;
	//SendInput(1, &ip, sizeof(INPUT));

	INPUT input {};
	input.type = INPUT_KEYBOARD;
	input.ki.dwExtraInfo = 0;
	input.ki.dwFlags = vk == 0 ? KEYEVENTF_UNICODE : 0;
	input.ki.time = 0;
	input.ki.wScan = scanCode;
	input.ki.wVk = vk;
	SendInput(1, &input, sizeof(input));
}

LRESULT AutoShift(int nCode, WPARAM wParam, LPARAM lParam) {

	static std::map<int, int> keyTable;

	static const std::map<int, int> keyMapEx = {
		{VK_OEM_3,		'~'},
		{'1',			'!'},
		{'2',			'@'},
		{'3',			'#'},
		{'4',			'$'},
		{'5',			'%'},
		{'6',			'^'},
		{'7',			'&'},
		{'8',			'*'},
		{'9',			'('},
		{'0',			')'},
		{VK_OEM_MINUS,	'_'},
		{VK_OEM_PLUS,	'+'},
		{VK_OEM_4,		'{'},
		{VK_OEM_6,		'}'},
		{VK_OEM_5,		'|'},
		{VK_OEM_1,		':'},
		{VK_OEM_7,		'"'},
		{VK_OEM_COMMA,	'<'},
		{VK_OEM_PERIOD,	'>'},
		{VK_OEM_2,		'?'},
	};

	LRESULT lResult = 0;

	if (!g_bAutoShift) 
		return lResult;

	const KBDLLHOOKSTRUCT* pKBDHS = (KBDLLHOOKSTRUCT*)lParam;
	if (!pKBDHS)
		return lResult;

	bool bKeyDown{};
	bool bExtended{};
	bool bInKorean{};
	switch (wParam) {
	case WM_KEYDOWN :
	case WM_SYSKEYDOWN :
		bExtended = (GetKeyState(VK_SHIFT) < 0) || (GetKeyState(VK_CONTROL) < 0)
			|| (GetKeyState(VK_MENU) < 0) || (GetKeyState(VK_LWIN) < 0) || (GetKeyState(VK_RWIN) < 0);
		bKeyDown = true;
		if (auto hWndFocused = GetForegroundWindow(); hWndFocused) {
			auto hIME = ImmGetDefaultIMEWnd(hWndFocused);
			if ( (::SendMessage(hIME, WM_IME_CONTROL, 0x0005, 0) != 0)
				&& (::SendMessage(hIME, WM_IME_CONTROL, 0x0001, 0) & IME_CMODE_HANGUL)
				)
			{
				// 한글 모드
				bInKorean = true;
			}
		}
		break;
	case WM_KEYUP :
	case WM_SYSKEYUP :
		bKeyDown = false;
		break;
	}
	bool bInjected = (pKBDHS->flags & LLKHF_LOWER_IL_INJECTED) ? true : false;

	if (bInjected) {
		return lResult;
	}

	auto scanCode = pKBDHS->scanCode;
	if (!scanCode)
		return lResult;

	if (bKeyDown && !bExtended && !bInKorean) {

		int cConverted{};
		auto vk = pKBDHS->vkCode;
		if ( (vk >= 'A') && (vk <= 'Z') ) {
			cConverted = vk;
			TRACE("ASCII - %c -> %c\n", vk, cConverted);
		} else if (auto iter = keyMapEx.find(vk); iter != keyMapEx.cend() ) {
			cConverted = iter->second;
			TRACE("Found - %c -> %c\n", vk, cConverted);
		}

		if (cConverted) {

			if (keyTable[scanCode] < g_minRepeatCount) {
				keyTable[scanCode] ++;
			}
			else {

				if (keyTable[scanCode] == g_minRepeatCount) {
					keyTable[scanCode] ++;

					SendKey(VK_BACK, VkKeyScan(VK_BACK));
				}

				if (keyTable[scanCode] == g_minRepeatCount+1) {
					keyTable[scanCode] ++;	// no more repeat.

					SendKey(0, (WORD)cConverted);
					TRACE("%c\n", cConverted);
				}

			}
		}
		if (keyTable[scanCode] > 1)
			lResult = 1;
	}
	else {
		keyTable[scanCode] = 0;
	}

	return lResult;
}
