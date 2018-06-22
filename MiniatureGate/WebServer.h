// WebServer.h

#ifndef _WEBSERVER_h
#define _WEBSERVER_h


#undef ARDUINO_BOARD
#define ARDUINO_BOARD "generic"
#include <memory>
#include "Singleton.h"
#include "PubSub.h"
#include "ArduinoLoopManager.h"
#include "Util.h"
#include <ESP8266WebServer.h>
#include "WiFiManager.h"

class IWebCommand
{
public:
	virtual const String& Name() const = 0;
	virtual const String& MenuEntry() const = 0; 
	virtual const String& ResultHTML() const = 0;
	virtual const String& TriggerUrl() const = 0;

    IWebCommand() = default;
    IWebCommand(const IWebCommand&) = delete;
    IWebCommand& operator=(const IWebCommand&) = delete;
    IWebCommand(const IWebCommand&&) = delete;
    IWebCommand& operator=(const IWebCommand&&) = delete;
    virtual ~IWebCommand() = default;
};

enum class PushButtonBehaviour
{
	Toggle,
	Pulse
};

struct DeviceSettings
{
	bool isFactoryReset{};
	String ssidName;
	String accessPointPassword;
	bool shouldUseAzureIoT{};
	String azureIoTHubConnectionString;
	String AzureIoTDeviceId;
	unsigned int longButtonPeriod{};
	unsigned int veryLongButtonPeriod{};
	unsigned int PulseActivationPeriod{};
    PushButtonBehaviour PBBehavior { PushButtonBehaviour::Pulse };
};

typedef std::shared_ptr<IWebCommand> WebCommandPtr_t;
typedef std::function<void(const String&)> WebNotificationPtr_t;
class WebServer : public Singleton<WebServer>, public IProcessor
{
	friend class Singleton<WebServer>;
private:
	char _setupHtmlBuffer[3300]{}; //for setup html result
	std::unique_ptr<DeviceSettings> _deviceSettings;
	ESP8266WebServer _server;
	PubSub<WebServer, const String&> _pubsub;
	String _header;
	const String _authorizedUrl;
	std::vector<WebCommandPtr_t> _webCommands;
	bool _isInit = false;
	std::function<String()> _gateStatusUpdater;
	std::function<void(const DeviceSettings&)> _configurationUpdater;
	//setup template processing variables
	int _templateIndex = 0;
	int _templateBufferIndex = 0;
	int _templateEnd = -1;
	Util::StringMap _templateValuesMap;
	bool _isHttpSetupRequestOn = false;

	void SendBackHtml(const String &message);
	void UpdateStatus(const ConnectionStatus& status);
	String CreateUrl(const String &s) const;
	bool PopulateHTMLSetupFromTemplate(const String& htmlTemplate, const Util::StringMap &map);
	WebServer(const WiFiManagerPtr_t& wifiManager, int port, const char *appKey, std::unique_ptr<DeviceSettings> deviceSettings, std::function<String ()> gateStatusUpdater);

	void HandleMain();
	void ProcessHTTPSetupRequest();
	void HandleSendAPScript();
	void HandleSetup();
	void HandleSendViewCSS();
	void HandleSendAPList();
	void HandleSetConfiguration();
	void HandleResetAccessPoint();
	void HandleError();
	void HandleCommand(const WebCommandPtr_t& webCommand);

 public:
	const char *GetName() const override { return "WebServer"; }
	void RegisterCommand(const WebCommandPtr_t& command);
	template<typename T>
	void SetWebSiteHeader(T header) { _header = std::forward<T>(header); }
	void Register(const WebNotificationPtr_t &subscriber) { _pubsub.Register(subscriber); }
	static bool IsConnected();
	void Loop() override;
	void SetUpdateConfiguration(std::function<void(const DeviceSettings&)> configurationUpdater);
};

typedef std::shared_ptr<WebServer> WebServerPtr_t;
#endif

