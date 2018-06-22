// AzureIoTHubHttpClient.h

#ifndef _AZUREIOTHUBHTTPCLIENT_h
#define _AZUREIOTHUBHTTPCLIENT_h

#ifdef __cplusplus
extern "C" {
#endif
	void AzureIoTHubLoop();
	bool AzureIoTHubInit(const char *connectionString);
	bool AzureIoTHubSendMessage(const char *deviceId, const char *status);

#ifdef __cplusplus
}
#endif

#endif

