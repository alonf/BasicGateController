
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;
using Microsoft.Azure.ServiceBus;
using System.Threading;

namespace BasicGateApp
{
	public partial class MainPage : ContentPage
	{
	    private GateMessages _gateMessages = new GateMessages();

	    private string ServiceBusConnectionString = "Endpoint=sb://gateservicebus.servicebus.windows.net/;SharedAccessKeyName=Listen;SharedAccessKey=hNIxnOj9aBzZPCsWEKXOtzEGvogOK1Q/mwq8j7h/AVo=";
	    private string QueueName = "telemetryqueue";
	    private IQueueClient _queueClient;

        public MainPage()
		{
		    InitializeComponent();
		    GateMessagesView.ItemsSource = _gateMessages.Messages;
		    CreateAndRegisterServiceBusQueueListener();

		}

	    private async Task ProcessMessagesAsync(Message message, CancellationToken token)
	    {
	        _gateMessages.Messages.Add(Encoding.UTF8.GetString(message.Body));
           
	        // Complete the message so that it is not received again.
	        // This can be done only if the _queueClient is opened in ReceiveMode.PeekLock mode (which is default).
	        await _queueClient.CompleteAsync(message.SystemProperties.LockToken);
	    }

	    private Task ExceptionReceivedHandler(ExceptionReceivedEventArgs exceptionReceivedEventArgs)
	    {
	        DisplayAlert("Error", $"Message handler encountered an exception {exceptionReceivedEventArgs.Exception}.", "OK");
	        return Task.CompletedTask;
	    }

	    private void RegisterOnMessageHandlerAndReceiveMessages()
	    {
	        // Configure the MessageHandler Options in terms of exception handling, number of concurrent messages to deliver etc.
	        var messageHandlerOptions = new MessageHandlerOptions(ExceptionReceivedHandler)
	        {
	            // Maximum number of Concurrent calls to the callback `ProcessMessagesAsync`, set to 1 for simplicity.
	            // Set it according to how many messages the application wants to process in parallel.
	            MaxConcurrentCalls = 1,

	            // Indicates whether MessagePump should automatically complete the messages after returning from User Callback.
	            // False value below indicates the Complete will be handled by the User Callback as seen in `ProcessMessagesAsync`.
	            AutoComplete = false
	        };

	        // Register the function that will process messages
	        _queueClient.RegisterMessageHandler(ProcessMessagesAsync, messageHandlerOptions);
	    }


	    private void CreateAndRegisterServiceBusQueueListener()
	    {
	        try
	        {
	            _queueClient = new QueueClient(ServiceBusConnectionString, QueueName);

	            // Register QueueClient's MessageHandler and receive messages in a loop
	            RegisterOnMessageHandlerAndReceiveMessages();
	        }
	        catch (Exception e)
	        {
	            DisplayAlert("Error", $"{e.Message}", "OK");
	        }
        }

        async Task<string> SendCloudToDeviceCommandAsync(string deviceId, string command)
        {
            using (var client = new HttpClient())
            {
                string result;

                try
                {
                    client.DefaultRequestHeaders.Accept.Clear();
                    client.DefaultRequestHeaders.Accept.Add(new MediaTypeWithQualityHeaderValue("text/html"));
                    var uri =
                        $"https://gateiothubsupport.azurewebsites.net/api/SendCloudToDeviceCommand?code=bv8gMLTAXMNLi7OVaIajxzG3wpGpqnhFGA0SaVsNf3Zq3KRv1ZAjng==&device={deviceId}&command={command}";
                    HttpResponseMessage response = await client.GetAsync(uri);

                    result = await response.Content.ReadAsStringAsync();
                    result = $"{response.StatusCode} : {result}";
                }
                catch (Exception exception)
                {
                    result = exception.Message;
                }
                return result;
            }
        }

        

        private  async Task OnActivateModelGateAsync(string command)
	    {
            var result = await SendCloudToDeviceCommandAsync("ModelGate", command);
            await DisplayAlert("Model Gate HTTP Get result", result, "OK");
        }

	    private async void OnStopModelGate(object sender, EventArgs e)
	    {
	        await OnActivateModelGateAsync("Stop");
	    }

	    private async void OnOpenModelGate(object sender, EventArgs e)
	    {
            await OnActivateModelGateAsync("Open");
        }

	    private async void OnCloseModelGate(object sender, EventArgs e)
	    {
            await OnActivateModelGateAsync("Close");
        }

        private async void OnSwitchToWebModelGate(object sender, EventArgs e)
	    {
            await OnActivateModelGateAsync("Web");
        }
	}
}
