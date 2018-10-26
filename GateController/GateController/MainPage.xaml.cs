using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
//using Microsoft.Azure.Devices;
using Xamarin.Forms;

namespace GateController
{
    public partial class MainPage : ContentPage
    {
        private readonly GateAccessorData _gateAccessorData;
        private readonly ObservableCollection<string> _messageCollection = new ObservableCollection<string>();
        //private readonly ResettableLazy<ServiceClient> _serviceClientLazy;
        //private ServiceClient ServiceClient => _serviceClientLazy.Value;

        private const string IotHubProxyFunction =
            "https://iothubgateproxy.azurewebsites.net/api/SendCommandToGate?code=l1dr7kawEXd2XOWrUNO9bYtzftPViqFB3Jnxy1rXUFfAg7JjhfZQSA==";

        private const string ServiceBusQueueStatus =
            "https://iothubgateproxy.azurewebsites.net/api/GetGateStatusMessages?code=J3hhy1Cq2imacpSnmkh5/Fkhau4g5ibRu1cBeLxLU2qkPoWXUnbGjQ==";

        private readonly GateMessages _gateMessages = new GateMessages();

        public MainPage()
        {
            InitializeComponent();

            _gateAccessorData = new GateAccessorData(_messageCollection);
            GateMessagesView.ItemsSource = _gateMessages.Messages;
            //_serviceClientLazy = new ResettableLazy<ServiceClient>(() =>
            //    ServiceClient.CreateFromConnectionString(_gateAccessorData.IoTHubConnectionString));
            InitGetDeviceStatusAsync();
        }

        private async void InitGetDeviceStatusAsync()
        {
            while (true)
            {

                try
                {
                    var result = await GetGateStatusMessage("minaturegatestatusqueue");
                    result = result.Trim('"');
                    if (!result.ToLower().StartsWith("error"))
                    {
                        _gateMessages.Messages.Add(result);
                    }
                }
                catch (Exception e)
                {
                    await DisplayAlert("Error", e.Message, "OK");
                }

            }
        }

        async Task<string> GetGateStatusMessage(string queueName)
        {
            using (var client = new HttpClient())
            {
                client.DefaultRequestHeaders.Accept.Clear();
                client.DefaultRequestHeaders.Accept.Add(new MediaTypeWithQualityHeaderValue("text/html"));
                var uri =
                    $"{ServiceBusQueueStatus}&statusQueue={queueName}";
                HttpResponseMessage response = await client.GetAsync(uri);

                var result = await response.Content.ReadAsStringAsync();

                return result;
            }
        }

        private async Task<bool> OnActivateModelGateAsync(string command)
        {
            if (await SendCloudToDeviceCommandAsync(command))
            {
                await DisplayAlert("Message to IoT Hub", "Async message was sent", "OK");
                return true;
            }

            //else
            return false;
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
            var result = await DisplayAlert("Reset to web",
                "Are you sure you want to turn off Azure IoT Hub communication?", "Yes", "No");
            if (result)
            {
                await OnActivateModelGateAsync("Web");
            }
        }

        private async void OnSettings(object sender, EventArgs e)
        {
            await Navigation.PushModalAsync(new SettingsPage(_gateAccessorData));
          //  _serviceClientLazy.Reset();
        }

        async Task<bool> SendCloudToDeviceCommandAsync(string command)
        {
            //TODO: what for a bug fix and call directly: https://github.com/Azure/azure-amqp/issues/127
            //await ServiceClient.SendAsync(_gateAccessorData.DeviceId, new Message(Encoding.ASCII.GetBytes(command)));


            using (var client = new HttpClient())
            {
                try
                {
                    var jsonObject =
                        $"{{ 'device' : '{_gateAccessorData.DeviceId}' , 'iothub' : '{_gateAccessorData.IoTHubConnectionString}' , 'command' : '{command}' }}";
                    var content = new StringContent(jsonObject.ToString(), Encoding.UTF8, "application/json");
                   
                    HttpResponseMessage response = await client.PostAsync(IotHubProxyFunction, content);

                    await response.Content.ReadAsStringAsync();
                }
                catch (Exception exception)
                {
                    await DisplayAlert("Error sending message", exception.Message, "OK");
                    return false;
                }

                return true;
            }
        }
    }
}

